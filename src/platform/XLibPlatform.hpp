#ifndef XLIB_PLATFORM_HPP
#define XLIB_PLATFORM_HPP

#include "IPlatform.hpp"

#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__) || defined(__MINGW32__)
#include <windef.h>
#endif // _WIN32
#include <GLFW/glfw3.h>


namespace platform
{
	extern class XLibPlatform* g_platform;

	class XLibPlatform // : public IPlatform
	{
	public:
		/// <param name="windowTitle">Text visible in the title bar of the window.</param>
		/// <param name="width">Horizontal resolution.</param>
		/// <param name="height">Vertical resolution.</param>
		XLibPlatform(const char* windowTitle,
					 int width, int height,
					 int monitorLeft, int monitorTop,
					 int monitorWidth, int monitorHeight,
					 bool fullscreen);
		~XLibPlatform();

		/// <summary>
		/// Get the system time, in milliseconds.
		/// </summary>
		long		GetTime() const;

#ifdef ENABLE_PROFILING
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

		bool		HandleMessages() const;
		void		SwapBuffers() const;

		void        NotifyClose() const;

		static void	GetMonitorRect(int* out_left, int* out_top, int* out_width, int* out_height);

#ifdef ENABLE_KEYBOARD_AND_MOUSE

		void		AddInputHandler(CharacterHandler characterHandler,
									KeyHandler keyHandler,
									MouseHandler mouseHandler,
									void* handlerContext);

#endif // ENABLE_KEYBOARD_AND_MOUSE

	private:
		GLFWwindow* m_window;
	};
}

#endif // XLIB_PLATFORM_HPP
