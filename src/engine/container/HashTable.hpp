#pragma once

#include "Array.hpp"

namespace Container
{
	template<typename K>
	struct HashTableCell {
		K key;
		bool empty;
		bool deleted;
	};

	template<typename K, typename V>
	/// <summary>
	/// Simple hash table.
	/// </summary>
	class HashTable
	{
	public:
		HashTable();
		HashTable(int max);
		~HashTable();

		void init(int max);
		void clear();

		const V* operator[](const K& k) const;
		V* operator[](const K& k);
		const K* findKeyFromValue(const V& value) const;

		V* add(const K& k, const V& v);
		void remove(const K& k);

	public:
		Array<HashTableCell<K>> cells;
		Array<V> values;
#if DEBUG
		int size;
#endif // DEBUG

	private:
		int _findKey(const K& k) const;
		int _findFreeSpot(const K& k) const;
	};
}
