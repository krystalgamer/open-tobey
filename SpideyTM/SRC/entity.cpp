//--------------------------------------------------
// ENTITY.CPP
// Copyright (c) 1999-2000 Treyarch Invention LLC.  ALL RIGHTS RESERVED.
//
// Home of entity and entity_id
//--------------------------------------------------
#include "global.h"

#include "entity.h"
#include "entity_anim.h"
#include "region.h"
#include "ini_parser.h"
#include "colgeom.h"
#include "mbi.h"
#include "profiler.h"
#include "widget_entity.h"
#include "bound.h"
#include "colmesh.h"

// @Ok
// @Matching
INLINE char* strdupcpp(const char* str)
{
  char* retstr;

  retstr = NEW char[ strlen(str)+1 ];
  strcpy( retstr, str );
  return retstr;
}
////////////////////////////////////////////////////////////////////////////////
//  Globals
////////////////////////////////////////////////////////////////////////////////

// These strings must be maintained in a 1-1 correspondence with entity_flavors.
// This is somewhat automatically enforced by the entity_manager constructor, using
// the designated stringx "null" at the end of the list.

// @Ok
// @Patch - made it match spidey
const char* entity_flavor_names[NUM_ENTITY_FLAVORS+1] =
{
	"CAMERA",
	"ENTITY",
	"MARKER",
	"MIC",
	"LIGHT_SOURCE",
	"PARTICLE",
	"PHYSICAL",
	"ITEM",
	"LIGHT",
	"MOBILE",
	"CONGLOMERATE",
	"TURRET",
	"BEAM",
	"SCANNER",
	"MORPHABLE_ITEM",
	"SKY",
	"MANIP",
	"SWITCH",
	"BOX_TRIGGER",
	"POLYTUBE",
	"LENSFLARE",
	"NEOLIGHT",
	"GAME_CAMERA",
	"INTERP_CAMERA",
	"SPIDERMAN_CAMERA",
	"MARKY_CAMERA",
	"MOUSELOOK_CAMERA",
	"SNIPER_CAMERA",
	"AI_CAMERA",

	"null"
};

// @Ok
// @Matching
// return entity_flavor_t corresponding to given string (NUM_ENTITY_FLAVORS if not found)
entity_flavor_t to_entity_flavor_t( const stringx& s )
{
  int i;
  for ( i=0; i<NUM_ENTITY_FLAVORS; ++i )
  {
    if ( s == entity_flavor_names[i] )
      break;
  }
  return (entity_flavor_t)i;
}


////////////////////////////////////////////////////////////////////////////////
//  entity_id
////////////////////////////////////////////////////////////////////////////////

// @Ok
// @Matching
INLINE entity_id::entity_id(const char* name)
{
  set_entity_id(name);
}

// @Ok
// @Matching
INLINE void entity_id::set_entity_id(const char* name)
{
  // from now on, entity_id's must be uppercase.  tools are responsible
  // for converting.
#ifndef NDEBUG
  for(unsigned i=0;i<strlen(name);++i)
  {
    if(islower(name[i]))
    {
      stringx warnmsg = stringx(name) + " is not all uppercase.";
      warning( warnmsg );
      break;  // so we can ignore and find more
    }
  }
#endif
  name_to_number_map& name_to_number = entity_manager::inst()->name_to_number;
  name_to_number_map::iterator it =
    name_to_number.find( charstarwrap( const_cast<char*>(name) ) );
  if( it == name_to_number.end() )
  {
    char* newname = strdupcpp( name );
/*    char* newname = NEW char[strlen(name)+1];
    memcpy(newname,name,strlen(name));
    newname[strlen(name)]=0;*/
	std::pair<name_to_number_map::iterator,bool> result = name_to_number.insert(
      name_to_number_map::value_type( charstarwrap( newname ), entity_manager::inst()->number_server++ ) );
    // will only insert if not in there already
    val = (*result.first).second;
  }
  else
    val = (*it).second;
}


// @Ok
// @Matching
void entity_id::delete_entity_id(entity_id id)
{
  name_to_number_map::iterator found;

  name_to_number_map& name_to_number = entity_manager::inst()->name_to_number;
  for (found = name_to_number.begin(); found != name_to_number.end(); ++found)

  {
    if ((*found).second == id.val)
    {
      delete[] (*found).first.str;
      name_to_number.erase(found);
      return;
    }
  }
  error("Tried to delete unrecognized entity_id: " + itos(id.val)); // obviously it isn't in the list, so we can't get the name
  //error("Tried to delete unrecognized entity_id: " + id.get_val());
  assert(false);
}


// @Ok
// @Matching
stringx entity_id::get_val() const
{
  name_to_number_map::iterator found;

  name_to_number_map& name_to_number = entity_manager::inst()->name_to_number;
  for( found = name_to_number.begin(); found != name_to_number.end(); ++found)
  {
    if( (*found).second == val )
    {
      return stringx((*found).first.str);
    }
  }

  assert(false);
  return stringx("garbage");

}



#define UNIQUE_ENTITY_ID_BASE "_ENTID_"
static int unique_entity_id_idx=0;
#ifdef DEBUG
//int agorra;
#endif
// @Ok
// @Matching
entity_id &entity_id::make_unique_id()
{
  static entity_id ret;
  stringx name( UNIQUE_ENTITY_ID_BASE );
#ifdef DEBUG
  //if (unique_entity_id_idx==258 || unique_entity_id_idx==271)

    //agorra = 1;
#endif
  name += itos( unique_entity_id_idx++ ); //name_to_number.size() );
  ret.set_entity_id(name.c_str());
  return ret;
}


// @Ok
// @Matching
void serial_in(chunk_file& io,entity_id* eid)
{
  stringx in;
  serial_in(io,&in);

  in.to_upper();
  entity_id id_in(in.c_str());
  *eid = id_in;
}

////////////////////////////////////////////////////////////////////////////////
//  entity
////////////////////////////////////////////////////////////////////////////////
unsigned int entity::visit_key = 0;
unsigned int entity::visit_key2 = 0;

entity::entity( const entity_id& _id, unsigned int _flags )
  : bone()
{
  _construct( _id, ENTITY_ENTITY, NO_ID, _flags );
}

entity::entity( const entity_id& _id, unsigned int _flags, const po & last_po_init_val )
  : bone()
{
  _construct( _id, ENTITY_ENTITY, NO_ID, _flags );
  last_po = NEW po;

  *last_po = last_po_init_val;
}


entity::entity( const entity_id& _id,
                entity_flavor_t _flavor,
                unsigned int _flags )
  : bone()
{
  _construct( _id, _flavor, NO_ID, _flags );
}


entity::entity( const entity_id& _id,
                entity_flavor_t _flavor,
                anim_id_t _anim_id )
  : bone()
{
  _construct( _id, _flavor, _anim_id );
}


entity::entity( const entity_id& _id,

                entity_flavor_t _flavor,
                const char* _anim_id )
  : bone()
{

  if ( _anim_id )

    _construct( _id, _flavor, anim_id_manager::inst()->anim_id( _anim_id ) );
  else
    _construct( _id, _flavor, NO_ID );
}



entity::~entity()
{
  destruct();

}


void entity::destruct()
{
	// @TODO
	PANIC;
}



void entity::_construct( const entity_id& _id,
                         entity_flavor_t _flavor,
                         anim_id_t _anim_id,
                         unsigned int _flags,
                         const po& _my_po )

{
	// @TODO
	PANIC;
}

int entity::get_hero_id( void )
{
	// @TODO
	PANIC;
	return 0;
}

void entity::initialize()
{
	// @TODO
	PANIC;
}


// this function will be called when an entity is acquired from the entity_maker cache
void entity::acquire( unsigned int _flags )
{
	// @TODO
	PANIC;
}

// this function will be called when the entity is released to the entity_maker cache

void entity::release()
{
  if ( get_anim() )
    get_anim()->detach();
  remove_from_terrain();
/*!  clear_parent();
  if ( ci )
  {
    child_list::const_iterator i = ci->children.begin();
    child_list::const_iterator i_end = ci->children.end();

    for ( ; i!=i_end; ++i )
    {
      entity* e = *i;
      assert( e->pi );
      // clear parent without affecting position in world
      e->pi->parent = NULL;
      e->my_po = e->pi->my_abs_po;
      e->update_abs_po();
    }

    ci->children.resize(0);
  }

!*/

//  if ( anim_trees != NULL )

  {
    int i;
    for ( i=0; i<MAX_ANIM_SLOTS; ++i )

      kill_anim( i );
  }
}


///////////////////////////////////////////////////////////////////////////////
// NEWENT File I/O
///////////////////////////////////////////////////////////////////////////////

entity::entity( chunk_file& fs,
                const entity_id& _id,
                entity_flavor_t _flavor,
                unsigned int _flags )
  :   bone()
{
	// @TODO
	PANIC;
}


void entity::optimize()

{
	// @TODO
	PANIC;
}


void entity::read_enx(chunk_file& fs)
{
  stringx label;
  serial_in(fs, &label);
//  read_enx(fs, label);

  // read all the labels...

  while (label.length() > 0)
  {
    if (!handle_enx_chunk(fs, label))
		{
#ifdef WEENIEASSERT  // this triggers every time KSPS starts so it's gon
      error( fs.get_filename() + ": unknown chunk '" + label +"' for entity " + get_name() + "(" + entity_flavor_names[get_flavor()] + ")");
#endif
		}

    serial_in(fs, &label);

  }
}


/*

void entity::read_enx( chunk_file& fs, stringx& lstr )
{
  // read all the labels...
  while( lstr.length() > 0 )

  {
    if(!handle_enx_chunk( fs, lstr ))
      error( fs.get_filename() + ": unknown chunk '" + lstr +"' for entity " + get_name() + "(" + entity_flavor_names[get_flavor()] + ")");

    serial_in( fs, &lstr );

  }
}
*/

bool entity::handle_enx_chunk( chunk_file& fs, stringx& label )

{
	// @TODO
	PANIC;
	return true;
}

/*
void entity::read_enx( chunk_file& fs, stringx& lstr )
{
  stringx label = lstr;


  // optional collision flags

  if ( label == "collision_flags:" )
  {
    // at the moment, these flags only apply to cg_mesh
    if ( !get_colgeom() || get_colgeom()->get_type()!=collision_geometry::MESH )
      error( get_id().get_val() + ": error reading " + fs.get_name() + ": cannot apply collision flags to an entity without a collision mesh" );

    cg_mesh* m = static_cast<cg_mesh*>( get_colgeom() );
    serial_in( fs, &label );
    for ( ; label!=chunkend_label; serial_in(fs,&label) )
    {
      if ( label=="camera_collision" || label=="CAMERA_COLLISION" )
        m->set_flag( cg_mesh::FLAG_CAMERA_COLLISION, true );
      else if ( label=="no_entity_collision" || label=="NO_ENTITY_COLLISION" )
        m->set_flag( cg_mesh::FLAG_ENTITY_COLLISION, false );
    }
    serial_in( fs, &label );
  }

  // optional collision flags
  if ( label == "targeting_flags:" )
  {
    error(fs.get_filename() + ": targeting_flags no longer supported. Move data into destroy_info.");
  }


  if ( label == "destroy_info:" )
  {
    if(!destroy_info)

      destroy_info = NEW destroyable_info(this);

    destroy_info->read_enx(fs, label);
    serial_in( fs, &label );
  }


  if ( label == "action_flags:" )
  {
    serial_in( fs, &label );
    for ( ; label!=chunkend_label; serial_in(fs,&label) )

    {
      if ( label=="actionable" || label=="ACTIONABLE" )
      {

//        bool actionable;
//        serial_in( fs, &actionable );
        set_actionable( true );

      }
      else if ( label=="character_anim" || label=="CHARACTER_ANIM" )
      {
        serial_in( fs, &character_action_anim);
        load_anim( character_action_anim );
      }
      else if ( label=="use_facing" || label=="USE_FACING" )
      {
//        bool use_facing;
//        serial_in( fs, &use_facing );
        set_action_uses_facing( true );
      }
    }
    serial_in( fs, &label );
  }

  if ( label == "scan_flags:" )
  {
    serial_in( fs, &label );
    for ( ; label!=chunkend_label; serial_in(fs,&label) )
    {
      if ( label=="scanable" || label=="SCANABLE" )
      {
//        bool scanable;
//        serial_in( fs, &scanable );
        set_scanable( true );
      }
      else if ( label=="beamable" || label=="BEAMABLE" )
      {
//        bool beamable;
//        serial_in( fs, &beamable );
        set_beamable( true );
      }
    }
    serial_in( fs, &label );

  }

  if ( label == "door_flags:" )
  {
    serial_in( fs, &label );
    for ( ; label!=chunkend_label; serial_in(fs,&label) )

    {
      if ( label=="is_door" || label=="IS_DOOR" )
        set_door( true );
      else if ( label=="open" || label=="OPEN" )
        set_door_closed( false );
    }
    serial_in( fs, &label );
  }

  if ( label == "script_objects:" )
    error( fs.get_filename() + ": script object attach is no longer supported" );
}
*/


///////////////////////////////////////////////////////////////////////////////
// Old File I/O

///////////////////////////////////////////////////////////////////////////////

entity::entity( const stringx& entity_fname,
                const entity_id& _id,
                entity_flavor_t _flavor,
                bool delete_stream )
  : bone()
{
	// @TODO
		PANIC;
}


///////////////////////////////////////////////////////////////////////////////
// Instancing

///////////////////////////////////////////////////////////////////////////////

entity* entity::make_instance( const entity_id& _id,
                               unsigned int _flags ) const
{

//  if(_id.get_val() == "FORGE01_FORGE")
//    int b = 1;

  entity* ent = NEW entity( _id, _flags );
  ent->copy_instance_data( *this );

  return ent;

}

void entity::copy_instance_data( const entity& b )
{
	// @TODO
	PANIC;
}



///////////////////////////////////////////////////////////////////////////////

// Interfaces
///////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Event signals
/////////////////////////////////////////////////////////////////////////////


static const char* entity_signal_names[] =

{
  #define MAC(label,str)  str,
  #include "entity_signals.h"

  #undef MAC
};

unsigned short entity::get_signal_id( const char *name )
{
  unsigned idx;


  for( idx = 0; idx < (sizeof(entity_signal_names)/sizeof(char*)); ++idx )
  {
    // compare with the end of entity_signal_name string
    // so you don't need to add the "entity::" part if you don't want
    unsigned offset = strlen(entity_signal_names[idx])-strlen(name);


    if( offset > strlen( entity_signal_names[idx] ) )
      continue;

    if( !strcmp(name,&entity_signal_names[idx][offset]) )
      return( idx + PARENT_SYNC_DUMMY + 1 );
  }

  // not found
  return signaller::get_signal_id( name );
}

// This static function must be implemented by every class which can generate
// signals, and is called once only by the application for each such class;
// the effect is to register the name and local id of each signal with the
// signal_manager.  This call must be performed before any signal objects are
// actually created for this class (via signaller::signal_ptr(); see signal.h).
void entity::register_signals()
{

  // for descendant class, replace "entity" with appropriate string

  #define MAC(label,str)  signal_manager::inst()->insert( str, label );

  #include "entity_signals.h"
  #undef MAC
}

// This virtual function, used only for debugging purposes, returns the
// name of the given local signal
const char* entity::get_signal_name( unsigned short idx ) const
{
  assert( idx < N_SIGNALS );
  if ( idx <= (unsigned short)PARENT_SYNC_DUMMY )
    return signaller::get_signal_name( idx );
  else

    return entity_signal_names[idx-PARENT_SYNC_DUMMY-1];
}


///////////////////////////////////////////////////////////////////////////////
// Po stuff
///////////////////////////////////////////////////////////////////////////////


void entity::get_direction(vector3d* target) const
{
  *target = get_abs_po().get_facing();
}



void entity::get_velocity(vector3d* target) const

{
  if (has_parent())

  {
    const bone * next_parent = link_ifc()->get_parent();
    po relative_to_parent = get_rel_po();
    while( next_parent->has_parent() )
    {
      relative_to_parent = relative_to_parent * next_parent->get_abs_po();
      next_parent = next_parent->link_ifc()->get_parent();
    }
    vector3d vel;
    ((entity *)next_parent)->get_velocity(&vel);

    vector3d angvel;
    ((entity *)next_parent)->get_angular_velocity(&angvel);
    *target=vel + cross( relative_to_parent.get_position(),angvel);

  }
  else

  {
    if (mi && mi->frame_delta_valid)
    {
      *target = mi->frame_delta.get_position()*(1/mi->frame_time);
    }
    else
      *target=vector3d(0,0,0);
  }
}

void entity::get_angular_velocity(vector3d* target) const

{
  *target=vector3d(0,0,0);
}


// local workspace for adding entity to regions
static std::vector<region_node*> new_regions(32);  // permanent


void entity::compute_sector( terrain& ter, bool use_high_res_intersect )

{
#ifndef REGIONCULL
  START_PROF_TIMER(proftimer_compute_sector);

  set_needs_compute_sector(false);

  // while forced to region(s), sector is irrelevant;
  // also, we're not allowing limbs to compute their sector

  if ( !is_flagged(EFLAG_REGION_FORCED) && !is_a_limb_body() )
  {
    vector3d curpos = get_abs_position();
    rational_t poshash = POSHASH(curpos);
    if ( poshash != last_compute_sector_position_hash )
    {
      last_compute_sector_position_hash = poshash;
      sector* sec = ter.find_sector( curpos ); //terrain_position() );
      if (sec)
      {
        assert( sec->get_region() );
        if ( terrain_radius() > 0 )
        {
          // store region of origin
          center_region = sec->get_region();
          // entity obviously intersects the region he currently belongs to;
          // adjacent regions will be checked recursively

          // NOTE: new_regions is a local workspace for listing the regions he intersects
          new_regions.resize(0);

          region::prepare_for_visiting();
          _intersect( sec->get_region(), use_high_res_intersect );
          // now remove entity from regions he has departed and add him to regions he has entered
          _update_regions();

        }
        else
        {
          // entity with zero radius

          if ( !in_regions.empty() )
          {
            if ( sec->get_region() != *in_regions.begin() )
            {
              // entity moved from one region to another
              remove_from_regions();
              center_region = sec->get_region();
              add_region( sec->get_region() );
            }
          }
          else
          {
            center_region = sec->get_region();
            add_region( sec->get_region() );

          }
        }
      }
//      set_flag(EFLAG_MISC_COMP_SECT_THIS_FRAME,true);
      my_sector = sec;
    }
  }


  ADD_PROF_COUNT(profcounter_compute_sector, 1);
  STOP_PROF_TIMER(proftimer_compute_sector);
#endif
}


// add given region to list and add entity to region
bool entity::add_region( region_node* r )
{
#ifndef REGIONCULL
  if ( r && in_regions.insert( r ).second )

  {
    // this virtual function allows descendant types to be recognized when
    // adding them to regions, so that the region class can maintain lists of
    // different entity types as desired
#endif
    add_me_to_region( r->get_data() );
    return true;
#ifndef REGIONCULL
  }
  return false;
#endif
}


// these virtual functions allow types descended from entity to be
// recognized when adding them to regions, so that the region class can
// maintain lists of different entity types as desired
void entity::add_me_to_region( region* r )
{
  r->add( this );
}

void entity::remove_me_from_region( region* r )
{
  r->remove( this );
}

// remove entity from all regions in which he is currently listed
void entity::remove_from_regions()
{
#ifndef REGIONCULL
	for ( region_node_pset::iterator i=in_regions.begin(); i!=in_regions.end(); i++ )
    remove_me_from_region( (*i)->get_data() );
  in_regions.clear();
#endif

}


void entity::remove_from_terrain()
{

  remove_from_regions();
  my_sector = NULL;
  center_region = NULL;
}


// remove entity from regions no longer inhabited (according to NEW list), and
// add entity to regions newly inhabited
void entity::_update_regions()
{
#ifndef REGIONCULL
  region_node_pset::iterator i,j;
  for ( i=in_regions.begin(); i!=in_regions.end(); )
  {

    region* r = (*i)->get_data();
    if ( !r->already_visited() )
    {
      remove_me_from_region( r );

      j = i;
      ++j;
      in_regions.erase( i );

      i = j;
    }

    else
      ++i;
  }
  vector<region_node*>::iterator k;
  for ( k=new_regions.begin(); k!=new_regions.end(); k++ )
    add_region( *k );
#endif
}


// INTERNAL
// add entity to given region and recurse into any adjacent intersected regions
void entity::_intersect( region_node* r, bool use_high_res_intersect )
{
	// @TODO
	PANIC;
}

// force entity to belong to given region until un-forced (see below)

void entity::force_region( region_node* r )
{
  if ( !is_flagged(EFLAG_REGION_FORCED) )
  {
    // first forced region for this entity;
    // remove from previous regions (if any)
    remove_from_regions();
  }
  _set_region_forced_status();
  // add to region list and add entity to region
  add_region( r );
}

void entity::change_visrep( const stringx& new_visrep_name )
{
  unload_visual_rep( my_visrep );
  my_visrep = find_visual_rep( new_visrep_name );
}


// Position history stuff

// Rel pos is guarenteed to be ZEROVEC when first_time = true;
vector3d entity::get_frame_abs_delta_position( bool first_time, const vector3d& rel_delta_pos ) const

{
  vector3d deltapos=ZEROVEC;
  if (!first_time)
  {
    deltapos = get_rel_po().non_affine_inverse_scaled_xform(rel_delta_pos);
  }
  if (mi && mi->frame_delta_valid)

    deltapos += mi->frame_delta.get_position();

  else
    deltapos += ZEROVEC;
  if (has_parent())
  {
    deltapos = ((entity *)link_ifc()->get_parent())->get_frame_abs_delta_position(false,deltapos);
  }
  return deltapos;
}

void entity::invalidate_frame_delta()

{
  if (mi)
  {
    mi->last_frame_delta_valid = mi->frame_delta_valid;
    mi->frame_delta_valid = false;
    mi->frame_delta=po_identity_matrix;
  }

}




vector3d entity::get_last_position() const
{
  return get_abs_position()-get_frame_abs_delta_position();
}


void entity::set_created_entity_default_active_status()
{
  switch ( flavor )
  {

    case ENTITY_ENTITY:

    case ENTITY_MARKER:

    case ENTITY_MIC:
    case ENTITY_LIGHT_SOURCE:
    case ENTITY_CONGLOMERATE:
      set_active( false );
      break;
    default:
      set_active( true );

      break;
  }
}


void entity::set_last_po( const po& the_po )

{
  if (last_po)
    *last_po = the_po; //get_colgeom_root_po();
}



const po& entity::get_last_po()
{
  if (last_po)

    return *last_po;
  else
    return get_abs_po();
}

void entity::set_family_visible( bool _vis, bool _cur_variant_only )
{
  set_visible( _vis );
/*!  if( has_children() )
  {
    list<entity*>::iterator ch;
    for( ch = ci->children.begin(); ch != ci->children.end(); ++ch )
    {

      if( (!_cur_variant_only) || (*ch)->is_flagged( EFLAG_MISC_MEMBER_OF_VARIANT ) )
      {

        (*ch)->set_family_visible( _vis, _cur_variant_only );
      }
    }

  }
!*/
}

////////////////////////////////////////////////////////////////////////////////

light_manager* entity::get_light_set() const
{
  if (!my_light_mgr && has_parent())
    return ((entity *)link_ifc()->get_parent())->get_light_set();

  return my_light_mgr;
}

void entity::create_light_set()
{
	// @TODO
	PANIC;
}


// Lock material frames

void entity::ifl_lock(int frame_index)
{
  if (my_visrep)
  {
    if ((frame_index >= 0) && (frame_index<my_visrep->get_anim_length()))
      frame_time_info.set_ifl_frame_locked(frame_index);
  }
 // else warning("No visual representation is available on the entity.");
}

void entity::ifl_pause()
{
  if (my_visrep)
  {
    int frame_locked = frame_time_info.get_ifl_frame_locked();
    if (frame_locked < 0) // No lock or pause on entity. Do nothing if entity is locked or paused.
    {
      int period = my_visrep->get_anim_length();
      if (period < 0)
        warning("The animation length of each material should be the same.");
      else
      {
        int current_frame = frame_time_info.time_to_frame(period);
        ifl_lock(current_frame); // Pause calls lock
      }
    }
  }
 // else
 //   warning("No visual representation is available on the entity.");
}


void entity::ifl_play()
{
  if (my_visrep)
  {
    int period = my_visrep->get_anim_length();
    if (period < 0)
      warning("The animation length of each material should be the same.");
    else
    {
      /*
      if (period > 1)
        nglPrintf("aaaaaaaaaaaaaaaaa\n");
     */
      frame_time_info.compute_boost_for_play(period);
      frame_time_info.set_ifl_frame_locked(-1);
    }
  }
 // else
 //   warning("No visual representation is available on the entity.");
}



bool entity::is_statically_visually_sortable()
{
	// @TODO
PANIC;
	return true;
}

/*!
entity* entity::get_flavor_parent(entity_flavor_t flav)
{
  entity *ent = get_parent();
  while(ent != NULL && ent->get_flavor() != flav)
    ent = ent->get_parent();


  return(ent);
}
!*/
void entity::set_door( bool d )
{
  if ( d )
  {
    ext_flags |= EFLAG_EXT_IS_DOOR;
    set_door_closed( true );
  }

  else
    ext_flags &=~ EFLAG_EXT_IS_DOOR;
}

void entity::set_door_closed(bool d)
{
	// @TODO
	PANIC;
}


void entity::set_alternative_materials( material_set* arg )
{
	// @TODO
	PANIC;
}

void entity::set_alternative_materials( const stringx& alt_mat_name )
{
	// @TOOD
	PANIC;
}

bool entity::get_distance_fade_ok() const
{
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_DISTANCE_FADING))
    return false;
  if (!my_visrep)
    return false;
  return my_visrep->get_distance_fade_ok();
}

void entity::copy_flags( const entity& b )
{
  flags |= (b.flags & EFLAG_COPY_MASK);
  flags &= (b.flags | ~EFLAG_COPY_MASK);
  ext_flags |= (b.ext_flags & EFLAG_EXT_COPY_MASK);
  ext_flags &= (b.ext_flags | ~EFLAG_EXT_COPY_MASK);

  if(is_ext_flagged(EFLAG_EXT_ENX_WALKABLE))
    set_walkable( true );

}


#ifdef DEBUG
stringx g_current_entity_name;
stringx g_debug_entity_name="#*!@$"; // Try not to check in as anything but this!
bool    g_use_debug_entity_name=false;
//stringx g_debug_entity_name="NIGHTSKY"; // in .SCN file
#endif


// in entity.cpps
extern void delete_entity_id(entity_id id);

// motion blur stuff
enum { MAX_TRAIL_LENGTH=16 };


// hack to offset ifl displays on entities
int random_ifl_frame_boost_table[256];

////////////////////////////////////////////////////////////////////////////////

// utility macro for drawing motion blur
#define RHW_XFORM( sv, mat, dv, rhw )  \
  { rational_t w = mat[0][3]*sv.x+mat[1][3]*sv.y+mat[2][3]*sv.z+mat[3][3];  \
    rhw = 1/w; \
    dv.x  = mat[0][0]*sv.x+mat[1][0]*sv.y+mat[2][0]*sv.z+mat[3][0];  \
    dv.y  = mat[0][1]*sv.x+mat[1][1]*sv.y+mat[2][1]*sv.z+mat[3][1];  \
    dv.z  = mat[0][2]*sv.x+mat[1][2]*sv.y+mat[2][2]*sv.z+mat[3][2]; }



vector3d entity::get_visual_center() const
{

  if (!my_visrep)
    return get_abs_position();
//  return get_abs_po().slow_xform(my_visrep->get_center(get_age()));
  //return get_abs_po().fast_8byte_xform(my_visrep->get_center(get_age()));
  vector3d ctr=my_visrep->get_center(get_age());
  assert(ctr.y>-1e9F && ctr.y<1e9F);
  return get_abs_po().fast_8byte_xform(ctr);
}


rational_t entity::get_visual_radius() const
{
  return my_visrep ? my_visrep->get_radius(get_age()) : 0;
}

void entity::compute_visual_xz_radius_rel_center()
{
  if ( is_flagged( EFLAG_MISC_NONSTATIC ) )

    vis_xz_rad_rel_center = get_visual_radius();
  else if ( my_visrep )

    vis_xz_rad_rel_center = my_visrep->compute_xz_radius_rel_center( get_abs_po() );
}


// set entity orientation such that it is facing the given world-coordinate point
void entity::look_at( const vector3d& abs_pos )
{
  // compute orientation matrix;
  // z-basis is normalized vector from my position to dest position
  vector3d bz = abs_pos - get_abs_position();
  rational_t bzl = bz.length();
  if ( bzl > 0.001f )
    bz *= 1.0f / bzl;
  else
    bz = ZVEC;
  // x-basis is always on the right
  vector3d bx( bz.z, 0, -bz.x );
  rational_t bxl = bx.length();
  if ( bxl > 0.00001f )
    bx /= bxl;
  else

    bx = XVEC;
  // y-basis is z cross x
  vector3d by = cross( bz, bx );
  // build NEW po
  po newpo( bx, by, bz, ZEROVEC );
//  entity* parent = get_parent();
  if ( has_parent() )
  {

    fast_po_mul(newpo, newpo, link_ifc()->get_parent()->get_abs_po().inverse());
//    newpo = newpo * link_ifc()->get_parent()->get_abs_po().inverse();
  }
  newpo.fixup();
  newpo.set_position( get_rel_position() );
  // set NEW po
  set_rel_po( newpo );
}


// force entity to belong to current region(s) until un-forced (see below)
void entity::force_current_region()
{
  _set_region_forced_status();
}

// INTERNAL
void entity::_set_region_forced_status()
{
  set_flag(EFLAG_REGION_FORCED,true);
  // while forced, the entity has no sector and will not compute one!
  my_sector = NULL;
  center_region = NULL;
}

// un-force forced region(s)
// NOTE: this does not perform an update, so I will have no locale until
// compute_sector is called
void entity::unforce_regions()
{

  if ( is_flagged(EFLAG_REGION_FORCED) )
  {
    remove_from_regions();
    set_flag(EFLAG_REGION_FORCED,false);
  }
}



// put me into the same region(s) as the given entity

void entity::force_regions( entity* e )

{
#ifndef REGIONCULL
  region_node_pset::const_iterator i;
  for ( i=e->get_regions().begin(); i!=e->get_regions().end(); i++ )
    force_region( *i );

#endif
}


int entity::is_in_active_region()
{
#ifndef REGIONCULL
  if(my_sector)
    return(my_sector->get_region()->get_data()->is_active());
  else
  {
    region_node_pset::const_iterator i;

    for ( i=in_regions.begin(); i!=in_regions.end(); ++i )

    {
      if((*i)->get_data()->is_active())

#endif
        return 1;
#ifndef REGIONCULL
    }
  }

  return 0;
#endif
}

bool entity::has_entity_collision() const
{

	// @TODO
	PANIC;
	return false;
}

bool entity::has_camera_collision() const
{
	// @TODO
	PANIC;
	return false;
}


collision_geometry* entity::get_updated_colgeom(po * replacement_po, rational_t radius_scale)
{
  if (get_colgeom() && !get_colgeom()->is_valid() && !is_stationary())
  {

    update_colgeom(replacement_po);
  }
  if (get_colgeom())
    get_colgeom()->apply_radius_scale(radius_scale);
  return get_colgeom();
}


void entity::update_colgeom(po * replacement_po)
{
	// @TODO
	PANIC;
}


void entity::delete_colgeom()
{
	// @TODO
	PANIC;
}


void entity::delete_visrep()
{
  if (my_visrep)
  {

    unload_visual_rep(my_visrep);
    my_visrep = NULL;
  }
}

// @Patch - removed
//extern profiler_timer proftimer_adv_visrep;

void entity::advance_age( time_value_t t )
{
	// @TODO
	PANIC;
}


// @Patch
/*
extern profiler_timer proftimer_IFC_sound;
extern profiler_timer proftimer_IFC_damage;
extern profiler_timer proftimer_IFC_physical;

extern profiler_timer proftimer_adv_light_mgr;
*/


void entity::frame_advance( time_value_t t )
{
	// @TODO
	PANIC;
}

void entity::updatelighting( time_value_t t, const int playerID )
{
	// @TODO
	PANIC;
}



// @Patch
/*
extern profiler_timer proftimer_draw_shadow;

extern profiler_timer proftimer_motion_trail;
extern profiler_timer proftimer_instance_render;
extern profiler_timer proftimer_visrep_rend_inst;
*/

extern bool loresmodelbydefault;

void entity::render( camera* camera_link, rational_t detail, render_flavor_t flavor, rational_t entity_translucency_pct )
{
	// @TODO
	PANIC;
}


	// 0 = dark shadow, 1 = player reflection
extern float shadow_reflective_value;



void entity::rendershadow( camera* camera_link, rational_t detail, render_flavor_t flavor, rational_t entity_translucency_pct, rational_t scale )
{
#ifdef NGL
  nglMesh* mesh=NULL;
  if (flavor & RENDER_SHADOW_MODEL)
	  mesh = get_shadow_mesh();
  if (flavor & RENDER_LORES_MODEL)
	  mesh = get_lores_mesh();
	if ( mesh==NULL )
  	mesh = get_mesh();
  if ( !mesh )

    return;

  nglRenderParams params;
	memset(&params,0,sizeof(params));
  params.Flags = 0;


	if ( usezbias )

	{
		params.Flags |= NGLP_ZBIAS;
		params.ZBias = zbias;
	}

  if (MaterialMask)
  {

	  params.Flags |= NGLP_MATERIAL_MASK;
	  params.MaterialMask = MaterialMask;
  }

  if (TextureFrame >= 0)
  {
	  params.Flags |= NGLP_TEXTURE_FRAME;

	  params.TextureFrame = TextureFrame;
  }
#if 0
  else
  {
    // workaround for some ifls not being updated correctly.
    extern int KS_TextureAnimFrame;

	  params.Flags |= NGLP_TEXTURE_FRAME;
	  params.TextureFrame = KS_TextureAnimFrame;
  }
#endif

  if (use_uv_scrolling)
  {
    params.Flags |= NGLP_TEXTURE_SCROLL;
    params.ScrollU = scroll_u;
    params.ScrollV = scroll_v;
  }


  // enable tint if neccessary.  duplicated in entity::render.
  params.Flags |= NGLP_TINT;
  params.TintColor[0] = shadow_reflective_value; //c.get_red() * i;
  params.TintColor[1] = shadow_reflective_value; //c.get_green() * i;
  params.TintColor[2] = shadow_reflective_value; //c.get_blue() * i;
  params.TintColor[3] = entity_translucency_pct; //c.get_alpha() * i;

//  vector3d s = get_render_scale();

// Without NGLP_SCALE, transform will be normalized, which removes the flip for lefties.  (dc 05/31/02)
//  if ( scale != 1.0f ) //s != vector3d( 1, 1, 1 ) )
  {
    params.Flags |= NGLP_SCALE;
    params.Scale[0] = scale;
    params.Scale[1] = scale;
    params.Scale[2] = scale;
    params.Scale[3] = 1.0f;
	if (scale < 0)
	{
		params.Flags ^= NGLP_REVERSE_BACKFACECULL;	// entity is rendered flipped (dc 04/30/02)
	}
  }
	params.Flags |= NGLP_NO_CULLING;
	params.Flags |= NGLP_NO_LIGHTING;
	#if !defined (TARGET_GC) && !defined(TARGET_XBOX)

	params.Flags |= NGLP_WRITE_FB_ALPHA;
	#endif


  po render_po;
  if ((flip_axis >= 0) && (flip_axis <= 2))

  {
	  render_po = get_handed_abs_po();
	  params.Flags ^= NGLP_REVERSE_BACKFACECULL;	// entity is rendered flipped (dc 04/30/02)
  }
  else
	  render_po = get_abs_po();


  START_PROF_TIMER( proftimer_render_add_mesh );
  nglListAddMesh( mesh, native_to_ngl( render_po ), &params );
  STOP_PROF_TIMER( proftimer_render_add_mesh );
#else
  #ifdef DEBUG
  if( g_use_debug_entity_name )
  {
    g_current_entity_name=get_name();
    if (g_current_entity_name==g_debug_entity_name)

    { // you can put a breakpoint here
      debug_print("rendering %s, flavor 0x%x",g_debug_entity_name.c_str(),flavor);
    }
  }
  #endif

//nglPrintf("rendering %s, flavor 0x%x\n", get_name().c_str(), flavor);

  light_manager* lm = get_light_set();
  render_heart( detail, flavor, lm, 0, entity_translucency_pct );
#endif

}

// permanent:

/*
#ifndef PROJECT_STEEL
//static vert_buf_xformed motiontrailverts; // I'd rather just use vert_workspace_xformed
#define motiontrailverts vert_workspace_xformed
#endif
*/
void entity::render_heart( rational_t detail, render_flavor_t flavor, light_manager* light_set, unsigned force_flags, rational_t entity_translucency_pct )
{
	// @TODO
	PANIC;
}


int entity::get_max_polys() const
{
  assert( my_visrep );
  return my_visrep->get_max_faces( get_age() );
}

int entity::get_min_polys() const
{
  assert( my_visrep );
  return my_visrep->get_min_faces( get_age() );

}


// Motion Blur support
extern rational_t g_level_time;

void entity::record_motion()
{
  // save the motion blur rotation as quaternions as we're going to want to
  // interpolate between records
  if(is_motion_blurred() || is_motion_trailed())
  {
    assert(mbi);

    if( mbi->last_motion_recording != g_level_time )
    {

//      matrix4x4 m = get_abs_po().get_matrix(); // unused, remove me?
      mbi->motion_trail_buffer[mbi->motion_trail_end].q = quaternion(get_abs_po().get_matrix());
      matrix4x4 n;

      mbi->motion_trail_buffer[mbi->motion_trail_end].q.to_matrix( &n );
      mbi->motion_trail_buffer[mbi->motion_trail_end].t = get_abs_po().get_position();
      mbi->motion_trail_end++;
      if(mbi->motion_trail_end >= mbi->motion_trail_length)

        mbi->motion_trail_end = 0;

      if(mbi->motion_trail_count < mbi->motion_trail_length)

      {
        ++mbi->motion_trail_count;
      }

      else
      {

        // whee, the circle is complete
        mbi->motion_trail_start = mbi->motion_trail_end;
      }
      mbi->last_motion_recording = g_level_time;
    }
  }
}


// turn on or off the motion blur effect
void entity::allocate_motion_info()

{
  assert( mbi == NULL );

  // CTT 07/22/00: for the Max Steel project, the NONSTATIC flag only matters

  // for walkable entities
  // guard against dynamic allocation in static entity
  assert( !is_walkable() || is_flagged(EFLAG_MISC_NONSTATIC) );

  // allocate the mbi the first time the user turns on motion blur or trail

  mbi = NEW motion_blur_info(MAX_TRAIL_LENGTH);
  // need to be frame advancing henceforth
//!  if ( flavor!=ENTITY_ACTOR && flavor!=ENTITY_CHARACTER &&
//!  if (flavor!=ENTITY_LIMB_BODY)
    set_flag( EFLAG_ACTIVE, true );

}


void entity::activate_motion_blur(int _blur_min_alpha,
                                  int _blur_max_alpha,
                                  int _num_blur_images,
                                  float _blur_spread)
{
  set_flag(EFLAG_GRAPHICS_MOTION_BLUR, true);
  if ( is_motion_blurred() )
  {
    assert( mbi );
    // note that we have no initial motion blurred copies
    mbi->motion_trail_start = 0;
    mbi->motion_trail_end   = 0;
    mbi->motion_trail_count = 0;

    mbi->blur_min_alpha = _blur_min_alpha;
    mbi->blur_max_alpha = _blur_max_alpha;

/*!    if(g_world_ptr->get_num_active_characters() > 2)

      mbi->num_blur_images = (int) ( (rational_t)_num_blur_images / (rational_t)(g_world_ptr->get_num_active_characters() - 1) + 0.5f );
    else
!*/
      mbi->num_blur_images = _num_blur_images;


    mbi->blur_spread = _blur_spread;
  }
}

void entity::deactivate_motion_blur()
{

  set_flag(EFLAG_GRAPHICS_MOTION_BLUR,false);
}

#ifdef ECULL
sound_emitter* entity::get_emitter()

{
/*
  if ( !emitter )
  {
    // dynamic allocation allowed even on static entities because we make a
    // special effort to save these pointers as necessary in save_dynamic_heap();
    // see dynamic save/restore support section, below, and app.cpp
    emitter = sound_device::inst()->create_emitter();
    // need to be frame advancing henceforth
//!    if ( flavor!=ENTITY_ACTOR && flavor!=ENTITY_CHARACTER && flavor!=ENTITY_LIMB_BODY)
      set_flag( EFLAG_ACTIVE, true );
    get_emitter()->set_position( get_abs_position() );
  }
  return emitter;
*/

  if(!has_sound_ifc())
    create_sound_ifc();

  return(sound_ifc()->get_emitter());

}
#endif




void entity::set_frame_delta(po const & bob, time_value_t t)

{
//  bool locked;
  if(t > 0.0f)
  {
    if ( !mi )
    {

      // CTT 07/22/00: for the Max Steel project, the NONSTATIC flag only matters
      // for walkable entities
  //    #ifdef PROJECT_STEEL
      // guard against dynamic allocation in static entity
      if ( is_walkable() && !is_flagged(EFLAG_MISC_NONSTATIC) )
      {
        error( "Tried to manipulate static walkable object, ID: " + id.get_val() );
      }

      mi = NEW movement_info;
      invalidate_frame_delta();
    }

    if(mi->frame_delta_valid)
    {
      fast_po_mul(mi->frame_delta, mi->frame_delta, bob);
  //    mi->frame_delta = mi->frame_delta*bob;

    }
    else
      mi->frame_delta = bob;


    mi->frame_time = t;
    mi->frame_delta_valid = true;
  }

  assert(!mi || !mi->frame_delta_valid || mi->frame_time > 0.0f);

//  assert(!mi || !mi->frame_delta_valid || mi->frame_delta.get_position().length() < 50.0f);
}

void entity::set_frame_delta_trans(const vector3d &bob, time_value_t t)
{
  if(t > 0.0f)
  {
    if ( !mi )
    {
      // CTT 07/22/00: for the Max Steel project, the NONSTATIC flag only matters
      // for walkable entities
  //    #ifdef PROJECT_STEEL
      // guard against dynamic allocation in static entity
      if ( is_walkable() && !is_flagged(EFLAG_MISC_NONSTATIC) )
      {

        error( "Tried to manipulate static walkable object, ID: " + id.get_val() );
      }


      mi = NEW movement_info;
      invalidate_frame_delta();
    }

    if(mi->frame_delta_valid)
    {
      static po trans;
      trans.set_translate(bob);
      fast_po_mul(mi->frame_delta, mi->frame_delta, trans);

  //    mi->frame_delta = mi->frame_delta*trans;

    }
    else
    {
      mi->frame_delta.set_position(bob);
    }

    mi->frame_time = t;
    mi->frame_delta_valid = true;
  }

  assert(!mi || !mi->frame_delta_valid || mi->frame_time > 0.0f);


//  assert(!mi || !mi->frame_delta_valid || mi->frame_delta.get_position().length() < 50.0f);
}


// turn on or off the motion trail effect
void entity::activate_motion_trail( int _trail_length,
                                    color32 _trail_color,
                                    int _trail_min_alpha,
                                    int _trail_max_alpha,
                                    const vector3d& tip
                                    )
{
  set_flag(EFLAG_GRAPHICS_MOTION_TRAIL, true);
  assert( mbi );
  mbi->motion_trail_length = _trail_length;
  assert( mbi->motion_trail_length <= mbi->buffer_size );
  mbi->trail_color = _trail_color;
  mbi->trail_min_alpha = _trail_min_alpha;
  mbi->trail_max_alpha = _trail_max_alpha;
  // note that we have no initial motion trail copies
  mbi->motion_trail_start = 0;

  mbi->motion_trail_end   = 0;
  mbi->motion_trail_count = 0;

  mbi->motion_trail_head = tip;
}

void entity::deactivate_motion_trail()
{
  set_flag(EFLAG_GRAPHICS_MOTION_TRAIL,false);

}



////////////////////////////////////////////////////////////////////////////////
// entity:  Physical Representation Methods

////////////////////////////////////////////////////////////////////////////////


// For use during setup, because updating the colgeom is bad before the game is
// running.  Or if you know the colgeom is updated (a *little* faster).
void entity::get_closest_point_along_dir( vector3d* target, const vector3d& axis ) const

{
  get_colgeom()->get_closest_point_along_dir( target, axis );
}

// For use once game is active.
vector3d entity::get_updated_closest_point_along_dir( const vector3d& axis )
{

  vector3d cpad;
  get_updated_colgeom()->get_closest_point_along_dir( &cpad, axis );
  return cpad;
}


void entity::invalidate_colgeom()
{

  if (get_colgeom())
  {
    get_colgeom()->invalidate();
  }
}


time_value_t entity::get_age() const
{
//  return age;

  return frame_time_info.get_age();
}


void entity::set_age(time_value_t new_age)

{
  frame_time_info.set_age(new_age);
}

void entity::rebirth()
{
//  age = 0;
  frame_time_info.set_age(0.0f);
}


////////////////////////////////////////////////////////////////////////////////

// Render optimization support
// /////////////////////////////////////////////////////////////////////////////


render_flavor_t entity::render_passes_needed() const
{
  if ( !my_visrep )
    return 0;

  render_flavor_t passes=my_visrep->render_passes_needed();
  if ( my_visrep->get_type() == VISREP_PMESH )
  {
    if ( render_color.get_alpha() < 0xFF )
      passes = RENDER_TRANSLUCENT_PORTION;
    else if ( is_motion_blurred() || is_motion_trailed() )
      passes |= RENDER_TRANSLUCENT_PORTION;
  }
  return passes;
}

////////////////////////////////////////////////////////////////////////////////
//   dummy light stubs
////////////////////////////////////////////////////////////////////////////////


color evil_color(0,0,0,0);


const color& entity::get_color() const
{
  stringx composite = id.get_val() + " is not a light source.";
  error(composite.c_str());
  return evil_color;
}

void   entity::set_color(const color&)
{
  stringx composite = id.get_val() + " is not a light source.";
  error(composite.c_str());
}

const color& entity::get_additive_color() const
{
  stringx composite = id.get_val() + " is not a light source.";
  error(composite.c_str());
  return evil_color;
}

void   entity::set_additive_color(const color&)
{

  stringx composite = id.get_val() + " is not a light source.";

  error(composite.c_str());
}


rational_t entity::get_near_range() const
{
  stringx composite = id.get_val() + " is not a light source.";
  error(composite.c_str());
  return 0;
}

void       entity::set_near_range(rational_t)
{

  stringx composite = id.get_val() + " is not a light source.";

  error(composite.c_str());
}


rational_t entity::get_cutoff_range() const

{
  stringx composite = id.get_val() + " is not a light source.";
  error(composite.c_str());

  return 0;
}

void       entity::set_cutoff_range(rational_t)
{
  stringx composite = id.get_val() + " is not a light source.";
  error(composite.c_str());
}


/////////////////////////////////////////////////////////////////////////////
// Animation interface
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////
// hierarchical animation interface

// maximum number of applied hierachical animations

// const int MAX_ANIM_SLOTS = 4;

// this function loads the named hierarchical animation data (must be performed
// before the animation can be played)
void entity::load_anim( const stringx& filename ) const
{
	// @TODO
	PANIC;
}

void entity::unload_anim( const stringx& filename ) const
{
	// @TODO
	PANIC;
}
// play a hierarchical animation in the given slot
entity_anim_tree* entity::play_anim( int slot,
                                     const stringx& filename,

                                     time_value_t start_time,
                                     unsigned short anim_flags,
                                     short loop )
{
	// @TODO
	PANIC;
	return NULL;
}

// play a hierarchical animation in the given slot
entity_anim_tree* entity::play_anim( int slot,
                                     const stringx& filenamea,
                                     const stringx& filenameb,
                                     rational_t blenda,
                                     rational_t blendb,
                                     time_value_t start_time,

                                     unsigned short anim_flags,
                                     short loop )

{
	// @TODO
	PANIC;
	return NULL;
}

// play a hierarchical animation in slot 0
entity_anim_tree* entity::play_anim( const stringx& filename,
                                     time_value_t start_time,
                                     unsigned short anim_flags,
                                     short loop )
{
  return play_anim( 0, filename, start_time, anim_flags, loop );
}

entity_anim_tree* entity::play_loop_anim( const stringx& filename, unsigned short anim_flags, short loop )
{
  return play_anim( filename, -0.25f, anim_flags, loop );
}


entity_anim_tree* entity::play_anim( int slot,
                                     const stringx& _name,

                                     const entity_track_tree& track,
                                     time_value_t start_time,
                                     unsigned short anim_flags,

                                     short loop )
{
	// @TODO
	PANIC;
	return NULL;
}


void entity::make_animateable( bool onOff )
{
	if ( onOff && anim_trees==NULL )
	{
    // this is the first time a hierarchical animation has been requested for
    // this entity, so we must construct the hierarchical animation list
//    anim_trees = NEW entity_anim_tree*[MAX_ANIM_SLOTS];
    memset( anim_trees, 0, sizeof(entity_anim_tree*)*MAX_ANIM_SLOTS );
	}
  /*
	else
	if ( !onOff && anim_trees!=NULL )
	{
		delete [] anim_trees;
	}
  */
}

entity_anim_tree* entity::play_anim( int slot,
                                     const stringx& _name,
                                     const entity_track_tree& tracka,
                                     const entity_track_tree& trackb,
                                     rational_t blenda,
                                     rational_t blendb,
                                     time_value_t start_time,
                                     unsigned short anim_flags,
                                     short loop )
{

	// @TODO
	PANIC;
	return NULL;
}



entity_anim_tree* entity::get_anim_tree( int slot ) const

{
  //assert(slot==ANIM_PRIMARY);
  assert( slot>=0 && slot<MAX_ANIM_SLOTS );
//  if ( anim_trees == NULL )
//    return NULL;
//  else
	if ( anim_trees )
  {
    entity_anim_tree* a = anim_trees[slot];
    if ( a && a->is_attached() )
      return a;

    else
      return NULL;
  }
	else
	  return NULL;
}


void entity::kill_anim( int slot )
{
	// @TODO
	PANIC;
}

bool entity::anim_finished( int slot ) const
{
 // assert(slot==ANIM_PRIMARY);
  entity_anim_tree* a = get_anim_tree( slot );
  return ( a==NULL || a->is_finished() );
}


// This function should ONLY be called when the world kills an entity_anim_tree.
void entity::clear_anim( entity_anim_tree* a )
{
//  assert( anim_trees != NULL );
  int slot;
  for ( slot=0; slot<MAX_ANIM_SLOTS; ++slot )

  {
    entity_anim_tree* local = get_anim_tree( slot );
    if ( a == local )
    {

      // deconstruct and detach this animation (effectively caches the memory used for any later anim in the same slot)
      local->deconstruct();
      local->detach();
      // now re-attach all lower-priority animations

      int i;
      for ( i=0; i<slot; i++ )
      {

        entity_anim_tree* a = get_anim_tree( i );
        if ( a && a->is_valid() )
          a->attach();
      }
      return;
    }
  }
  assert( 0 );  // this should never happen
}

// this function causes the internal animation nodes of each attached hierarchical
// animation to be destroyed (made necessary by the actor limb_tree_pool system)
void entity::deconstruct_anim_trees()

{
  if ( has_anim_trees() )
  {
    int i;
    for ( i=0; i<MAX_ANIM_SLOTS; i++ )
    {

      entity_anim_tree* a = get_anim_tree( i );
      if ( a )
        a->deconstruct();
    }
  }
}


// this function reattaches all hierarchical animations
void entity::reconstruct_anim_trees()
{

  if ( has_anim_trees() )
  {
    int i;
    for ( i=0; i<MAX_ANIM_SLOTS; i++ )
    {
      entity_anim_tree* a = get_anim_tree( i );
      if ( a )
      {
        a->reconstruct( i );
      }
    }

  }
}


///////////////////////////////////////
// entity_anim interface


// attach given animation
// NOTE: this causes the removal of any previously attached animation

bool entity::attach_anim( entity_anim* new_anim )
{

  assert( new_anim );
  if ( current_anim==NULL || new_anim->get_priority()>=current_anim->get_priority() )
  {
    if ( current_anim && new_anim!=current_anim )
    {
      // another animation is currently attached;
      // detach it
      current_anim->detach();

    }
    current_anim = new_anim;
    return true;
  }
  else
    return false;
}

// detach given animation if it matches current_anim; returns true if successful
void entity::detach_anim()
{
  current_anim = NULL;
}


////////////////////////////////////////////////////////////////////////////////

//  bounding box
////////////////////////////////////////////////////////////////////////////////

void entity::compute_bounding_box()
{
  if ( get_colgeom() && get_colgeom()->get_type()==collision_geometry::MESH )
  {
    if ( !has_bounding_box() )
      bbi = NEW bounding_box;
    cg_mesh* m = static_cast<cg_mesh*>( get_colgeom() );
    int i;
    for ( i=0; i<m->get_num_verts(); ++i )
    {
//      vector3d v = get_abs_po().slow_xform( m->get_vert_ptr(i)->get_point() );
      vector3d v = get_abs_po().fast_8byte_xform( m->get_vert_ptr(i)->get_point() );

      if ( v.x < bbi->vmin.x )
        bbi->vmin.x = v.x;
      if ( v.x > bbi->vmax.x )
        bbi->vmax.x = v.x;
      if ( v.y < bbi->vmin.y )
        bbi->vmin.y = v.y;
      if ( v.y > bbi->vmax.y )
        bbi->vmax.y = v.y;
      if ( v.z < bbi->vmin.z )
        bbi->vmin.z = v.z;
      if ( v.z > bbi->vmax.z )

        bbi->vmax.z = v.z;
    }
  }
}


void entity::check_nonstatic()
{
  // CTT 07/22/00: for the Max Steel project, the NONSTATIC flag only matters
  // for walkable entities

  #ifndef BUILD_BOOTABLE
//  #ifdef PROJECT_STEEL
  if ( is_walkable() && !is_flagged(EFLAG_MISC_NONSTATIC) )

  {
    warning( get_id().get_val() + ": walkable entity is static; change to NONSTATIC if you wish to modify this entity at run-time." );
//P    add_onscreen_error(get_id().get_val() + " is static and walkable, change to NONSTATIC");
    set_flag( EFLAG_MISC_NONSTATIC, true );  // do this to avoid further warnings
  }

/*
  #else
  if ( !is_flagged( EFLAG_MISC_NONSTATIC ) )
  {
    warning( get_id().get_val() + ": entity is static; change to NONSTATIC if you wish to modify this entity at run-time." );
//P    add_onscreen_error(get_id().get_val() + " is static, change to NONSTATIC");
    set_flag( EFLAG_MISC_NONSTATIC, true );  // do this to avoid further warnings
  }
  #endif

*/
  #endif // !BUILD_BOOTABLE
}


int g_iflrand_counter=0;
int entity::get_random_ifl_frame_boost() const
{
  g_iflrand_counter++;
  return random_ifl_frame_boost_table[0xff&(get_id().get_numerical_val()*3)];
}

void init_random_ifl_frame_boost_table()

{
	// @TODO
	PANIC;
}


region_node * entity::get_primary_region() const
{
#ifndef REGIONCULL
  if (flags&EFLAG_REGION_FORCED)

    return (in_regions.empty() ? NULL : *in_regions.begin());
  else

#endif
    return center_region;
}

// maintains parented entities region info
region_node * entity::update_region(bool parent_computed)

{
#ifndef REGIONCULL
  set_needs_compute_sector(false);

  if (flags&EFLAG_REGION_FORCED)
  {
    return (in_regions.empty() ? NULL : *in_regions.begin());
  }

  if (has_parent())// /*! pi && pi->parent !*/ && flags&EFLAG_MISC_NONSTATIC)
  {
    region_node * parents_region;
/*!    if (get_flavor()==ENTITY_LIMB_BODY)
      parents_region = ((limb_body *) this)->get_my_limb()->get_my_actor()->update_region();
    else
!*/
    parents_region = parent_computed ? ((entity *)link_ifc()->get_parent())->get_primary_region() : ((entity *)link_ifc()->get_parent())->update_region();
    if (get_primary_region()!=parents_region /*! && get_flavor()!=ENTITY_LIMB_BODY !*/)
      compute_sector(g_world_ptr->get_the_terrain());
  }
#endif
  return get_primary_region();
}

////////////////////////////////////////////////////////////////////////////////
//  entity_manager
////////////////////////////////////////////////////////////////////////////////


// @Test - used for testing asm gen
entity_manager* entity_manager::my_inst;

// @Ok
// @Matching
entity_manager::entity_manager() : number_server( 0 )
{
#if defined(TARGET_XBOX)

  assert( !strcmp(entity_flavor_names[NUM_ENTITY_FLAVORS], "null") );
#else
  assert( entity_flavor_names[NUM_ENTITY_FLAVORS]=="null" );
#endif /* TARGET_XBOX JIV DEBUG */

  name_to_number.clear();
  number_server = 0;
  name_to_number.insert( name_to_number_map::value_type( strdupcpp("UNREG"), -1) );
}

// @Ok
// @Matching
entity_manager::~entity_manager()
{
  name_to_number_map::iterator mi;
  mi = name_to_number.begin();
  for(; mi != name_to_number.end(); ++mi )
  {

    delete[] (*mi).first.str;
  }
  erase( begin(), end() );
  name_to_number = name_to_number_map();
}

// @Ok
// @Matching
entity* entity_manager::find_entity(const entity_id& target_entity,
                                    entity_flavor_t flavor, bool unknown_ok)
{
  entity * outval;

  iterator it = find( target_entity );
  if( ( unknown_ok != FIND_ENTITY_UNKNOWN_OK ) && (it==end() ) )

  {
    stringx composite = stringx("Unable to find entity ") + target_entity.get_val();

    error( composite.c_str() );
  }

  if (it!= end())
  {
    if (flavor!=IGNORE_FLAVOR)
    {
      if((*it).second->get_flavor() != flavor )
      {
        stringx composite = stringx("Entity ") + target_entity.get_val() + " is not a " + entity_flavor_names[flavor];
        error( composite.c_str() );
      }
    }
    outval = (*it).second;
  }
  else
    outval = NULL;
  return outval;
}

#ifdef SPIDEY_SIM
entity* entity_manager::find_nearest_entity_to_line(const vector3d& pos1, const vector3d& pos2, entity_flavor_t flavor )
{
  entity * outval;

  iterator it = begin();
  float best = 0;
  iterator winner = end();
  vector3d unit_ray = pos2 - pos1;
  unit_ray.normalize();
  for( ; it != end(); it++ )
  {
    if( (*it).second->get_flavor() == flavor )
    {
      vector3d entpos = (*it).second->get_abs_position();
      // is in front of spidey?
      float dotty = dot( (entpos-pos1), unit_ray );
      if( dotty > 0 )
      {
        // project entpos onto ray
        vector3d projection = pos1 + dotty * unit_ray;

        // find distance to line
        vector3d ent_to_proj = entpos - projection;
        float distance = ent_to_proj.length();

        // find distance to spidey
        vector3d proj_to_pos1 = projection - pos1;
        float distance2 = proj_to_pos1.length();
        // disqualify anything ridiculous
        if( distance2<50 )  // this will actually vary from level to level
        {
          // quality is length over width

          float quality = distance2 / distance;
          if( quality > best )
          {
            best = quality;
            winner = it;
          }
        }
      }
    }
  }
  if( winner != end() )
    return (*winner).second;
  else
    return NULL;
}

#endif /* SPIDEY_SIM */
// @Ok
// @Matching
void entity_manager::purge()
{
  name_to_number_map::iterator mi;

  mi = name_to_number.begin();
  for(; mi != name_to_number.end(); ++mi )
  {
    delete[] (*mi).first.str;
  }
  erase( begin(), end() );
  name_to_number = name_to_number_map();
  number_server = 0;
  // *weird:  this causes allocations at the end of a heap we're going to delete most of
//  pair<name_to_number_map::iterator,bool> result =  // unused, remove me?
	//nglPrintf("Weird 6 byte leak\n");
  //name_to_number.insert( name_to_number_map::value_type( strdupcpp("UNREG"), -1) );
}


void entity_manager::stl_prealloc(void)
{
	// @TODO
	PANIC;
}


// @OK
// @Matching
void entity_manager::register_entity(entity* e)

{
  if( !(e->get_id() == ANONYMOUS) )
  {
	  std::pair<iterator,bool> retval = insert( value_type(e->get_id(), e) );

    if(!retval.second)
    {
      stringx composite = stringx("Same entity name appears twice: ")+ e->get_id().get_val();
      error( composite.c_str() );
    }
  }
}

// @Ok
// @PartialMatching - stl extra thread safety
void entity_manager::deregister_entity(entity* e)
{
  if( !(e->get_id() == ANONYMOUS) )
  {
    iterator it = find( e->get_id() );
    entity_id::delete_entity_id(e->get_id());
    erase(it);

  }
}

////////////////////////////////////////////////////////////////////////////////
//  motion_blur stuff
motion_blur_info::motion_blur_info( int max_trail_length )
  : trail_min_alpha(0),
    trail_max_alpha(0),
    blur_min_alpha(0),
    blur_max_alpha(0),
    num_blur_images(0),
    blur_spread(0.0f)
{
  motion_trail_start = 0;
  motion_trail_end = 0;
  motion_trail_count = 0;
  motion_trail_length = 0;

  motion_trail_buffer = NEW qt[max_trail_length];
  buffer_size = max_trail_length;
  motion_trail_head = vector3d(0.0f, 0.0f, 1.1f);
  motion_trail_tail = vector3d(0.0f, 0.0f, 0.0f);
}

motion_blur_info::~motion_blur_info()
{ delete[] motion_trail_buffer; }


#include "my_assertions.h"

void validate_entity_manager(void)
{
	VALIDATE_SIZE(entity_manager, 0x20);

	VALIDATE(entity_manager, name_to_number, 0x10);
	VALIDATE(entity_manager, number_server, 0x1C);
}

void validate_entity_id(void)
{
	VALIDATE_SIZE(entity_id, 0x4);

	VALIDATE(entity_id, val, 0x0);
}

void validate_entity(void)
{
	VALIDATE(entity, id, 0x70);
}

#include "my_patch.h"

void patch_entity(void)
{
}

void patch_entity_id(void)
{
	PATCH_PUSH_RET(0x004E3FC0, entity_id::get_val);
	PATCH_PUSH_RET(0x004E3DD0, entity_id::set_entity_id);

	PATCH_PUSH_RET_POLY(0x004E3CF0, entity_id::entity_id(const char*), "??0entity_id@@QAE@PBD@Z");

	PATCH_PUSH_RET(0x004E3EB0, entity_id::delete_entity_id);

	// @TODO - can't patch yet because it was inlined in other places
	// and it references a static variable
	//PATCH_PUSH_RET(0x004E4060, entity_id::make_unique_id);

	PATCH_PUSH_RET_POLY(0x004E4200, serial_in, "?serial_in@@YAXAAVchunk_file@@PAVentity_id@@@Z");
}

void patch_entity_manager(void)
{
	PATCH_PUSH_RET(0x004F24C0, entity_manager::register_entity);
	PATCH_PUSH_RET(0x004F2590, entity_manager::deregister_entity);

	PATCH_PUSH_RET(0x004F22B0, entity_manager::purge);

	PATCH_PUSH_RET(0x004F2100, entity_manager::find_entity);

	PATCH_PUSH_RET_POLY(0x004F1C90, entity_manager::entity_manager, "??0entity_manager@@AAE@XZ");
}

void patch_str(void)
{
	PATCH_PUSH_RET(0x004EBEA0, strdupcpp);
	PATCH_PUSH_RET(0x004E3C60, to_entity_flavor_t);
}
