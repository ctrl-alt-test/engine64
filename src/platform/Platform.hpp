#pragma once

#ifdef _WIN32

#include "Win32Platform.hpp"

namespace platform
{
	typedef Win32Platform Platform;
}

#elif LINUX

#include "XLibPlatform.hpp"

namespace platform
{
	typedef XLibPlatform Platform;
}

#else // !(_WIN32 || LINUX)

NOT_IMPLEMENTED;

#endif // !(_WIN32 || LINUX)
