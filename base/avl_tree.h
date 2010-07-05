#ifndef AVL_TREE_H
#define AVL_TREE_H

#include <algorithm>
#include <cassert>
#include <memory>

template <typename NodePtr>
NodePtr avl_min(NodePtr n)
{
	while (n->left)
		n = n->left;

	return n;
}

template <typename NodePtr>
NodePtr avl_max(NodePtr n)
{
	while (n->right)
		n = n->right;

	return n;
}

template <typename NodePtr>
NodePtr avl_successor(NodePtr n)
{
	if (n->right)
		return avl_min(n->right);

	while (n->parent && n->parent->right == n)
		n = n->parent;

	return n->parent;
}

template <typename NodePtr>
NodePtr avl_predecessor(NodePtr n)
{
	if (n->left)
		return avl_max(n->left);

	while (n->parent && n->parent->left == n)
		n = n->parent;

	return n->parent;
}

template <typename NodeTp>
struct avl_node_base_t {
	avl_node_base_t *parent;
	avl_node_base_t *left;
	avl_node_base_t *right;
	int              height;

	typedef NodeTp node_type;

	avl_node_base_t()
		: parent(0), left(0), right(0), height(0)
	{}
};

template <typename ValueTp>
struct avl_node_t : public avl_node_base_t<avl_node_t<ValueTp> > {
	typedef ValueTp value_type;

	value_type  value;

	avl_node_t(const value_type &value)
		: value(value)
	{}
};

template <typename T, typename NodeTp, typename DiffTp>
struct avl_tree_iterator_t {
	typedef T value_type;

	typedef std::bidirectional_iterator_tag iterator_category;
	typedef DiffTp                          difference_type;
	typedef value_type&                     reference;
	typedef value_type*                     pointer;

	NodeTp *n;
	typedef typename NodeTp::node_type     node_type;

	value_type &operator*()  const { return  static_cast<node_type*>(n)->value; }
	value_type *operator->() const { return &static_cast<node_type*>(n)->value; }

	friend bool operator==(const avl_tree_iterator_t &a, const avl_tree_iterator_t &b) { return a.n == b.n; }
	friend bool operator!=(const avl_tree_iterator_t &a, const avl_tree_iterator_t &b) { return !(a == b); }

	avl_tree_iterator_t &operator++() { n = avl_successor(n); return *this; }
	avl_tree_iterator_t  operator++(int) { avl_tree_iterator_t tmp = *this; ++*this; return tmp; }

	avl_tree_iterator_t &operator--() { n = avl_predecessor(n); return *this; }
	avl_tree_iterator_t  operator--(int) { avl_tree_iterator_t tmp = *this; --*this; return tmp; }

	avl_tree_iterator_t left()  const { return avl_tree_iterator_t(n->left); }
	avl_tree_iterator_t right() const { return avl_tree_iterator_t(n->right); }

	avl_tree_iterator_t()
		: n(0)
	{}

	explicit avl_tree_iterator_t(NodeTp *n)
		: n(n)
	{}
};

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

template <typename NodePtr>
NodePtr avl_rotate_right(NodePtr q)
{
	assert(q->left);

	NodePtr p = q->left;

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

template <typename NodePtr>
NodePtr avl_rotate_left(NodePtr p)
{
	assert(p->right);

	NodePtr q = p->right;

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

template <typename NodePtr>
NodePtr avl_rebalance(NodePtr n)
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

template <typename T, typename Allocator = std::allocator<T> >
class avl_tree_t
{
	// Private types
	typedef avl_node_t<T>                   node;
	typedef avl_node_base_t<avl_node_t<T> > node_base_t;

public:
	// Public types
	typedef T value_type;

	typedef       avl_tree_iterator_t<value_type, node_base_t, typename Allocator::difference_type>       iterator;
	typedef const avl_tree_iterator_t<value_type, const node_base_t, typename Allocator::difference_type> const_iterator;

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
	node_base_t *_root;
	node_base_t  _end_node;

	// Private methods
	      node_base_t *end_node()       { return &_end_node; }
	const node_base_t *end_node() const { return &_end_node; }

	pointer rebalance(pointer n);

public:
	avl_tree_t()
		: _root(end_node())
	{}

	iterator insert(iterator i, const value_type &v)
	{
		node_base_t *n = new node(v);

		if (!i.n->left)
		{
			i.n->left = n;
			n->parent = i.n;
		}
		else
		{
			node_base_t *p = avl_predecessor(i.n);
			p->right = n;
			n->parent = p;
		}

		_root = avl_rebalance(n);

		return iterator(n);
	}
};

#endif
