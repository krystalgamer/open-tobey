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
#include "item.h"
#include "entityflags.h"
#include "collide.h"
#include "lightmgr.h"
#include "controller.h"
#include "entity_interface.h"
#include "vm_thread.h"


// @Patch
#include "ai_interface.h"
#include "damage_interface.h"

// @TODO - REMOVE
DEFINE_SINGLETON(anim_id_manager);

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

// @Ok
// @NotMatching - it seems it knows that signal_error is a _notreturn
// but declaring it in the header is not easy because it'd mean I have to move signal_manager up
// doesn't justify the effort
void entity::signal_error(unsigned int a2, const stringx& parm)
{
	signaller::signal_error(a2,
			"Entity: " + this->id.get_val() +
			"\nFlavor: "  + stringx(entity_flavor_names[this->flavor]) +
			"\n");
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


// @Ok
// @Matching
void entity::optimize()
{
	if (my_visrep)
	{
		if (my_visrep->get_type()==VISREP_PMESH)
		{
			vr_pmesh* mesh = static_cast<vr_pmesh*>(my_visrep);
			mesh->shrink_memory_footprint();
		}
	}
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
	PANIC;

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

// @Ok
// @Matching
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


// @Ok
// @Matching
// these virtual functions allow types descended from entity to be
// recognized when adding them to regions, so that the region class can
// maintain lists of different entity types as desired
void entity::add_me_to_region( region* r )
{
  r->add( this );
}

// @Ok
// @Matching
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
	PANIC;
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




// @Ok
// @Matching
vector3d entity::get_last_position() const
{
	if (this->mi)
	{
		return this->mi->frame_delta.get_position();
	}

	return get_abs_position();
}


// @Ok
// @Matching
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

// @Ok
// @Matching
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

// @Ok
// @Matching
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


// @Ok
// @Matching
// force entity to belong to current region(s) until un-forced (see below)
void entity::force_current_region()
{
  _set_region_forced_status();
}

// INTERNAL
// @Ok
// @Matching
INLINE void entity::_set_region_forced_status()
{
  set_flag(EFLAG_REGION_FORCED,true);
  // @Patch - removed sector part
  // while forced, the entity has no sector and will not compute one!
  //my_sector = NULL;
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
	PANIC;
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
	PANIC;
}

void entity::set_frame_delta_trans(const vector3d &bob, time_value_t t)
{
	PANIC;
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


// @Ok
// @Matching
// For use during setup, because updating the colgeom is bad before the game is
// running.  Or if you know the colgeom is updated (a *little* faster).
void entity::get_closest_point_along_dir( vector3d* target, const vector3d& axis ) const
{
  get_colgeom()->get_closest_point_along_dir( target, axis );
}

// @Ok
// @Matching
// For use once game is active.
vector3d entity::get_updated_closest_point_along_dir( const vector3d& axis )
{

  vector3d cpad;
  get_updated_colgeom()->get_closest_point_along_dir( &cpad, axis );
  return cpad;
}


// @Ok
// @Matching
void entity::invalidate_colgeom()
{
  if (get_colgeom())
  {
    get_colgeom()->invalidate();
  }
}


// @Ok
// @Matching
time_value_t entity::get_age() const
{
//  return age;

  return frame_time_info.get_age();
}


// @Ok
// @Matching
void entity::set_age(time_value_t new_age)
{
  frame_time_info.set_age(new_age);
}

// @Ok
// @Matching
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


// @Ok
color evil_color(0,0,0,0);


// @Ok
// @Matching
const color& entity::get_color() const
{
  stringx composite = id.get_val() + " is not a light source.";
  error(composite.c_str());
  return evil_color;
}

// @Ok
// @Matching
void   entity::set_color(const color&)
{
  stringx composite = id.get_val() + " is not a light source.";
  error(composite.c_str());
}

// @Ok
// @Matching
const color& entity::get_additive_color() const
{
  stringx composite = id.get_val() + " is not a light source.";
  error(composite.c_str());
  return evil_color;
}

// @Ok
// @Matching
void   entity::set_additive_color(const color&)
{

  stringx composite = id.get_val() + " is not a light source.";

  error(composite.c_str());
}


// @Ok
// @Matching
rational_t entity::get_near_range() const
{
  stringx composite = id.get_val() + " is not a light source.";
  error(composite.c_str());
  return 0;
}

// @Ok
// @Matching
void       entity::set_near_range(rational_t)
{

  stringx composite = id.get_val() + " is not a light source.";

  error(composite.c_str());
}


// @Ok
// @Matching
rational_t entity::get_cutoff_range() const

{
  stringx composite = id.get_val() + " is not a light source.";
  error(composite.c_str());

  return 0;
}

// @Ok
// @Matching
void       entity::set_cutoff_range(rational_t)
{
  stringx composite = id.get_val() + " is not a light source.";
  error(composite.c_str());
}

// @Ok
// @Matching
void       entity::add_light_category(int)
{
  stringx composite = id.get_val() + " is not a light source.";
  error(composite.c_str());
}

// @Ok
// @Matching
void       entity::remove_light_category(int)
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
	// @TODO
	PANIC;
	return true;
}

// detach given animation if it matches current_anim; returns true if successful
void entity::detach_anim()
{
	PANIC;
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
	PANIC;
	return NULL;
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

destroyable_info* destroyable_info::make_instance(entity *ent)
{

  destroyable_info* info = NEW destroyable_info(ent);

  info->copy_instance_data(this);
  return info;
}

void destroyable_info::copy_instance_data(destroyable_info* data)
{
  flags = data->flags;
  destroy_lifetime = data->destroy_lifetime;

#ifdef ECULL
  destroy_sound = data->destroy_sound;
#endif
  destroy_fx = data->destroy_fx;
  destroy_script = data->destroy_script;
  preload_script = data->preload_script;
  destroyed_visrep = data->destroyed_visrep;

  hit_points = data->hit_points;


//  dread_net_cue = data->dread_net_cue;

  if(data->destroyed_mesh)
    destroyed_mesh = new_visrep_instance(data->destroyed_mesh);

  else
    destroyed_mesh = NULL;
}


destroyable_info::destroyable_info(entity *ent)
{
  flags = 0;

  destroy_lifetime = 1.0f;
#ifdef ECULL
  destroy_sound = empty_string;
#endif
  destroy_fx = empty_string;
  destroy_script = empty_string;
  preload_script = empty_string;
  destroyed_visrep = empty_string;

  destroyed_mesh = NULL;

  hit_points = 0;

//  dread_net_cue = dread_net::UNDEFINED_AV_CUE;

  owner = ent;
  assert(owner);
}

destroyable_info::~destroyable_info()
{
  if(destroyed_mesh)

  {
    unload_visual_rep(destroyed_mesh);
    destroyed_mesh = NULL;
  }
}

void destroyable_info::read_enx_data( chunk_file& fs, stringx& lstr )

{
	PANIC;
}

void destroyable_info::reset()
{
  // reset the destroy info structure
  set_hit_points(0);
  set_has_hit_points(false);
  set_destroy_lifetime(1.0f);
  set_has_destroy_fx(false);
  set_has_destroy_script(false);
  set_has_preload_script(false);
  set_has_preload_script_run(false);
#ifdef ECULL
  set_has_destroy_sound(false);
#endif
  set_has_destroyed_visrep(false);
  set_single_blow(false);
  set_remain_visible(false);
  set_remain_active(false);
  set_no_collision(false);
  set_remain_collision(false);
//  dread_net_cue = dread_net::UNDEFINED_AV_CUE;
}


int destroyable_info::apply_damage(int damage, const vector3d &pos, const vector3d &norm)
{
  if ( has_hit_points() )
  {
    if ( !is_single_blow() || damage>=hit_points )
      hit_points -= damage;
    if ( hit_points <= 0 )
      hit_points = 0;
    return hit_points;
  }

  else
    return 1;
}



void destroyable_info::preload()
{
  if(!has_preload_script_run())
  {
    set_has_preload_script_run(true);
    entity::exec_preload_function(get_preload_script());

  }
}


vm_thread* entity::spawn_entity_script_function( const stringx& function_name ) const
{
	PANIC;
	return NULL;
}

void entity::exec_preload_function(const stringx &preload_func)
{
	// @TODO
	PANIC;
}


void entity::apply_damage(int damage, const vector3d &pos, const vector3d &norm, int _damage_type, entity *attacker, int dmg_flags)
{

  assert(0);
/*
  if ( is_destroyable() )
  {
    if(attacker)
    {
      if(attacker->is_a_handheld_item())
      {
        dmg_info.attacker = ((handheld_item *)attacker)->get_owner();
        dmg_info.attacker_itm = (item *)attacker;
      }
      else
      {
        dmg_info.attacker = attacker;
        dmg_info.attacker_itm = NULL;
      }
    }
    else

    {
      dmg_info.attacker = NULL;
      dmg_info.attacker_itm = NULL;
    }


    // make sure members of a conglomerate cannot hurt itself... (for tanks and turrets)
    if(is_a_conglomerate() && ((conglomerate *)this)->has_member(dmg_info.attacker))
      return;

    dmg_info.damage = damage;
    dmg_info.dir = -norm;
    dmg_info.loc = pos;
    dmg_info.type = _damage_type;
    dmg_info.push_wounded = true;
    dmg_info.push_death = true;
    dmg_info.flags = dmg_flags;

    raise_signal( DAMAGED );

    damage = (int)((((rational_t)damage) * damage_resist_modifier) + 0.5f);

    #ifdef TARGET_PC
      char    outbuf[100];
      sprintf( outbuf, "%s takes %d damage", id.get_val().c_str(), damage );
      app::inst()->get_game()->get_message_board()->post( stringx(outbuf), 2.0F );
    #endif


    if ( !is_invulnerable() && !destroy_info->apply_damage(damage, pos, norm) )

    {
      set_auto_targetable(false);
      apply_destruction_fx();
      target_timer = 0.0f;
      raise_signal( DESTROYED );
    }
  }
*/

}


// add an item to this container;

// returns true if no like item already in list; otherwise, adds to existing item's count
bool entity::add_item( item* it )
{
	// @TODO
	PANIC;
	return false;
}



// returns NULL if index is out-of=range
item* entity::get_item( unsigned int n ) const
{
  if ( is_container() && n<(unsigned)get_num_items() )
    return coninfo->items[n];
  else
    return NULL;
}


// returns null pointer if no like item found
item* entity::find_like_item( item* it ) const
{
	PANIC;
	return NULL;
}

// returns null pointer if no like item found
item* entity::find_item_by_name( const stringx &name ) const
{
  if ( is_container() )
  {
    item_list_t::const_iterator i = coninfo->items.begin();
    item_list_t::const_iterator i_end = coninfo->items.end();
    for ( ; i!=i_end; ++i )
    {

      item* lit = *i;
      if ( lit && lit->get_name() == name )

        return lit;
    }
  }
  return NULL;
}

#if 0 // BIGCULL
handheld_item* entity::find_item_by_id( const stringx &id ) const
{
  if ( is_container() )

  {
    item_list_t::const_iterator i = coninfo->items.begin();
    item_list_t::const_iterator i_end = coninfo->items.end();
    for ( ; i!=i_end; ++i )
    {
      item* lit = *i;
      if ( lit && lit->is_a_handheld_item() && ((handheld_item *)lit)->get_item_id() == id )
        return((handheld_item *)lit);
    }
  }
  return NULL;
}
#endif // BIGCULL

// returns -1 if item is not found in list
int entity::get_item_index( item* it ) const
{
  if ( is_container() )
  {

    item_list_t::const_iterator i = coninfo->items.begin();
    item_list_t::const_iterator i_end = coninfo->items.end();
    int index = 0;
    for ( ; i!=i_end; ++i,++index )
    {
      if ( *i == it )
        return index;
    }
  }
  return -1;
}



// returns the next item in the list after the given one (wraps around);
// returns NULL if given item is not found
item* entity::get_next_item( item* itm ) const
{
  int n = get_num_items();
  if ( n )
  {
    int base_cur_item = get_item_index( itm );

    if(base_cur_item == -1)
      base_cur_item = 0;

    int c = base_cur_item;
    do
    {

      ++c;
      if ( c >= n )

        c = 0;
    } while ( (!get_item(c) || get_item(c)->get_number()==0) && c!=base_cur_item );

    item* newitm = get_item( c );
    if ( newitm && newitm->get_number()>0 )

      return newitm;
  }
  return NULL;
}


// returns the previous item in the list before the given one (wraps around);
// returns NULL if given item is not found
item* entity::get_prev_item( item* itm ) const
{
  int n = get_num_items();
  if ( n )
  {
    int base_cur_item = get_item_index( itm );

    if(base_cur_item == -1)
      base_cur_item = 0;

    int c = base_cur_item;
    do
    {
      --c;
      if ( c < 0 )
        c = n - 1;
    } while ( (!get_item(c) || get_item(c)->get_number()==0) && c!=base_cur_item );

    item* newitm = get_item( c );
    if ( newitm && newitm->get_number()>0 )
      return newitm;
  }
  return NULL;
}


// disgorge any items I may be carrying
void entity::disgorge_items(entity *target)
{
error("Disgorge_items not supported in KS.");
#if 0  //BIGCULL
  if ( is_container() )
  {
    item_list_t::const_iterator i = coninfo->items.begin();
    item_list_t::const_iterator i_end = coninfo->items.end();
    for ( ; i!=i_end; ++i )
    {

      item* it = *i;

      if(it && (it->get_number() > 0 || (it->is_a_thrown_item() && ((thrown_item *)it)->is_a_radio_detonator())) && !it->is_brain_weapon())
      {

        if(target == NULL)
        {

          it->set_visible( true );
          vector3d newpos( (((float)(random(100)))*.03f)-1.5f, .5f, (((float)(random(100)))*.03f)-1.5f );
          newpos *= 0.25f;
          newpos += get_abs_position();
          po newpo = po_identity_matrix;
          newpo.set_position( newpos );
//!          it->set_parent( NULL );
          it->set_rel_po( newpo );
          it->set_pickup_timer(0.5f);
          it->compute_sector( g_world_ptr->get_the_terrain() );
          it->raise_signal( item::SCHWING );
        }
        else

        {
/*!          if(target->is_a_character())
            it->give_to_character((character *)target);
          else
!*/
            target->add_item(it);
        }
      }
    }

    coninfo->items.resize(0);
  }
#endif //BIGCULL
}

void entity::use_item(item *it)
{
  if(it != NULL)
  {
    last_item_used = it;

    it->apply_effects( this );

    raise_signal(entity::USE_ITEM);
  }
}

void entity::copy_visrep(entity *ent)
{
	// @TODO
	PANIC;
}

/*!
void entity::activate_by_character(character *chr)
{

  action_character = chr;


  raise_signal(ACTIVATED_BY_CHARACTER);
}

!*/



// @Ok
// @Matching
bool entity::allow_targeting() const
{
	return is_combat_target();

	// @Patch - restore old code
  //return 1;
#if 0 // BIGCULL

  if (!has_damage_ifc())
    return false;

  return (damage_ifc()->is_alive() && is_combat_target());
#endif // BIGCULL
}

bool entity::test_combat_target( const vector3d& p0, const vector3d& p1,
                                 vector3d* impact_pos, vector3d* impact_normal,
                                 rational_t default_radius, bool rear_cull ) const
{
  return(collide_segment_entity(p0, p1, this, impact_pos, impact_normal, default_radius, rear_cull));
}

void entity::process_extra_scene_flags(unsigned int scn_flags)
{
  if(scn_flags & BEAMABLE_FLAG)
  {
    if(!is_beamable())

      set_beamable(true);
  }
  else if(scn_flags & NO_BEAMABLE_FLAG)
  {
    if(is_beamable())

      set_beamable(false);

  }

  if ( scn_flags & SCANABLE_FLAG )

  {
    if ( !is_scannable() )
    {
      if ( has_mesh() )
      {
        set_scannable( true );
      }
      else
        warning( get_id().get_val() + ": entity must have mesh visrep to be set as SCANABLE" );
    }

  }
  else if ( scn_flags & NO_SCANABLE_FLAG )

  {
    if ( is_scannable() )

      set_scannable(false);
  }


  if ( get_colgeom() && get_colgeom()->get_type() == collision_geometry::MESH )
  {
    cg_mesh* m = static_cast<cg_mesh*>( get_colgeom() );


    if(scn_flags & CAMERA_COLL_FLAG)
    {
      if(!m->is_camera_collision())
        m->set_flag( cg_mesh::FLAG_CAMERA_COLLISION, true );

    }
    else if(scn_flags & NO_CAMERA_COLL_FLAG)
    {

      if(m->is_camera_collision())
        m->set_flag( cg_mesh::FLAG_CAMERA_COLLISION, false );

    }

    if(scn_flags & ENTITY_COLL_FLAG)
    {

      if(!m->is_entity_collision())
        m->set_flag( cg_mesh::FLAG_ENTITY_COLLISION, true );
    }
    else if(scn_flags & NO_ENTITY_COLL_FLAG)

    {
      if(m->is_entity_collision())
        m->set_flag( cg_mesh::FLAG_ENTITY_COLLISION, false );
    }
  }

  if(scn_flags & ACTIONABLE_FLAG)
  {
    if(!is_actionable())
      set_actionable(true);
  }
  else if(scn_flags & NO_ACTIONABLE_FLAG)
  {
    if(is_actionable())
      set_actionable(false);
  }

  if(scn_flags & ACTION_FACING_FLAG)
  {
    if(!action_uses_facing())
      set_action_uses_facing(true);
  }
  else if(scn_flags & NO_ACTION_FACING_FLAG)
  {
    if(action_uses_facing())

      set_action_uses_facing(false);
  }

  if(scn_flags & IS_DOOR_FLAG)
  {
    if(!is_door())
      set_door(true);
  }
  else if(scn_flags & NO_IS_DOOR_FLAG)
  {
    if(is_door())
      set_door(false);
  }

  if(is_door())
  {
    if(scn_flags & DOOR_OPEN_FLAG)
    {
      if(is_door_closed())
        set_door_closed(false);
    }
    else if(scn_flags & DOOR_CLOSED_FLAG)
    {
      if(!is_door_closed())
        set_door_closed(true);

    }
  }

#if _ENABLE_WORLD_EDITOR
  scene_flags = scn_flags & OVERIDE_MASK_FLAG;


  if(is_beamable())
    scene_flags |= BEAMABLE_FLAG;

  if(is_scannable())

    scene_flags |= SCANABLE_FLAG;


  if ( get_colgeom() && get_colgeom()->get_type() == collision_geometry::MESH )

  {
    cg_mesh* m = static_cast<cg_mesh*>( get_colgeom() );

    if(m->is_camera_collision())
      scene_flags |= CAMERA_COLL_FLAG;

    if(m->is_entity_collision())
      scene_flags |= ENTITY_COLL_FLAG;
  }


  if(is_actionable())
    scene_flags |= ACTIONABLE_FLAG;

  if(action_uses_facing())
    scene_flags |= ACTION_FACING_FLAG;

  if(is_door())
  {

    scene_flags |= IS_DOOR_FLAG;

    if(!is_door_closed())
      scene_flags |= DOOR_OPEN_FLAG;
  }
#endif
}

bool entity::parse_instance( const stringx& pcf, chunk_file& fs )
{
	// @TODO
	PANIC;
	return true;
}


// @Ok
// @Matching
void entity::suspend()
{
	if (!is_flagged(EFLAG_MISC_SUSPENDED))
	{
		this->flags |= EFLAG_MISC_SUSPENDED;

		if (has_ai_ifc())
		{
			ai_ifc()->push_disable();
		}
	}
}



// @Ok
// @Matching
void entity::unsuspend()
{
	if (is_flagged(EFLAG_MISC_SUSPENDED))
	{
		this->flags &= ~EFLAG_MISC_SUSPENDED;

		if (has_ai_ifc())
		{
			ai_ifc()->pop_disable();
		}
	}
}


void entity::set_controller(entity_controller * c)
{

  assert(my_controller == NULL);

  my_controller = c;


  if ( c )
    c->set_active( is_active()/* || c->is_a_brain()*/ );
}


/*
brain * entity::get_brain()
{
//  assert (this!=g_world_ptr->get_hero_ptr() && my_controller->is_a_brain());
  if(my_controller && my_controller->is_a_brain())
    return (brain *)my_controller;

  else
    return(NULL);
}
*/

// @Ok
// @Matching
bool entity::is_alive() const
{
	// @Patch - undo BIGCULL
	return(!has_damage_ifc() || damage_ifc()->is_alive());
return true;
}


// @Ok
// @Matching
bool entity::is_dying() const
{
  return(false);
}

// @Ok
// @Matching
bool entity::is_alive_or_dying() const
{
  return(is_alive() || is_dying());
}

bool entity::is_hero() const

{
	// @TODO
	PANIC;
	return true;
}



bool entity::possibly_active() const
{
	PANIC;
	return false;
}


// @Ok
// @Matching
bool entity::possibly_aging() const
{
  return(my_visrep != NULL && ( ( my_visrep->get_anim_length() > 1)||( my_visrep->is_uv_animated() ) ));
}


// @Ok
// @Matching
void entity::set_active( bool a )
{
  if(entity::is_active() != a)
  {
    if ( a )
      flags|=EFLAG_ACTIVE;
    else

      flags&=~EFLAG_ACTIVE;

//    region_update_poss_active();
	// @Patch - remove my_controller stuff
	/*
    if ( my_controller )
      set_control_active( a );
	*/
  }
}

void entity::set_visible( bool a )
{
	if(entity::is_visible() != a)
	{
		if( a )
		{
			if (!is_ext_flagged(EFLAG_EXT_WAS_VISIBLE))	// we turned it off in the draw menu
			{
				flags|=EFLAG_GRAPHICS_VISIBLE;
			}
		}
		else
		{
			flags&=~EFLAG_GRAPHICS_VISIBLE;
		}

		region_update_poss_render();
	}
}

void entity::set_collisions_active( bool a, bool update_reg )

{
  if(entity::are_collisions_active() != a)
  {
    if(a)

    {
      flags|=EFLAG_PHYSICS_COLLISIONS_ACTIVE;
    }
    else
    {
      flags&=~EFLAG_PHYSICS_COLLISIONS_ACTIVE;

    }

    if(update_reg)
      region_update_poss_collide();
  }
}

void entity::region_update_poss_active()

{
#ifndef REGIONCULL
  region_node_pset::iterator i; //,j;
  for ( i=in_regions.begin(); i!=in_regions.end(); ++i)
  {
    region* r = (*i)->get_data();
    if(r)
      r->update_poss_active(this);
  }

#endif
}

void entity::region_update_poss_render()
{
#ifndef REGIONCULL
  region_node_pset::iterator i; //,j;
  for ( i=in_regions.begin(); i!=in_regions.end(); ++i)
  {
    region* r = (*i)->get_data();
    if(r)
      r->update_poss_render(this);
  }
#endif
}

void entity::region_update_poss_collide()
{
#ifndef REGIONCULL
  region_node_pset::iterator i; //,j;
  for ( i=in_regions.begin(); i!=in_regions.end(); ++i)
  {
    region* r = (*i)->get_data();
    if(r)

      r->update_poss_collide(this);
  }
#endif
}


// @Ok
// @Matching
void entity::preload()
{
  if(!was_preloaded())

  {
    set_preloaded(true);


	// @Patch - remove destroy_info
	/*
    if(destroy_info != NULL)
      destroy_info->preload();
	  */

//    if(get_brain() != NULL)
//      get_brain()->preload();
  }

}

void entity::clear_all_raised_signals()
{
  signals_raised[0] = 0;

  signals_raised[1] = 0;
}

void entity::clear_signal_raised(unsigned short sig_id)
{
  assert(sig_id > 0 && sig_id < N_SIGNALS && sig_id < 64);
  signals_raised[(sig_id < 32 ? 1 : 0)] &= ~(0x80000000 >> (sig_id < 32 ? sig_id : (sig_id - 32)));
}

bool entity::signal_raised(unsigned short sig_id)
{

  assert(sig_id > 0 && sig_id < N_SIGNALS && sig_id < 64);
  return((signals_raised[(sig_id < 32 ? 1 : 0)] & (0x80000000 >> (sig_id < 32 ? sig_id : (sig_id - 32)))) != 0);

}


static void entity_signal_callback_footstep(signaller* sig, const char*pccdata)
{
#ifdef ECULL
  entity *whoami = (entity*)sig;
  if(whoami->has_sound_ifc())
  {

    static pstring footstep("FOOTSTEP");
    whoami->sound_ifc()->play_3d_sound_grp(footstep);
  }

#endif
}

static void entity_signal_callback_attack(signaller* sig, const char*pccdata)
{
#if 0 // BIGCULL
  entity *whoami = (entity*)sig;

  if ( whoami->is_hero() )
  {
    g_spiderman_controller_ptr->apply_attack(g_spiderman_controller_ptr->get_combo_move(), g_spiderman_controller_ptr->get_combo_damage(), g_spiderman_controller_ptr->get_combo_flags(), g_spiderman_controller_ptr->get_combo_wounded_anim());

  }
  else if(whoami->has_ai_ifc() && whoami->ai_ifc()->get_target())// || whoami->get_brain()->get_anim_attack_type() != DAMAGE_NONE)
  {
    // This has been moved into the AI (ai_actions.cpp, class: attack_ai_action), where it should be (JDB 4-10-01)....
/*
    entity *pc = whoami->ai_ifc()->get_target();//g_world_ptr->get_hero_ptr();
    vector3d mypos = whoami->get_abs_position();
    vector3d pcpos = pc->get_abs_position();
    vector3d vec = mypos - pcpos;
    vector3d invvec = pcpos - mypos;
    rational_t len = vec.length();
    int attack_type = DAMAGE_MELEE;//whoami->get_brain() ? whoami->get_brain()->get_anim_attack_type() : DAMAGE_DIRECT;

    assert(pc->has_damage_ifc());

//    int damage = whoami->get_brain() ? whoami->get_brain()->get_anim_damage() : 10;
    int damage = 10;


    // THE multiply THING IS A FUDGE FACTOR
    // TO MAKE THE SPHERE MORE OF AN OBLATE SPHEROID
    // SINCE I'M NOT SUCKING THE CAPSULE
    rational_t rad = (whoami->get_radius()+pc->get_radius());
    if( len*1.0f < rad )
    {
      rational_t ab = angle_between( invvec, whoami->get_abs_po().get_facing() );
      // GUESSTIMATE FOR 90deg EXPRESSED IN RADS, GOUACHE AND MIXED MEDIA ON CANVAS
      if( ab <= 1.570795f )
      {
        pc->damage_ifc()->apply_damage( whoami, damage, (eDamageType)attack_type, mypos, invvec );
        if(whoami->has_sound_ifc())
        {
          pstring p("IMPACT");
          whoami->sound_ifc()->play_3d_sound_grp( p );
        }
      }
    }

*/
  }
#endif // BIGCULL
}

void entity_signal_callback_raiser(signaller* sig, const char* sig_id)
{
  assert(sig->is_an_entity());
  unsigned short id = (unsigned short)sig_id;
  ((entity *)sig)->signals_raised[(id < 32 ? 1 : 0)] |= (0x80000000 >> (id < 32 ? id : (id - 32)));

}


void entity::add_signal_callbacks()
{
  signal_ptr( ATTACK )->add_callback( entity_signal_callback_attack, NULL );

  signal_ptr( FOOTSTEP_L )->add_callback( entity_signal_callback_footstep, NULL );
  signal_ptr( FOOTSTEP_R )->add_callback( entity_signal_callback_footstep, NULL );

  for(unsigned short i=0; i<N_SIGNALS; ++i)

    signal_ptr( i )->add_callback( entity_signal_callback_raiser, (char *)i );
}


bool entity::get_ifc_num(const pstring &att, rational_t &val)
{
	PANIC;
	return true;
}

bool entity::set_ifc_num(const pstring &att, rational_t val)

{
	PANIC;
	return false;
}

bool entity::get_ifc_vec(const pstring &att, vector3d &val)
{
	PANIC;
	return true;
}

bool entity::set_ifc_vec(const pstring &att, const vector3d &val)
{
	PANIC;
	return true;
}

bool entity::get_ifc_str(const pstring &att, stringx &val)
{
	PANIC;
	return true;
}

bool entity::set_ifc_str(const pstring &att, const stringx &val)
{
	PANIC;
	return true;
}


entity::entity_search_list entity::found_entities;

int entity::find_entities(int flags, const vector3d &pos, rational_t radius, region_node *reg, bool only_active_portals)
{
PANIC;
	return 0;
}


int entity::find_entities(int flags)
{
	PANIC;
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool entity::has_mesh()
{
#if defined(TARGET_PS2) || defined(TARGET_XBOX) || defined(TARGET_GC)
  if( get_mesh() )
    return true;
  else
    return false;
#else
  if( get_vrep() )
    if( get_vrep()->get_type()==VISREP_PMESH )
      return true;
  return false;
#endif

}


////////////////////////////////////////////////////////////////////////////////////////////////////
int entity::num_mesh_bones()
{
  if( has_mesh() )
  {
#if defined(TARGET_PS2) || defined(TARGET_XBOX) || defined(TARGET_GC)
    return get_mesh()->NBones;
#else
    return ((vr_pmesh*)get_vrep())->get_num_bones();
#endif
  }

  return 0;
}

void entity::set_max_lights( unsigned int ml )
{
	PANIC;
}


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
	// @TODO - it's way too big to uncomment this now
	//VALIDATE_SIZE(entity, 0xF4);

	VALIDATE(entity, flags, 0x60);
	VALIDATE(entity, ext_flags, 0x64);

	VALIDATE(entity, flavor, 0x6C);
	VALIDATE(entity, id, 0x70);

	VALIDATE(entity, my_mesh, 0x7C);

	VALIDATE(entity, my_visrep, 0x84);

	VALIDATE(entity, mi, 0x90);

	VALIDATE(entity, anim_trees, 0x94);

	VALIDATE(entity, center_region, 0x98);

	VALIDATE(entity, radius, 0xA8);

	VALIDATE(entity, my_ai_interface, 0xB0);

	VALIDATE(entity, my_damage_interface, 0xB8);

	VALIDATE(entity, frame_time_info, 0xD0);

	VALIDATE(entity, my_light_mgr, 0xDC);

	//VALIDATE(entity, colgeom, 0x100);

	// @Temp
	//VALIDATE(entity, my_sector, 0x118);

	// entity vtable validation

	VALIDATE_VTABLE(entity, is_an_entity, 1);
	VALIDATE_VTABLE(entity, is_a_trigger, 2);
	VALIDATE_VTABLE(entity, signal_error, 3);
	VALIDATE_VTABLE(entity, raise_signal, 4);
	VALIDATE_VTABLE(entity, construct_signal_list, 5);
	VALIDATE_VTABLE(entity, get_signal_name, 6);
	VALIDATE_VTABLE(entity, po_changed, 7);
	VALIDATE_VTABLE(entity, get_ifc_num, 8);
	VALIDATE_VTABLE(entity, set_ifc_num, 9);
	VALIDATE_VTABLE(entity, get_ifc_vec, 10);
	VALIDATE_VTABLE(entity, set_ifc_vec, 11);
	VALIDATE_VTABLE(entity, get_ifc_str, 12);
	VALIDATE_VTABLE(entity, set_ifc_str, 13);
	VALIDATE_VTABLE(entity, frame_advance, 14);
	VALIDATE_VTABLE(entity, render, 15);
	VALIDATE_VTABLE(entity, read_enx, 16);
	VALIDATE_VTABLE(entity, handle_enx_chunk, 17);
	VALIDATE_VTABLE(entity, parse_instance, 18);
	VALIDATE_VTABLE(entity, initialize, 19);
	VALIDATE_VTABLE(entity, make_instance, 20);
	VALIDATE_VTABLE(entity, set_flag_recursive, 21);
	VALIDATE_VTABLE(entity, set_ext_flag_recursive, 22);
	VALIDATE_VTABLE(entity, set_created_entity_default_active_status, 23);
	VALIDATE_VTABLE(entity, optimize, 24);
	VALIDATE_VTABLE(entity, set_radius, 25);
	VALIDATE_VTABLE(entity, get_radius, 26);
	VALIDATE_VTABLE(entity, is_time_limited, 27);
	VALIDATE_VTABLE(entity, set_time_limited, 28);
	VALIDATE_VTABLE(entity, is_active, 29);
	VALIDATE_VTABLE(entity, set_active, 30);
	VALIDATE_VTABLE(entity, get_forced_active, 31);
	VALIDATE_VTABLE(entity, are_collisions_active, 32);
	VALIDATE_VTABLE(entity, set_collisions_active, 33);
	VALIDATE_VTABLE(entity, are_character_collisions_active, 34);
	VALIDATE_VTABLE(entity, are_terrain_collisions_active, 35);
	VALIDATE_VTABLE(entity, are_moving_terrain_only_collisions_active, 36);
	VALIDATE_VTABLE(entity, set_character_collisions_active, 37);
	VALIDATE_VTABLE(entity, set_terrain_collisions_active, 38);
	VALIDATE_VTABLE(entity, set_moving_terrain_only_collisions_active, 39);
	VALIDATE_VTABLE(entity, is_sticky, 40);
	VALIDATE_VTABLE(entity, set_sticky, 41);
	VALIDATE_VTABLE(entity, is_stationary, 42);
	VALIDATE_VTABLE(entity, set_stationary, 43);
	VALIDATE_VTABLE(entity, is_walkable, 44);
	VALIDATE_VTABLE(entity, set_walkable, 45);
	VALIDATE_VTABLE(entity, is_repulsion, 46);
	VALIDATE_VTABLE(entity, set_repulsion, 47);
	VALIDATE_VTABLE(entity, is_visible, 48);
	VALIDATE_VTABLE(entity, set_visible, 49);
	VALIDATE_VTABLE(entity, is_still_visible, 50);
	//VALIDATE_VTABLE(entity, render_trail, 51);
	VALIDATE_VTABLE(entity, is_motion_blurred, 52);
	VALIDATE_VTABLE(entity, activate_motion_blur, 53);
	//VALIDATE_VTABLE(entity, render_trail, 54);
	VALIDATE_VTABLE(entity, is_motion_trailed, 55);
	/*
	VALIDATE_VTABLE(entity, activate_motion_trail, 56);
	VALIDATE_VTABLE(entity, activate_motion_trail, 57);
	VALIDATE_VTABLE(entity, activate_motion_trail, 58);
	*/
	VALIDATE_VTABLE(entity, get_externally_controlled, 59);
	VALIDATE_VTABLE(entity, get_in_use, 60);
	VALIDATE_VTABLE(entity, set_in_use, 61);
	VALIDATE_VTABLE(entity, get_member_hidden, 62);
	VALIDATE_VTABLE(entity, set_member_hidden, 63);
	VALIDATE_VTABLE(entity, get_colgeom, 64);
	VALIDATE_VTABLE(entity, set_colgeom_flag_recursive, 65);
	VALIDATE_VTABLE(entity, update_colgeom, 66);
	VALIDATE_VTABLE(entity, invalidate_colgeom, 67);
	VALIDATE_VTABLE(entity, get_updated_colgeom, 68);
	VALIDATE_VTABLE(entity, get_damage_capsule, 69);
	VALIDATE_VTABLE(entity, get_updated_damage_capsule, 70);
	VALIDATE_VTABLE(entity, get_inter_capsule_radius_scale, 71);
	VALIDATE_VTABLE(entity, get_velocity, 72);
	VALIDATE_VTABLE(entity, get_angular_velocity, 73);
	VALIDATE_VTABLE(entity, get_water_dist, 74);
	VALIDATE_VTABLE(entity, get_underwater_pct, 75);
	VALIDATE_VTABLE(entity, get_water_normal, 76);
	VALIDATE_VTABLE(entity, get_underwater_time, 77);
	VALIDATE_VTABLE(entity, get_effective_collision_velocity, 78);
	VALIDATE_VTABLE(entity, get_effective_collision_mass, 79);
	VALIDATE_VTABLE(entity, get_closest_point_along_dir, 80);
	VALIDATE_VTABLE(entity, is_picked_up, 81);
	VALIDATE_VTABLE(entity, phys_render, 82);
	VALIDATE_VTABLE(entity, get_visrep_ending_time, 83);
	VALIDATE_VTABLE(entity, get_visual_center, 84);
	VALIDATE_VTABLE(entity, get_visual_radius, 85);
	VALIDATE_VTABLE(entity, get_vrep, 86);
	VALIDATE_VTABLE(entity, get_mesh, 87);
	VALIDATE_VTABLE(entity, set_fade_away, 88);
	VALIDATE_VTABLE(entity, get_fade_away, 89);
	VALIDATE_VTABLE(entity, get_filename, 90);
	VALIDATE_VTABLE(entity, get_dirname, 91);
	VALIDATE_VTABLE(entity, has_dirname, 92);
	VALIDATE_VTABLE(entity, is_a_beam, 93);
	VALIDATE_VTABLE(entity, is_a_camera, 94);
	VALIDATE_VTABLE(entity, is_a_station_camera, 95);
	VALIDATE_VTABLE(entity, is_a_game_camera, 96);
	VALIDATE_VTABLE(entity, is_a_marky_camera, 97);
	VALIDATE_VTABLE(entity, is_a_mouselook_camera, 98);
	VALIDATE_VTABLE(entity, is_a_sniper_camera, 99);
	VALIDATE_VTABLE(entity, is_a_conglomerate, 100);
	VALIDATE_VTABLE(entity, is_a_turret, 101);
	VALIDATE_VTABLE(entity, is_a_ladder, 102);
	VALIDATE_VTABLE(entity, is_a_light_source, 103);
	VALIDATE_VTABLE(entity, is_a_limb_body, 104);
	VALIDATE_VTABLE(entity, is_a_marker, 105);
	VALIDATE_VTABLE(entity, is_a_rectangle_marker, 106);
	VALIDATE_VTABLE(entity, is_a_cube_marker, 107);
	VALIDATE_VTABLE(entity, is_a_crawl_marker, 108);
	VALIDATE_VTABLE(entity, is_a_particle_generator, 109);
	VALIDATE_VTABLE(entity, is_a_physical_entity, 110);
	VALIDATE_VTABLE(entity, is_a_crate, 111);
	VALIDATE_VTABLE(entity, is_an_item, 112);
	VALIDATE_VTABLE(entity, is_a_switch, 113);
	VALIDATE_VTABLE(entity, is_a_visual_item, 114);
	VALIDATE_VTABLE(entity, is_a_handheld_item, 115);
	VALIDATE_VTABLE(entity, is_a_gun, 116);
	VALIDATE_VTABLE(entity, is_a_thrown_item, 117);
	VALIDATE_VTABLE(entity, is_a_melee_item, 118);
	VALIDATE_VTABLE(entity, is_a_morphable_item, 119);
	VALIDATE_VTABLE(entity, is_a_projectile, 120);
	VALIDATE_VTABLE(entity, is_a_rigid_body, 121);
	VALIDATE_VTABLE(entity, is_a_grenade, 122);
	VALIDATE_VTABLE(entity, is_a_rocket, 123);
	VALIDATE_VTABLE(entity, is_a_scanner, 124);
	VALIDATE_VTABLE(entity, is_a_sky, 125);
	VALIDATE_VTABLE(entity, advance_age, 126);
	VALIDATE_VTABLE(entity, frame_done, 127);
	VALIDATE_VTABLE(entity, add_position_increment, 128);
	VALIDATE_VTABLE(entity, terrain_position, 129);
	VALIDATE_VTABLE(entity, terrain_radius, 130);
	VALIDATE_VTABLE(entity, get_colgeom_root_po, 131);
	VALIDATE_VTABLE(entity, get_colgeom_root, 132);
	VALIDATE_VTABLE(entity, add_me_to_region, 133);
	VALIDATE_VTABLE(entity, remove_me_from_region, 134);
	VALIDATE_VTABLE(entity, compute_sector, 135);
	VALIDATE_VTABLE(entity, get_region, 136);

	VALIDATE_VTABLE(entity, force_region, 137);
	VALIDATE_VTABLE(entity, force_current_region, 138);
	VALIDATE_VTABLE(entity, unforce_regions, 139);
	VALIDATE_VTABLE(entity, force_regions, 140);
	VALIDATE_VTABLE(entity, record_motion, 141);
	VALIDATE_VTABLE(entity, camera_set_target, 142);
	VALIDATE_VTABLE(entity, camera_set_roll, 143);
	VALIDATE_VTABLE(entity, camera_set_collide_with_world, 144);
	VALIDATE_VTABLE(entity, camera_slide_to, 145);
	VALIDATE_VTABLE(entity, camera_slide_to_orbit, 146);
	VALIDATE_VTABLE(entity, camera_orbit, 147);
	VALIDATE_VTABLE(entity, get_color, 148);
	VALIDATE_VTABLE(entity, set_color, 149);
	VALIDATE_VTABLE(entity, get_additive_color, 150);
	VALIDATE_VTABLE(entity, set_additive_color, 151);
	VALIDATE_VTABLE(entity, get_near_range, 152);
	VALIDATE_VTABLE(entity, set_near_range, 153);
	VALIDATE_VTABLE(entity, get_cutoff_range, 154);
	VALIDATE_VTABLE(entity, set_cutoff_range, 155);
	VALIDATE_VTABLE(entity, add_light_category, 156);
	VALIDATE_VTABLE(entity, remove_light_category, 157);
	VALIDATE_VTABLE(entity, get_light_set, 158);
	VALIDATE_VTABLE(entity, create_light_set, 159);
	VALIDATE_VTABLE(entity, is_frame_delta_valid, 160);
	VALIDATE_VTABLE(entity, is_last_frame_delta_valid, 161);
	VALIDATE_VTABLE(entity, get_frame_delta, 162);
	VALIDATE_VTABLE(entity, set_frame_delta, 163);
	VALIDATE_VTABLE(entity, set_frame_delta_trans, 164);
	VALIDATE_VTABLE(entity, invalidate_frame_delta, 165);
	VALIDATE_VTABLE(entity, get_last_capsule, 166);
	VALIDATE_VTABLE(entity, get_last_position, 167);
	VALIDATE_VTABLE(entity, render_passes_needed, 168);
	VALIDATE_VTABLE(entity, set_recursive_age, 169);
	VALIDATE_VTABLE(entity, attach_anim, 170);
	VALIDATE_VTABLE(entity, detach_anim, 171);
	VALIDATE_VTABLE(entity, acquire, 172);
	VALIDATE_VTABLE(entity, release, 173);
	VALIDATE_VTABLE(entity, get_hit_points, 174);
	VALIDATE_VTABLE(entity, get_full_hit_points, 175);
	VALIDATE_VTABLE(entity, add_item, 176);
	VALIDATE_VTABLE(entity, use_item, 177);
	VALIDATE_VTABLE(entity, compute_bounding_box, 178);
	VALIDATE_VTABLE(entity, apply_damage, 179);
	VALIDATE_VTABLE(entity, copy_visrep, 180);
	VALIDATE_VTABLE(entity, allow_targeting, 181);
	VALIDATE_VTABLE(entity, test_combat_target, 182);
	VALIDATE_VTABLE(entity, get_detonate_position, 183);
	VALIDATE_VTABLE(entity, add_signal_callbacks, 184);
	VALIDATE_VTABLE(entity, get_distance_fade_ok, 185);
	VALIDATE_VTABLE(entity, suspend, 186);
	VALIDATE_VTABLE(entity, unsuspend, 187);
	VALIDATE_VTABLE(entity, possibly_active, 188);
	VALIDATE_VTABLE(entity, possibly_aging, 189);
	VALIDATE_VTABLE(entity, region_update_poss_active, 190);
	VALIDATE_VTABLE(entity, region_update_poss_render, 191);
	VALIDATE_VTABLE(entity, region_update_poss_collide, 192);
	VALIDATE_VTABLE(entity, is_alive, 193);
	VALIDATE_VTABLE(entity, is_dying, 194);
	VALIDATE_VTABLE(entity, is_alive_or_dying, 195);
	VALIDATE_VTABLE(entity, preload, 196);
	VALIDATE_VTABLE(entity, ifl_play, 197);
	VALIDATE_VTABLE(entity, ifl_lock, 198);
	VALIDATE_VTABLE(entity, ifl_pause, 199);
	VALIDATE_VTABLE(entity, set_render_color, 200);
	VALIDATE_VTABLE(entity, set_render_scale, 201);
	VALIDATE_VTABLE(entity, set_render_zbias, 202);


	VALIDATE_VAL(EFLAG_MISC_IN_USE, 0x00010000);
	VALIDATE_VAL(EFLAG_MISC_REPULSION, 0x00001000);
	VALIDATE_VAL(EFLAG_GRAPHICS_VISIBLE, 0x200);
	VALIDATE_VAL(EFLAG_PHYSICS_WALKABLE, 8);

	VALIDATE_VAL(EFLAG_PHYSICS_MOVING, 4);
	VALIDATE_VAL(EFLAG_PHYSICS_STICKY, 0x40);

	VALIDATE_VAL(EFLAG_EXT_TIME_LIMITED, 0x10000);
	VALIDATE_VAL(EFLAG_MEMBER_HIDDEN, 0x80000000);

	VALIDATE_VAL(EFLAG_EXT_NONTARGET, 0x20000);

	VALIDATE_VAL(EFLAG_MISC_SUSPENDED, 0x40000000);

	VALIDATE_VAL(EFLAG_EXT_PRELOADED, 0x80000);
}

void validate_movement_info(void)
{
	VALIDATE_SIZE(entity::movement_info, 0x58);

	VALIDATE(entity::movement_info, frame_delta_valid, 0x0);
	VALIDATE(entity::movement_info, last_frame_delta_valid, 0x1);

	VALIDATE(entity::movement_info, frame_delta, 0x18);
}

#include "my_patch.h"

void patch_movement_info(void)
{
}

void patch_entity(void)
{
	PATCH_PUSH_RET_POLY(0x004A0E10, entity::is_visible, "?is_visible@entity@@UBE_NXZ");

	PATCH_PUSH_RET_POLY(0x004A0CD0, entity::is_sticky, "?is_sticky@entity@@UBE_NXZ");
	PATCH_PUSH_RET_POLY(0x004A0CF0, entity::set_sticky, "?set_sticky@entity@@UAEX_N@Z");

	PATCH_PUSH_RET_POLY(0x004A0D20, entity::is_stationary, "?is_stationary@entity@@UBE_NXZ");
	PATCH_PUSH_RET_POLY(0x004A0D40, entity::set_stationary, "?set_stationary@entity@@UAEX_N@Z");

	PATCH_PUSH_RET_POLY(0x004A0D70, entity::is_walkable, "?is_walkable@entity@@UBE_NXZ");
	PATCH_PUSH_RET_POLY(0x004A0D90, entity::set_walkable, "?set_walkable@entity@@UAEX_N@Z");

	PATCH_PUSH_RET_POLY(0x004A0DC0, entity::is_repulsion, "?is_repulsion@entity@@UBE_NXZ");
	PATCH_PUSH_RET_POLY(0x004A0DE0, entity::set_repulsion, "?set_repulsion@entity@@UAEX_N@Z");

	PATCH_PUSH_RET_POLY(0x004A0BD0, entity::is_active, "?is_active@entity@@UBE_NXZ");
	PATCH_PUSH_RET_POLY(0x004F4AB0, entity::set_active, "?set_active@entity@@UAEX_N@Z");

	PATCH_PUSH_RET_POLY(0x004A0B70, entity::is_time_limited, "?is_time_limited@entity@@UBE_NXZ");
	PATCH_PUSH_RET_POLY(0x004A0B90, entity::set_time_limited, "?set_time_limited@entity@@UAEX_N@Z");

	PATCH_PUSH_RET_POLY(0x004A0B50, entity::get_radius, "?get_radius@entity@@UBEMXZ");
	PATCH_PUSH_RET_POLY(0x004A0B30, entity::set_radius, "?set_radius@entity@@UAEXM@Z");

	PATCH_PUSH_RET_POLY(0x004A1280, entity::is_an_entity, "?is_an_entity@entity@@UBE_NXZ");

	PATCH_PUSH_RET_POLY(0x004EA910, entity::get_signal_name, "?get_signal_name@entity@@MBEPBDG@Z");
	PATCH_PUSH_RET_POLY(0x004A0AC0, entity::construct_signal_list, "?construct_signal_list@entity@@EAEPAV?$fast_vector@PAVsignal@@@@XZ");

	PATCH_PUSH_RET_POLY(0x004A0EF0, entity::get_in_use, "?get_in_use@entity@@UBE_NXZ");
	PATCH_PUSH_RET_POLY(0x004A0F10 , entity::set_in_use, "?set_in_use@entity@@UAEX_N@Z");

	PATCH_PUSH_RET_POLY(0x004F5F50 , entity::signal_error, "?signal_error@entity@@UAEXIABVstringx@@@Z");

	PATCH_PUSH_RET_POLY(0x004A0A40 , entity::set_flag_recursive, "?set_flag_recursive@entity@@UAEXI_N@Z");
	PATCH_PUSH_RET_POLY(0x004A0A80 , entity::set_ext_flag_recursive, "?set_ext_flag_recursive@entity@@UAEXI_N@Z");

	PATCH_PUSH_RET_POLY(0x004EBE10 , entity::set_created_entity_default_active_status, "?set_created_entity_default_active_status@entity@@UAEXXZ");

	PATCH_PUSH_RET_POLY(0x004E6820 , entity::optimize, "?optimize@entity@@UAEXXZ");

	PATCH_PUSH_RET_POLY(0x004A0F70 , entity::set_member_hidden, "?set_member_hidden@entity@@UAEX_N@Z");
	PATCH_PUSH_RET_POLY(0x004A0F50 , entity::get_member_hidden, "?get_member_hidden@entity@@UBE_NXZ");

	PATCH_PUSH_RET_POLY(0x004A1180 , entity::get_vrep, "?get_vrep@entity@@UBEPAVvisual_rep@@XZ");
	PATCH_PUSH_RET_POLY(0x004A11A0 , entity::get_mesh, "?get_mesh@entity@@UBEPAUnglMesh@@XZ");

	PATCH_PUSH_RET_POLY(0x004A11C0 , entity::set_fade_away, "?set_fade_away@entity@@UAEX_N@Z");
	PATCH_PUSH_RET_POLY(0x004A11E0 , entity::get_fade_away, "?get_fade_away@entity@@UBE_NXZ");

	PATCH_PUSH_RET_POLY(0x004A1200 , entity::get_filename, "?get_filename@entity@@UBE?AVstringx@@XZ");

	PATCH_PUSH_RET_POLY(0x004A1230 , entity::get_dirname, "?get_dirname@entity@@UBE?AVstringx@@XZ");
	PATCH_PUSH_RET_POLY(0x004A1260 , entity::has_dirname, "?has_dirname@entity@@UBE_NXZ");

	PATCH_PUSH_RET_POLY(0x004A12A0 , entity::is_a_beam, "?is_a_beam@entity@@UBE_NXZ");
	PATCH_PUSH_RET_POLY(0x004D2940 , entity::is_a_camera, "?is_a_camera@entity@@UBE_NXZ");
	PATCH_PUSH_RET_POLY(0x004A12C0 , entity::is_a_station_camera, "?is_a_station_camera@entity@@UBE_NXZ");
	PATCH_PUSH_RET_POLY(0x004D2960 , entity::is_a_game_camera, "?is_a_game_camera@entity@@UBE_NXZ");
	PATCH_PUSH_RET_POLY(0x004A12E0 , entity::is_a_marky_camera, "?is_a_marky_camera@entity@@UBE_NXZ");
	PATCH_PUSH_RET_POLY(0x004A1300 , entity::is_a_mouselook_camera, "?is_a_mouselook_camera@entity@@UBE_NXZ");
	PATCH_PUSH_RET_POLY(0x004A1320 , entity::is_a_sniper_camera, "?is_a_sniper_camera@entity@@UBE_NXZ");
	PATCH_PUSH_RET_POLY(0x004A1340 , entity::is_a_conglomerate, "?is_a_conglomerate@entity@@UBE_NXZ");

	PATCH_PUSH_RET_POLY(0x004A1360 , entity::is_a_turret, "?is_a_turret@entity@@UBE_NXZ");
	PATCH_PUSH_RET_POLY(0x004A1380 , entity::is_a_ladder, "?is_a_ladder@entity@@UBE_NXZ");
	PATCH_PUSH_RET_POLY(0x004A13A0 , entity::is_a_light_source, "?is_a_light_source@entity@@UBE_NXZ");
	PATCH_PUSH_RET_POLY(0x004A13C0 , entity::is_a_limb_body, "?is_a_limb_body@entity@@UBE_NXZ");
	PATCH_PUSH_RET_POLY(0x004A13E0 , entity::is_a_marker, "?is_a_marker@entity@@UBE_NXZ");
	PATCH_PUSH_RET_POLY(0x004A1400 , entity::is_a_rectangle_marker, "?is_a_rectangle_marker@entity@@UBE_NXZ");
	PATCH_PUSH_RET_POLY(0x004A1420 , entity::is_a_cube_marker, "?is_a_cube_marker@entity@@UBE_NXZ");
	PATCH_PUSH_RET_POLY(0x004A1440 , entity::is_a_crawl_marker, "?is_a_crawl_marker@entity@@UBE_NXZ");
	PATCH_PUSH_RET_POLY(0x004A1460 , entity::is_a_particle_generator, "?is_a_particle_generator@entity@@UBE_NXZ");
	PATCH_PUSH_RET_POLY(0x004A1480 , entity::is_a_physical_entity, "?is_a_physical_entity@entity@@UBE_NXZ");

	PATCH_PUSH_RET_POLY(0x004A14A0 , entity::is_a_crate, "?is_a_crate@entity@@UBE_NXZ");
	PATCH_PUSH_RET_POLY(0x004A14C0 , entity::is_an_item, "?is_an_item@entity@@UBE_NXZ");
	PATCH_PUSH_RET_POLY(0x004A14E0 , entity::is_a_switch, "?is_a_switch@entity@@UBE_NXZ");
	PATCH_PUSH_RET_POLY(0x004A1500 , entity::is_a_visual_item, "?is_a_visual_item@entity@@UBE_NXZ");
	PATCH_PUSH_RET_POLY(0x004A1520 , entity::is_a_handheld_item, "?is_a_handheld_item@entity@@UBE_NXZ");
	PATCH_PUSH_RET_POLY(0x004A1540 , entity::is_a_gun, "?is_a_gun@entity@@UBE_NXZ");
	PATCH_PUSH_RET_POLY(0x004A1560 , entity::is_a_thrown_item, "?is_a_thrown_item@entity@@UBE_NXZ");
	PATCH_PUSH_RET_POLY(0x004A1580 , entity::is_a_melee_item, "?is_a_melee_item@entity@@UBE_NXZ");
	PATCH_PUSH_RET_POLY(0x004A15A0 , entity::is_a_morphable_item, "?is_a_morphable_item@entity@@UBE_NXZ");
	PATCH_PUSH_RET_POLY(0x004A15C0 , entity::is_a_projectile, "?is_a_projectile@entity@@UBE_NXZ");

	PATCH_PUSH_RET_POLY(0x004A15E0 , entity::is_a_rigid_body, "?is_a_rigid_body@entity@@UBE_NXZ");
	PATCH_PUSH_RET_POLY(0x004A1600 , entity::is_a_grenade, "?is_a_grenade@entity@@UBE_NXZ");
	PATCH_PUSH_RET_POLY(0x004A1620 , entity::is_a_rocket, "?is_a_rocket@entity@@UBE_NXZ");
	PATCH_PUSH_RET_POLY(0x004A1640 , entity::is_a_scanner, "?is_a_scanner@entity@@UBE_NXZ");
	PATCH_PUSH_RET_POLY(0x004A1660 , entity::is_a_sky, "?is_a_sky@entity@@UBE_NXZ");

	PATCH_PUSH_RET_POLY(0x004A1680 , entity::frame_done, "?frame_done@entity@@UAEXXZ");
	PATCH_PUSH_RET_POLY(0x004A16A0 , entity::add_position_increment, "?add_position_increment@entity@@UAE_NAAVvector3d@@@Z");
	PATCH_PUSH_RET_POLY(0x004A16C0 , entity::terrain_position, "?terrain_position@entity@@UBEABVvector3d@@XZ");
	PATCH_PUSH_RET_POLY(0x004A16E0 , entity::terrain_radius, "?terrain_radius@entity@@UBEMXZ");

	PATCH_PUSH_RET_POLY(0x004A1700 , entity::get_colgeom_root_po, "?get_colgeom_root_po@entity@@UBEABVpo@@XZ");
	PATCH_PUSH_RET_POLY(0x004A1720 , entity::get_colgeom_root, "?get_colgeom_root@entity@@UBEPBV1@XZ");

	PATCH_PUSH_RET_POLY(0x004EB200 , entity::add_me_to_region, "?add_me_to_region@entity@@UAEXPAVregion@@@Z");
	PATCH_PUSH_RET_POLY(0x004EB220 , entity::remove_me_from_region, "?remove_me_from_region@entity@@UAEXPAVregion@@@Z");
	PATCH_PUSH_RET_POLY(0x004A1740 , entity::get_region, "?get_region@entity@@UBEPAVnode@?$graph@Vstringx@@PAVregion@@PAVportal@@U?$less@Vstringx@@@std@@@@XZ");

	PATCH_PUSH_RET_POLY(0x004EC790 , entity::force_current_region, "?force_current_region@entity@@UAEXXZ");

	PATCH_PUSH_RET_POLY(0x004A1760 , entity::camera_set_target, "?camera_set_target@entity@@UAEXABVvector3d@@@Z");
	PATCH_PUSH_RET_POLY(0x004A1780 , entity::camera_set_roll, "?camera_set_roll@entity@@UAEXM@Z");
	PATCH_PUSH_RET_POLY(0x004A17A0 , entity::camera_set_collide_with_world, "?camera_set_collide_with_world@entity@@UAEX_N@Z");
	PATCH_PUSH_RET_POLY(0x004A17C0 , entity::camera_slide_to, "?camera_slide_to@entity@@UAE_NABVvector3d@@0MM@Z");
	PATCH_PUSH_RET_POLY(0x004A17E0 , entity::camera_slide_to_orbit, "?camera_slide_to_orbit@entity@@UAE_NABVvector3d@@MMMM@Z");
	PATCH_PUSH_RET_POLY(0x004A1800 , entity::camera_orbit, "?camera_orbit@entity@@UAEXABVvector3d@@MMM@Z");

	PATCH_PUSH_RET_POLY(0x004EFD00 , entity::get_color, "?get_color@entity@@UBEABVcolor@@XZ");
	PATCH_PUSH_RET_POLY(0x004EFD70 , entity::set_color, "?set_color@entity@@UAEXABVcolor@@@Z");
	PATCH_PUSH_RET_POLY(0x004EFDE0 , entity::get_additive_color, "?get_additive_color@entity@@UBEABVcolor@@XZ");
	PATCH_PUSH_RET_POLY(0x004EFE50 , entity::set_additive_color, "?set_additive_color@entity@@UAEXABVcolor@@@Z");
	PATCH_PUSH_RET_POLY(0x004EFEC0 , entity::get_near_range, "?get_near_range@entity@@UBEMXZ");
	PATCH_PUSH_RET_POLY(0x004EFF30 , entity::set_near_range, "?set_near_range@entity@@UAEXM@Z");
	PATCH_PUSH_RET_POLY(0x004EFFA0 , entity::get_cutoff_range, "?get_cutoff_range@entity@@UBEMXZ");
	PATCH_PUSH_RET_POLY(0x004F0010 , entity::set_cutoff_range, "?set_cutoff_range@entity@@UAEXM@Z");
	PATCH_PUSH_RET_POLY(0x004F0080 , entity::add_light_category, "?add_light_category@entity@@UAEXH@Z");
	PATCH_PUSH_RET_POLY(0x004F00F0 , entity::remove_light_category, "?remove_light_category@entity@@UAEXH@Z");

	PATCH_PUSH_RET_POLY(0x004EBEE0 , entity::get_light_set, "?get_light_set@entity@@UBEPAVlight_manager@@XZ");

	PATCH_PUSH_RET_POLY(0x004A1820 , entity::is_frame_delta_valid, "?is_frame_delta_valid@entity@@UBE_NXZ");
	PATCH_PUSH_RET_POLY(0x004A1850 , entity::is_last_frame_delta_valid, "?is_last_frame_delta_valid@entity@@UBE_NXZ");

	PATCH_PUSH_RET_POLY(0x004A18B0 , entity::get_last_capsule, "?get_last_capsule@entity@@UAEHXZ");

	PATCH_PUSH_RET_POLY(0x004F2980 , entity::apply_damage, "?apply_damage@entity@@UAEXHABVvector3d@@0HPAV1@H@Z");

	PATCH_PUSH_RET_POLY(0x004A1900 , entity::get_detonate_position, "?get_detonate_position@entity@@UBE?AVvector3d@@XZ");

	PATCH_PUSH_RET_POLY(0x004EC220 , entity::get_distance_fade_ok, "?get_distance_fade_ok@entity@@UBE_NXZ");

	PATCH_PUSH_RET_POLY(0x004F3590 , entity::allow_targeting, "?allow_targeting@entity@@UBE_NXZ");

	PATCH_PUSH_RET_POLY(0x004F4840 , entity::suspend, "?suspend@entity@@UAEXXZ");
	PATCH_PUSH_RET_POLY(0x004F4880 , entity::unsuspend, "?unsuspend@entity@@UAEXXZ");

	PATCH_PUSH_RET_POLY(0x004F4A60 , entity::possibly_aging, "?possibly_aging@entity@@UBE_NXZ");

	PATCH_PUSH_RET_POLY(0x004F48C0 , entity::is_alive, "?is_alive@entity@@UBE_NXZ");
	PATCH_PUSH_RET_POLY(0x004F48F0 , entity::is_dying, "?is_dying@entity@@UBE_NXZ");
	PATCH_PUSH_RET_POLY(0x004F4910 , entity::is_alive_or_dying, "?is_alive_or_dying@entity@@UBE_NXZ");

	PATCH_PUSH_RET_POLY(0x004F4D90 , entity::preload, "?preload@entity@@UAEXXZ");

	PATCH_PUSH_RET_POLY(0x004EBDC0 , entity::get_last_position, "?get_last_position@entity@@UBE?AVvector3d@@XZ");

	PATCH_PUSH_RET(0x004EFBC0 , entity::set_age);
	PATCH_PUSH_RET(0x004EFBA0 , entity::get_age);
	PATCH_PUSH_RET(0x004EFBE0 , entity::rebirth);

	PATCH_PUSH_RET_POLY(0x004EFB70 , entity::invalidate_colgeom, "?invalidate_colgeom@entity@@UAEXXZ");

	PATCH_PUSH_RET_POLY(0x004EFAE0 , entity::get_closest_point_along_dir, "?get_closest_point_along_dir@entity@@UBEXPAVvector3d@@ABV2@@Z");

	PATCH_PUSH_RET(0x004EFB00 , entity::get_updated_closest_point_along_dir);
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
