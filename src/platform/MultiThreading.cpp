#include "MultiThreading.hpp"

#include "engine/core/msys_temp.hpp"
#include <cstring>

using namespace platform;

#if _WIN32

int MultiThreading::GetNumberOfCores()
{
	static int s_numberOfCores = -1;
	if (s_numberOfCores > 0)
	{
		return s_numberOfCores;
	}

	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	s_numberOfCores = sysinfo.dwNumberOfProcessors;
	return s_numberOfCores;
}

void MultiThreading::StartThread(ThreadData* threadData, ThreadFunc func, ThreadArg arg)
{
	*threadData = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)func, arg, 0, nullptr);
}

void MultiThreading::WaitAllThreads(const ThreadData* threads, int numberOfThreads)
{
	WaitForMultipleObjects(numberOfThreads, threads, TRUE, INFINITE);
}

#else // !_WIN32
#include <unistd.h>

//
// FIXME: this probably doesn't work because I have no idea what I'm
// doing. At all.
//

int MultiThreading::GetNumberOfCores()
{
	static int s_numberOfCores = -1;
	if (s_numberOfCores > 0)
	{
		return s_numberOfCores;
	}

	s_numberOfCores = sysconf(_SC_NPROCESSORS_ONLN);
	return s_numberOfCores;
}

void MultiThreading::StartThread(ThreadData* threadData, ThreadFunc func, ThreadArg arg)
{
	pthread_create(threadData, nullptr, (void*(*)(void*))func, arg);
}

void MultiThreading::WaitAllThreads(const ThreadData* threads, int numberOfThreads)
{
	for (int i = 0; i < numberOfThreads; ++i)
	{
		pthread_join(threads[i], nullptr);
	}
}

#endif // !_WIN32
