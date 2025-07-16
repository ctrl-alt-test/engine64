#pragma once

#include "Algorithm.hpp"
#include "Array.hxx"
#include "engine/debug/Assert.hpp"
#include "engine/noise/Rand.hpp"
#include <cstring>

#include "engine/core/StringUtils.hpp"
#include "engine/debug/Debug.hpp"

namespace Container
{
	template<typename T>
	bool defaultCompare(const T& lhs, const T& rhs, const void* state)
	{
		return lhs > rhs;
	}

	template<typename T>
	void swap(T& a, T& b)
	{
		T temp = a;
		a = b;
		b = temp;
	}

	template<typename T>
	inline
	int find(const Array<T>& array, const T& item)
	{
		return find_lowLevel((const void*)array.elt, (const void*)&item, sizeof(T), array.size);
	}

	template<typename T>
	int binarySearch(const Array<T>& array, const T& item)
	{
		return binarySearch(array.elt, item, array.size);
	}

	template<typename T>
	int binarySearch(const T* array, const T& item, int count)
	{
		int first = 0;
		int last = count - 1;
		while (first <= last)
		{
			ASSERT(!(array[first] > array[last]));

			const int med = (first + last) / 2;
			if (array[med] >= item)
			{
				last = med - 1;
			}
			else
			{
				first = med + 1;
			}
		}
		return first;
	}

	template<typename T>
	void shuffle(Noise::Rand& rand, Array<T>& array)
	{
		shuffle(rand, array.elt, array.size);
	}

	template<typename T>
	void shuffle(Noise::Rand& rand, T* array, int count)
	{
		// Fisher-Yates / Knuth algorithm.
		for (int i = count - 1; i > 0; --i)
		{
			swap(array[i], array[rand.igen(i)]);
		}
	}

	template<typename T>
	bool isSorted(const Array<T>& array, compareFunction<T> compare, const void* state)
	{
		return isSorted(array.elt, array.size, compare, state);
	}

	template<typename T>
	bool isSorted(const T* array, int count, compareFunction<T> compare, const void* state)
	{
		for (int i = 0; i < count - 1; ++i)
		{
			if (compare(array[i], array[i + 1], state))
			{
				return false;
			}
		}
		return true;
	}

	template<typename T>
	void bubbleSort(Array<T>& array, compareFunction<T> compare, const void* state)
	{
		bubbleSort(array.elt, array.size, compare, state);
	}

	template<typename T>
	void bubbleSort(T* array, int count, compareFunction<T> compare, const void* state)
	{
		ASSERT(count > 0);

		for (int i = count - 1; i > 0; --i)
		{
			bool swapped = false;

			for (int j = 0; j < i; ++j)
			{
				if (compare(array[j], array[j + 1], state))
				{
					swap(array[j], array[j + 1]);
					swapped = true;
				}
			}

			if (!swapped)
			{
				break;
			}
		}
	}

	template<typename T>
	void insertionSort(Array<T>& array, compareFunction<T> compare, const void* state)
	{
		insertionSort(array.elt, array.size, compare, state);
	}

	template<typename T>
	void insertionSort(T* array, int count, compareFunction<T> compare, const void* state)
	{
		ASSERT(count > 0);

		for (int i = 0; i <= count - 1; ++i)
		{
			int j;
			const T v = array[i];
			for (j = i - 1; j >= 0; --j)
			{
				if (!compare(array[j], v, state))
				{
					break;
				}
				array[j + 1] = array[j];
			}
			array[j + 1] = v;
		}
	}

	template<typename T>
	void quickSort(Array<T>& array, compareFunction<T> compare, const void* state)
	{
		quickSort(array.elt, array.size, compare, state);
	}

	//
	// Hoare partition algorithm.
	//
	// Note: unlike the Lomuto partition, the Hoare partition doesn't
	// put the pivot at its final position.
	//
	template<typename T>
	int partition(T* array, int count, compareFunction<T> compare, const void* state)
	{
		// Choose the center of the array as a pivot.
		T pivot = array[(count - 1) / 2];
		int i = -1;
		int j = count;
		while (true)
		{
			while (compare(pivot, array[++i], state));
			while (compare(array[--j], pivot, state));

			if (i >= j)
			{
				return j;
			}
			swap(array[i], array[j]);
		}
	}

	template<typename T>
	void quickSort(T* array, int count, compareFunction<T> compare, const void* state)
	{
		if (count <= 1)
		{
			return;
		}
		const int pivotIndex = partition(array, count, compare, state) + 1;

		// When using Hoare partition, recursion is on [low..pivot] and
		// [pivot+1..high], whereas with Lomuto partition it would be
		// [low..pivot-1] and [pivot..high].
		quickSort(array, pivotIndex, compare, state);
		quickSort(array + pivotIndex, count - pivotIndex, compare, state);
	}

	template<typename T>
	void mergeSort(Array<T>& array, compareFunction<T> compare, const void* state)
	{
		mergeSort(array.elt, array.size, compare, state);
	}

	template<typename T>
	void merge(T* dst,
			   const T* left, int leftLength,
			   const T* right, int rightLength,
			   compareFunction<T> compare,
			   const void* state)
	{
		int i = 0;
		int j = 0;
		while (i < leftLength || j < rightLength)
		{
			bool pickRight = (i >= leftLength ||
							  (j < rightLength && compare(left[i], right[j], state)));
			*dst++ = (pickRight ? right[j++] : left[i++]);
		}
	}

	template<typename T>
	void mergeSort(T* array, int count, compareFunction<T> compare, const void* state)
	{
		const int arrayLength = count;
		T* temp = (T*)malloc(arrayLength * sizeof(T));

		// As a trick: the src and dst are inverted first, so we don't
		// overwrite src with the uninitialized data in tmp in case the
		// loop is not executed.
		T* dst = array;
		T* src = temp;

		for (int mergeLength = 1; mergeLength < arrayLength; mergeLength *= 2)
		{
			swap(src, dst);

			for (int mergeStart = 0; mergeStart < arrayLength; mergeStart += 2 * mergeLength)
			{
				T* left = src + mergeStart;
				int leftLength = msys_min(mergeLength, arrayLength - mergeStart);
				T* right = left + leftLength;
				int rightLength = msys_min(mergeLength, arrayLength - mergeStart - leftLength);

				merge(dst + mergeStart,
					  left, leftLength,
					  right, rightLength,
					  compare, state);
			}
		}

		// If the final sorted array is in temp, copy it back.
		if (dst == temp)
		{
			memcpy(array, temp, arrayLength * sizeof(T));
		}
		free(temp);
	}

	template<typename T>
	T& quickSelect(Array<T>& array, int k, compareFunction<T> compare, const void* state)
	{
		return quickSelect(array.elt, array.size, k, compare, state);
	}

	template<typename T>
	T& quickSelect(T* array, int count, int k, compareFunction<T> compare, const void* state)
	{
		ASSERT(count > 0);
		if (count == 1)
		{
			return array[0];
		}
		const int pivotIndex = partition(array, count, compare, state) + 1;
		if (k < pivotIndex)
		{
			return quickSelect(array, pivotIndex, k, compare, state);
		}
		else
		{
			return quickSelect(array + pivotIndex, count - pivotIndex, k - pivotIndex, compare, state);
		}
	}
}
