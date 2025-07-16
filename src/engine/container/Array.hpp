#pragma once

namespace Container
{
	template<typename T>
	/// <summary>
	/// Simple array.
	/// </summary>
	class Array
	{
	public:
		int size;
#if DEBUG
		int size_in_bytes;
		int max_size;
#endif
		T* elt;

		Array();
		Array(int max);
		~Array();

		void		init(int max);
		void		copyFrom(const Array<T>& src);
		void		clear();

		const T&	operator [](int i) const;
		T&			operator [](int i);

		const T&	first() const;
		T&			first();
		const T&	last() const;
		T&			last();
		T&			getNew();

		void		add(const T& item);
		void		remove(int n);
		void		pop(){remove(size - 1);}

	private:
		// No array copy.
		Array(const Array<T>& src);
	};
}
