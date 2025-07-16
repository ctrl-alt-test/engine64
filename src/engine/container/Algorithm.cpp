#include "Algorithm.hxx"
#include <cstring>

using namespace Container;

int Container::fltcmp(const float* fptr1, const float* fptr2, size_t n)
{
	if (!n)
		return 0;
	for (size_t i = 0; i < n; ++i)
	{
		if (fptr1[i] != fptr2[i])
			return (fptr1[i] != fptr2[i]);
	}
	return 0;
}

// Specialization for arrays of strings.
template<>
int Container::find(const Array<const char*>& array, const char* const& item)
{
	for (int i = 0; i < array.size; ++i)
	{
		if (strcmp(item, array[i]) == 0)
		{
			return i;
		}
	}
	return -1;
}

int Container::find_lowLevel(const void* array, const void* item, size_t sizeOfType, int count)
{
	for (int i = 0; i < count; ++i)
	{
		if (memcmp((char*)array + i * sizeOfType, item, sizeOfType) == 0)
		{
			return i;
		}
	}
	return -1;
}
