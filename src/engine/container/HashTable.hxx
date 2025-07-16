//
// Implémentation de table à adressage ouvert, où les collisions sont
// résolues en cherchant une autre cellule libre.
//

#pragma once

#include "HashTable.hpp"

#include "Array.hxx"
#include "engine/core/msys_temp.hpp"
#include "engine/debug/Assert.hpp"
#include "engine/noise/Hash.hpp"
#include <cstring>

#define MAX_COLLISION_SEQUENCE 5

namespace Container
{
	template<typename K, typename V>
	HashTable<K, V>::HashTable()
	{
	}

	template<typename K, typename V>
	HashTable<K, V>::HashTable(int max): cells(max), values(max)
	{
		cells.size = max;
		values.size = max;
		clear();
	}

	template<typename K, typename V>
	HashTable<K, V>::~HashTable()
	{
	}

	template<typename K, typename V>
	void HashTable<K, V>::init(int max)
	{
		cells.init(max);
		values.init(max);

		cells.size = max;
		values.size = max;
		clear();
	}

	template<typename K, typename V>
	inline void HashTable<K, V>::clear()
	{
		for (int i = 0; i < cells.size; ++i) {
			cells[i].empty = true;
			cells[i].deleted = false;
		}
#if DEBUG
		size = 0;
#endif // DEBUG
	}

	template<typename T>
	inline bool areEqual(T const& a, T const& b)
	{
		return a == b;
	}

	inline bool areEqual(const char* a, const char* b)
	{
		return strcmp(a, b) == 0;
	}

	template<typename K, typename V>
	int HashTable<K, V>::_findKey(const K& k) const
	{
		// The hash table has to be initialized.
		ASSERT(cells.size > 0);

		// Double hashing: h(k, i) = (h1(k) + i.h2(k)) mod m
		// h2 is computed only if needed (collision).
		const int h1 = Noise::Hash::get32(k);
		int h2 = 0;

		int h = 0;
		int i = 0;
		do {
			h = (h1 + i * h2) % cells.size;
			h += (h < 0) * cells.size;
			if (!cells[h].empty && !cells[h].deleted && areEqual(cells[h].key, k)) {
				return h;
			}
			if (i == 0) {
				h2 = Noise::Hash::get32(h1 + 1);

				// A special case to avoid.
				if (h2 % cells.size == 0) {
					++h2;
				}
			}
		} while (i++ < MAX_COLLISION_SEQUENCE && !cells[h].empty);

		return -1;
	}

	template<typename K, typename V>
	int HashTable<K, V>::_findFreeSpot(const K& k) const
	{
		// The hash table has to be initialized.
		ASSERT(cells.size > 0);

		// Double hashing: h(k, i) = (h1(k) + i.h2(k)) mod m
		// h2 is computed only if needed (collision).
		const int h1 = Noise::Hash::get32(k);
		int h2 = 0;

		int h = 0;
		int i = 0;
		do {
			h = (h1 + i * h2) % cells.size;
			h += (h < 0) * cells.size;
			if (cells[h].empty || cells[h].deleted) {
				return h;
			}
			if (i == 0) {
				h2 = Noise::Hash::get32(h1 + 1);

				// A special case to avoid.
				while (h2 % cells.size == 0) {
					++h2;
				}
			}
		} while (i++ < MAX_COLLISION_SEQUENCE);

		// Hash table overflow. Use a bigger table?
		ASSERT(false);
		return -1;
	}

	template<typename K, typename V>
	inline const V* HashTable<K, V>::operator[](const K& k) const
	{
		const int h = _findKey(k);
		return (h >= 0 ? &values[h] : nullptr);
	}

	template<typename K, typename V>
	inline V* HashTable<K, V>::operator[](const K& k)
	{
		const int h = _findKey(k);
		return (h >= 0 ? &values[h] : nullptr);
	}

	template<typename K, typename V>
	const K* HashTable<K, V>::findKeyFromValue(const V& value) const
	{
		for (int i = 0; i < cells.size; ++i) {
			if (!cells[i].empty && !cells[i].deleted && values[i] == value) {
				return &cells[i].key;
			}
		}
		return nullptr;
	}

	template<typename K, typename V>
	V* HashTable<K, V>::add(const K& k, const V& v)
	{
		// Make sure the key hasn't been used already.
		ASSERT(_findKey(k) < 0);
		ASSERT(size < cells.size);

		const int h = _findFreeSpot(k);
		if (h < 0) {
			ASSERT(false);
			return nullptr;
		}

		cells[h].key = k;
		cells[h].empty = false;
		cells[h].deleted = false;

		values[h] = v;
#if DEBUG
		++size;
#endif // DEBUG
		return &values[h];
	}

	template<typename K, typename V>
	inline void HashTable<K, V>::remove(const K& k)
	{
		const int h = _findKey(k);
		if (h < 0) {
			ASSERT(false);
			return;
		}

		// Leave cells[h].empty to false so the find function still
		// works if this key was colliding with another.
		cells[h].deleted = true;
#if DEBUG
		--size;
		ASSERT(size >= 0);
#endif // DEBUG
	}
}
