#include "StringUtils.hpp"

#include "engine/debug/Assert.hpp"
#include "engine/debug/Debug.hpp"

#if ENABLE_EDITING
#include <algorithm>
#include <cstring>
#include <sstream>
#endif

#if ENABLE_EDITING

std::string& Core::RemoveBackSlashNewlines(std::string& str)
{
	size_t pos = 0;
	while ((pos = str.find("\\\n", pos)) != std::string::npos)
	{
		str.erase(pos, 2);
	}

	pos = 0;
	while ((pos = str.find("\\\r\n", pos)) != std::string::npos)
	{
		str.erase(pos, 3);
	}
	return str;
}

std::string& Core::RemoveComments(std::string& str)
{
	size_t pos = 0;
	while (true)
	{
		size_t singleLineComment = str.find("//", pos);
		size_t multiLineComment = str.find("/*", pos);

		if (multiLineComment < singleLineComment)
		{
			pos = multiLineComment;
			size_t end = str.find("*/", pos + 2);
			if (end != std::string::npos)
			{
				str.replace(pos, end - pos + 2, " ");
				continue;
			}
		}
		else if (singleLineComment != std::string::npos)
		{
			pos = singleLineComment;

			// Ignore ShaderMinifier directives
			if (str.compare(pos, 3, "//[") == 0 ||
				str.compare(pos, 3, "//]") == 0)
			{
				pos += 3;
				continue;
			}

			size_t end = str.find_first_of("\r\n", pos + 2);
			if (end != std::string::npos)
			{
				str.erase(pos, end - pos);
				continue;
			}
		}
		break;
	}
	return str;
}

std::string& Core::ReplaceAll(std::string& str, const char* pattern, const char* replace)
{
	const size_t patternLength = strlen(pattern);
	const size_t replaceLength = strlen(replace);

	if (patternLength > 0)
	{
		size_t pos = 0;
		while ((pos = str.find(pattern, pos)) != std::string::npos)
		{
			str.replace(pos, patternLength, replace);
			pos += replaceLength;
		}
	}
	return str;
}

std::string& Core::ReplaceIdentifier(std::string& str, const char* identifier, const char* replace)
{
	const size_t identifierLength = strlen(identifier);
	const size_t replaceLength = strlen(replace);

	if (identifierLength > 0)
	{
		size_t pos = 0;
		while ((pos = str.find(identifier, pos)) != std::string::npos)
		{
			if ((pos == 0 ||
				(!isalnum(str[pos - 1]) &&
				str[pos - 1] != '.')) && // don't replace "x" in "v.x"
				!isalnum(str[pos + identifierLength])) // don't replace "texture2" in "texture2D"
			{
				str.replace(pos, identifierLength, replace);
				pos += replaceLength;
			}
			else
			{
				pos += identifierLength;
			}
		}
	}
	return str;
}

std::string Core::ArrayToString(int* array, int count, int* highlight, int highlightCount)
{
	std::stringstream cppIsUgly;
	for (int i = 0; i < count; ++i)
	{
		bool braces = false;
		for (int j = 0; j < highlightCount; ++j)
		{
			if (i == highlight[j])
			{
				braces = true;
				break;
			}
		}

		if (braces)
		{
			cppIsUgly << "[";
		}
		cppIsUgly << array[i];
		if (braces)
		{
			cppIsUgly << "]";
		}
		if (i < count - 1)
		{
			cppIsUgly << ", ";
		}
	}
	return cppIsUgly.str();
}

#endif // ENABLE_EDITING

void Core::reverse(char str[], int length)
{
	int start = 0;
	int end = length -1;
	while (start < end)
	{
		char tmp = str[start];
		str[start] = str[end];
		str[end] = tmp;
		start++;
		end--;
	}
}

char* Core::itoa(int num, char* str)
{
	ASSERT(num >= 0);

	int i = 0;
	if (num == 0)
	{
		str[i++] = '0';
		str[i] = '\0';
		return str;
	}

	while (num != 0)
	{
		char rem = (char)(num % 10);
		str[i++] = rem + '0';
		num = num / 10;
	}
	str[i] = '\0';
	reverse(str, i);
	return str + i;
}
