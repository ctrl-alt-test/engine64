#include "Win32Platform.hpp"
#include "gfx/OpenGL/wglext.h"
#include "engine/debug/Assert.hpp"
#include "engine/debug/Debug.hpp"

using namespace platform;

class Win32Platform* platform::g_platform = nullptr;

static const char* k_demoWindowClassName = "Demo";

#if ENABLE_KEYBOARD_AND_MOUSE

#define MAX_INPUT_HANDLERS	8

InputState s_inputState = {};
KeyboardHandler s_keyboardHandlers[MAX_INPUT_HANDLERS] = {};
MouseHandler s_mouseHandlers[MAX_INPUT_HANDLERS] = {};
void* s_handlerContexts[MAX_INPUT_HANDLERS] = {};
int s_nextHandler = 0;

// Virtual keys documentation:
// https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731%28v=vs.85%29.aspx
//
// Articles on how to properly handle keys on Windows:
// https://blog.molecular-matters.com/2011/09/05/properly-handling-keyboard-input/
// https://www-user.tu-chemnitz.de/~heha/petzold/ch06c.htm
static KeyCode::Enum GetKeyCode(WPARAM wParam)
{
	if (wParam >= '0' && wParam <= '9')
	{
		return (KeyCode::Enum)(KeyCode::key0 + (int)(wParam - (WPARAM)'0'));
	}

	if (wParam >= 'A' && wParam <= 'Z')
	{
		return (KeyCode::Enum)(KeyCode::keyA + (int)(wParam - (WPARAM)'A'));
	}

	if (wParam >= VK_NUMPAD0 && wParam <= VK_NUMPAD9)
	{
		return (KeyCode::Enum)(KeyCode::keyNumpad0 + (int)(wParam - VK_NUMPAD0));
	}

	if (wParam >= VK_F1 && wParam <= VK_F24)
	{
		return (KeyCode::Enum)(KeyCode::keyF1 + (int)(wParam - VK_F1));
	}

	// MSDN virtual-key codes documentation:
	// https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731.aspx
	switch (wParam)
	{
	case VK_LBUTTON:			break;
	case VK_RBUTTON:			break;
	case VK_CANCEL:				break;
	case VK_MBUTTON:			break;
	case VK_XBUTTON1:			break;
	case VK_XBUTTON2:			break;
	case VK_BACK:		return KeyCode::keyBackspace;
	case VK_TAB:		return KeyCode::keyTab;
	case VK_CLEAR:				break;
	case VK_RETURN:		return KeyCode::keyEnter;
	case VK_SHIFT:		return KeyCode::keyLeftShift;
	case VK_CONTROL:	return KeyCode::keyLeftControl;
	case VK_MENU:		return KeyCode::keyRightAlt; // Right Alt / AltGr
	case VK_PAUSE:		return KeyCode::keyPause;
	case VK_CAPITAL:	return KeyCode::keyCapsLock;
	case VK_KANA:				break; // IME, unsupported.
	case VK_JUNJA:				break; // IME, unsupported.
	case VK_FINAL:				break; // IME, unsupported.
	case VK_HANJA:				break; // IME, unsupported.
	case VK_ESCAPE:		return KeyCode::keyEscape;
	case VK_CONVERT:			break; // IME, unsupported.
	case VK_NONCONVERT:			break; // IME, unsupported.
	case VK_ACCEPT:				break; // IME, unsupported.
	case VK_MODECHANGE:			break; // IME, unsupported.
	case VK_SPACE:		return KeyCode::keySpace;
	case VK_PRIOR:		return KeyCode::keyPageUp;
	case VK_NEXT:		return KeyCode::keyPageDown;
	case VK_END:		return KeyCode::keyEnd;
	case VK_HOME:		return KeyCode::keyHome;
	case VK_LEFT:		return KeyCode::keyArrowLeft;
	case VK_UP:			return KeyCode::keyArrowUp;
	case VK_RIGHT:		return KeyCode::keyArrowRight;
	case VK_DOWN:		return KeyCode::keyArrowDown;
	case VK_SELECT:				break;
	case VK_PRINT:				break;
	case VK_EXECUTE:			break;
	case VK_SNAPSHOT:	return KeyCode::keyPrintScreen;
	case VK_INSERT:		return KeyCode::keyInsert;
	case VK_DELETE:		return KeyCode::keyDelete;
	case VK_HELP:				break;
		//
		// All the alphanumeric keys are after VK_HELP.
		// They are handled in the loop above.
		//
	case VK_LWIN:		return KeyCode::keyLeftCommand;
	case VK_RWIN:		return KeyCode::keyRightCommand;
	case VK_APPS:		return KeyCode::keyMenu;
	case VK_SLEEP:				break;
		//
		// All the numpad numeric keys are after VK_SLEEP.
		// They are handled in the loop above.
		//
	case VK_MULTIPLY:	return KeyCode::keyNumpadMultiply;
	case VK_ADD:		return KeyCode::keyNumpadPlus;
	case VK_SEPARATOR:			break;
	case VK_SUBTRACT:	return KeyCode::keyNumpadMinus;
	case VK_DECIMAL:	return KeyCode::keyNumpadDecimal;
	case VK_DIVIDE:		return KeyCode::keyNumpadDivide;
		//
		// All the Fxx keys are after VK_DIVIDE.
		// They are handled in the loop above.
		//
	case VK_NUMLOCK:	return KeyCode::keyNumLock;
	case VK_SCROLL:		return KeyCode::keyScrollLock;
	case VK_LSHIFT:		return KeyCode::keyLeftShift;
	case VK_RSHIFT:		return KeyCode::keyRightShift;
	case VK_LCONTROL:	return KeyCode::keyLeftControl;
	case VK_RCONTROL:	return KeyCode::keyRightControl;
	case VK_LMENU:		return KeyCode::keyLeftAlt;
	case VK_RMENU:		return KeyCode::keyRightAlt;
	case VK_BROWSER_BACK:		break;
	case VK_BROWSER_FORWARD:	break;
	case VK_BROWSER_REFRESH:	break;
	case VK_BROWSER_STOP:		break;
	case VK_BROWSER_SEARCH:		break;
	case VK_BROWSER_FAVORITES:	break;
	case VK_BROWSER_HOME:		break;
	case VK_VOLUME_MUTE:		break;
	case VK_VOLUME_DOWN:		break;
	case VK_VOLUME_UP:			break;
	case VK_MEDIA_NEXT_TRACK:	break;
	case VK_MEDIA_PREV_TRACK:	break;
	case VK_MEDIA_STOP:			break;
	case VK_MEDIA_PLAY_PAUSE:	break;
	case VK_LAUNCH_MAIL:		break;
	case VK_LAUNCH_MEDIA_SELECT:break;
	case VK_LAUNCH_APP1:		break;
	case VK_LAUNCH_APP2:		break;
	case VK_OEM_1:				break; // Varies with keyboard. ; on US, $ on FR.
	case VK_OEM_PLUS:	return KeyCode::keyPlus;
	case VK_OEM_COMMA:	return KeyCode::keyComma;
	case VK_OEM_MINUS:	return KeyCode::keyMinus; // Doesn't work on FR layout?
	case VK_OEM_PERIOD:	return KeyCode::keyPeriod;
	case VK_OEM_2:				break; // Varies with keyboard layout. / on US, : on FR.
	case VK_OEM_3:				break; // Varies with keyboard layout. ` on US, ù on FR.
	case VK_OEM_4:				break; // Varies with keyboard layout.
	case VK_OEM_5:				break; // Varies with keyboard layout. \ on US.
	case VK_OEM_6:				break; // Varies with keyboard layout. ] on US, ^ on FR.
	case VK_OEM_7:				break; // Varies with keyboard layout. ' on US, ² on FR.
	case VK_OEM_8:				break; // Varies with keyboard layout. ! on FR.
	case VK_OEM_102:			break; // Varies with keyboard layout.
	case VK_PROCESSKEY:			break; // IME, unsupported.
	case VK_PACKET:				break; // Unicode.
	case VK_ATTN:				break;
	case VK_CRSEL:				break;
	case VK_EXSEL:				break;
	case VK_EREOF:				break;
	case VK_PLAY:				break;
	case VK_ZOOM:				break;
	case VK_NONAME:				LOG_DEBUG("VK_NONAME"); break;
	case VK_PA1:				break;
	case VK_OEM_CLEAR:			break;
	}
	return KeyCode::numberOfKeys;
}

static KeyboardState GetKeyboardState()
{
	const KeyboardState state = {
		(GetKeyState(VK_LMENU) < 0),               // L-Alt key down
		(GetKeyState(VK_RMENU) < 0),               // R-Alt key down
		(GetKeyState(VK_LWIN) < 0),                // L-Command key down
		(GetKeyState(VK_RWIN) < 0),                // R-Command key down
		(GetKeyState(VK_LCONTROL) < 0),            // L-Ctrl key down
		(GetKeyState(VK_RCONTROL) < 0),            // R-Ctrl key down
		(GetKeyState(VK_LSHIFT) < 0),              // L-Shift key down
		(GetKeyState(VK_RSHIFT) < 0),              // R-Shift key down
		((0x0001 & GetKeyState(VK_CAPITAL)) != 0), // Caps-Lock active
		((0x0001 & GetKeyState(VK_NUMLOCK)) != 0), // Num-Lock active
		((0x0001 & GetKeyState(VK_SCROLL)) != 0),  // Scroll-Lock active
	};
	return state;
}
#endif // ENABLE_KEYBOARD_AND_MOUSE

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if ((uMsg == WM_SYSCOMMAND && wParam == SC_SCREENSAVE) ||
		(uMsg == WM_SYSCOMMAND && wParam == SC_MONITORPOWER))
	{
		return 0;
	}

	if (uMsg == WM_CLOSE ||
		uMsg == WM_DESTROY ||
		(uMsg == WM_KEYDOWN && wParam == VK_ESCAPE))
	{
		PostQuitMessage(0);
		return 0;
	}

#if ENABLE_KEYBOARD_AND_MOUSE
	if (uMsg == WM_CHAR || uMsg == WM_SYSCHAR)
	{
		const KeyboardEvent event = KeyboardEvent::Character(wParam);
		s_inputState.keyboard = GetKeyboardState();
		for (int i = 0; i < MAX_INPUT_HANDLERS; ++i)
		{
			if (s_keyboardHandlers[i] != nullptr)
			{
				if (s_keyboardHandlers[i](s_handlerContexts[i], event, s_inputState))
				{
					break;
				}
			}
		}
		return 0;
	}

	if (uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN ||
		uMsg == WM_KEYUP || uMsg == WM_SYSKEYUP)
	{
		const KeyboardEvent event = KeyboardEvent::Key(GetKeyCode(wParam), (uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN));
		s_inputState.keyboard = GetKeyboardState();
		for (int i = 0; i < MAX_INPUT_HANDLERS; ++i)
		{
			if (s_keyboardHandlers[i] != nullptr)
			{
				if (s_keyboardHandlers[i](s_handlerContexts[i], event, s_inputState))
				{
					break;
				}
			}
		}
		return 0;
	}

	if (GetFocus() == hWnd)
	{
		POINT p;
		const BOOL getCursorPosResult = GetCursorPos(&p);
		ASSERT(getCursorPosResult);
		int wheel = 0;
		if (uMsg == WM_MOUSEWHEEL)
		{
			wheel = GET_WHEEL_DELTA_WPARAM(wParam);
		}
		int left = GetAsyncKeyState(VK_LBUTTON);
		int right = GetAsyncKeyState(VK_RBUTTON);
		int middle = GetAsyncKeyState(VK_MBUTTON);
		int x1 = GetAsyncKeyState(VK_XBUTTON1);
		int x2 = GetAsyncKeyState(VK_XBUTTON2);

		RECT windowArea;
		GetWindowRect(hWnd, &windowArea);
		MapWindowPoints(HWND_DESKTOP, GetParent(hWnd), (LPPOINT) &windowArea, 2);

		if (p.x >= windowArea.left && p.x <= windowArea.right &&
			p.y >= windowArea.top && p.y <= windowArea.bottom)
		{
			// Conversion from screen coordinates to window coordinates:
			// https://msdn.microsoft.com/en-us/library/windows/desktop/dd162952(v=vs.85).aspx
			const BOOL screenToClientResult = ScreenToClient(hWnd, &p);
			ASSERT(screenToClientResult);
			//p.x -= windowArea.left;
			//p.y -= windowArea.top;

			MouseState oldMouseState = s_inputState.mouse;
			s_inputState.mouse.x = p.x;
			s_inputState.mouse.y = p.y;
			s_inputState.mouse.wheel = wheel;
			s_inputState.mouse.left = (left != 0);
			s_inputState.mouse.right = (right != 0);
			s_inputState.mouse.middle = (middle != 0);
			s_inputState.mouse.x1 = (x1 != 0);
			s_inputState.mouse.x2 = (x2 != 0);

			s_inputState.keyboard = GetKeyboardState();

			MouseState event;
			event.x = (s_inputState.mouse.x - oldMouseState.x);
			event.y = (s_inputState.mouse.y - oldMouseState.y);
			event.wheel = s_inputState.mouse.wheel;
			event.left = (s_inputState.mouse.left != oldMouseState.left);
			event.right = (s_inputState.mouse.right != oldMouseState.right);
			event.middle = (s_inputState.mouse.middle != oldMouseState.middle);
			event.x1 = (s_inputState.mouse.x1 != oldMouseState.x1);
			event.x2 = (s_inputState.mouse.x2 != oldMouseState.x2);
			for (int i = 0; i < MAX_INPUT_HANDLERS; ++i)
			{
				if (s_mouseHandlers[i] != nullptr)
				{
					if (s_mouseHandlers[i](s_handlerContexts[i], event, s_inputState))
					{
						break;
					}
				}
			}
		}
	}
#endif // ENABLE_KEYBOARD_AND_MOUSE

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

Win32Platform::Win32Platform(const char* windowTitle,
							 int windowWidth, int windowHeight,
							 int monitorLeft, int monitorTop,
							 int monitorWidth, int monitorHeight,
							 bool fullscreen)
{
	g_platform = this;
#if ENABLE_PROFILING
	{
		QueryPerformanceFrequency((LARGE_INTEGER *)&m_highPrecisionCounterFrequency);
		m_highPrecisionCounterFrequency /= 1000000; // Convert from Hz to uHz.
	}
#endif // ENABLE_PROFILING

	DWORD			dwExStyle;
	DWORD			dwStyle;
	RECT			rec;
	WNDCLASSA		wc;

	m_hInstance = GetModuleHandle(0);

	memset(&wc, 0, sizeof(WNDCLASSA));
	wc.style			= CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
	wc.lpfnWndProc		= WndProc;
	wc.hInstance		= m_hInstance;
	wc.lpszClassName	= k_demoWindowClassName;

	if (!RegisterClass((WNDCLASSA*)&wc))
	{
		Debug::TerminateOnFatalError("Could not register window class.");
		return;
	}

	if (fullscreen)
	{
		dwExStyle	= WS_EX_APPWINDOW;
		dwStyle		= WS_VISIBLE | WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		ShowCursor(0);
		rec.left	= monitorLeft;
		rec.top		= monitorTop;
	}
	else
	{
		dwExStyle	= WS_EX_APPWINDOW;// | WS_EX_WINDOWEDGE;
		dwStyle		= WS_VISIBLE | WS_CAPTION | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU;
		rec.left	= monitorLeft + ((monitorWidth - windowWidth) >> 1);
		rec.top		= monitorTop + ((monitorHeight - windowHeight) >> 1);
	}
	rec.right	= rec.left + windowWidth;
	rec.bottom	= rec.top + windowHeight;

	AdjustWindowRect(&rec, dwStyle, 0);
	m_hWnd = CreateWindowEx(dwExStyle, wc.lpszClassName, wc.lpszClassName, dwStyle,
							rec.left, rec.top, rec.right - rec.left, rec.bottom - rec.top,
							0, 0, m_hInstance, 0);

	if (!m_hWnd)
	{
		Debug::TerminateOnFatalError("Could not create window.");
		return;
	}

	SetWindowTitle(windowTitle);
	SetForegroundWindow(m_hWnd);
	SetFocus(m_hWnd);

	static const PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		32,
		8, 0, 8, 0, 8, 0, 8, 0,
		0, 0, 0, 0, 0,
		32,             // zbuffer bpp
		0,              // stencil bpp
		0,
		PFD_MAIN_PLANE,
		0, 0, 0, 0
	};

	if (!(m_hDC = GetDC(m_hWnd)))
	{
		Debug::TerminateOnFatalError("Could not create drawing context.");
	}

	unsigned int PixelFormat;
	//wglChoosePixelFormatARB()
	if (!(PixelFormat = ChoosePixelFormat(m_hDC, &pfd)))
	{
		Debug::TerminateOnFatalError("Could not choose pixel format.");
	}

	if (!SetPixelFormat(m_hDC, PixelFormat, &pfd))
	{
		Debug::TerminateOnFatalError("Could not set pixel format.");
	}

	// In theory we should call wglCreateContextAttribsARB to open an
	// OpenGL 3.x context. However just calling wglCreateContext seems
	// to work for now.
	//
	// The spec apparently says:
	//     The legacy context creation routines can only return OpenGL
	//     3.1 contexts if the GL_ARB_compatibility extension is
	//     supported, and can only return OpenGL 3.2 or greater
	//     contexts implementing the compatibility profile. This
	//     ensures compatibility for existing applications. However,
	//     3.0-aware applications are encouraged to use
	//     wglCreateContextAttribsARB instead of the legacy routines.
	//
	// https://www.opengl.org/discussion_boards/showthread.php/182941-wglCreateContextAttribsARB-not-needed-anymore
	//
	//  * * *
	//
	// Also, we want the "compatibility profile" and not the "core
	// profile", because wglUseFontBitmaps returns fonts as display
	// lists, which is a legacy OpenGL feature.
	//
	// https://msdn.microsoft.com/en-us/library/windows/desktop/dd374392%28v=vs.85%29.aspx
	//
	//  * * *
	//
	// Finally, according to this slide from 2009, NVidia plans to stay
	// backward compatible. Their official statement has disappeared
	// since though.
	//
	// https://www.slideshare.net/Mark_Kilgard/opengl-32-and-more/36-Deprecation_NVIDIA_view_ulliWe_will
	// https://web.archive.org/web/20090826084131/http://developer.nvidia.com/object/opengl_3_driver.html
	//
	//  * * *
	//
	// Addendum: according to glGetString(GL_VERSION) it seems
	// requesting the "compatibility profile" always gets the newest
	// version, while "core profile" gets exactly the requested
	// version.

	m_hRC = wglCreateContext(m_hDC);

	// For reference, here is the kind of code necessary to request a
	// compatibility profile:
#if 0
	wglMakeCurrent(m_hDC, m_hRC);
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	if (wglCreateContextAttribsARB)
	{
		wglMakeCurrent(0, 0);
		wglDeleteContext(m_hRC);

		int attribList[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
			WGL_CONTEXT_MINOR_VERSION_ARB, 6,
#if DEBUG
			WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB | WGL_CONTEXT_DEBUG_BIT_ARB,
#endif
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,//WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
			0
		};
		m_hRC = wglCreateContextAttribsARB(m_hDC, 0, attribList);
	}
#endif

	if (!m_hRC)
	{
		Debug::TerminateOnFatalError("Could not create OpenGL rendering context.");
	}

	// Vertical sync, if available
	PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
	if (wglSwapIntervalEXT)
	{
		wglSwapIntervalEXT(1);
	}

	if (!wglMakeCurrent(m_hDC, m_hRC))
	{
		Debug::TerminateOnFatalError("Could not use rendering context.");
	}

#if DEBUG
	int vMajor = 0;
	int vMinor = 0;
	glGetIntegerv(GL_MAJOR_VERSION, &vMajor);
	glGetIntegerv(GL_MINOR_VERSION, &vMinor);
	LOG_INFO("OpenGL %d.%d context:", vMajor, vMinor);
	LOG_INFO("Vendor    : %s", (char*)glGetString(GL_VENDOR));
	LOG_INFO("Renderer  : %s", (char*)glGetString(GL_RENDERER));
	LOG_INFO("Version   : %s", (char*)glGetString(GL_VERSION));
	LOG_INFO("Extensions: %s", (char*)glGetString(GL_EXTENSIONS));
#endif // DEBUG
}

Win32Platform::~Win32Platform()
{
	if (m_hDC)
	{
		wglMakeCurrent(0, 0);
		wglDeleteContext(m_hRC);
		ReleaseDC(m_hWnd, m_hDC);
	}

	if (m_hWnd)
	{
		DestroyWindow(m_hWnd);
	}

	UnregisterClass(k_demoWindowClassName, m_hInstance);

	// Restore display settings and mouse cursor in case we changed them.
	ChangeDisplaySettings(0, 0);
	ShowCursor(1);
}

long Win32Platform::GetTime() const
{
	// https://docs.microsoft.com/en-us/windows/win32/api/timeapi/nf-timeapi-timegettime
	//
	// The timeGetTime function retrieves the system time, in
	// milliseconds. The system time is the time elapsed since Windows
	// was started.
	//
	// Note that the value returned by the timeGetTime function is a
	// DWORD value. The return value wraps around to 0 every 2^32
	// milliseconds, which is about 49.71 days. This can cause problems
	// in code that directly uses the timeGetTime return value in
	// computations, particularly where the value is used to control
	// code execution. You should always use the difference between two
	// timeGetTime return values in computations.

#if 0
	// The default precision of the timeGetTime function can be five
	// milliseconds or more, depending on the machine. You can use the
	// timeBeginPeriod and timeEndPeriod functions to increase the
	// precision of timeGetTime. If you do so, the minimum difference
	// between successive values returned by timeGetTime can be as
	// large as the minimum period value set using timeBeginPeriod and
	// timeEndPeriod.

	timeBeginPeriod(1);
	long time = timeGetTime();
	timeEndPeriod(1);

	return time;
#else
	return timeGetTime();
#endif
}

#if ENABLE_PROFILING
long long Win32Platform::GetPreciseTime() const
{
	long long result;
	QueryPerformanceCounter((LARGE_INTEGER*)&(result));
	return result / m_highPrecisionCounterFrequency;
}
#endif // ENABLE_PROFILING

void Win32Platform::Sleep(long milliseconds) const
{
	::Sleep(milliseconds);
}

void Win32Platform::SetWindowTitle(const char* title) const
{
	SetWindowText(m_hWnd, title);
}

#if ENABLE_KEYBOARD_AND_MOUSE
void Win32Platform::AddInputHandler(KeyboardHandler keyboardHandler,
									MouseHandler mouseHandler,
									void* handlerContext)
{
	ASSERT(s_nextHandler < MAX_INPUT_HANDLERS);
	s_keyboardHandlers[s_nextHandler] = keyboardHandler;
	s_mouseHandlers[s_nextHandler] = mouseHandler;
	s_handlerContexts[s_nextHandler] = handlerContext;
	++s_nextHandler;
}
#endif // ENABLE_KEYBOARD_AND_MOUSE

bool Win32Platform::HandleMessages() const
{
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return false;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return true;
}

void Win32Platform::SwapBuffers() const
{
	::SwapBuffers(m_hDC);
}

void Win32Platform::GetMonitorRect(int* out_left, int* out_top, int* out_width, int* out_height)
{
	*out_left = 0;
	*out_top = 0;
	*out_width = GetSystemMetrics(SM_CXSCREEN);
	*out_height = GetSystemMetrics(SM_CYSCREEN);
}
