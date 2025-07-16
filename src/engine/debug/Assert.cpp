#include "Assert.hpp"
#include "Debug.hpp"
#include "engine/core/StringUtils.hpp"

using namespace Debug;

#if DEBUG

#ifdef _WIN32 // Windows
#include <windows.h>

bool Assert::ShouldBreakOnAssert()
{
	return IsDebuggerPresent() != FALSE;
}

void Assert::FailAssertBeforeBreak(const char* expression, const char* fileName, int line)
{
	LOG_ERROR("Assertion failed.");
	LOG_RAW("%s(%d): (%s) is false.\n", fileName, line, expression);
}

void Assert::FailAssertAndDie(const char* expression, const char* fileName, int line)
{
	LOG_ERROR("Assertion failed.");
	LOG_RAW("%s(%d): (%s) is false.\n", fileName, line, expression);

#define BUFFER_SIZE 2048
	char errorMessage[BUFFER_SIZE];
	_snprintf(errorMessage, BUFFER_SIZE, "Assertion failed in file:\n%s\nline %d\nExpression: (%s)",
	          fileName, line, expression);
	MessageBox(nullptr, errorMessage, "Assertion failed", MB_OK | MB_ICONERROR);
#undef BUFFER_SIZE

	// Using ExitProcess() instead of exit(), which requires libc.
	ExitProcess(2);
}

#else // Linux

bool Assert::ShouldBreakOnAssert()
{
	return false;
}

void Assert::FailAssertBeforeBreak(const char* expression, const char* fileName, int line)
{
	LOG_ERROR("Assertion failed.");
	LOG_RAW("%s(%d): (%s) is false.\n", fileName, line, expression);
}

void Assert::FailAssertAndDie(const char* expression, const char* fileName, int line)
{
	LOG_ERROR("Assertion failed.");
	LOG_RAW("%s(%d): (%s) is false.\n", fileName, line, expression);

	exit(2);
}

#endif // !_WIN32
#endif // DEBUG
