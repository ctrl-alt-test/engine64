#pragma once

#if _WIN32
#include <Windows.h>

namespace platform
{
	typedef HANDLE ThreadData;
	typedef void (__cdecl *ThreadFunc)(void*);
	typedef void* ThreadArg;

	struct MultiThreading
	{
		static int GetNumberOfCores();
		static void StartThread(ThreadData* threadData, ThreadFunc func, ThreadArg arg);
		static void WaitAllThreads(const ThreadData* threads, int numberOfThreads);
	};
}

#else // !_WIN32
#include <pthread.h>

namespace platform
{
	typedef pthread_t ThreadData;
	typedef void (*ThreadFunc)(void*);
	typedef void* ThreadArg;

	struct MultiThreading
	{
		static int GetNumberOfCores();
		static void StartThread(ThreadData* threadData, ThreadFunc func, ThreadArg arg);
		static void WaitAllThreads(const ThreadData* threads, int numberOfThreads);
	};
}

#endif // !_WIN32
