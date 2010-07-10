#ifndef RANGE_SET_H
#define RANGE_SET_H

#include "avl_tree.h"

template <typename T>
class range_set_t : public avl_tree_t<T>
{
public:
	typedef T value_type;

	typedef typename avl_tree_t<T>::iterator       iterator;
	typedef typename avl_tree_t<T>::const_iterator const_iterator;

	template <typename U>
	iterator lower_bound(const U &begin)
	{
		iterator r = this->end();
		iterator n = this->root();

		while (n.n)
		{
			if (n == this->end() || !(n->begin() < begin))
			{
				r = n;
				n = n.left();
			}
			else
				n = n.right();
		}

		return r;
	}

	template <typename U>
	iterator upper_bound(const U &begin)
	{
		iterator r = this->end();
		iterator n = this->root();

		while (n.n)
		{
			if (n == this->end() || begin < n->begin())
			{
				r = n;
				n = n.left();
			}
			else
				n = n.right();
		}

		return r;
	}

	template <typename U>
	iterator find_with_begin(const U &begin)
	{
		iterator i = lower_bound(begin);

		if (i == this->end() || (begin < i->begin() && i != this->begin()))
			--i;

		if (i->begin() <= begin)
			return i;

		return this->end();
	}

	template <typename U>
	iterator find(const U &begin)
	{
		iterator i = find_with_begin(begin);

		if (i != this->end() && begin < i->end())
			return i;

		return this->end();
	}

	iterator insert(const value_type &v)
	{
		return avl_tree_t<T>::insert(lower_bound(v.begin()), v);
	}
};

template <typename T>
struct range_t {
	T _begin;
	T _end;

	const T &begin() const { return _begin; }
	void begin(const T &b) { _begin = b; }

	const T &end()   const { return _end; }
	void end(const T &e) { _end = e; }

	bool contains(const T &i) const {
		return _begin <= i && i < _end;
	}

	range_t()
		: _begin(0), _end(0)
	{}

	range_t(const T &_begin)
		: _begin(_begin), _end(_begin)
	{}

	range_t(const T &_begin, const T &_end)
		: _begin(_begin), _end(_end)
	{}

	virtual ~range_t() {}
};

#endif
