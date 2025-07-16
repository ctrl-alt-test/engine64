//
// Win32Dialog.hpp for demotest
// Made by nicuveo <crucuny@gmail.com>
//

#pragma once

#include "engine/container/Array.hpp"
#include "engine/core/Settings.hpp"
#include <windows.h>

namespace platform
{
	class Dialog
	{
	public:
		Dialog();

		void run(const char* title);

	public:
		bool			isOk;
		bool			fullscreen;
		int				renderWidth;
		int				renderHeight;
		int				monitorLeft;
		int				monitorTop;
		int				monitorWidth;
		int				monitorHeight;
		int				dpi;

		static Dialog*	s_instance;

	private:
		Dialog(const Dialog&);
		Dialog& operator = (const Dialog&) = delete;

		HWND CreateButton(const char* name,
						  int x, int y,
						  int* id);
		HWND CreateCheckBox(const char* name,
							int x, int y,
							bool isChecked,
							int* id);
		HWND CreateSelectionMenu(const char* label,
								 int x, int y,
								 const char** items,
								 int numberOfItems,
								 int selectedDefault);

		static LRESULT CALLBACK input(HWND h, UINT msg, WPARAM wp, LPARAM lp);

		HINSTANCE	m_hInstance;
		HWND		m_dialogHandle;
		HFONT		m_fontHandle;

		HWND		m_combobox_resolutions;
		HWND		m_checkbox_fullscreen;
		HWND		m_button_ok;

		int			m_id_checkbox_fullscreen;
		int			m_id_button_ok;

		int			m_ids;
	
#if ENABLE_QUALITY_OPTION
	public:
		int			quality;
	private:
		HWND		m_combobox_qualities;
#endif // ENABLE_QUALITY_OPTION
	};
}
