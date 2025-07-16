#pragma once

#include "IPlatform.hpp"
#include <windows.h>

namespace platform
{
	extern class Win32Platform* g_platform;

	/// <summary>
	/// Wrapper abstracting the boiler plate necessary for window and
	/// draw context creation.
	/// </summary>
	class Win32Platform // : public IPlatform
	{
	public:
		/// <param name="windowTitle">Text visible in the title bar of the window.</param>
		/// <param name="width">Horizontal resolution.</param>
		/// <param name="height">Vertical resolution.</param>
		Win32Platform(const char* windowTitle,
					  int windowWidth, int windowHeight,
					  int monitorLeft, int monitorTop,
					  int monitorWidth, int monitorHeight,
					  bool fullscreen);
		~Win32Platform();

		/// <summary>
		/// Get the system time, in milliseconds.
		/// </summary>
		long		GetTime() const;

#if ENABLE_PROFILING
		/// <summary>
		/// Get the system time with sub millisecond precision, in
		/// microseconds.
		/// </summary>
		//
		// The windows API requires two function calls to get the
		// precise timing. The frequency needs to be queried only once
		// though. Since we want to query the timer including from low
		// level functions which we don't want to pass a state to, I
		// think the most satisfying solution is to store the Platform
		// instance in a global.
		long long	GetPreciseTime() const;
#endif // ENABLE_PROFILING

		/// <summary>
		/// Sleep for a given duration, in milliseconds.
		/// </summary>
		void		Sleep(long milliseconds) const;

		void		SetWindowTitle(const char* title) const;
		void		AddInputHandler(KeyboardHandler keyboardHandler,
									MouseHandler mouseHandler,
									void* handlerContext);
		bool		HandleMessages() const;
		void		SwapBuffers() const;

		static void	GetMonitorRect(int* out_left, int* out_top, int* out_width, int* out_height);

	private:
#if ENABLE_PROFILING
		long long	m_highPrecisionCounterFrequency;
#endif // ENABLE_PROFILING

		HINSTANCE	m_hInstance;
		HWND		m_hWnd;

		// Set as public as a temporary hack, until we figure a better
		// abstraction.
	public:
		HDC			m_hDC;

	private:
		HGLRC		m_hRC;
	};
}
