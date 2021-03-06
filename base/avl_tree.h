#ifndef AVL_TREE_H
#define AVL_TREE_H

#include <algorithm>
#include <cassert>
#include <memory>

/*
 * struct avl_node_base_t;
 *
 * avl_node_base_t *avl_min(avl_node_base_t *n)
 * avl_node_base_t *avl_max(avl_node_base_t *n)
 * avl_node_base_t *avl_successor(avl_node_base_t *n)
 * avl_node_base_t *avl_predecessor(avl_node_base_t *n)
 *
 * avl_node_base_t *avl_rotate_right(avl_node_base_t *q)
 * avl_node_base_t *avl_rotate_left(avl_node_base_t *p)
 * avl_node_base_t *avl_rebalance(avl_node_base_t *n)
 *
 * struct avl_node_t;
 * struct avl_tree_iterator_t;
 * class avl_tree_t;
*/


struct avl_node_base_t {
	avl_node_base_t *parent;
	avl_node_base_t *left;
	avl_node_base_t *right;
	int              height;

	avl_node_base_t()
		: parent(0), left(0), right(0), height(0)
	{}
};

inline
avl_node_base_t *avl_min(avl_node_base_t *n)
{
	while (n->left)
		n = n->left;

	return n;
}

inline
avl_node_base_t *avl_max(avl_node_base_t *n)
{
	while (n->right)
		n = n->right;

	return n;
}

inline
avl_node_base_t *avl_successor(avl_node_base_t *n)
{
	if (n->right)
		return avl_min(n->right);

	while (n->parent && n->parent->right == n)
		n = n->parent;

	return n->parent;
}

inline
avl_node_base_t *avl_predecessor(avl_node_base_t *n)
{
	if (n->left)
		return avl_max(n->left);

	while (n->parent && n->parent->left == n)
		n = n->parent;

	return n->parent;
}

/*
 * avl_rotate_right:
 *
 *      q            p
 *     / \          / \
 *    p   c   =>   a   q
 *   / \              / \
 *  a   b            b   c
 *
 */

#define h(n) ((n) ? (n)->height : 0)

inline
avl_node_base_t *avl_rotate_right(avl_node_base_t *q)
{
	assert(q->left);

	avl_node_base_t *p = q->left;

	q->left  = p->right;
	p->right = q;

	if (q->parent) {
		if (q->parent->left == q)
			q->parent->left = p;
		else
			q->parent->right = p;
	}
	p->parent = q->parent;
	q->parent = p;
	if (q->left) q->left->parent = q;

	q->height = 1 + std::max(h(q->left), h(q->right));
	p->height = 1 + std::max(h(p->left), h(p->right));

	return p;
}

/*
 * avl_rotate_left:
 *
 *      p            q
 *     / \          / \
 *    a   q   =>   p   c
 *       / \      / \
 *      b   c    a   b
 *
 */

inline
avl_node_base_t *avl_rotate_left(avl_node_base_t *p)
{
	assert(p->right);

	avl_node_base_t *q = p->right;

	p->right = q->left;
	q->left  = p;

	if (p->parent) {
		if (p->parent->left == p)
			p->parent->left = q;
		else
			p->parent->right = q;
	}
	q->parent = p->parent;
	p->parent = q;
	if (p->right) p->right->parent = p;

	p->height = 1 + std::max(h(p->left), h(p->right));
	q->height = 1 + std::max(h(q->left), h(q->right));

	return q;
}

#define hdiff(n) (h(n->left) - h(n->right))

inline
avl_node_base_t *avl_rebalance(avl_node_base_t *n)
{
	if (!n) return n;

	for (;;) {
		if (hdiff(n) == 2)
		{
			if (hdiff(n->left) == -1)
				avl_rotate_left(n->left);
			n = avl_rotate_right(n);
		}
		else if (hdiff(n) == -2)
		{
			if (hdiff(n->right) == 1)
				avl_rotate_right(n->right);
			n = avl_rotate_left(n);
		}
		else
			n->height = 1 + std::max(h(n->left), h(n->right));

		if (!n->parent)
			return n;

		n = n->parent;
	}
}

#undef hdiff
#undef h

template <typename ValueTp>
struct avl_node_t : public avl_node_base_t
{
	typedef ValueTp value_type;

	value_type value;

	avl_node_t(const value_type &value)
		: value(value)
	{}
};

template <typename T, typename NodeTp, typename DiffTp>
class avl_tree_iterator_t
{
	typedef NodeTp               node_type;
	typedef node_type           *node_pointer_type;
	typedef avl_tree_iterator_t  iterator_type;

public:
	avl_node_base_t *n;

	typedef T value_type;

	typedef std::bidirectional_iterator_tag iterator_category;
	typedef DiffTp                          difference_type;
	typedef value_type&                     reference;
	typedef value_type*                     pointer;

	reference operator*()  const { return  static_cast<node_type*>(n)->value; }
	pointer   operator->() const { return &static_cast<node_type*>(n)->value; }

	friend bool operator==(const iterator_type &a, const iterator_type &b) { return a.n == b.n; }
	friend bool operator!=(const iterator_type &a, const iterator_type &b) { return !(a == b); }

	iterator_type &operator++() { this->n = avl_successor(this->n); return *this; }
	iterator_type  operator++(int) { iterator_type tmp = *this; ++*this; return tmp; }

	iterator_type &operator--() { this->n = avl_predecessor(this->n); return *this; }
	iterator_type  operator--(int) { iterator_type tmp = *this; --*this; return tmp; }

	iterator_type left()  const { return iterator_type(n->left); }
	iterator_type right() const { return iterator_type(n->right); }

public:
	avl_tree_iterator_t()
	{}

	explicit avl_tree_iterator_t(avl_node_base_t *n)
		: n(n)
	{}
};

template <typename T, typename NodeTp, typename DiffTp>
class avl_tree_const_iterator_t
{
	typedef NodeTp                     non_const_node_type;

	typedef const NodeTp               node_type;
	typedef node_type                 *node_pointer_type;
	typedef avl_tree_const_iterator_t  iterator_type;

	typedef avl_tree_iterator_t<T, non_const_node_type, DiffTp> non_const_iterator_type;

public:
	avl_node_base_t *n;

	typedef T value_type;

	typedef std::bidirectional_iterator_tag iterator_category;
	typedef DiffTp                          difference_type;
	typedef const value_type&               reference;
	typedef const value_type*               pointer;

	reference operator*()  const { return  static_cast<node_type*>(n)->value; }
	pointer   operator->() const { return &static_cast<node_type*>(n)->value; }

	friend bool operator==(const iterator_type &a, const iterator_type &b) { return a.n == b.n; }
	friend bool operator!=(const iterator_type &a, const iterator_type &b) { return !(a == b); }

	iterator_type &operator++() { this->n = avl_successor(this->n); return *this; }
	iterator_type  operator++(int) { iterator_type tmp = *this; ++*this; return tmp; }

	iterator_type &operator--() { this->n = avl_predecessor(this->n); return *this; }
	iterator_type  operator--(int) { iterator_type tmp = *this; --*this; return tmp; }

	iterator_type left()  const { return iterator_type(n->left); }
	iterator_type right() const { return iterator_type(n->right); }

	avl_tree_const_iterator_t()
		: n(0)
	{}

	avl_tree_const_iterator_t(non_const_iterator_type i)
		: n(i.n)
	{}

	explicit avl_tree_const_iterator_t(avl_node_base_t *n)
		: n(n)
	{}
};

template <typename T, typename Allocator = std::allocator<T> >
class avl_tree_t
{
	// Private types
	typedef avl_node_base_t node_base;
	typedef avl_node_t<T>   node;

public:
	// Public types
	typedef T value_type;

	typedef avl_tree_iterator_t      <value_type, node, typename Allocator::difference_type>       iterator;
	typedef avl_tree_const_iterator_t<value_type, node, typename Allocator::difference_type> const_iterator;

	typedef       value_type&               reference;
	typedef const value_type&         const_reference;

	typedef       value_type*               pointer;

	typedef Allocator allocator_type;
	typedef typename allocator_type::difference_type difference_type;
	typedef typename allocator_type::size_type       size_type;

	// Public methods
	iterator       begin()       { return iterator(avl_min(_root)); }
	const_iterator begin() const { return const_iterator(avl_min(_root)); }

	iterator       end()         { return iterator(end_node()); }
	const_iterator end()   const { return const_iterator(end_node()); }

	size_type      size()  const { return std::distance(begin(), end()); }
	bool           empty() const { return begin() == end(); }

	iterator       root()        { return iterator(_root); }
	const_iterator root()  const { return const_iterator(_root); }

private:
	// Private members
	node_base *_root;
	node_base  _end_node;

	// Private methods
	      node_base *end_node()       { return &_end_node; }
	const node_base *end_node() const { return &_end_node; }

	pointer rebalance(pointer n);

public:
	avl_tree_t()
		: _root(end_node()), _end_node()
	{}
	virtual ~avl_tree_t()
	{}

protected:
	iterator insert(iterator i, const value_type &v)
	{
		node_base *n = new node(v);

		if (!i.n->left)
		{
			i.n->left = n;
			n->parent = i.n;
		}
		else
		{
			node_base *p = avl_predecessor(i.n);
			p->right = n;
			n->parent = p;
		}

		_root = avl_rebalance(n);

		return iterator(n);
	}
private:
	avl_tree_t(const avl_tree_t&);
	const avl_tree_t& operator= (const avl_tree_t&);
};

#endif
