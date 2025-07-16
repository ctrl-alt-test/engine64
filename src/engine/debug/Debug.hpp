#pragma once

#include "engine/EngineConfig.hpp"

#if _HAS_EXCEPTIONS
#include <stdexcept>
#endif // _HAS_EXCEPTIONS

#include "Log.hpp"

namespace Debug
{
	/// <summary>
	/// Displays the text one way or another, and terminates the program.
	/// </summary>
	void TerminateOnFatalError(const char* text);

/// <summary>
/// DO_ONCE will execute a given expression, while making it behave like
/// an isolated block at the calling site, to avoid unintended code
/// branches during macro expansion.
/// </summary>
///
/// <remarks>
/// So far I haven't found a convenient way to make it work with
/// variadic arguments (__VA_ARGS__).
/// </remarks>
#define DO_ONCE(expression) \
	do {                    \
		expression;         \
	} while (0)

/// <summary>
/// UNUSED_EXPR will tell the compiler not to warn about a given
/// variable being unused. "yeah, we know - this is unused."
/// </summary>
///
/// <remarks>
/// the internet says that (void)sizeof(expr) is the right way to do
/// this, but not for us, not with our compilers. the below is the
/// result of much experimentation by @lucas, who says that we have at
/// least one compiler that does not consider sizeof(expr) to be a
/// 'usage' of the variable(s) inside of expr.
///
/// also note that we do not have the 'if+const expr' warning enabled
/// because combining #if and if expression/constants (which we often
/// need to do - for example
/// 'caps->gles.requireClearAlpha = UNITY_WEBGL || UNITY_STV') is super
/// noisy.
///
/// This macro and its documentation are copied verbatim after a code
/// snippet posted by Unity's Aras Pranckevicius on Twitter.
/// https://twitter.com/aras_p/status/794952299240181760
/// </remarks>
#define UNUSED_EXPR(expression) DO_ONCE(if (false) { (void)(expression); })

#ifndef _WIN32
#define MAYBE_UNUSED __attribute__((unused))
#else
#define MAYBE_UNUSED
#endif

#if ENABLE_LOG
	extern Log log;
	// Due to variadic arguments, the while loop is written explicitely
	// instead of using DO_ONCE.
#define LOG(level, format, ...)                         \
	do {                                                \
		::Debug::log.Add(level, format, ##__VA_ARGS__); \
	} while (0)
#else // !ENABLE_LOG
#define LOG(level, format, ...)
#endif // !ENABLE_LOG

#if DEBUG
#define LOG_DEBUG(format, ...) LOG(::Debug::LogLevel::Debug, format, ##__VA_ARGS__)
#else // !DEBUG
#define LOG_DEBUG(format, ...)
#endif // !DEBUG

#define LOG_RAW(format, ...) LOG(::Debug::LogLevel::Raw, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...) LOG(::Debug::LogLevel::Info, format, ##__VA_ARGS__)
#define LOG_WARNING(format, ...) LOG(::Debug::LogLevel::Warning, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) LOG(::Debug::LogLevel::Error, format, ##__VA_ARGS__)
#define LOG_FATAL(format, ...) LOG(::Debug::LogLevel::Fatal, format, ##__VA_ARGS__)

#if _HAS_EXCEPTIONS
#define NOT_IMPLEMENTED DO_ONCE(throw new ::Debug::NotImplementedException())
	class NotImplementedException: public std::logic_error
	{
	public:
		NotImplementedException();
	};
#else // !_HAS_EXCEPTIONS
#define NOT_IMPLEMENTED
#endif // !_HAS_EXCEPTIONS

	// FIXME: unify API between REPORT_ERROR and TerminateOnFatalError.	
#if _HAS_EXCEPTIONS
#define REPORT_ERROR(errorMessage) DO_ONCE(throw std::runtime_error(errorMessage))
#else
#define REPORT_ERROR(errorMessage) DO_ONCE(LOG_ERROR(errorMessage.c_str()))
#endif
}
