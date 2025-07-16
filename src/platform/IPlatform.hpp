#pragma once

#include "InputEvents.hpp"
#include "gfx/IGraphicLayer.hpp"

namespace platform
{
	/// <summary>
	/// Keyboard input callback.
	/// Returns true if the event is processed and should not be passed
	/// through, false otherwise.
	/// </summary>
	typedef bool (*KeyboardHandler)(void* handlerContext, KeyboardEvent event, const InputState& state);

	/// <summary>
	/// Mouse input callback.
	/// Returns true if the event is processed and should not be passed
	/// through, false otherwise.
	/// </summary>
	typedef bool (*MouseHandler)(void* handlerContext, const MouseState& event, const InputState& state);

	//
	// Unused.
	// Just here to show what a platform class is expected to implement.
	//

#if 0
	class IPlatform
	{
	public:
		virtual		~IPlatform() {}

		long		GetTime() const;
#if ENABLE_PROFILING
		long long	GetPreciseTime() const;
#endif // ENABLE_PROFILING
		void		Sleep(long milliseconds) const;

		void		SetWindowTitle(const char* title) const = 0;
		void		AddInputHandler(CharacterHandler characterHandler,
									KeyboardHandler keyboardHandler,
									MouseHandler mouseHandler,
									void* handlerContext) = 0;
		bool		HandleMessages() const = 0;
		void		SwapBuffers() const = 0;

		static void	GetMonitorRect(int* out_left, int* out_top, int* out_width, int* out_height);
	};
#endif
}
