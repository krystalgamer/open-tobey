// entity_maker.cpp

// Copyright (c) 2000 Treyarch Invention LLC.  ALL RIGHTS RESERVED.
// now contains functions for creating entities
// since we now want to be able to have entities
// outside the wds

#include "global.h"

#include "entity.h"
#include "path.h"

#include "entity_maker.h"

#include "entityflags.h"
#include "wds.h"
#include "profiler.h"
#include "marker.h"

#include "light.h"
#include "particle.h"
#include "item.h"
// BIGCULL #include "turret.h"
// BIGCULL #include "scanner.h"
#include "sky.h"

// BIGCULL #include "gun.h"
// BIGCULL #include "thrown_item.h"
// BIGCULL #include "melee_item.h"
#include "pmesh.h"
#include "file_finder.h"
#include "widget_entity.h"
#include "conglom.h"
#include "osdevopts.h"
// BIGCULL #include "manip_obj.h"

// BIGCULL#include "switch_obj.h"
#include "polytube.h"
#include "lensflare.h"
//#include "FrontEndManager.h"


#include <cstdarg>


extern world_dynamics_system * g_world_ptr;
extern file_finder *g_file_finder;


entity_maker::entity_maker()
{
  owning_widget = NULL;
}


entity_maker::~entity_maker()
{
}



stringx entity_maker::open( chunk_file& fs, const stringx& filename, const stringx& extension, int io_flags )
{
	PANIC;
	return stringx();
}


/*
extern profiler_counter profcounter_total_blocks;
extern profiler_counter profcounter_alloced_mem;
extern profiler_counter profcounter_terrain_mem;
extern profiler_counter profcounter_entity_mem;
extern profiler_counter profcounter_physent_mem;
extern profiler_counter profcounter_actor_mem;
extern profiler_counter profcounter_character_mem;
extern profiler_counter profcounter_hero_mem;

extern profiler_counter profcounter_conglom_mem;
extern profiler_counter profcounter_item_mem;
extern profiler_counter profcounter_marker_mem;
extern profiler_counter profcounter_particle_mem;
extern profiler_counter profcounter_light_mem;
extern profiler_counter profcounter_ladder_mem;
extern profiler_counter profcounter_script_mem;
extern profiler_counter profcounter_texture_mem;
extern profiler_counter profcounter_audio_mem;
*/

// This function decides what sort of entity to create based on the first entry in the .ent file


entity* entity_maker::create_entity_or_subclass( const stringx& entity_name,
                   entity_id id,
                   po const & loc,
                   const stringx& scene_root,
                   unsigned int scene_flags,
                   const region_node_list *forced_regions)
{
	// @TODO
	typedef entity* (__fastcall *func_ptr)(const entity_maker*, int, entity_id, const po&, const stringx&, unsigned int, const region_node_list*);
	func_ptr func = (func_ptr)0x004FA960;
	return func(this, 0,
			id, loc, scene_root, scene_flags, forced_regions);
}


entity *entity_maker::create_entity( chunk_file& fs,
                                   const entity_id& id,
                                   unsigned int flags,
                                   bool add_bones )
{
  entity* ent = NEW entity( fs, id, ENTITY_ENTITY, flags );

  if(add_bones || ent->get_bone_idx() < 0)
    create_entity( ent );

  return ent;
}


void entity_maker::create_entity( entity* e )
{
  if ( owning_widget == NULL )
  {
    g_world_ptr->add_to_entities( e );
  }
  else
    e->set_owning_widget( owning_widget );

  // non-uniform scaling is not allowed;
  // uniform scaling is allowed only on entities that have no collision geometry

  check_po( e );

  // if you need the compute_sector call, use add_dynamic_instanced_entity instead
  // some flavors of entity are inactive by default

  switch ( e->get_flavor() )
  {
    case ENTITY_ENTITY:
    case ENTITY_MARKER:
    case ENTITY_BEAM:

    case ENTITY_MIC:
    case ENTITY_LIGHT_SOURCE:
    case ENTITY_CONGLOMERATE:
    {
      visual_rep *vrep = e->get_vrep();
      if( !(vrep && vrep->get_type()==VISREP_PMESH && ((vr_pmesh*)vrep)->is_uv_animated()) )
        e->set_flag( EFLAG_ACTIVE, false );
      break;
    }
    default:
      break;
  }
  // this needs to be called to set up last-frame info
  e->frame_done();
}

void entity_maker::read_meshes( chunk_file& fs ) // puts the meshes into the bank

{
	PANIC;
}


conglomerate *entity_maker::create_conglomerate( chunk_file& fs,

                                         const entity_id& id,
                                         unsigned int flags )
{

  conglomerate* ent = NULL;

  stringx fname = fs.get_filename();

  fname.to_lower();

  ent = NEW conglomerate( fs, id, ENTITY_CONGLOMERATE, flags );
  create_entity( ent );
  return ent;
}





void entity_maker::creating_widget_error( const stringx &entity_kind ) const
{
  stringx msg = "Widgets cannot be a" + entity_kind;
  error( msg );
  assert( 0 );
}



void entity_maker::destroy_entity( entity* e )
{
  if ( g_world_ptr && e->get_owning_widget() == NULL )
  {
    g_world_ptr->destroy_entity( e );
  }
  else
    delete e;
}


///////////////////////////////////////////////////////////////////////////////
// entity caching interface

entity* entity_maker::acquire_entity( const stringx& name, unsigned int flags )
{
  return entity_cache.acquire( name, flags );
}

entity* entity_maker::acquire_beam( unsigned int flags )
{
  return entity_cache.acquire_beam( flags );
}

void entity_maker::release_entity( entity* e )
{
  if ( e->get_entity_pool() != NULL )
    e->get_entity_pool()->release( e );
}

void entity_maker::purge_entity_cache()
{
  entity_cache.purge();
}



///////////////////////////////////////////////////////////////////////////////
// support for entity caching system
///////////////////////////////////////////////////////////////////////////////


entity_pool::entity_pool()
: entities(),
  avail( 0 )
{
}

entity_pool::~entity_pool()
{
/*
  entity_list::const_iterator i = entities.begin();
  entity_list::const_iterator i_end = entities.end();
  for ( ; i!=i_end; ++i )
  {

    entity *e = *i;

    if ( e )
      e->set_entity_pool(NULL);
  }
*/
}

entity* entity_pool::acquire( unsigned int flags )
{
	PANIC;
	return NULL;
}

void entity_pool::add( entity* e )
{
  assert( !e->is_ext_flagged(EFLAG_EXT_UNUSED) );
  entities.push_back( e );
  e->set_entity_pool( this );

}

#include <algorithm>

void entity_pool::release( entity* e )
{
  assert( find(entities.begin(),entities.end(),e)!=entities.end() && !e->is_ext_flagged(EFLAG_EXT_UNUSED) );
  e->set_ext_flag( EFLAG_EXT_UNUSED, true );
  e->release();
  e->force_region( NULL );
  e->set_active( false );
  ++avail;
}



entity_pool_set::entity_pool_set()

: entity_pools(),

  aux_entity_pools()
{

}


entity_pool_set::~entity_pool_set()
{
  purge();
}


entity* entity_pool_set::acquire( const stringx& name, unsigned int flags )
{
	PANIC;
	return NULL;
}


entity* entity_pool_set::acquire_beam( unsigned int flags )
{
  // find an entry corresponding to the given name

  typedef entity_pool* ep_ptr_t;
  ep_ptr_t& epool = entity_pools["_BEAM"];
  if ( epool == NULL )
  {
    // need to construct the base entity

    entity* base = g_world_ptr->add_beam( entity_id::make_unique_id(), flags );

    epool = NEW entity_pool();
    epool->add( base );
    return base;
  }
  else
  {
    // acquire an entity from the given pool
    return epool->acquire( flags );
  }
}

void entity_pool_set::purge()
{

  set_t::const_iterator i = entity_pools.begin();
  set_t::const_iterator i_end = entity_pools.end();
  for ( ; i!=i_end; ++i )
    delete (*i).second;

  entity_pools.clear();
  aux_t::const_iterator ai = aux_entity_pools.begin();
  aux_t::const_iterator ai_end = aux_entity_pools.end();
  for ( ; ai!=ai_end; ++ai )
    delete *ai;
  aux_entity_pools.resize(0);
}
