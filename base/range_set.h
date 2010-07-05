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

	iterator lower_bound(const value_type &v)
	{
		iterator r = this->end();
		iterator n = this->root();

		while (n.n)
		{
			if (n == this->end() || !(n->begin() < v.begin()))
			{
				r = n;
				n = n.left();
			}
			else
				n = n.right();
		}

		return r;
	}

	iterator upper_bound(const value_type &v)
	{
		iterator r = this->end();
		iterator n = this->root();

		while (n.n)
		{
			if (n == this->end() || v.begin() < n->begin())
			{
				r = n;
				n = n.left();
			}
			else
				n = n.right();
		}

		return r;
	}

	iterator insert(const value_type &v)
	{
		return avl_tree_t<T>::insert(lower_bound(v), v);
	}

	template <typename U>
	void insert(U first, U last)
	{
		while (first != last)
			this->insert(*first++);
	}

	void dump(iterator i)
	{
		putchar('(');
		if (i.left().n)
			dump(i.left());

		printf("%d", i->begin());

		if (i.right().n)
			dump(i.right());
		putchar(')');
	}

	void dump()
	{
		dump(this->root());
		putchar('\n');
	}
};

struct range_t {
	int _begin;
	int _end;

	int begin() const { return _begin; }
	int end()   const { return _end; }

	range_t(int _begin)
		: _begin(_begin)
	{}
};

#endif
