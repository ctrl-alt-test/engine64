#pragma once

#include "engine/EngineConfig.hpp"
#if ENABLE_EDITING
#include <string>
#include <cstring>
#endif // ENABLE_EDITING

namespace Core
{
#if ENABLE_EDITING
	std::string&	RemoveBackSlashNewlines(std::string& str);
	std::string&	RemoveComments(std::string& str);
	std::string&	ReplaceAll(std::string& str, const char* pattern, const char* replace);
	std::string&	ReplaceIdentifier(std::string& str, const char* identifier, const char* replace);
	std::string		ArrayToString(int* array, int count, int* highlight = nullptr, int highlightCount = 0);
#endif // ENABLE_EDITING

	void reverse(char str[], int length);

	/// <summary>
	/// Writes num into str in base 10.
	/// </summary>
	///
	/// <returns>The position _after_ the number.</returns>
	char* itoa(int num, char* str);
}
