////////////////////////////////////////////////////////////////////////////////

// terrain.cpp
// Copyright (c) 1999-2000 Treyarch Invention LLC.  ALL RIGHTS RESERVED.

// This is the home of various classes which define the terrain.  It includes the
// portals, regions, and the BSP tree->

////////////////////////////////////////////////////////////////////////////////

#include "global.h"


#include "terrain.h"
#include "debug.h"
#include "colmesh.h"
#include "wds.h"
#include "collide.h"
//P #include "memorycontext.h"
#include "osdevopts.h"
#include "profiler.h"
#include "oserrmsg.h"
#include <vector>
#include <algorithm>
#include "bsp_tree.h"
#include "osalloc.h"
#include "iri.h"

#include "bsp_tree.h"

partition3 *global_plane;



////////////////////////////////////////////////////////////////////////////////
// CLASS partition3
////////////////////////////////////////////////////////////////////////////////

// Constructors

partition3::partition3()
  : _H(),
    my_sector( NULL )
{
}

partition3::partition3( const partition3& b )
  : _H( b ),
    my_sector( b.my_sector )
{
}

partition3::partition3( const vector3d& c, const vector3d& n, sector* s)
  : _H( c, n ),
    my_sector( s )
{

}



// SUBCLASS terrain::tree_t

// Constructors

tree_t::tree_t()
  : _Tr()
{
}


// Methods


// return sector corresponding to given world-space coordinate position;
// return NULL if outside legal world space
sector* tree_t::find_sector(const vector3d& pos) const
{
  branch br = find_leaf(pos);
  if (br.child == branch::LEFT)

    return NULL;
  assert(br.child==branch::RIGHT && br.parent._Mynode());
  return br.parent->get_sector();
}


////////////////////////////////////////////////////////////////////////////////

// CLASS terrain

////////////////////////////////////////////////////////////////////////////////

// Statics

entity * terrain::last_elevation_entity;

// Constructors

terrain::terrain()
{
}

terrain::terrain(const stringx& filename)
{
	PANIC;
}

terrain::~terrain()
{
  tree->clear();
  delete tree;
  for (int i=regions.size(); --i>=0; )
  {
    delete regions[i];
  }
  regions.resize(0);
  sectors.resize(0);
  portal_list::iterator pli;
  for( pli = portals.begin(); pli != portals.end(); ++pli )
    delete *pli;
}

// Internal Methods

void terrain::_load_tree(chunk_file& fs)

{
#ifndef TARGET_XBOX	// XBox stl clears the reserved memory as well as the data! (dc 02/06/02)
  sectors.resize(0);
  tree->clear();
#endif
  // set up stack for "recursion"
  branch_vector recursion_stack;
  recursion_stack.reserve( 500 );
  recursion_stack.push_back( tree_t::branch() );
  // perform recursive load
  _recursive_load_tree( fs, recursion_stack );
}


// find an intersection by BSP tree solid geometry and shooting a ray from p0 to p1 and
// return the intersection pi and the normal n
bool terrain::find_intersection(const vector3d &p0, const vector3d &p1, vector3d &pi, vector3d &n)
{
	return tree->find_intersection(p0, p1, pi, n);
}

bool terrain::in_world(const vector3d &p, const rational_t r, const vector3d &v, vector3d &n, vector3d &pi)
{
	return tree->in_world(p, r, v, n, pi);
}



// this creates sorted lists of entities in each region according to their
// bounding box info and
// visual_xz_radius_rel_center
void terrain::sort_entities_within_each_region()
{
  region_list::iterator ri;
  region_list::const_iterator ri_end = regions.end();
  for ( ri=regions.begin(); ri!=ri_end; ++ri )
    (*ri)->sort_entities();

}



vector3d terrain::get_gradient( const vector3d& normal, const vector3d& direction )
{
  vector3d outvec = cross(normal,cross(direction,normal).normalize());
  return outvec;
}


////////////////////////////////////////////////////////////////////////////////
// serial IO
////////////////////////////////////////////////////////////////////////////////

void serial_in( chunk_file& fs, terrain* ter )
{
	PANIC;
}

void terrain::_recursive_load_tree( chunk_file& fs, branch_vector & recursion_stack )

{
	PANIC;
}


const rational_t MIN_GET_ELEVATION_NORMAL_Y = 0.7f;


extern profiler_timer proftimer_get_elevation;

rational_t terrain::get_elevation( const vector3d& p,
                                   vector3d& normal,
                                   region_node *default_region,
                                   unsigned char* hit_surface_type )
{
  proftimer_get_elevation.start();

  vector3d pos(p);
  // familiar kludge to avoid out of world problems.
  pos.y += 0.1f;

  rational_t outval = BOTTOM_OF_WORLD;
  normal = YVEC;

  vector3d ground_loc, ground_normal;
  vector3d low_pos = pos - YVEC*6; // DEEP;  New theory says 4 meters is enough.  We shall see.


  sector* s = g_world_ptr->get_the_terrain().find_sector(pos);
  region_node* r;
  if ( s )
    r = s->get_region();
  else
    r = default_region;

  vector3d local_entity_ground_loc, local_entity_ground_normal;
  vector3d entity_ground_loc, entity_ground_normal;
  int search_direction = 1;
//  bool done = false;
  last_elevation_entity = NULL;

  if ( r )
  {
    const region* rg = r->get_data();
    const vector3d vdown(0,-1,0);

    bool hit;

    if ( hit_surface_type )
    {
      const vr_pmesh* pM = NULL;
      face_ref rF;
      hit = collide_segment_region_with_poly_data( pos, low_pos, rg, ground_loc, ground_normal, PP_REAR_CULL, vector3d(0,-1,0), &pM, &rF );
      if ( hit )
	  {
        //*hit_surface_type = pM->get_surface_type( rF );
		  PANIC;
	  }
    }

    else
      hit = collide_segment_region( pos, low_pos, rg, ground_loc, ground_normal, PP_REAR_CULL, vector3d(0,-1,0) );
    if ( hit )

    {
      if ( ground_normal.y < MIN_GET_ELEVATION_NORMAL_Y )
        hit = false;
      else
      {
        outval = ground_loc.y;
        normal = ground_normal;
        low_pos = ground_loc;
      }
    }
    //profcounter_get_elevation_a.add_count(1);

    // let's just check the entities in the current region;
    // this might produce anomalies near portals that are not perfectly vertical,
    // but there should be precious few of those anyway

    // first check moving walkable entities
    entity *ent;

//    region::entity_list::const_iterator ei = rg->get_entities().begin();
//    region::entity_list::const_iterator ei_end = rg->get_entities().end();
    region::entity_list::const_iterator ei = rg->get_possible_collide_entities().begin();
    region::entity_list::const_iterator ei_end = rg->get_possible_collide_entities().end();

    for ( ; ei!=ei_end; ++ei )
    {

      ent = *ei;
      if ( ent && ent->get_colgeom() && ent->is_walkable() && ent->is_flagged(EFLAG_MISC_NONSTATIC) )
      {
        assert( ent->is_stationary() );
        vector3d diff = ent->get_abs_position() - pos;
        rational_t rad = ent->get_radius();

        if ( diff.xz_length2() < rad*rad )
        {
          bool hit;
          ent->get_colgeom()->set_owner( ent );
          g_world_ptr->get_origin_entity()->set_radius( ent->get_radius() );

          vector3d local_pos = ent->get_abs_po().fast_inverse_xform(pos);
          vector3d local_low_pos = ent->get_abs_po().fast_inverse_xform(low_pos);


          vector3d local_vdown = ent->get_abs_po().fast_non_affine_inverse_xform(vdown);

          hit = collide_segment_mesh( local_pos, local_low_pos, (cg_mesh*)ent->get_updated_colgeom(), local_entity_ground_loc, local_entity_ground_normal, PP_REAR_CULL, local_vdown );
          if ( hit )
          {
            entity_ground_loc = ent->get_abs_po().fast_8byte_xform( local_entity_ground_loc );
            entity_ground_normal = ent->get_abs_po().fast_8byte_non_affine_xform( local_entity_ground_normal );
            if ( hit_surface_type )
            {
              // CTT 04/18/00: TODO: obtain surface type info from entity and fill hit_surface_type
            }

          }

          if ( hit )
          {
            if ( entity_ground_normal.y < MIN_GET_ELEVATION_NORMAL_Y )
              hit = false;
            else if ( (entity_ground_loc.y-outval)*search_direction > 0 )
            {
              outval = entity_ground_loc.y;
              normal = entity_ground_normal;
              last_elevation_entity = ent;
              low_pos = entity_ground_loc;
            }
          }
        }
      }
    }

    // now check static walkable entities
    int low = rg->get_low_xsorted_entity( pos.x );

    if ( low >= 0 )
    {
      int high = rg->get_high_xsorted_entity( pos.x );
      if ( high >= low )
      {
        ei = rg->get_x_sorted_entities().begin() + low;
        ei_end = rg->get_x_sorted_entities().begin() + high;
        for ( ; ei<=ei_end; ++ei )
        {
          ent = *ei;
          if ( ent->is_walkable() )
          {
            assert( ent->is_stationary() );
            if ( ent->get_colgeom() && ent->get_bounding_box().xz_intersect( pos ) )
            {
              bool hit;
              ent->get_colgeom()->set_owner( ent );
              g_world_ptr->get_origin_entity()->set_radius( ent->get_radius() );

              vector3d local_pos = ent->get_abs_po().fast_inverse_xform(pos);
              vector3d local_low_pos = ent->get_abs_po().fast_inverse_xform(low_pos);

              vector3d local_vdown = ent->get_abs_po().fast_non_affine_inverse_xform(vdown);

              hit = collide_segment_mesh( local_pos, local_low_pos, (cg_mesh*)ent->get_updated_colgeom(), local_entity_ground_loc, local_entity_ground_normal, PP_REAR_CULL, local_vdown );
              if ( hit )
              {
                entity_ground_loc = ent->get_abs_po().fast_8byte_xform( local_entity_ground_loc );
                entity_ground_normal = ent->get_abs_po().fast_8byte_non_affine_xform( local_entity_ground_normal );
                if ( hit_surface_type )
                {
                  // CTT 04/18/00: TODO: obtain surface type info from entity and fill hit_surface_type
                }

              }

              if ( hit )
              {
                if ( entity_ground_normal.y < MIN_GET_ELEVATION_NORMAL_Y )
                  hit = false;
                else if ( (entity_ground_loc.y-outval)*search_direction > 0 )
                {
                  outval = entity_ground_loc.y;
                  normal = entity_ground_normal;
                  last_elevation_entity = ent;
                  low_pos = entity_ground_loc;
                }
              }
            }
          }
        }
      }
    }
  }

  proftimer_get_elevation.stop();

  return outval;
}


// Return vertical distance (negative below, positive above) of nearest surface
// of given type within given region; optionally, output surface normal.
rational_t terrain::get_vertical_distance( const vector3d& p,
                                           region_node* r,
                                           surface_type_t surface_type,

                                           vector3d* normal )
{
  rational_t dist = -10000;
  if ( !r )
  {
    // need a region to search
    sector* s = find_sector( p );
    if ( s )

      r = s->get_region();
    else
      return dist;
  }


  // first check region's terrain mesh
  region* rg = r->get_data();
  vector3d p1 = p + vector3d(0,1000,0);
  vector3d p2 = p - vector3d(0,1000,0);

  // for now, only supports WATER
  if ( surface_type == WATER )
  {
    int low_idx = rg->get_low_water_index( p.x );
    if ( low_idx >= 0 )
    {

      int high_idx = rg->get_high_water_index( p.x );
      if ( low_idx <= high_idx )
      {
		  std::vector<cface_replacement>::const_iterator cfi = rg->get_sorted_water().begin() + low_idx;
          std::vector<cface_replacement>::const_iterator cfi_end = rg->get_sorted_water().begin() + high_idx;
        for ( ; cfi<=cfi_end; ++cfi )
        {
          const cface_replacement& f = *cfi;
          rational_t fr = f.get_radius();
          vector3d xz_diff = p - f.get_center();
          if ( xz_diff.x*xz_diff.x + xz_diff.z*xz_diff.z < fr*fr )
          {
            // within XZ radius of a cface

            vector3d hit_loc;
            if ( collide_polygon_segment( f.rF, f.pP, p1, p2, hit_loc ) )
            {
              rational_t d = hit_loc.y - p.y;
              if ( __fabs(d) < __fabs(dist) )
              {
                dist = d;
                if ( normal )
                  *normal = f.get_normal();
              }
            }
          }
        }
      }
    }
  }

  return dist;
}



sector* terrain::find_sector(const vector3d& pos) const
{
  return tree->find_sector(pos);
}

void terrain::optimize()
{
  for (int i=regions.size(); --i>=0; )
  {
    regions[i]->optimize();
  }

  // Causes some bp_tree memory to be preallocated.  (dc 03/27/02)
  vector3d dummy;
    // PLEASE don't use nan!

  dummy.x=0.0f;
  dummy.y=0.0f;
  dummy.z=0.0f;
  tree->in_world(dummy, 0, dummy, dummy, dummy);
}
