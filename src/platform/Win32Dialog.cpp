#include "Win32Dialog.hpp"

#include "Win32Platform.hpp"
#include "engine/container/Algorithm.hxx"
#include "engine/container/Array.hxx"
#include "engine/container/Utils.hpp"
#include "engine/core/StringUtils.hpp"

static const char* k_setupDialogClassName = "Setup";

// Set to 1 to launch the demo on the display the dialog box is on.
// Set to 0 to always launch the demo on the primary display.
// Note that the dialog doesn't update its content at runtime, when
// moving to a different display.
#define ENABLE_MULTIPLE_MONITOR_SUPPORT 1

using namespace platform;

#define MAX_LISTED_VERTICAL_RESOLUTION 8
static int s_verticalResolutions[MAX_LISTED_VERTICAL_RESOLUTION];

static int listResolutions(const char* descriptions[])
{
	const int nativeHeight = GetSystemMetrics(SM_CYSCREEN);

	// List of typical display vertical resolution we have gotten used to.
	const int vResolutions[] = { 480, 720, 1080, 1440, nativeHeight };
	const char* vResolution_descs[] = { "480p", "720p", "1080p", "1440p", "Native" };

	int count = 0;
	for (int i = 0; i < ARRAY_LEN(vResolution_descs); ++i)
	{
		const int vRes = vResolutions[i];
		if (nativeHeight > vRes || i == ARRAY_LEN(vResolution_descs) - 1)
		{
			descriptions[count] = vResolution_descs[i];
			s_verticalResolutions[count] = vRes;
			++count;
		}
	}
	return count;
}

Dialog * Dialog::s_instance;

Dialog::Dialog(): isOk(false), fullscreen(false), m_ids(0)
{
}

void Dialog::run(const char* title)
{
#define DEFAULT_DPI 96
	::SetProcessDPIAware();
	dpi = ::GetDeviceCaps(GetDC(nullptr), LOGPIXELSX);

	m_hInstance = GetModuleHandle(0);

	WNDCLASS wc = {
		CS_OWNDC,
		input,
		0,
		0,
		m_hInstance,
		0,
		0,
		(HBRUSH)COLOR_WINDOW,
		0,
		k_setupDialogClassName,
	};

	ATOM res = RegisterClass(&wc);
	// TODO: error handling

	const DWORD style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
	const DWORD exstyle = WS_EX_APPWINDOW;

	const int desiredWidth = (400*dpi)/DEFAULT_DPI;
	const int desiredHeight = (170*dpi)/DEFAULT_DPI;

	RECT desktopRect;
	GetClientRect(GetDesktopWindow(), &desktopRect);

	RECT dialogRect;
	dialogRect.left = (desktopRect.right - desiredWidth) / 2;
	dialogRect.top = (desktopRect.bottom - desiredHeight) / 2;
	dialogRect.right  = dialogRect.left + desiredWidth;
	dialogRect.bottom = dialogRect.top + desiredHeight;
	AdjustWindowRectEx(&dialogRect, style, false, exstyle);

	m_dialogHandle = CreateWindowEx(exstyle, k_setupDialogClassName, title, style,
									dialogRect.left, dialogRect.top,
									dialogRect.right - dialogRect.left,
									dialogRect.bottom - dialogRect.top,
									0, 0, m_hInstance, 0);
	// TODO: error handling

	// Documentation about GetStockObject(DEFAULT_GUI_FONT):
	// https://msdn.microsoft.com/en-us/library/windows/desktop/dd144925%28v=vs.85%29.aspx
	//
	// "It is not recommended that you employ this method to obtain the
	// current font used by dialogs and windows. Instead, use the
	// SystemParametersInfo function with the SPI_GETNONCLIENTMETRICS
	// parameter to retrieve the current font. SystemParametersInfo
	// will take into account the current theme and provides font
	// information for captions, menus, and message dialogs."

	// Code snippet from:
	// http://stackoverflow.com/questions/2942828/correct-dialog-ui-font-on-windows/2942869#2942869
	NONCLIENTMETRICS metrics;
	metrics.cbSize = sizeof(NONCLIENTMETRICS);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &metrics, 0);
	m_fontHandle = CreateFontIndirect(&metrics.lfMessageFont);

	SendMessage(m_dialogHandle, WM_SETFONT, (WPARAM)m_fontHandle, TRUE);

	const int vSpace = 30;
	const int x = 30;
	int y = vSpace;
	{
		const char* descriptions[MAX_LISTED_VERTICAL_RESOLUTION];
		const int count = listResolutions(descriptions);

		m_combobox_resolutions = CreateSelectionMenu("Resolution:",
			x, y, descriptions, count, count - 1);
	}

#if ENABLE_QUALITY_OPTION
	y += vSpace;
	{
		static const char* qualities[] = { "Low", "Medium", "High" };
		m_combobox_qualities = CreateSelectionMenu("Graphics:",
			x, y, qualities, ARRAY_LEN(qualities), ARRAY_LEN(qualities) - 1);
	}
#endif // ENABLE_QUALITY_OPTION

	y += vSpace;
	m_checkbox_fullscreen = CreateCheckBox("Fullscreen", 185, y, true, &m_id_checkbox_fullscreen);

	y += vSpace;
	m_button_ok = CreateButton("Test", 120, y, &m_id_button_ok);

	ShowWindow(m_dialogHandle, SW_SHOW);
	UpdateWindow(m_dialogHandle);
	SetFocus(m_button_ok);

	MSG msg;
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

HWND Dialog::CreateButton(const char* label,
						  int x, int y,
						  int* id)
{
	x = (x * dpi) / DEFAULT_DPI;
	y = (y * dpi) / DEFAULT_DPI;
	const int w = (150 * dpi) / DEFAULT_DPI;
	const int h = (28 * dpi) / DEFAULT_DPI;

	const HWND buttonHandle = CreateWindow("BUTTON", label,
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
		x, y, w, h, m_dialogHandle, (HMENU)++m_ids,
		(HINSTANCE)GetWindowLong(m_dialogHandle, GWL_HINSTANCE), 0);
	SendMessage(buttonHandle, WM_SETFONT, (WPARAM)m_fontHandle, TRUE);

	*id = m_ids;
	return buttonHandle;
}

HWND Dialog::CreateCheckBox(const char* label,
							int x, int y,
							bool isChecked,
							int* id)
{
	x = (x * dpi) / DEFAULT_DPI;
	y = (y * dpi) / DEFAULT_DPI;
	const int w = (110 * dpi) / DEFAULT_DPI;
	const int h = (28 * dpi) / DEFAULT_DPI;

	const HWND buttonHandle = CreateWindow("BUTTON", label,
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
		x, y, w, h, m_dialogHandle, (HMENU)++m_ids,
		(HINSTANCE)GetWindowLong(m_dialogHandle, GWL_HINSTANCE), 0);
	SendMessage(buttonHandle, WM_SETFONT, (WPARAM)m_fontHandle, TRUE);
	CheckDlgButton(m_dialogHandle, m_ids, (isChecked ? BST_CHECKED : BST_UNCHECKED));

	*id = m_ids;
	return buttonHandle;
}

HWND Dialog::CreateSelectionMenu(const char* label,
								 int x, int y,
								 const char** items,
								 int numberOfItems,
								 int selectedDefault)
{
	x = (x * dpi) / DEFAULT_DPI;
	y = (y * dpi) / DEFAULT_DPI;
	const int w1 = (150 * dpi) / DEFAULT_DPI;
	const int w2 = (150 * dpi) / DEFAULT_DPI;
	const int h = (130 * dpi) / DEFAULT_DPI;

	const HWND labelHandle = CreateWindow("STATIC", label,
		WS_CHILD | WS_VISIBLE | SS_RIGHT,
		x, y + 5, w1, h,
		m_dialogHandle, (HMENU)++m_ids,
		(HINSTANCE)GetWindowLong(m_dialogHandle, GWL_HINSTANCE), 0);
	SendMessage(labelHandle, WM_SETFONT, (WPARAM)m_fontHandle, TRUE);

	const HWND comboBoxHandle = CreateWindow("COMBOBOX", nullptr,
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | WS_VSCROLL,
		x + w1 + 25, y, w2, h, m_dialogHandle, (HMENU)++m_ids,
		(HINSTANCE)GetWindowLong(m_dialogHandle, GWL_HINSTANCE), 0);
	SendMessage(comboBoxHandle, WM_SETFONT, (WPARAM)m_fontHandle, TRUE);

	for (int i = 0; i < numberOfItems; ++i)
	{
		SendMessage(comboBoxHandle, CB_ADDSTRING, 0, (LPARAM)items[i]);
	}
	SendMessage(comboBoxHandle, CB_SETCURSEL, (WPARAM)selectedDefault, 0);

	return comboBoxHandle;
}

inline bool isChecked(HWND hDlg, int cb)
{
  return IsDlgButtonChecked(hDlg, cb) == BST_CHECKED;
}

LRESULT CALLBACK Dialog::input(HWND h, UINT msg, WPARAM wp, LPARAM lp)
{
	int wmi;
	int wme;

	switch (msg)
	{
	case WM_COMMAND:
		wmi = LOWORD(wp);
		wme = HIWORD(wp);

		if (wmi == s_instance->m_id_button_ok)
		{
			LRESULT resolutionIndex = SendMessage(s_instance->m_combobox_resolutions, CB_GETCURSEL, 0, 0);

#if ENABLE_QUALITY_OPTION
			LRESULT qualityIndex = SendMessage(s_instance->m_combobox_qualities, CB_GETCURSEL, 0, 0);
			s_instance->quality = qualityIndex;
#if DEBUG
			s_instance->quality += 2; // see Settings.hpp; skip RidiculouslyLow and VeryLow
#endif
#endif	// ENABLE_QUALITY_OPTION
			s_instance->isOk = true;
			s_instance->fullscreen = isChecked(s_instance->m_dialogHandle, s_instance->m_id_checkbox_fullscreen);

			// Resolution:
#if ENABLE_MULTIPLE_MONITOR_SUPPORT
			HMONITOR monitor = MonitorFromWindow(h, MONITOR_DEFAULTTONEAREST);
			MONITORINFO monitorInfo;
			monitorInfo.cbSize = sizeof(monitorInfo);
			GetMonitorInfo(monitor, &monitorInfo);
			s_instance->monitorLeft = monitorInfo.rcMonitor.left;
			s_instance->monitorTop = monitorInfo.rcMonitor.top;
			s_instance->monitorWidth = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
			s_instance->monitorHeight = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
#else // !ENABLE_MULTIPLE_MONITOR_SUPPORT
			Win32Platform::GetMonitorRect(
				&s_instance->monitorLeft,
				&s_instance->monitorTop,
				&s_instance->monitorWidth,
				&s_instance->monitorHeight);
#endif // !ENABLE_MULTIPLE_MONITOR_SUPPORT

			const int height = s_verticalResolutions[resolutionIndex];
			s_instance->renderWidth = (height * GetSystemMetrics(SM_CXSCREEN)) / GetSystemMetrics(SM_CYSCREEN);
			s_instance->renderHeight = height;

			DestroyWindow(s_instance->m_dialogHandle);
			return 0;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(h, msg, wp, lp);
}
