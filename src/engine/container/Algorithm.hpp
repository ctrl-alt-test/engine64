#pragma once

namespace Noise
{
	class Rand;
}

namespace Container
{
	template<typename T> class Array;

	template<typename T>
	using compareFunction = bool (*)(const T& lhs, const T& rhs, const void* state);

	template<typename T>
	bool defaultCompare(const T& lhs, const T& rhs, const void* state);

	/// <summary>
	/// Swap the two arguments.
	/// </summary>
	template<typename T> void swap(T& a, T& b);

	/// <summary>
	/// Compares two arrays of floats. The behaviour mimics memcmp, but
	/// uses float comparision rather than binary comparision.
	/// </summary>
	/// <param name="n">The number of float to compare.</param>
	/// <returns>0 if the two arrays contain the same values, !0 otherwise.</returns>
	int fltcmp(const float* fptr1, const float* fptr2, size_t n);
	inline
	int fltcmp(const void* fptr1, const void* fptr2, size_t n)
	{
		return fltcmp((const float*)fptr1, (const float*)fptr2, n);
	}

	/// <summary>
	/// Performs a linear search on an array.
	/// </summary>
	///
	/// <returns>Index of the first matching element, -1 if no match is
	/// found.</returns>
	template<typename T> int find(const Array<T>& array, const T& item);
	template<> int find(const Array<const char*>& array, const char* const& items);

	/// <summary>
	/// Performs a linear search on a sub-array.
	/// </summary>
	///
	/// <returns>Index of the first matching element, -1 if no match is
	/// found.</returns>
	int find_lowLevel(const void* array, const void* item, size_t sizeOfType, int count);

	/// <summary>
	/// Performs a binary search on an array.
	/// The array is assumed to be sorted in increasing order.
	/// </summary>
	///
	/// <returns>Index of the first matching element, or the index
	/// where the element should be inserted to keep the ordering.</returns>
	template<typename T> int binarySearch(const Array<T>& array, const T& item);

	/// <summary>
	/// Performs a binary search on a sub-array.
	/// The sub-array is assumed to be sorted in increasing order.
	/// </summary>
	///
	/// <returns>Index of the first matching element, or the index
	/// where the element should be inserted to keep the ordering.</returns>
	template<typename T> int binarySearch(const T* array, const T& item, int count);

	/// <summary>
	/// Randomizes the order of the elements of an array.
	/// </summary>
	template<typename T> void shuffle(Noise::Rand& rand, Array<T>& array);

	/// <summary>
	/// Randomizes the order of the elements of a sub-array.
	/// </summary>
	template<typename T> void shuffle(Noise::Rand& rand, T* array, int count);

	/// <summary>
	/// Returns true if the array is sorted in increasing order, false
	/// otherwise.
	/// </summary>
	template<typename T> bool isSorted(const Array<T>& array,
									   compareFunction<T> compare = defaultCompare,
									   const void* state = nullptr);

	/// <summary>
	/// Returns true if the sub-array is sorted in increasing order, false
	/// otherwise.
	/// </summary>
	template<typename T> bool isSorted(const T* array,
									   int count,
									   compareFunction<T> compare = defaultCompare,
									   const void* state = nullptr);

	/// <summary>
	/// Sorts the array using bubble sort.
	/// - O(n^2).
	/// - Sort done in place.
	/// </summary>
	template<typename T> void bubbleSort(Array<T>& array,
										 compareFunction<T> compare = defaultCompare,
										 const void* state = nullptr);
	template<typename T> void bubbleSort(T* array,
										 int count,
										 compareFunction<T> compare = defaultCompare,
										 const void* state = nullptr);

	/// <summary>
	/// Sorts the array using insertion sort.
	/// - O(n^2).
	/// - Sort done in place.
	/// </summary>
	template<typename T> void insertionSort(Array<T>& array,
											compareFunction<T> compare = defaultCompare,
											const void* state = nullptr);
	template<typename T> void insertionSort(T* array,
											int count,
											compareFunction<T> compare = defaultCompare,
											const void* state = nullptr);

	/// <summary>
	/// Sorts the array using quick sort.
	/// - O(n.log2(n)) amortized.
	/// - O(n^2) in the worst case.
	/// - Sort done in place.
	/// - Sort not stable.
	/// </summary>
	template<typename T> void quickSort(Array<T>& array,
										compareFunction<T> compare = defaultCompare,
										const void* state = nullptr);
	template<typename T> void quickSort(T* array,
										int count,
										compareFunction<T> compare = defaultCompare,
										const void* state = nullptr);

	/// <summary>
	/// Sorts the array using merge sort.
	/// - O(n.log2(n)).
	/// - Current implementation is out of place.
	/// </summary>
	template<typename T> void mergeSort(Array<T>& array,
										compareFunction<T> compare = defaultCompare,
										const void* state = nullptr);
	template<typename T> void mergeSort(T* array,
										int count,
										compareFunction<T> compare = defaultCompare,
										const void* state = nullptr);

	/// <summary>
	/// Selects the k-th smallest element of the array using quick
	/// select. k starts at 0; it's the index the element would have if
	/// the array was sorted.
	/// After returning, the array will also be partitioned based on k.
	/// - O(n) amortized.
	/// - O(n^2) in the worst case.
	/// - Partition done in place.
	/// </summary>
	///
	/// <returns>k-th smallest element.</returns>
	template<typename T> T& quickSelect(Array<T>& array,
										int k,
										compareFunction<T> compare = defaultCompare,
										const void* state = nullptr);
	template<typename T> T& quickSelect(T* array,
										int count,
										int k,
										compareFunction<T> compare = defaultCompare,
										const void* state = nullptr);
}
