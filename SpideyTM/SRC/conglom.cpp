// conglom.cpp
// Copyright (c) 1999-2000 Treyarch Invention LLC.  ALL RIGHTS RESERVED.
#include "global.h"

//!#include "character.h"
#include "conglom.h"
#include "wds.h"
#include "light.h"
#include "lightmgr.h"
//#include "oserrmsg.h"
#include "particle.h"
//!#include "ladder.h"
#include "pmesh.h"
#include "osdevopts.h"
#include "renderflav.h"
#include "iri.h"
#include "colgeom.h"
#include "colmesh.h"
// BIGCULL #include "scanner.h"
#include "controller.h"
// BIGCULL #include "handheld_item.h"
#include "entity_maker.h"
#include "physical_interface.h"
// BIGCULL #include "damage_interface.h"
#include "ai_interface.h"
#include "collide.h"
#include "profiler.h"

//#include "ngl_support.h"

#ifdef TARGET_GC
#include <dolphin/mtx.h>
#endif //TARGET_GC

///////////////////////////////////////////////////////////////////////////////
// Generic Constructors
///////////////////////////////////////////////////////////////////////////////

conglomerate::conglomerate( const entity_id& _id, unsigned int _flags )
  : entity( _id, ENTITY_CONGLOMERATE, _flags )
{
}

conglomerate::conglomerate( const entity_id& _id,
                            entity_flavor_t _flavor,
                            unsigned int _flags )
  : entity( _id, _flavor, _flags )
{
}

conglomerate::~conglomerate()
{
  pentity_vector::iterator i;
  for ( i=members.begin(); i!=members.end(); ++i )
  {
    entity *ent = (*i);
//    warning("%s", (ent != NULL ? ent->get_name().c_str() : "NULL"));


    if ( ent->get_bone_idx() >= 0 )
      delete ent;
    else
      g_entity_maker->destroy_entity( ent );
  }

  members.resize(0); // PEH BETA
  names.resize(0); // PEH BETA
  parents.resize(0); // PEH BETA
}

#include "skeleton_interface.h"
#include "hard_attrib_interface.h"
#include "soft_attrib_interface.h"


///////////////////////////////////////////////////////////////////////////////
// File I/O
///////////////////////////////////////////////////////////////////////////////


conglomerate::conglomerate( chunk_file& fs,
                            const entity_id& _id,
                            entity_flavor_t _flavor,
                            unsigned int _flags )
  : entity( fs, _id, _flavor, _flags )
{
	PANIC;
}


void conglomerate::read_node( chunk_file& fs, entity* _parent, bool has_skeleton )
{
  chunk_flavor cf;

  // node name
  stringx name;
  serial_in( fs, &name );
  name.to_upper();


  // member id is "conglomerate_id.node_id"
  entity_id _id( (id.get_val()+"_"+name).c_str() );
  entity* ent = NULL;

  serial_in( fs, &cf );

  // optional anim ID
  anim_id_t ent_animid = NO_ID;
  if ( cf == chunk_flavor("animid") )
  {
    stringx idstr;
    serial_in( fs, &idstr );
    ent_animid = anim_id_manager::inst()->anim_id( idstr );
    serial_in( fs, &cf );
  }

  // optional po
  po ent_po = po_identity_matrix;
  if ( cf == chunk_flavor("po") )
  {
    serial_in( fs, &ent_po );

    serial_in( fs, &cf );
  }

  // entity type is one of the following:

  // generic entity

  if ( cf == chunk_flavor("entity") )
  {
    ent = g_entity_maker->create_entity( fs, _id, flags, false );
  }


#if 0 // BIGCULL
  else if ( cf == chunk_flavor("switch") )
  {
    ent = (entity*)g_world_ptr->add_switch_obj( fs, _id, flags );

  }
#endif // BIGCULL
  else if ( cf == chunk_flavor("polytube") )
  {

    ent = (entity*)g_world_ptr->add_polytube( fs, _id, flags );
  }

  else if ( cf == chunk_flavor("lensflare") )
  {
    ent = (entity*)g_world_ptr->add_lensflare( fs, _id, flags );
  }


  // physical entity
  else if ( cf == chunk_flavor("physical") )

  {
    assert( g_entity_maker->get_owning_widget() == NULL );
//    ent = (entity*)g_world_ptr->add_physent( fs, _id, flags );
    assert(0);
  }


  // light source
  else if ( cf == chunk_flavor("light") )
  {
    assert( g_entity_maker->get_owning_widget() == NULL );
    ent = (entity*)g_world_ptr->add_light_source( fs, _id, flags );
  }

  // trigger
  else if( cf == chunk_flavor( "trigger" ) )
  {
    assert( g_entity_maker->get_owning_widget( ) == NULL );
    convex_box binfo;

//#pragma fixme( "replace this with a serial_in( fs, trigger_props* ) or some such. -mkv 4/6/01" )

    for( serial_in( fs, &cf ); cf != CHUNK_END; serial_in( fs, &cf ) )
    {

      if( cf == chunk_flavor( "convxbox" ) )
      {
        serial_in( fs, &binfo );
      }
      else
      {
        // some other data in the future?
      }

    }


    ent = g_world_ptr->add_box_trigger( _id, ent_po, binfo, NULL );
  }


  // particle generator
  else if ( cf == chunk_flavor("partsys") )
  {
    assert( g_entity_maker->get_owning_widget() == NULL );

    // read particle system data (for now, filename only)
    stringx fname;
    serial_in( fs, &fname );
    serial_in( fs, &cf );
    if ( cf != CHUNK_END )
      error( fs.get_name() + ": unexpected chunk '" + cf.to_stringx() + "' in particle_system node" );
    // create particle_generator
    ent = (entity*)g_world_ptr->add_particle_generator( fname, _id, flags );
  }

  // ladder
/*!  else if ( cf == chunk_flavor("ladder") )
  {
    assert( g_entity_maker->get_owning_widget() == NULL );
    ent = (entity*)g_world_ptr->add_ladder( fs, _id, flags );
  }
!*/
  // physical entity
#if 0 // BIGCULL
  else if ( cf == chunk_flavor("scanner") )
  {

    assert( g_entity_maker->get_owning_widget() == NULL );
    ent = (entity*)g_world_ptr->add_scanner( fs, _id, flags );
  }
#endif // BIGCULL

  else
    error( fs.get_name() + ": unexpected chunk '" + cf.to_stringx() + "' in conglomerate" );


  // set entity stuff
  assert( ent );

  ent->set_anim_id( ent_animid );

  // massage 'has_skeleton' if our bone index is < 0
//#pragma fixme("This is incorrect;  it assumes all bones or no bones  jdf 4-19-01")
  if( ent->get_bone_idx() < 0 ) {

    has_skeleton = false;
  }

  assert(has_link_ifc());
  if (has_skeleton) {
      // set skeleton to handle abs_po
      ent->link_ifc()->link_does_not_manage_abs_po();
  }

  ent->set_rel_po_no_children( ent_po );
  ent->link_ifc()->set_parent( _parent );


  // <Reid
  ent->compute_visual_xz_radius_rel_center();
  // Reid>

  // non-uniform scaling is not allowed;
  // uniform scaling is allowed only on entities that have no collision geometry
  check_po( ent );

  // we can't compute bounding boxes here because when the first
  // conglomerate is constructed it doesn't know it's po yet.  Now
  // compute_bounding_box is overloaded for conglomerates--cbb
#if 0
  // <Reid
  // we want each static entity to have an axis-aligned bounding box for collisions
  // (note that the bounding box will only be created if the entity has a cg_mesh)
  if ( ent->get_flavor()==ENTITY_ENTITY && !ent->is_flagged(EFLAG_MISC_NONSTATIC) )

    ent->compute_bounding_box();
  // Reid>
#endif

  // add to member list
  add_member( ent, name );

/*
  stringx parent_name = "<NONE>";

  if(_parent)
    parent_name = _parent->get_name();
*/

  // read children, if any
  for ( serial_in(fs,&cf); cf!=CHUNK_END; serial_in(fs,&cf) )
  {
    if ( cf == chunk_flavor("node") )
      read_node( fs, ent, has_skeleton );
    else
      error( fs.get_name() + ": unexpected chunk '" + cf.to_stringx() + "' in conglomerate" );
  }
}



void conglomerate::compute_radius()
{
//  set_rel_po( po_identity_matrix );

//  update_abs_po();

  // can take radius from collision mesh, if any
  collision_geometry* cg = get_colgeom();
  if ( cg && cg->get_type()==collision_geometry::MESH )
  {
    set_radius( cg->get_radius() );
  }

  pentity_vector::iterator i = members.begin();
  pentity_vector::iterator i_end = members.end();
  for ( ; i!=i_end; ++i )
  {
    entity* e = *i;
    rational_t r = (e->get_abs_position()-get_abs_position()).length() + e->get_radius();
    if ( r > get_radius() )
      set_radius( r );
  }
}


// This function allows parsing instance data according to entity type.
// If it recognizes the given chunk_flavor as a chunk of instance
// data for this type, it will parse the data; otherwise it will hand
// the parsing up to the parent class.
bool conglomerate::parse_instance( const stringx& pcf, chunk_file& fs )

{
  if ( pcf == stringx("scanner") )

  {
    // look for a member that is a scanner
    pentity_vector::iterator i = members.begin();
    pentity_vector::iterator i_end = members.end();
    for ( ; i!=i_end; i++ )
    {
      if ( (*i)->get_flavor() == ENTITY_SCANNER )
        break;
    }
    if ( i == i_end )
      error( get_id().get_val() + ": parse_instance(): no scanner member found in conglomerate" );
    else
      return (*i)->parse_instance( pcf, fs );
  }
  else
    return entity::parse_instance( pcf, fs );
  return false;
}


///////////////////////////////////////////////////////////////////////////////
// Instancing
///////////////////////////////////////////////////////////////////////////////

entity* conglomerate::make_instance( const entity_id& _id,
                                     unsigned int _flags ) const
{
  conglomerate* newcg = NEW conglomerate( _id, _flags );
  newcg->copy_instance_data( *this );
  return (entity*)newcg;
}



void conglomerate::copy_instance_data( const conglomerate& b )

{
	PANIC;
}



///////////////////////////////////////////////////////////////////////////////

// Misc.
///////////////////////////////////////////////////////////////////////////////


void conglomerate::set_visible( bool a )
{
  entity::set_visible( a );
  pentity_vector::const_iterator i = members.begin();
  pentity_vector::const_iterator i_end = members.end();
  for ( ; i!=i_end; ++i )
  {
    entity* e = *i;
    if ( e->get_bone_idx() < 0 )
      e->set_visible( a );

  }
}

bool conglomerate::is_still_visible() const
{

  if ( is_visible() )
    return true;

  pentity_vector::const_iterator i = members.begin();
  pentity_vector::const_iterator i_end = members.end();
  for ( ; i!=i_end; ++i )
  {

    entity* e = *i;

    if ( e->is_still_visible() )
      return true;
  }
  return false;

}



rational_t conglomerate::terrain_radius() const
{

  rational_t r = get_visual_radius();
  if ( r<0.1f && get_radius()>r )
    r = get_radius();
  return r;
}



void conglomerate::force_region( region_node* r )
{
  entity::force_region( r );
  pentity_vector::const_iterator i = members.begin();

  pentity_vector::const_iterator i_end = members.end();
  for ( ; i!=i_end; ++i )
  {
    entity* e = *i;
    if ( e->get_bone_idx() < 0 )
      e->force_region( r );
  }
}


void conglomerate::force_current_region()
{

  entity::force_current_region();
  pentity_vector::const_iterator i = members.begin();

  pentity_vector::const_iterator i_end = members.end();
  for ( ; i!=i_end; ++i )
  {
    entity* e = *i;
    if ( e->get_bone_idx() < 0 )
      e->force_current_region();
  }
}


void conglomerate::unforce_regions()
{

  entity::unforce_regions();
  pentity_vector::const_iterator i = members.begin();
  pentity_vector::const_iterator i_end = members.end();
  for ( ; i!=i_end; ++i )
  {
    entity* e = *i;
    if ( e->get_bone_idx() < 0 )
      e->unforce_regions();
  }

}


#include <algorithm>

void conglomerate::add_member( entity* ent, const stringx& nodename )
{
  members.push_back( ent );
  names.push_back( nodename );

  ent->set_ext_flag(EFLAG_EXT_MEMBER, true);


  char pidx = -1;
  assert( ent->has_parent() );
  pentity_vector::iterator ei;
  ei = std::find( members.begin(), members.end(), ent->link_ifc()->get_parent() );
  if ( ei != members.end() )
    pidx = ei - members.begin();
  parents.push_back( pidx );

}



// get pointer to member by node name (found by reconstructing member id from
// conglomerate id plus node name)
entity* conglomerate::get_member( const stringx& nodename )
{
  //stringx searchname = nodename;
  //searchname.to_upper();
  pentity_vector::const_iterator i = members.begin();
  pentity_vector::const_iterator i_end = members.end();

  std::vector<stringx>::const_iterator j = names.begin();
  for ( ; i!=i_end; ++i,++j )
  {
    if ( *j == nodename )
      return *i;

  }
  // not found

  return NULL;
}

const stringx& conglomerate::get_member_nodename( entity *member )
{
  if ( member != NULL )

  {
    pentity_vector::const_iterator i = members.begin();
    pentity_vector::const_iterator i_end = members.end();
	std::vector<stringx>::const_iterator j = names.begin();
    for ( ; i!=i_end; ++i,++j )
    {
      entity* e = *i;
      if ( e == member )
        return *j;
    }
  }
  return empty_string;
}

bool conglomerate::has_member(entity *ent) const
{
  if ( ent != NULL )

  {
    pentity_vector::const_iterator i = members.begin();
    pentity_vector::const_iterator i_end = members.end();
    for ( ; i!=i_end; ++i )
    {
      entity* e = *i;
      if ( e == ent )
        return true;
    }
  }
  return false;
}


entity* conglomerate::get_member_by_flavor( entity_flavor_t flav )
{
  pentity_vector::const_iterator i = members.begin();
  pentity_vector::const_iterator i_end = members.end();
  for ( ; i!=i_end; ++i )
  {
    entity* e = *i;
    if ( e->get_flavor() == flav )
      return e;

  }
  return NULL;
}

void conglomerate::compute_sector( terrain& ter, bool use_high_res_intersect )
{
  entity::compute_sector( ter, use_high_res_intersect );

  pentity_vector::const_iterator i = members.begin();
  pentity_vector::const_iterator i_end = members.end();

  for ( ; i!=i_end; ++i )
  {
    entity* e = *i;
    if ( e->get_bone_idx() < 0 )
      e->update_region( true );
//      e->compute_sector( ter, use_high_res_intersect );
  }
}

void conglomerate::compute_bounding_box()
{
  pentity_vector::const_iterator i = members.begin();
  pentity_vector::const_iterator i_end = members.end();
  for ( ; i!=i_end; ++i )
  {
    entity* e = *i;
    if ( e->get_bone_idx() < 0 )
    {
      if ( e->is_statically_sortable() )
        e->compute_bounding_box();
    }
  }
}


// *********** ifl operations *************
void conglomerate::ifl_lock(int frame_number)
{
  entity::ifl_lock(frame_number);
  pentity_vector::const_iterator i = members.begin();
  pentity_vector::const_iterator i_end = members.end();

  for ( ; i!=i_end; ++i )
  {
    entity* e = *i;
    if ( e->get_bone_idx() < 0 )
      e->ifl_lock( frame_number );
  }
}



void conglomerate::ifl_play()
{
  entity::ifl_play();
  pentity_vector::const_iterator i = members.begin();
  pentity_vector::const_iterator i_end = members.end();
  for ( ; i!=i_end; ++i )

  {
    entity* e = *i;
    if ( e->get_bone_idx() < 0 )
      e->ifl_play();
  }
}


void conglomerate::ifl_pause()
{
  entity::ifl_pause();

  pentity_vector::const_iterator i = members.begin();
  pentity_vector::const_iterator i_end = members.end();
  for ( ; i!=i_end; ++i )
  {
    entity* e = *i;

    if ( e->get_bone_idx() < 0 )
      e->ifl_pause();
  }
}

void conglomerate::set_render_color( const color32 new_color )
{
  entity::set_render_color( new_color );
  pentity_vector::const_iterator i = members.begin();
  pentity_vector::const_iterator i_end = members.end();
  for ( ; i!=i_end; ++i )
  {
    entity* e = *i;
    if ( e->get_bone_idx() < 0 )
      e->set_render_color( new_color );

  }
}

// *****************************************


bool conglomerate::handle_enx_chunk( chunk_file& fs, stringx& label )
{
	PANIC;
	return true;
}

void conglomerate::set_min_detail(int md)
{
	PANIC;
}

void conglomerate::apply_destruction_fx()
{
	PANIC;
}


entity *conglomerate::get_child( entity *ent, entity *prev_child )
{
  if(ent != NULL)
  {
    pentity_vector::iterator it;

    if(prev_child != NULL)
    {
      it = std::find( members.begin(), members.end(), prev_child);

      // skip the prev_child if found

      if(it != members.end())
        ++it;
    }
    else
      it = members.begin();

    while( it != members.end() )
    {
      if((*it)->link_ifc()->get_parent() == ent)
        return((*it));


      ++it;

    }
  }


  return NULL;

}



void conglomerate::updatelighting( time_value_t t, const int playerID )
{
  if (lightmgr)
  {
    region_node* my_region_node = get_region();
    region* my_region = my_region_node ? my_region_node->get_data() : 0;
    lightmgr->set_bound_sphere(sphere(get_visual_center(),get_visual_radius()));
    lightmgr->frame_advance(my_region, t, playerID); // let it update light blending/fading
  }
}



void conglomerate::frame_advance(time_value_t t)

{
	PANIC;
}


light_manager* conglomerate::get_light_set() const
{
  if (lightmgr) return lightmgr;
  return entity::get_light_set();
}

static instance_render_info iri;


bool loresmodelbydefault=false;



void conglomerate::render( camera* camera_link, rational_t detail, render_flavor_t render_flavor, rational_t entity_translucency_pct )
{
	PANIC;
}

bool zerotints=true;

int maxshadowbones=40;
bool shadowhasbones=true;

extern float shadow_reflective_value;

void conglomerate::rendershadow( camera* camera_link, rational_t detail, render_flavor_t render_flavor, rational_t entity_translucency_pct, rational_t scale )
{
	PANIC;
}

/*
void conglomerate::add_members_to_bones( entity* ent, matrix4x4* bones, int* num_bones )
{
  int bone_idx = ent->get_bone_idx();
  assert( bone_idx < MAX_BONES );

  if(bone_idx >= 0)

  {
    matrix4x4* bone_m4x4 = &bones[bone_idx];
    *(po*)bone_m4x4 = ent->get_abs_po();

    // and that's that
    if( (bone_idx+1)>*num_bones)
      *num_bones = bone_idx+1;
  }
}
*/
void conglomerate::frame_done_including_members()
{
  entity::frame_done();

  pentity_vector::const_iterator i = members.begin();
  pentity_vector::const_iterator i_end = members.end();
  for ( ; i!=i_end; ++i )
  {

    entity* e = *i;

    if ( e->is_a_conglomerate() )
      ((conglomerate *)e)->frame_done_including_members();
    else
      e->frame_done();
  }
}


///////////////////////////////////////////////////////////////////////////////
// entity_maker caching interface

void conglomerate::acquire( unsigned int _flags )

{
  entity::acquire( _flags );
  pentity_vector::iterator i = members.begin();

  pentity_vector::iterator i_end = members.end();
  for ( ; i!=i_end; ++i )
  {
    entity* e = *i;
    assert( e->get_entity_pool() != NULL );
    *i = e->get_entity_pool()->acquire( _flags );
  }
  i = members.begin();
  std::vector<char>::const_iterator j = parents.begin();
  for ( ; i!=i_end; ++i,++j )
  {
    entity* e = *i;
    unsigned char pidx = (unsigned char)(*j);
    if ( pidx == 0xFF )
      e->link_ifc()->set_parent( this );
    else
      e->link_ifc()->set_parent( members[pidx] );
  }
}

void conglomerate::release()

{
  entity::release();

  pentity_vector::const_iterator i = members.begin();
  pentity_vector::const_iterator i_end = members.end();
  for ( ; i!=i_end; ++i )
  {

    entity* e = *i;

    g_entity_maker->release_entity( e );
//!   e->set_parent_rel( NULL );
    e->link_ifc()->clear_parent();
  }
}


void conglomerate::set_ext_flag_recursive(register unsigned int f, register bool set)
{
  entity::set_ext_flag_recursive(f, set);

  pentity_vector::const_iterator i = members.begin();
  pentity_vector::const_iterator i_end = members.end();
  for ( ; i!=i_end; ++i )

  {
    entity* e = *i;
    e->set_ext_flag_recursive(f, set);
  }
}
