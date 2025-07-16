#pragma once

#include <cstdarg>

namespace Debug
{
	enum class LogLevel {
		Raw,
		Debug,
		Info,
		Warning,
		Error,
		Fatal, // should be last
	};

	class Log
	{
	public:
		Log();

		/// <summary>
		/// Adds an entry to the log.
		/// The reason for using "Add" instead of something like "Print"
		/// is so output can be delayed, and some entries can be discarded
		/// on certain conditions.
		/// </summary>
		void Add(LogLevel level, const char* format, ...);

		/// <summary>
		/// Adds an entry to the log.
		/// </summary>
		void AddList(LogLevel level, const char* format, va_list argList);

		// Returns the number of logs at the given level.
		int Count(LogLevel level) const { return count[static_cast<int>(level)]; }

		static const int LogLevelCount = static_cast<int>(LogLevel::Fatal) + 1;

	private:
		int count[LogLevelCount];
	};
}
