////////////////////////////////////////////////////////////////////////////////
/*
  item.cpp

  Items in game.  Inventory management and Item effects

*/
////////////////////////////////////////////////////////////////////////////////
#include "global.h"

#include "osalloc.h"

//!#include "character.h"
#include "stringx.h"
#include "item.h"
#include "wds.h"
//#include "msgboard.h"
#include "terrain.h"
#include "hwaudio.h"
#include "script_object.h"
//!#include "attrib.h"

#include "vm_thread.h"
#include "controller.h"
//#include "interface.h"

#if _CONSOLE_ENABLE
#include "console.h"
#endif

//#include "dread_net.h"
#include "entity_maker.h"
#include "entityflags.h"
// BIGCULL #include "spiderman_controller.h"
// BIGCULL #include "damage_interface.h"
#include "sound_interface.h"


////////////////////////////////////////////////////////////////
//  item
////////////////////////////////////////////////////////////////



item::item( const entity_id& _id, unsigned int _flags )
  : entity( _id, ENTITY_ITEM, _flags )
{
  usage_type = INVALID;
  picked_up = false;
  pickup_timer = 0;

  set_radius(.25f);

  count = default_count = 1;

//  need_to_initialize = false;

#if _ENABLE_WORLD_EDITOR
  original_count = count;
#endif

  max_num = 10;

  icon_scale = 1.0f;

  interface_orientation = 225.0f;

  //set_gravity( false );
  //set_physical( false );

//  dread_net_use_cue = dread_net::UNDEFINED_AV_CUE;

  preload_script_called = false;
  item_script_called = false;
  linked = false;
}


item::item( const entity_id& _id,
            entity_flavor_t _flavor,
            unsigned int _flags )
  : entity( _id, _flavor, _flags )
{
  usage_type = INVALID;
  picked_up = false;
  pickup_timer = 0;


  set_radius(.25f);
  default_count = count = 1;
//  need_to_initialize = false;


#if _ENABLE_WORLD_EDITOR

  original_count = count;
#endif

  max_num = 10;


  icon_scale = 1.0f;


  interface_orientation = 225.0f;


//  set_gravity( false );
//  set_physical( false );

//  dread_net_use_cue = dread_net::UNDEFINED_AV_CUE;

  preload_script_called = false;
  item_script_called = false;
  linked = false;

}



item::~item()

{
}



///////////////////////////////////////////////////////////////////////////////
// NEWENT File I/O

///////////////////////////////////////////////////////////////////////////////

item::item( chunk_file& fs,

            const entity_id& _id,
            entity_flavor_t _flavor,
            unsigned int _flags )
  : entity( fs, _id, _flavor, _flags )
{
  // TODO: handle flags correctly under NEWENT
  usage_type = INVALID;
  picked_up = false;
  pickup_timer = 0;


  set_radius(.25f);
  default_count = count = 1;
//  need_to_initialize = false;


#if _ENABLE_WORLD_EDITOR
  original_count = count;

#endif

  max_num = 10;


  icon_scale = 1.0f;

  interface_orientation = 225.0f;

//  set_gravity( false );
//  set_physical( false );


  set_active( true );
  set_stationary( false );

//  dread_net_use_cue = dread_net::UNDEFINED_AV_CUE;

  preload_script_called = false;
  item_script_called = false;
  linked = false;
}

void item::read_item_data( chunk_file& fs, stringx& label )

{
  // Now the actual data

  // get item name from filename
  filespec spec( fs.get_name() );

  name = spec.name;
  name.to_upper();

  stringx usage_type_name;
  serial_in( fs, &usage_type_name );
  if ( usage_type_name == "INSTANT" )
    usage_type = INSTANT;
  else if ( usage_type_name == "INVENTORY" )
    usage_type = INVENTORY;
  else if ( usage_type_name == "UTILITY" )
    usage_type = UTILITY;
  else if ( usage_type_name == "PERMANENT" )

    usage_type = PERMANENT;

  else if ( usage_type_name == "GUN" )
    usage_type = GUN;
  else if ( usage_type_name == "MELEE" )
    usage_type = MELEE;
  else if ( usage_type_name == "THROWN" )
    usage_type = THROWN;
  else if ( usage_type_name == "AMMO" )
    usage_type = AMMO;

  else if ( usage_type_name == "HEALTH" )

    usage_type = HEALTH;
  else if ( usage_type_name == "ARMOR" )

    usage_type = ARMOR;

  else if ( usage_type_name == "ENERGY" )

    usage_type = ENERGY;
  else
    error( fs.get_name() + ": invalid usage type: " + usage_type_name );

  for ( serial_in(fs,&label); label!=chunkend_label; serial_in(fs,&label) )

  {
    if ( label == "count" )
    {
      serial_in( fs, &default_count );

      count = default_count;


      #if _ENABLE_WORLD_EDITOR
        original_count = count;
      #endif
    }
    else if ( label == "max" )
    {
      serial_in( fs, &max_num );
    }
    else if ( label == "icon_scale" )
    {
      serial_in( fs, &icon_scale );
    }
    else if ( label == "interface_orientation" )
    {
      serial_in( fs, &interface_orientation );
    }
/*
    else if ( label == "dread_net_cue" )
    {
      stringx cue;
      serial_in(fs, &cue);
      cue.to_upper();

      dread_net_use_cue = dread_net::get_cue_type(cue);
    }
*/
    else
    {
      error( fs.get_filename() + ": unknown keyword '" + label + "' in item section" );
    }
  }

}





bool item::handle_enx_chunk( chunk_file& fs, stringx& label )
{
	PANIC;
	return false;
}




///////////////////////////////////////////////////////////////////////////////

// Old File I/O
///////////////////////////////////////////////////////////////////////////////


item::item( const stringx& item_type_filename,
            const entity_id& _id,
            entity_flavor_t _flavor,
            bool _active,
            bool _stationary,
            bool delete_stream)
  :   entity( _id, _flavor )
{
	PANIC;
}



///////////////////////////////////////////////////////////////////////////////
// Instancing
///////////////////////////////////////////////////////////////////////////////


entity* item::make_instance( const entity_id& _id,
                             unsigned int _flags ) const
{
  item* newit = NEW item( _id, _flags );
  newit->copy_instance_data( *this );

  return (entity*)newit;
}


void item::copy_instance_data( const item& b )
{
  entity::copy_instance_data( b );

  usage_type = b.usage_type;
  default_count = count = b.default_count;

#if _ENABLE_WORLD_EDITOR
  original_count = count;
#endif

  max_num = b.max_num;
  name = b.name;

  icon_scale = b.icon_scale;

  interface_orientation = b.interface_orientation;


//  set_gravity( false );
//  set_physical( false );

  set_active( true );
  set_stationary( false );


//  dread_net_use_cue = b.dread_net_use_cue;
  linked = b.linked;
}


// This function allows the entity to any post-level-load initialization
void item::initialize()
{
//  if ( need_to_initialize )
  {
    spawn_item_script();

  }
}


///////////////////////////////////////////////////////////////////////////////
// Misc.

///////////////////////////////////////////////////////////////////////////////

// return true if given item is considered the same (for inventory purposes)
bool item::is_same_item( const item& b ) const
{
  if ( get_usage_type() != b.get_usage_type() )

    return false;
  switch ( get_usage_type() )
  {
    case INVENTORY:
    case UTILITY:
    case PERMANENT:
      return false;
    case GUN:
    case THROWN:
      return get_name() == b.get_name();
    default:
      break;
  }

  return true;
}


// @Ok
// @Matching
void item::render(rational_t detail, render_flavor_t flavor, rational_t entity_translucency_pct)
{
	if ( count > 0 )
	{
		entity::render(detail, flavor, entity_translucency_pct );
	}
}

// @Ok
// @Matching
void item::set_count(int c)
{
	this->count = c;
	// @Patch - added this
	if (this->picked_up)
	{
		if (c)
		{
			this->set_family_visible(true, false);
			this->compute_sector(GET_WORLD_PTR->get_the_terrain(), false);

			this->picked_up = false;
			this->item_count_rel_two = 0.0f;
		}
	}
}


bool item::check_for_pickup()
{
  bool outval = false;
  #ifdef GCCULL

  static hires_clock_t gulp_timer;


  pickup_timer -= g_world_ptr->get_cur_time_inc();

  if ( pickup_timer < 0 )
    pickup_timer = 0;

  if ( count && pickup_timer==0 && !was_spawned())

  {
    if ( g_world_ptr->get_hero_ptr(g_game_ptr->get_active_player())->get_hit_points() > 0 && g_world_ptr->get_hero_ptr(g_game_ptr->get_active_player())->point_in_radius( get_abs_position() ) )
    {
      int old_num = get_number();

      outval = give_to_entity( g_world_ptr->get_hero_ptr(g_game_ptr->get_active_player()) );

      // item pick up effects
      extern int global_frame_counter;
      if ( ( outval || get_number()!=old_num ) && global_frame_counter > 5 )
      {
        // Don't over-do the 'gulp' item pick-up sound if we get a lot of stuff at once.
        if (gulp_timer.elapsed() > 0.2f)
        {
          gulp_timer.reset();
          #ifdef GCCULL
          if(g_world_ptr->get_hero_ptr(0)->has_sound_ifc())
          {
            static pstring pickup("PICK_UP");
            g_world_ptr->get_hero_ptr(g_game_ptr->get_active_player())->sound_ifc()->play_3d_sound_grp(pickup);
          }
          #endif
        }
      }

      if ( outval )
        remove_from_terrain();
    }

  }
	#endif
	
  return outval;
}

// returns true if the inventory did NOT already have a copy of the item
bool item::give_to_entity(entity *target)
{
  bool outval = false;


  // first decide whether this is the first time an item of this kind has been picked up
  switch ( get_usage_type() )
  {
    case UTILITY:
    case INVENTORY:
    case GUN:
    case THROWN:
      outval = target->find_like_item(this)? false : true;
      break;

    case INSTANT:
    case PERMANENT:
      outval = true;
      break;
    case AMMO:

    case HEALTH:
    case ARMOR:
    case ENERGY:
      outval = false;
      break;

    default:
      assert(0);
      break;
  }

  // if the character had not previously picked up an item of this kind,
  // automatically spawn and run the special X_callbacks(item) script function
  if ( outval )

  {
    if ( g_world_ptr->get_current_level_global_script_object() )
      spawn_item_script();
//    else
//      need_to_initialize = true;
  }

  int old_count = get_number();
  bool force_raise_signal = false;

  switch( get_usage_type() )
  {
    case INSTANT:
      #if _CONSOLE_ENABLE
      console_log("%s picked up a '%s' (instant)", target->get_name().c_str(), get_name().c_str());
      #endif
      apply_effects( target );
      set_count( 0 );
      break;

    case UTILITY:
    case INVENTORY:
    case GUN:
    case THROWN:
      {
#if _CONSOLE_ENABLE
        int old = get_number();
#endif


        bool new_item = target->add_item( this );

        if ( new_item )
        {
          // this entity needs to frame_advance() even when ineligible by the usual rules

          g_world_ptr->guarantee_active( this );
          force_raise_signal = true;
        }

#if _CONSOLE_ENABLE

        if(new_item || old != get_number())
          console_log("%s picked up a '%s' (x%d)", target->get_name().c_str(), get_name().c_str(), new_item ? get_number() : old - get_number());
#endif
      }

      break;

    case AMMO:

      {
#if 0 // BIGCULL
        if(target->is_hero())
        {
          g_spiderman_controller_ptr->set_webbing_carts(g_spiderman_controller_ptr->get_webbing_carts() + count);
          set_count(0);
        }
#endif
      }
      break;
/*
      if(target->get_ammo_points() + get_count() > target->get_max_ammo_points())

      {
        int delta = (target->get_max_ammo_points() - target->get_ammo_points());
        if(delta)
        {
          target->inc_ammo_points(delta);
          set_count(get_count() - delta);

          #if _CONSOLE_ENABLE
          console_log("%s picked up ammo (x%d, left x%d)", target->get_name().c_str(), delta, get_count());
          #endif
        }
      }
      else
      {
        #if _CONSOLE_ENABLE

        console_log("%s picked up ammo (x%d)", target->get_name().c_str(), get_count());
        #endif


        target->inc_ammo_points(get_count());
        set_count(0);
      }
      break;
*/

    case HEALTH:
      {
        error ("Health items not supported in KS.");
#if 0 // BIGCULL
        if(target->has_damage_ifc())
        {
          target->damage_ifc()->inc_hit_points(count);
          set_count(0);
        }
#endif // BIGCULL
      }
      break;
/*
      if(target->get_hit_points() + get_count() > target->get_full_hit_points())
      {
        int delta = (target->get_full_hit_points() - target->get_hit_points());
        if(delta)
        {
          target->inc_hit_points(delta);
          set_count(get_count() - delta);

          #if _CONSOLE_ENABLE
          console_log("%s picked up health (x%d, left x%d)", target->get_name().c_str(), delta, get_count());
          #endif
        }
      }
      else
      {
        #if _CONSOLE_ENABLE
        console_log("%s picked up health (x%d)", target->get_name().c_str(), get_count());
        #endif

        target->inc_hit_points(get_count());
        set_count(0);
      }
      break;

*/

    case ARMOR:
      {
        error ("Armor items not supported in KS.");
#if 0 // BIGCULL
        if(target->has_damage_ifc())
        {
          target->damage_ifc()->inc_armor_points(count);
          set_count(0);

        }
#endif //BIGCULL

      }
      break;
/*

      if ( target->get_armor_points() < target->get_full_armor_points() )
      {
        #if _CONSOLE_ENABLE
        console_log("%s picked up armor (x%d)", target->get_name().c_str(), get_count());
        #endif

        target->inc_armor_points( get_count() );
        set_count( 0 );
      }
      break;
*/
/*


    case ENERGY:
      if ( target->get_nanotech_energy() < target->get_full_nanotech_energy() )
      {
        #if _CONSOLE_ENABLE
        console_log("%s picked up nanotech energy (x%d)", target->get_name().c_str(), get_count());
        #endif

        target->inc_nanotech_energy( get_count() );
        set_count( 0 );
      }
      break;
*/
/*
    case PERMANENT:
    {
      #if _CONSOLE_ENABLE
      console_log("%s picked up a '%s' (perm)", target->get_name().c_str(), get_name().c_str());

      #endif


      target->add_permanent_item(this);
      apply_effects( target );

      force_raise_signal = true;
    }
    break;
*/
    default:
      assert(0);
      break;
  }


  if(old_count != get_number() || force_raise_signal)
    raise_signal( PICKUP );

  if ( outval || get_count()==0 )

    picked_up = true;

  if ( target == g_world_ptr->get_hero_ptr(g_game_ptr->get_active_player()) )
  {
//    target->get_player_controller()->add_inv_item();
    disgorge_items(target);
  }

//  if(g_world_ptr->get_dread_net())
//    g_world_ptr->get_dread_net()->add_cue(dread_net::get_cue_type("PICKUP_ITEM"), target);

  return outval;
}



void item::spawn_preload_script()
{
  if(!preload_script_called)
  {
    preload_script_called = true;


    // there might be a script function for preloading additional assets needed by item
    stringx preload_func_name = get_name() + "_preload()";
    preload_func_name.to_lower();
    script_object* gso = g_world_ptr->get_current_level_global_script_object();
    assert( gso );
    int fidx = gso->find_func( preload_func_name );
    if ( fidx >= 0 )
    {
      script_object::instance* gsoi = g_world_ptr->get_current_level_global_script_object_instance();
      assert( gsoi );
      // spawn thread for function

      vm_thread* newt = gsoi->add_thread( &gso->get_func(fidx) );
      // run the NEW thread immediately
      gsoi->run_single_thread( newt, false );
    }
  }
}


void item::spawn_item_script()
{

  if(!item_script_called)
  {
    item_script_called = true;

    // search for matching script function
    stringx fname = get_name() + "_callbacks(item)";
    fname.to_lower();
    script_object* gso = g_world_ptr->get_current_level_global_script_object();
    int fidx = gso->find_func( fname );
    if ( fidx >= 0 )
    {
      script_object::instance* gsoi = g_world_ptr->get_current_level_global_script_object_instance();
      // spawn thread for function
      char parms[4];

      *(unsigned int*)parms = (unsigned int)this;
      vm_thread* newt = gsoi->add_thread( &gso->get_func(fidx), parms );

      // run the NEW thread immediately
      gsoi->run_single_thread( newt, false );
    }
  }
}


void item::preload()

{
  entity::preload();

  spawn_preload_script();

}


// @Ok
// @Matching
bool item::is_picked_up()
{
  return picked_up;
}


const rational_t ITEM_ROTATION_SPEED = PI;

void item::frame_advance( time_value_t t )
{

	typedef void (__fastcall *func_ptr)(item*, int, time_value_t);
	func_ptr func = (func_ptr)0x005FE8E0;
	func(this, 0, t);
	return;
PANIC;
  if ( !is_picked_up() )
  {
    if ( has_physical_ifc() )
      entity::frame_advance( t );
    else
    {
      // rotate item about absolute (non-affine) Y axis
      po r;
      r.set_rotate_y( ITEM_ROTATION_SPEED * t );
      po newpo = get_rel_po();
      vector3d p = newpo.get_position();

      newpo.set_position( ZEROVEC );

      fast_po_mul(newpo, newpo, r);
//      newpo = newpo * r;

      newpo.fixup();

      newpo.scale( icon_scale );
      newpo.set_position( p );
      set_rel_po( newpo );
    }
  }
}



// @Ok
// @Matching
void item::apply_effects( entity* target )
{
  raise_signal( USE );


//  if(dread_net_use_cue != dread_net::UNDEFINED_AV_CUE)
//    g_world_ptr->get_dread_net()->add_cue((dread_net::eAVCueType)dread_net_use_cue, target);
}



// This function allows parsing instance data according to entity type.
// If it recognizes the given chunk_flavor as a chunk of instance
// data for this type, it will parse the data; otherwise it will hand
// the parsing up to the parent class.
bool item::parse_instance( const stringx& pcf, chunk_file& fs )
{
  if ( pcf == stringx("item") )
  {
    stringx cf;
    for ( serial_in(fs,&cf); cf!=chunkend_label; serial_in(fs,&cf) )
    {

      if ( cf == stringx("count") )
      {
        serial_in( fs, &count );

        #if _ENABLE_WORLD_EDITOR
          original_count = count;
        #endif
      }
    }
  }
  else
    return entity::parse_instance( pcf, fs );

  return true;
}

/*!
int item::get_max_allowed(character *chr)

{
  return max_num;
}
!*/

/////////////////////////////////////////////////////////////////////////////
// Event signals
/////////////////////////////////////////////////////////////////////////////

// This static function must be implemented by every class which can generate
// signals, and is called once only by the application for each such class;

// the effect is to register the name and local id of each signal with the
// signal_manager.  This call must be performed before any signal objects are
// actually created for this class (via signaller::signal_ptr(); see signal.h).

// @Ok
// @Matching
void item::register_signals()
{
  #define MAC(label,str)  signal_manager::inst()->insert( str, label );
  #include "item_signals.h"
  #undef MAC
}

static const char* item_signal_names[] =
{
  #define MAC(label,str)  str,
  #include "item_signals.h"
  #undef MAC
};

unsigned short item::get_signal_id( const char *name )
{
  int idx;

  for( idx = 0; idx < (int)(sizeof(item_signal_names)/sizeof(char*)); ++idx )
  {
    int offset = strlen(item_signal_names[idx])-strlen(name);

    if( offset > (int)strlen( item_signal_names[idx] ) )

      continue;

    if( !strcmp(name,&item_signal_names[idx][offset]) )
      return( idx + PARENT_SYNC_DUMMY + 1 );
  }

  // not found
  return entity::get_signal_id( name );
}

// This virtual function, used only for debugging purposes, returns the
// name of the given local signal
const char* item::get_signal_name( unsigned short idx ) const
{
  assert( idx < N_SIGNALS );
  if ( idx <= PARENT_SYNC_DUMMY )
    return entity::get_signal_name( idx );
  return item_signal_names[idx-PARENT_SYNC_DUMMY-1];
}


#if _VIS_ITEM_DEBUG_HELPER
beam *visual_item::visual_item_beamer = NULL;
#endif

visual_item::visual_item( const entity_id& _id, unsigned int _flags )
  :   entity( _id, ENTITY_ENTITY, _flags )

{
#if _VIS_ITEM_DEBUG_HELPER
  render_axis = false;
#endif


//  set_gravity( false );
//  set_physical( false );
  owner = NULL;
}

visual_item::~visual_item()
{
#if _VIS_ITEM_DEBUG_HELPER
  if ( visual_item_beamer != NULL )
  {
    delete visual_item_beamer;
    visual_item_beamer = NULL;
  }
#endif
}

void visual_item::set_placement(entity *_owner, const stringx& limb, const po& offset, bool drawn)
{

  owner = _owner;


  assert(owner);


  switch(owner->get_flavor())
  {
/*!
    case ENTITY_CHARACTER:
    {
      character *tmp_owner = (character *)owner;
      if(tmp_owner->is_active())
      {
        entity *lb;


        anim_id_t aid = anim_id_manager::inst()->anim_id( limb );

        if ( tmp_owner->limb_valid(aid) )
          lb = tmp_owner->limb_ptr(aid)->get_body();
        else if((lb = tmp_owner->has_limb_tree() ? tmp_owner->nonlimb_ptr(aid) : NULL) == NULL)
        {
          aid = drawn ? RIGHT_HAND : RIGHT_UPPER_LEG;

          if ( tmp_owner->limb_valid(aid) )
            lb = tmp_owner->limb_ptr(aid)->get_body();
//          else
//            error("No valid attach limb/nonlimb");
        }

        set_parent( lb );
      }
    }
    break;
!*/
    case ENTITY_CONGLOMERATE:
    {
      conglomerate *conglom = (conglomerate *)owner;


      entity *node = NULL /*conglom->get_member(limb)*/;
	  PANIC;
      if ( node )
        link_ifc()->set_parent( node );
      else
        link_ifc()->set_parent( owner );
    }
    break;

    default:
    {
      link_ifc()->set_parent(owner);
    }
    break;
  }

  set_rel_po(offset);

  set_visible(true);
}


#if _VIS_ITEM_DEBUG_HELPER
bool g_render_vis_item_debug_info = false;

void visual_item::set_placement(entity *_owner, const stringx& limb, rational_t s, const vector3d& p, const vector3d& r, bool drawn)
{
  owner = _owner;

  assert(owner);

  switch(owner->get_flavor())

  {
/*!    case ENTITY_CHARACTER:
    {
      character *tmp_owner = (character *)owner;
      if(tmp_owner->is_active())
      {

        entity* lb = tmp_owner;

        anim_id_t aid = anim_id_manager::inst()->anim_id( limb );
        if ( tmp_owner->limb_valid(aid) )
          lb = tmp_owner->limb_ptr(aid)->get_body();
        else if((lb = tmp_owner->has_limb_tree() ? tmp_owner->nonlimb_ptr(aid) : NULL) == NULL)
        {
          aid = drawn ? RIGHT_HAND : RIGHT_UPPER_LEG;


          if ( tmp_owner->limb_valid(aid) )
            lb = tmp_owner->limb_ptr(aid)->get_body();
//          else
//            error("No valid attach limb/nonlimb");
        }

        set_parent( lb );
      }
    }
    break;
!*/
    case ENTITY_CONGLOMERATE:
    {
      conglomerate *conglom = (conglomerate *)owner;

      //entity *node = conglom->get_member( limb );
      entity *node = NULL;
	  PANIC;
      if ( node )

        link_ifc()->set_parent( node );
      else
        link_ifc()->set_parent( owner );

    }
    break;

    default:
    {
      link_ifc()->set_parent(owner);

    }
    break;
  }

  alter_placement( s, p, r );
  set_visible( true );
}

void visual_item::alter_placement(rational_t s, const vector3d& p, const vector3d& r)

{
  po offset;
  po tmp1;

  offset = po_identity_matrix;
  offset.set_position( p );

  tmp1 = po_identity_matrix;
  tmp1.set_rotate_x( DEG_TO_RAD(r.x) );
  offset.add_increment( &tmp1 );


  tmp1 = po_identity_matrix;
  tmp1.set_rotate_y( DEG_TO_RAD(r.y) );
  offset.add_increment( &tmp1 );

  tmp1 = po_identity_matrix;
  tmp1.set_rotate_z( DEG_TO_RAD(r.z) );

  offset.add_increment( &tmp1 );

  tmp1 = po_identity_matrix;
  tmp1.set_scale( vector3d(s, s, s) );
  offset.add_increment( &tmp1 );


  set_rel_po(offset);
}

#endif


// @Ok
// @Matching
light_manager* visual_item::get_light_set()
{
  if(owner)
  {
    if(owner->is_visible())
      return owner->get_light_set();
  }
  return NULL;
}

// @Ok
// @Matching
render_flavor_t visual_item::render_passes_needed() const
{

  render_flavor_t passes = entity::render_passes_needed();
/*!
  if ( owner && owner->is_a_character() )
  {
    character* c = (character*)owner;
    if ( c->is_visible()
      && c->is_alive_or_dying()
      && c->render_passes_needed() & RENDER_TRANSLUCENT_PORTION
      && ( c->get_death_fade_alpha()<1.0f || c->get_translucency_for_stealth()<1.0f )
      )
    {
      passes = RENDER_TRANSLUCENT_PORTION;
    }
  }
!*/
  return passes;
}

// @Ok
// @Matching
void visual_item::render(rational_t detail, render_flavor_t flavor, rational_t entity_translucency_pct )
{

#if _VIS_ITEM_DEBUG_HELPER
  if ( render_axis )
  {
    if ( visual_item_beamer == NULL )
      visual_item_beamer = NEW beam(entity_id::make_unique_id(), (unsigned int)(EFLAG_ACTIVE|EFLAG_MISC_NONSTATIC));

    if ( is_visible() )
    {
      vector3d vec = get_abs_position();
      entity *par = (entity *)link_ifc()->get_parent();

      visual_item_beamer->set_point_to_point(vec, vec + (par ? par->get_abs_po().fast_8byte_non_affine_xform(XVEC) : XVEC));

      visual_item_beamer->set_beam_color(color32_red);
      visual_item_beamer->frame_advance(0);
      visual_item_beamer->render(visual_item_beamer->get_max_polys(), RENDER_TRANSLUCENT_PORTION | ((visual_item_beamer->get_flags()&EFLAG_MISC_NO_CLIP_NEEDED)?0:RENDER_CLIPPED_FULL_DETAIL), 1.0f);

      visual_item_beamer->set_point_to_point(vec, vec + (par ? par->get_abs_po().fast_8byte_non_affine_xform(YVEC) : YVEC));
      visual_item_beamer->set_beam_color(color32_green);
      visual_item_beamer->frame_advance(0);
      visual_item_beamer->render(visual_item_beamer->get_max_polys(), RENDER_TRANSLUCENT_PORTION | ((visual_item_beamer->get_flags()&EFLAG_MISC_NO_CLIP_NEEDED)?0:RENDER_CLIPPED_FULL_DETAIL), 1.0f);

      visual_item_beamer->set_point_to_point(vec, vec + (par ? par->get_abs_po().fast_8byte_non_affine_xform(ZVEC) : ZVEC));
      visual_item_beamer->set_beam_color(color32_blue);
      visual_item_beamer->frame_advance(0);
      visual_item_beamer->render(visual_item_beamer->get_max_polys(), RENDER_TRANSLUCENT_PORTION | ((visual_item_beamer->get_flags()&EFLAG_MISC_NO_CLIP_NEEDED)?0:RENDER_CLIPPED_FULL_DETAIL), 1.0f);
    }
  }
#endif


	entity::render(detail, flavor, entity_translucency_pct );

/*!
  if ( owner && owner->is_a_character() )
  {
    character* c = (character*)owner;
    if ( c->is_visible() && c->is_alive_or_dying() )
    {
      float new_translucency_pct = entity_translucency_pct;
      #if !defined(TARGET_MKS)
      if ( flavor & RENDER_TRANSLUCENT_PORTION )
      #endif
      {

        new_translucency_pct *= c->get_translucency_for_stealth();
        new_translucency_pct = min( new_translucency_pct, c->get_death_fade_alpha() );
      }
      entity::render( detail, flavor, new_translucency_pct );
    }
  }
!*/
}



















morphable_item::morphable_item( const entity_id& _id, unsigned int _flags )
  :   item( _id, _flags )
{
  ranges.resize(0);
  old_count = -1;
}


morphable_item::morphable_item( const entity_id& _id,
            entity_flavor_t _flavor,
            unsigned int _flags )
  :   item( _id, _flavor, _flags )
{
  ranges.resize(0);
  old_count = -1;
}


morphable_item::~morphable_item()
{
  dump_ranges();

  old_count = -1;
}



///////////////////////////////////////////////////////////////////////////////
// File I/O
///////////////////////////////////////////////////////////////////////////////


morphable_item::morphable_item( const stringx& item_type_filename,
            const entity_id& _id,
            entity_flavor_t _flavor,
            bool _active,
            bool _stationary )
  :   item( item_type_filename, _id, _flavor, _active, _stationary, SKIP_DELETE_STREAM)
{
	PANIC;
}



void morphable_item::read_morph_ranges( chunk_file& fs, stringx& label )
{

  assert(label == "morph_ranges:");

  serial_in( fs, &label );


  while(!(label== chunkend_label))
  {
    if(label == "range")
    {
      morphable_item_range *range = NEW morphable_item_range();

      serial_in( fs, &range->low );

      serial_in( fs, &label );
      if(label == "-")
      {
        serial_in( fs, &range->high );
        serial_in( fs, &range->vis_rep );
      }
      else
      {
        range->vis_rep = label;
      }

      range->vis_rep = fs.get_dir() + range->vis_rep;

      visual_rep* v = load_new_visual_rep( range->vis_rep, 0 );
      if( v->get_type()==VISREP_PMESH )
        static_cast<vr_pmesh*>(v)->shrink_memory_footprint();

      ranges.push_back(range);
    }
    else
    {
      error( fs.get_filename() + ": unknown keyword '" + label + "' in morph_ranges section" );
    }

    // get the next label
    serial_in( fs, &label );
  }

}



///////////////////////////////////////////////////////////////////////////////
// Instancing
///////////////////////////////////////////////////////////////////////////////


entity* morphable_item::make_instance( const entity_id& _id,
                             unsigned int _flags ) const
{
  morphable_item* newit = NEW morphable_item( _id, _flags );
  newit->copy_instance_data( *this );
  return (entity*)newit;
}


void morphable_item::copy_instance_data( const morphable_item& b )
{

  item::copy_instance_data( b );

  // it doesn't make sense for a morphable item not to be visible
  set_visible( true );

  dump_ranges();

  std::vector<morphable_item_range *>::const_iterator range = b.ranges.begin();
  std::vector<morphable_item_range *>::const_iterator range_end = b.ranges.end();

  for( ; range != range_end; ++range)
    ranges.push_back((*range)->make_instance());

  old_count = -1;
}

// @Ok
// @Matching
void morphable_item::frame_advance(time_value_t t)
{
  item::frame_advance(t);

  if(count != old_count)
    set_range_visrep(count);
}

// @Ok
// @Matching
INLINE void morphable_item::set_range_visrep(int cnt)
{
	std::vector<morphable_item_range *>::iterator range = ranges.begin();
	std::vector<morphable_item_range *>::iterator range_end = ranges.end();

  for( ; range != range_end; ++range)
  {
    if((*range)->in_range(count) && !(*range)->in_range(old_count))
    {
      change_visrep((*range)->vis_rep);

      set_flag(EFLAG_GRAPHICS, 1);
      break;
    }
  }


  old_count = count;
}


void morphable_item::dump_ranges()
{

	std::vector<morphable_item_range*>::iterator range_iter;


  for(range_iter = ranges.begin(); range_iter != ranges.end(); )
  {
    morphable_item_range *range = (*range_iter);

    if(range)
    {
      delete range;
      range_iter = ranges.erase( range_iter );
    }
    else
      ++range_iter;
  }
}

#include "my_assertions.h"
void validate_item(void)
{
	VALIDATE_SIZE(item, 0x128);

	VALIDATE(item, usage_type, 0xF8);
	VALIDATE(item, name, 0xFC);

	VALIDATE(item, count, 0x104);
	VALIDATE(item, default_count, 0x108);

	VALIDATE(item, picked_up, 0x10C);

	VALIDATE(item, pickup_timer, 0x110);
	VALIDATE(item, icon_scale, 0x114);
	VALIDATE(item, interface_orientation, 0x118);

	VALIDATE(item, item_init_rel, 0x11C);

	VALIDATE(item, item_count_rel_two, 0x120);

	VALIDATE(item, max_num, 0x124);

	VALIDATE_VAL(item::USE, 0x27);
	VALIDATE_VAL(item::SCHWING, 0x28);
	VALIDATE_VAL(item::DETONATE, 0x29);
	VALIDATE_VAL(item::ARMED, 0x2A);

	VALIDATE_VTABLE(item, is_an_entity, 1);
	VALIDATE_VTABLE(item, is_a_trigger, 2);
	VALIDATE_VTABLE(item, signal_error, 3);
	VALIDATE_VTABLE(item, raise_signal, 4);
	VALIDATE_VTABLE(item, construct_signal_list, 5);
	VALIDATE_VTABLE(item, get_signal_name, 6);
	VALIDATE_VTABLE(item, po_changed, 7);
	VALIDATE_VTABLE(item, get_ifc_num, 8);
	VALIDATE_VTABLE(item, set_ifc_num, 9);
	VALIDATE_VTABLE(item, get_ifc_vec, 10);
	VALIDATE_VTABLE(item, set_ifc_vec, 11);
	VALIDATE_VTABLE(item, get_ifc_str, 12);
	VALIDATE_VTABLE(item, set_ifc_str, 13);
	VALIDATE_VTABLE(item, frame_advance, 14);
	VALIDATE_VTABLE(item, render, 15);
	VALIDATE_VTABLE(item, read_enx, 16);
	VALIDATE_VTABLE(item, handle_enx_chunk, 17);
	VALIDATE_VTABLE(item, parse_instance, 18);
	VALIDATE_VTABLE(item, initialize, 19);
	VALIDATE_VTABLE(item, make_instance, 20);
	VALIDATE_VTABLE(item, set_flag_recursive, 21);
	VALIDATE_VTABLE(item, set_ext_flag_recursive, 22);
	VALIDATE_VTABLE(item, set_created_entity_default_active_status, 23);
	VALIDATE_VTABLE(item, optimize, 24);
	VALIDATE_VTABLE(item, set_radius, 25);
	VALIDATE_VTABLE(item, get_radius, 26);
	VALIDATE_VTABLE(item, is_time_limited, 27);
	VALIDATE_VTABLE(item, set_time_limited, 28);
	VALIDATE_VTABLE(item, is_active, 29);
	VALIDATE_VTABLE(item, set_active, 30);
	VALIDATE_VTABLE(item, get_forced_active, 31);
	VALIDATE_VTABLE(item, are_collisions_active, 32);
	VALIDATE_VTABLE(item, set_collisions_active, 33);
	VALIDATE_VTABLE(item, are_character_collisions_active, 34);
	VALIDATE_VTABLE(item, are_terrain_collisions_active, 35);
	VALIDATE_VTABLE(item, are_moving_terrain_only_collisions_active, 36);
	VALIDATE_VTABLE(item, set_character_collisions_active, 37);
	VALIDATE_VTABLE(item, set_terrain_collisions_active, 38);
	VALIDATE_VTABLE(item, set_moving_terrain_only_collisions_active, 39);
	VALIDATE_VTABLE(item, is_sticky, 40);
	VALIDATE_VTABLE(item, set_sticky, 41);
	VALIDATE_VTABLE(item, is_stationary, 42);
	VALIDATE_VTABLE(item, set_stationary, 43);
	VALIDATE_VTABLE(item, is_walkable, 44);
	VALIDATE_VTABLE(item, set_walkable, 45);
	VALIDATE_VTABLE(item, is_repulsion, 46);
	VALIDATE_VTABLE(item, set_repulsion, 47);
	VALIDATE_VTABLE(item, is_visible, 48);
	VALIDATE_VTABLE(item, set_visible, 49);
	VALIDATE_VTABLE(item, is_still_visible, 50);

	// @TODO - come back and fix this shit
	/*
	VALIDATE_VTABLE(item, render_trail, 51);
	VALIDATE_VTABLE(item, is_motion_blurred, 52);
	VALIDATE_VTABLE(item, activate_motion_blur, 53);
	VALIDATE_VTABLE(item, render_trail, 54);
	VALIDATE_VTABLE(item, is_motion_trailed, 55);

	VALIDATE_VTABLE(item, activate_motion_trail, 56);
	VALIDATE_VTABLE(item, activate_motion_trail, 57);
	VALIDATE_VTABLE(item, activate_motion_trail, 58);
	*/

	VALIDATE_VTABLE(item, get_externally_controlled, 59);
	VALIDATE_VTABLE(item, get_in_use, 60);
	VALIDATE_VTABLE(item, set_in_use, 61);
	VALIDATE_VTABLE(item, get_member_hidden, 62);
	VALIDATE_VTABLE(item, set_member_hidden, 63);
	VALIDATE_VTABLE(item, get_colgeom, 64);
	VALIDATE_VTABLE(item, set_colgeom_flag_recursive, 65);
	VALIDATE_VTABLE(item, update_colgeom, 66);
	VALIDATE_VTABLE(item, invalidate_colgeom, 67);
	VALIDATE_VTABLE(item, get_updated_colgeom, 68);
	VALIDATE_VTABLE(item, get_damage_capsule, 69);
	VALIDATE_VTABLE(item, get_updated_damage_capsule, 70);
	VALIDATE_VTABLE(item, get_inter_capsule_radius_scale, 71);
	VALIDATE_VTABLE(item, get_velocity, 72);
	VALIDATE_VTABLE(item, get_angular_velocity, 73);
	VALIDATE_VTABLE(item, get_water_dist, 74);
	VALIDATE_VTABLE(item, get_underwater_pct, 75);
	VALIDATE_VTABLE(item, get_water_normal, 76);
	VALIDATE_VTABLE(item, get_underwater_time, 77);
	VALIDATE_VTABLE(item, get_effective_collision_velocity, 78);
	VALIDATE_VTABLE(item, get_effective_collision_mass, 79);
	VALIDATE_VTABLE(item, get_closest_point_along_dir, 80);
	VALIDATE_VTABLE(item, is_picked_up, 81);
	VALIDATE_VTABLE(item, phys_render, 82);
	VALIDATE_VTABLE(item, get_visrep_ending_time, 83);
	VALIDATE_VTABLE(item, get_visual_center, 84);
	VALIDATE_VTABLE(item, get_visual_radius, 85);
	VALIDATE_VTABLE(item, get_vrep, 86);
	VALIDATE_VTABLE(item, get_mesh, 87);
	VALIDATE_VTABLE(item, set_fade_away, 88);
	VALIDATE_VTABLE(item, get_fade_away, 89);
	VALIDATE_VTABLE(item, get_filename, 90);
	VALIDATE_VTABLE(item, get_dirname, 91);
	VALIDATE_VTABLE(item, has_dirname, 92);
	VALIDATE_VTABLE(item, is_a_beam, 93);
	VALIDATE_VTABLE(item, is_a_camera, 94);
	VALIDATE_VTABLE(item, is_a_station_camera, 95);
	VALIDATE_VTABLE(item, is_a_game_camera, 96);
	VALIDATE_VTABLE(item, is_a_marky_camera, 97);
	VALIDATE_VTABLE(item, is_a_mouselook_camera, 98);
	VALIDATE_VTABLE(item, is_a_sniper_camera, 99);
	VALIDATE_VTABLE(item, is_a_conglomerate, 100);
	VALIDATE_VTABLE(item, is_a_turret, 101);
	VALIDATE_VTABLE(item, is_a_ladder, 102);
	VALIDATE_VTABLE(item, is_a_light_source, 103);
	VALIDATE_VTABLE(item, is_a_limb_body, 104);
	VALIDATE_VTABLE(item, is_a_marker, 105);
	VALIDATE_VTABLE(item, is_a_rectangle_marker, 106);
	VALIDATE_VTABLE(item, is_a_cube_marker, 107);
	VALIDATE_VTABLE(item, is_a_crawl_marker, 108);
	VALIDATE_VTABLE(item, is_a_particle_generator, 109);
	VALIDATE_VTABLE(item, is_a_physical_entity, 110);
	VALIDATE_VTABLE(item, is_a_crate, 111);
	VALIDATE_VTABLE(item, is_an_item, 112);
	VALIDATE_VTABLE(item, is_a_switch, 113);
	VALIDATE_VTABLE(item, is_a_visual_item, 114);
	VALIDATE_VTABLE(item, is_a_handheld_item, 115);
	VALIDATE_VTABLE(item, is_a_gun, 116);
	VALIDATE_VTABLE(item, is_a_thrown_item, 117);
	VALIDATE_VTABLE(item, is_a_melee_item, 118);
	VALIDATE_VTABLE(item, is_a_morphable_item, 119);
	VALIDATE_VTABLE(item, is_a_projectile, 120);
	VALIDATE_VTABLE(item, is_a_rigid_body, 121);
	VALIDATE_VTABLE(item, is_a_grenade, 122);
	VALIDATE_VTABLE(item, is_a_rocket, 123);
	VALIDATE_VTABLE(item, is_a_scanner, 124);
	VALIDATE_VTABLE(item, is_a_sky, 125);
	VALIDATE_VTABLE(item, advance_age, 126);
	VALIDATE_VTABLE(item, frame_done, 127);
	VALIDATE_VTABLE(item, add_position_increment, 128);
	VALIDATE_VTABLE(item, terrain_position, 129);
	VALIDATE_VTABLE(item, terrain_radius, 130);
	VALIDATE_VTABLE(item, get_colgeom_root_po, 131);
	VALIDATE_VTABLE(item, get_colgeom_root, 132);
	VALIDATE_VTABLE(item, add_me_to_region, 133);
	VALIDATE_VTABLE(item, remove_me_from_region, 134);
	VALIDATE_VTABLE(item, compute_sector, 135);
	VALIDATE_VTABLE(item, get_region, 136);
	VALIDATE_VTABLE(item, force_region, 137);
	VALIDATE_VTABLE(item, force_current_region, 138);
	VALIDATE_VTABLE(item, unforce_regions, 139);
	VALIDATE_VTABLE(item, force_regions, 140);
	VALIDATE_VTABLE(item, record_motion, 141);
	VALIDATE_VTABLE(item, camera_set_target, 142);
	VALIDATE_VTABLE(item, camera_set_roll, 143);
	VALIDATE_VTABLE(item, camera_set_collide_with_world, 144);
	VALIDATE_VTABLE(item, camera_slide_to, 145);
	VALIDATE_VTABLE(item, camera_slide_to_orbit, 146);
	VALIDATE_VTABLE(item, camera_orbit, 147);
	VALIDATE_VTABLE(item, get_color, 148);
	VALIDATE_VTABLE(item, set_color, 149);
	VALIDATE_VTABLE(item, get_additive_color, 150);
	VALIDATE_VTABLE(item, set_additive_color, 151);
	VALIDATE_VTABLE(item, get_near_range, 152);
	VALIDATE_VTABLE(item, set_near_range, 153);
	VALIDATE_VTABLE(item, get_cutoff_range, 154);
	VALIDATE_VTABLE(item, set_cutoff_range, 155);
	VALIDATE_VTABLE(item, add_light_category, 156);
	VALIDATE_VTABLE(item, remove_light_category, 157);
	VALIDATE_VTABLE(item, get_light_set, 158);
	VALIDATE_VTABLE(item, create_light_set, 159);
	VALIDATE_VTABLE(item, is_frame_delta_valid, 160);
	VALIDATE_VTABLE(item, is_last_frame_delta_valid, 161);
	VALIDATE_VTABLE(item, get_frame_delta, 162);
	VALIDATE_VTABLE(item, set_frame_delta, 163);
	VALIDATE_VTABLE(item, set_frame_delta_trans, 164);
	VALIDATE_VTABLE(item, invalidate_frame_delta, 165);
	VALIDATE_VTABLE(item, get_last_capsule, 166);
	VALIDATE_VTABLE(item, get_last_position, 167);
	VALIDATE_VTABLE(item, render_passes_needed, 168);
	VALIDATE_VTABLE(item, set_recursive_age, 169);
	VALIDATE_VTABLE(item, attach_anim, 170);
	VALIDATE_VTABLE(item, detach_anim, 171);
	VALIDATE_VTABLE(item, acquire, 172);
	VALIDATE_VTABLE(item, release, 173);
	VALIDATE_VTABLE(item, get_hit_points, 174);
	VALIDATE_VTABLE(item, get_full_hit_points, 175);
	VALIDATE_VTABLE(item, add_item, 176);
	VALIDATE_VTABLE(item, use_item, 177);
	VALIDATE_VTABLE(item, compute_bounding_box, 178);
	VALIDATE_VTABLE(item, apply_damage, 179);
	VALIDATE_VTABLE(item, copy_visrep, 180);
	VALIDATE_VTABLE(item, allow_targeting, 181);
	VALIDATE_VTABLE(item, test_combat_target, 182);
	VALIDATE_VTABLE(item, get_detonate_position, 183);
	VALIDATE_VTABLE(item, add_signal_callbacks, 184);
	VALIDATE_VTABLE(item, get_distance_fade_ok, 185);
	VALIDATE_VTABLE(item, suspend, 186);
	VALIDATE_VTABLE(item, unsuspend, 187);
	VALIDATE_VTABLE(item, possibly_active, 188);
	VALIDATE_VTABLE(item, possibly_aging, 189);
	VALIDATE_VTABLE(item, region_update_poss_active, 190);
	VALIDATE_VTABLE(item, region_update_poss_render, 191);
	VALIDATE_VTABLE(item, region_update_poss_collide, 192);
	VALIDATE_VTABLE(item, is_alive, 193);
	VALIDATE_VTABLE(item, is_dying, 194);
	VALIDATE_VTABLE(item, is_alive_or_dying, 195);
	VALIDATE_VTABLE(item, preload, 196);
	VALIDATE_VTABLE(item, ifl_play, 197);
	VALIDATE_VTABLE(item, ifl_lock, 198);
	VALIDATE_VTABLE(item, ifl_pause, 199);
	VALIDATE_VTABLE(item, set_render_color, 200);
	VALIDATE_VTABLE(item, set_render_scale, 201);
	VALIDATE_VTABLE(item, set_render_zbias, 202);
	VALIDATE_VTABLE(item, copy_instance_data, 203);
	VALIDATE_VTABLE(item, get_count, 204);
	VALIDATE_VTABLE(item, inc_count, 205);
	VALIDATE_VTABLE(item, dec_count, 206);
	VALIDATE_VTABLE(item, set_count, 207);
	VALIDATE_VTABLE(item, is_usable, 208);
	VALIDATE_VTABLE(item, get_number, 209);
	VALIDATE_VTABLE(item, give_to_entity, 210);
	VALIDATE_VTABLE(item, apply_effects, 211);
	VALIDATE_VTABLE(item, is_ammo, 212);
	VALIDATE_VTABLE(item, is_health, 213);
	VALIDATE_VTABLE(item, is_armor, 214);
	VALIDATE_VTABLE(item, is_brain_weapon, 215);
	VALIDATE_VTABLE(item, holster, 216);
	VALIDATE_VTABLE(item, draw, 217);
	VALIDATE_VTABLE(item, hide, 218);
	VALIDATE_VTABLE(item, show, 219);
}

void validate_visual_item(void)
{
	VALIDATE_SIZE(visual_item, 0xF8);

	VALIDATE(visual_item, owner, 0xF4);
}

void validate_morphable_item(void)
{
	VALIDATE_SIZE(morphable_item, 0x138);

	VALIDATE(morphable_item, ranges, 0x128);
	VALIDATE(morphable_item, old_count, 0x134);
}

void validate_morphable_item_range(void)
{
	VALIDATE_SIZE(morphable_item_range, 0x10);

	VALIDATE(morphable_item_range, low, 0x0);
	VALIDATE(morphable_item_range, high, 0x4);
	VALIDATE(morphable_item_range, vis_rep, 0x8);
}

#include "my_patch.h"

void patch_morphable_item(void)
{
	PATCH_PUSH_RET_POLY(0x00600250, morphable_item::frame_advance, "?frame_advance@morphable_item@@UAEXM@Z");
	PATCH_PUSH_RET     (0x00600320, morphable_item::set_range_visrep);
}

void patch_visual_item(void)
{
	PATCH_PUSH_RET_POLY(0x005FF280, visual_item::render, "?render@visual_item@@UAEXMIM@Z");
	PATCH_PUSH_RET_POLY(0x005FF220, visual_item::get_light_set, "?get_light_set@visual_item@@UAEPAVlight_manager@@XZ");
	PATCH_PUSH_RET_POLY(0x005FF260, visual_item::get_light_set, "?render_passes_needed@visual_item@@UBEIXZ");
}

void patch_item(void)
{
	PATCH_PUSH_RET_POLY(0x005FD8E0, item::set_count, "?set_count@item@@UAEXH@Z");
	PATCH_PUSH_RET_POLY(0x005FDBD0, item::render, "?render@item@@UAEXMIM@Z");

	PATCH_PUSH_RET_POLY(0x005FE8C0, item::is_picked_up, "?is_picked_up@item@@UAE_NXZ");
	PATCH_PUSH_RET_POLY(0x005FEBE0, item::apply_effects, "?apply_effects@item@@UAEXPAVentity@@@Z");

	PATCH_PUSH_RET     (0x005FEDB0, item::register_signals);

	PATCH_PUSH_RET_POLY(0x005FCA50, item::holster, "?holster@item@@UAEX_N@Z");
	PATCH_PUSH_RET_POLY(0x005FCA70, item::draw, "?draw@item@@UAEX_N@Z");
	PATCH_PUSH_RET_POLY(0x005FCA90, item::hide, "?hide@item@@UAEXXZ");
	PATCH_PUSH_RET_POLY(0x005FCAB0, item::show, "?show@item@@UAEXXZ");

	PATCH_PUSH_RET_POLY(0x005FCA30, item::is_brain_weapon, "?is_brain_weapon@item@@UBE_NXZ");
	PATCH_PUSH_RET_POLY(0x005E7BD0, item::is_armor, "?is_armor@item@@UBE_NXZ");
	PATCH_PUSH_RET_POLY(0x005E7BB0, item::is_health, "?is_health@item@@UBE_NXZ");
	PATCH_PUSH_RET_POLY(0x005E7B90, item::is_ammo, "?is_ammo@item@@UBE_NXZ");

	PATCH_PUSH_RET_POLY(0x005E7B70, item::get_number, "?get_number@item@@UBEHXZ");
	PATCH_PUSH_RET_POLY(0x005E7B40, item::is_usable, "?is_usable@item@@UBE_NXZ");

	PATCH_PUSH_RET_POLY(0x005E7B20, item::dec_count, "?dec_count@item@@UAEXXZ");
	PATCH_PUSH_RET_POLY(0x005E7B00, item::inc_count, "?inc_count@item@@UAEXXZ");
}
