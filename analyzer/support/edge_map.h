#ifndef EDGES_H
#define EDGES_H

#include "base/base.h"

#include <map>


class edge_map_t
{
	typedef std::multimap<uint32, uint32> _edge_map_t;

	_edge_map_t                 _edge;
	_edge_map_t                 _back_edge;

public:
	typedef _edge_map_t::const_iterator const_iterator;

	const _edge_map_t &edge()      const { return _edge; }
	const _edge_map_t &back_edge() const { return _back_edge; }

	void add_edge(uint32 src_ea, uint32 dst_ea)
	{
		_edge.insert(std::make_pair(src_ea, dst_ea));
		_back_edge.insert(std::make_pair(dst_ea, src_ea));
	}
};

#endif
