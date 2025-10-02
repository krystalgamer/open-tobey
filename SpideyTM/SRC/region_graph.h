// region.h
// Copyright (C) 2000 Treyarch LLC    ALL RIGHTS RESERVED

#ifndef REGION_GRAPH_H
#define REGION_GRAPH_H

class region;
class portal;

typedef graph< stringx, region*, portal*
,	std::less<stringx>
//	,	malloc_alloc
  > region_graph;
typedef region_graph::node region_node;

// @Patch
/*
typedef graph< stringx, region*, portal*
	,	less<stringx>
//	,	malloc_alloc

  > region_graph;
typedef region_graph::node region_node;

// region_node_pset used for list of regions intersected by entity
typedef std::set<region_node*
	,	less<region_node *>
	#ifdef TARGET_PS2	
	,	malloc_alloc        //__STL_DEFAULT_ALLOCATOR(region_node *)
	#endif
	> region_node_pset;

typedef std::set<region *> trig_region_pset;

typedef std::list< region_node*
  #ifdef TARGET_PS2
		,malloc_alloc
	#endif	
  > region_node_list;

*/



#endif // REGION_H
