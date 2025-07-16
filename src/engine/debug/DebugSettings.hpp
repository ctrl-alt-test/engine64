#pragma once

#include "engine/container/HashTable.hpp"

namespace Debug
{
#if DEBUG
#define DEBUG_BOOLEAN(settings, name) ((settings).debug.Boolean(name))

	struct DebugSettings {
		DebugSettings();

		// Pointers so it won't affect Settings' constness.
		Container::HashTable<const char*, bool>* booleans;
		Container::HashTable<const char*, int>* integers;

		bool Boolean(const char* settingName) const;
		int Integer(const char* settingName) const;
	};
#else // !DEBUG
#define DEBUG_BOOLEAN(settings, name) (false)
#endif // !DEBUG
}
