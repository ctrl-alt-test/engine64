#include "DebugSettings.hpp"

#include "engine/container/HashTable.hxx"

using namespace Debug;

#if DEBUG
DebugSettings::DebugSettings()
{
	booleans = new Container::HashTable<const char*, bool>(1024);
	integers = new Container::HashTable<const char*, int>(1024);
}

bool DebugSettings::Boolean(const char* settingName) const
{
	bool* p = (*booleans)[settingName];
	if (p == nullptr) {
		p = booleans->add(settingName, false);
	}
	return *p;
}

int DebugSettings::Integer(const char* settingName) const
{
	int* p = (*integers)[settingName];
	if (p == nullptr) {
		p = integers->add(settingName, 0);
	}
	return *p;
}
#endif // DEBUG
