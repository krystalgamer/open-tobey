// region.h
// Copyright (C) 2000 Treyarch LLC    ALL RIGHTS RESERVED

#ifndef REGION_H
#define REGION_H

#include "warnlvl.h"

#include "algebra.h"
#include "stringx.h"
#include "pmesh.h"
#include "hull.h"


#ifdef TARGET_PS2
#include "ngl_ps2.h"
#endif
#if defined(TARGET_XBOX)
#include "ngl.h"
#elif defined(TARGET_GC)
#include "ngl.h"
#endif /* TARGET_XBOX JIV DEBUG */

#include <list>
#include <vector>

struct cface_replacement
{

  vr_pmesh const* pP;
  face_ref rF;
  vector3d center;
  rational_t radius;
  vector3d normal;


  cface_replacement() : pP(0), rF(UNINITIALIZED_FACE_REF) {}
  cface_replacement( const vr_pmesh *pPP, face_ref rFF );

  const vector3d& get_corner_point( int which ) const
  {
	  // @TODO
	  PANIC;
  }

  const vector3d& get_center() const { return center; }
  rational_t get_radius() const { return radius; }
  const vector3d& get_normal() const { return normal; }

  bool operator<( const cface_replacement& cfr ) const
  {
    assert( (rF>=0) && (cfr.rF>=0) );
    return ( center.x < cfr.center.x );
  }

  bool operator==( const cface_replacement& cfr ) const
  {
    return ( cfr.pP==pP && cfr.rF==rF );
  }

  const cface_replacement& operator=( const cface_replacement& cfr )
  {
    pP = cfr.pP;
    rF = cfr.rF;
    center = cfr.center;
    radius = cfr.radius;
    normal = cfr.normal;
    return( *this );
  }

  bool is_cosmetic() const { return pP->is_cosmetic( rF ); }

  void set( const vr_pmesh *pPP, face_ref rFF );

  void const verify_integrity() const;
};


class terrain;
class trigger;
class entity;
class light_source;
class vm_thread;
class chunk_file;
class crawl_box;
class ai_polypath_cell;


//#pragma todo("Test this to see if it is a bonus or detriment (JDB 04-04-01)")
#define USE_POSS_RENDER_LIST 1


class region
{
public:
  // Types

  typedef std::vector<entity*> entity_list;
  typedef std::vector<light_source*> light_list;
  typedef std::vector<trigger*> trigger_list;
  typedef std::list<crawl_box*> crawl_list;
  typedef std::vector<ai_polypath_cell *> pathcell_list; // MUST match what the 'CELL_ARRAY' typedef is in class ai_polypath

  enum flags_t
  {

    ACTIVE =  0x0001,
    LOCKED =  0x0002,
    LIGHTING = 0x0004
  };

  EXPORT region();
  EXPORT region(const stringx& region_name);
  EXPORT ~region();


  EXPORT void optimize();

  EXPORT int get_low_index(rational_t val) const;
  EXPORT int get_high_index(rational_t val) const;
  EXPORT const std::vector<cface_replacement>& get_sorted() const { return sorted; }

  EXPORT int get_low_water_index( rational_t val ) const;
  EXPORT int get_high_water_index( rational_t val ) const;
  EXPORT const std::vector<cface_replacement>& get_sorted_water() const { return sorted_water; }

#if defined(TARGET_PS2) || defined(TARGET_XBOX) || defined(TARGET_GC)
  EXPORT int get_num_meshes() const { return ps2_meshes.size(); }

  EXPORT nglMesh* get_mesh(int idx) const
  {
    assert(idx<(int)ps2_meshes.size());
    return ps2_meshes[idx];
  }
#else
  EXPORT int get_num_visreps() const { return visreps.size(); }
  EXPORT visual_rep* get_visrep(int idx) const
  {
    assert(idx<(int)visreps.size());
    return visreps[idx];
  }
#endif


  EXPORT const stringx& get_name() const { return name; }

  EXPORT const color& get_ambient() const { return ambient; }
  EXPORT void set_ambient( const color& c ) { ambient = c; }
  EXPORT void set_ambient( uint8 r, uint8 g, uint8 b ) { ambient = color(r,g,b,1); }

  // lists of entities attached to region, by type
  EXPORT const entity_list& get_entities() const { return entities; }
  EXPORT const entity_list& get_possible_active_entities() const { return possible_active_ents; }

#if USE_POSS_RENDER_LIST
  EXPORT const entity_list& get_possible_render_entities() const { return possible_render_ents; }
#else
  EXPORT const entity_list& get_possible_render_entities() const { return entities; }

#endif

  EXPORT const entity_list& get_possible_collide_entities() const { return possible_collide_ents; }
  EXPORT void add( entity* e );
  EXPORT void remove( entity* e );
  EXPORT void update_poss_active( entity* e );
#if USE_POSS_RENDER_LIST
  EXPORT void update_poss_render( entity* e );
#else
  EXPORT inline void update_poss_render( entity* e ) {}
#endif

  EXPORT void update_poss_collide( entity* e );

  EXPORT const entity_list& get_camera_collision_entities() const { return cam_coll_ents; }

  EXPORT const light_list& get_lights() const { return lights; }
  EXPORT bool has_affect_terrain_lights() const { return num_affect_terrain_lights > 0; }
  EXPORT void add( light_source* e );
  EXPORT void remove( light_source* e );

  EXPORT trigger_list& get_triggers() { return triggers; }
  EXPORT void add( trigger* e );
  EXPORT void remove( trigger* e );

  EXPORT crawl_list& get_crawls( void ) { return crawls; }
  EXPORT void add( crawl_box* cb );

  EXPORT void remove( crawl_box* cb );

  EXPORT const pathcell_list& get_pathcells( void ) { return pathcells; }
  EXPORT void add( ai_polypath_cell* cell );
  EXPORT void remove( ai_polypath_cell* cell );

public:

  // convenience for any algorithm that wants to avoid re-visiting regions
  EXPORT static void prepare_for_visiting() { ++visit_key; }
  EXPORT void visit() { visited = visit_key; }
  EXPORT void unvisit() { visited = visit_key-1; }
  EXPORT bool already_visited() const { return (visited == visit_key); }

  // Marks a region when it has been seen, and needs to be processed.
  EXPORT bool is_active() const { return (flags&ACTIVE); }
  EXPORT void set_active( bool v );

  // @Ok
  // @Matching
  // when locked is true, a region's active status cannot be changed
  EXPORT bool is_locked() const { return (flags&LOCKED); }

  EXPORT void set_locked( bool v ) { flags = v? (flags|LOCKED) : (flags&~LOCKED); }


  // View frustum that goes through this region this frame (limited by the entry portal)
  // This frustum only applies if is_active() is true.  Nonactive regions do not have frusta.
  EXPORT const hull& frustum() const { return view_frustum; }
        hull& frustum()       { return view_frustum; }

  // create various sorted lists, listed below
  EXPORT void sort_entities();
  EXPORT void x_sort_entities_by_bounding_box_info();

  // sort entities in this region based on bounding box info

  EXPORT const entity_list& get_x_sorted_entities() const { return x_sorted_entities; }
  EXPORT int get_low_xsorted_entity( rational_t x ) const;
  EXPORT int get_high_xsorted_entity( rational_t x ) const;

  EXPORT void add_local_thread(vm_thread * thr);
  EXPORT void remove_local_thread(vm_thread * thr);

  EXPORT void set_region_ambient_sound( stringx &sndname );
  EXPORT void set_region_ambient_sound_volume( rational_t vol ){ region_ambient_sound_volume = vol; }
  EXPORT stringx &get_region_ambient_sound_name(){ return region_ambient_sound_name; }
  EXPORT rational_t get_region_ambient_sound_volume(){ return region_ambient_sound_volume; }

private:
  EXPORT void build_sorted();
  EXPORT void add_cam_coll_ent( entity* e );
  EXPORT void remove_cam_coll_ent( entity* e );

  // only one of these is valid

  std::list<vm_thread *> local_thread_list;
  rational_t solid_min;

  // water sold separately
  std::vector<cface_replacement> sorted_water;
  std::vector<int> sorted_water_lookup_low;
  std::vector<int> sorted_water_lookup_high;
  rational_t water_min;

#if defined(TARGET_PS2) || defined(TARGET_XBOX) || defined(TARGET_GC)
  typedef std::vector<nglMesh*> ps2_mesh_list;
  ps2_mesh_list ps2_meshes;
#else
  typedef std::vector<visual_rep*> VisRepList;
  VisRepList visreps;
#endif


// USED AS A TEMPORARY DURING CONSTRUCTION
// THEN QUICKLY DESTROYED
// ick. --Sean
  cg_mesh * my_cg_mesh;

  stringx name;
  color ambient;

  entity_list entities;  // list of entities in the region

  entity_list cam_coll_ents;  // shadow list of entities marked for camera collision
  entity_list possible_active_ents;  // shadow list of entities That can be considered for activation

#if USE_POSS_RENDER_LIST
  entity_list possible_render_ents;  // shadow list of entities That can be considered for rendering
#endif

  entity_list possible_collide_ents;  // shadow list of entities That can be considered for collision


  light_list lights;     // list of light_sources attached to region
  trigger_list triggers; // list of triggers attached to region
  crawl_list crawls;
  pathcell_list pathcells;

  hull view_frustum;

  PADDING(0xD0-0xC8);

  unsigned short flags;

  PADDING(0xDC-0xD0-2);

  stringx region_ambient_sound_name;


  // lists of entities sorted in X axis based on bounding box info
  entity_list x_sorted_entities;
  rational_t x_sorted_ent_min;
  rational_t x_sorted_ent_max;
  std::vector<unsigned char> x_sorted_ent_lookup_low;
  std::vector<unsigned char> x_sorted_ent_lookup_high;


  static unsigned int visit_key;
  unsigned int visited;

  short num_affect_terrain_lights;

  rational_t region_ambient_sound_volume;

  // @Patch - moved lower because they don't seem to be spidey related
  std::vector<cface_replacement> sorted;

  std::vector<int> sorted_lookup_low;
  std::vector<int> sorted_lookup_high;

  friend void serial_in( chunk_file& fs, region* r, terrain* ter );
  friend class terrain;

	friend void patch_region(void);
	friend void validate_region(void);
};
typedef std::vector<region *> region_list;

#endif // REGION_H
