#pragma once

#include "Array.hpp"
#include "engine/debug/Assert.hpp"
#include <cstdlib>
#include <cstring>

#include "engine/core/msys_temp.hpp"

namespace Container
{
	template<typename T>
	Array<T>::Array(): size(0), elt(nullptr)
	{
#if DEBUG
		size_in_bytes = 0;
		max_size = 0;
#endif
	}

	template<typename T>
	Array<T>::Array(int max): size(0)
	{
		elt = (T*)malloc(max * sizeof(T));
		ASSERT(elt != nullptr);

#if DEBUG
		size_in_bytes = max * sizeof(T);
		max_size = max;
#endif
	}

	template<typename T>
	Array<T>::~Array()
	{
		if (elt != nullptr)
		{
			free(elt);
			elt = nullptr;
		}
	}

	template<typename T>
	void Array<T>::init(int max)
	{
		ASSERT(max > 0);
		ASSERT(max_size == 0);
		ASSERT(elt == nullptr);

		size = 0;
		elt = (T*)malloc(max * sizeof(T));
		ASSERT(elt != nullptr);

#if DEBUG
		size_in_bytes = max * sizeof(T);
		max_size = max;
#endif
	}

	template<typename T>
	inline
	void Array<T>::copyFrom(const Array<T>& src)
	{
#if DEBUG
		ASSERT(max_size >= src.size);
#endif
		size = src.size;
		memcpy(elt, src.elt, src.size * sizeof(T));
	}

	template<typename T>
	inline
	void Array<T>::clear()
	{
		size = 0;
	}

	template<typename T>
	inline
	const T& Array<T>::operator [](int i) const
	{
		ASSERT(i >= 0);
		ASSERT(i < size);
		return elt[i];
	}

	template<typename T>
	inline
	T& Array<T>::operator [](int i)
	{
		ASSERT(i >= 0);
		ASSERT(i < size);
		return elt[i];
	}

	template<typename T>
	inline
	const T& Array<T>::first() const
	{
		ASSERT(size > 0);
		return elt[0];
	}

	template<typename T>
	inline
	T& Array<T>::first()
	{
		ASSERT(size > 0);
		return elt[0];
	}

	template<typename T>
	inline
	const T& Array<T>::last() const
	{
		ASSERT(size > 0);
		return elt[size - 1];
	}

	template<typename T>
	inline
	T& Array<T>::last()
	{
		ASSERT(size > 0);
		return elt[size - 1];
	}

	template<typename T>
	inline
	T& Array<T>::getNew()
	{
#if DEBUG
		ASSERT(size < max_size);
#endif
		++size;
		return last();
	}

	template<typename T>
	inline
	void Array<T>::add(const T& item)
	{
#if DEBUG
		ASSERT(size < max_size);
#endif
		memcpy(&elt[size], &item, sizeof(T));
		++size;
	}

	template<typename T>
	inline
	void Array<T>::remove(int n)
	{
		ASSERT(n >= 0);
		ASSERT(n < size);
		--size;
		elt[n] = elt[size];
	}
}
