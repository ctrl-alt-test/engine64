#pragma once

/// <summary>
/// Number of elements of a statically declared const array.
/// </summary>
#define ARRAY_LEN(array) static_cast<int>(sizeof(array) / sizeof(array[0]))
