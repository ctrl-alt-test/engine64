#pragma once

#if DEBUG
namespace Debug
{
	struct Assert {
		/// <summary>
		/// Returns true if a debugger is attached to the program
		/// (meaning that a breakpoint should be used on failed
		/// assertion), false otherwise.
		/// </summary>
		static bool ShouldBreakOnAssert();

		/// <summary>
		/// Displays a failed assertion message, but doesn't break. The
		/// breakpoint is expected to be raised in a macro, so the
		/// debugger shows the ASSERT location.
		/// </summary>
		static void FailAssertBeforeBreak(const char* expression, const char* fileName, int line);

		/// <summary>
		/// Displays a failed assertion message, and abruptly
		/// terminates the program.
		/// </summary>
		static void FailAssertAndDie(const char* expression, const char* fileName, int line);
	};
}

//
// The reason for defining the code below with macros is to have the
// breakpoint inserted directly at the location of the assertion. This
// way, when the assertion fails, the debugger will show the code we
// actually want to see, and not the ASSERT() implementation details.
// Abstracting this code with function would defeat that purpose.
//
// This implementation is inspired by a discussion on Twitter involving
// Tom Forsyth and Fabian Giesen:
// https://twitter.com/tom_forsyth/status/1058269257530667008
//
// Tom: Why is the default C assert so heavyweight? Use this one:
//      #define ASSERT(exp) { if (!(exp)){_asm { int 3 };} }
// ryg: FWIW: with VC++, never use "asm { int 3; }", just use
//      __debugbreak(); (works in 64-bit too).
//      ARM A32/T32: __asm__("bkpt #0")
//      ARM A64: __asm__("brk #0")
//

#define ASSERT(exp)             \
	do {                        \
		if (!(exp)) {           \
			ASSERT_FAILED(exp); \
		}                       \
	} while (false)

#define ASSERT_ONCE(exp)             \
	do {                             \
		static bool already = false; \
		if (!already && !(exp)) {    \
			already = true;          \
			ASSERT_FAILED(exp);      \
		}                            \
	} while (false)

#define ASSERT_FAILED(exp)                                                \
	if (::Debug::Assert::ShouldBreakOnAssert()) {                         \
		::Debug::Assert::FailAssertBeforeBreak(#exp, __FILE__, __LINE__); \
		ASSERT_BREAKPOINT;                                                \
	} else {                                                              \
		::Debug::Assert::FailAssertAndDie(#exp, __FILE__, __LINE__);      \
	}

#ifdef _WIN32
void __debugbreak(); // Defined when including <windows.h>
#define ASSERT_BREAKPOINT __debugbreak()
#else
#define ASSERT_BREAKPOINT
#endif

#else // !DEBUG
#define ASSERT(exp)
#define ASSERT_ONCE(exp)
#endif // !DEBUG
