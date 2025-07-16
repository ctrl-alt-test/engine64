#include "Log.hpp"
#include "engine/debug/Assert.hpp"
#include <cstdio>
#include <cstring>
#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

//
// 1 to display log on debugger display,
// 0 for standard output.
//
// The debugger display appears in Visual Studio,
// or can also be captured by DebugView.
//
#define OUTPUT_TO_DEBUGGER_DISPLAY 1

using namespace Debug;

static const char* k_levelHeaders[] = {
	nullptr, // No header for raw format
	"DEBUG", "INFO", "WARNING", "ERROR", "FATAL",
};

Log::Log()
{
	ASSERT(sizeof(k_levelHeaders) / sizeof(k_levelHeaders[0]) == LogLevelCount);
	for (int i = 0; i < LogLevelCount; ++i) {
		count[i] = 0;
	}
}

void Log::Add(LogLevel level, const char* format, ...)
{
	va_list argList;
	va_start(argList, format);
	AddList(level, format, argList);
	va_end(argList);
}

void Log::AddList(LogLevel level, const char* format, va_list argList)
{
	count[static_cast<int>(level)]++;

#if defined(_WIN32) && OUTPUT_TO_DEBUGGER_DISPLAY

	const int maxLength = 2048;
	char buffer[maxLength] = { 0 };
	char* p = buffer;

	if (k_levelHeaders[static_cast<int>(level)] != nullptr) {
		p += sprintf(p, "%s: ", k_levelHeaders[static_cast<int>(level)]);
		size_t count = maxLength - (p - buffer);
		int n = vsnprintf(p, count, format, argList);
		if (n < (int)count) {
			p += n;
			if (p - buffer == 0 || p[-1] != '\n') {
				// Add a new line if the string doesn't already include one
				strcpy(p, "\n");
			}
		} else {
			// Note: if it's longer than the available buffer, the text
			// is truncated.
			strcpy(buffer + maxLength - 7, "[...]\n");
		}
	} else {
		p += vsnprintf(p, maxLength, format, argList);
	}
	OutputDebugString(buffer);

#else // !defined(_WIN32) || !OUTPUT_TO_DEBUGGER_DISPLAY

	if (level != LogLevel::Raw) {
		printf("%s: ", k_levelHeaders[level]);
		vprintf(format, argList);
		int len = strlen(format);
		if (len == 0 || format[len - 1] != '\n') {
			printf("\n");
		}
	} else {
		vprintf(format, argList);
	}

#endif // !defined(_WIN32) || !OUTPUT_TO_DEBUGGER_DISPLAY
}
