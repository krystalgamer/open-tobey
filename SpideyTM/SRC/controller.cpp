////////////////////////////////////////////////////////////////////////////////
/*
  controller.cpp


  home of the highest level of player or AI control of game systems.

*/
////////////////////////////////////////////////////////////////////////////////
#include "global.h"

#include "controller.h"
//!#include "character.h"
//!#include "charhead.h"
#include "mcs.h"
#include "inputmgr.h"
#include "commands.h"
#include "osdevopts.h"
#include "menusys.h"
//#include "FrontEndManager.h"

////////////////////////////////////////////////////////////////////////////////
// character_controller
////////////////////////////////////////////////////////////////////////////////



entity_controller::entity_controller( entity* ent )
{
  owner = ent;
  state = NONE;
}

entity_controller::~entity_controller()

{
}

player_controller * entity_controller::as_player_controller()
{
  stringx composite = owner->get_id().get_val() + " does not have a player controller.";

  error(composite.c_str());
  return NULL;

}


entity_controller* entity_controller::make_instance( entity *ent ) const
{
  entity_controller* c = NEW entity_controller( ent );
  c->copy_instance_data( *this );
  return c;
}

void entity_controller::copy_instance_data( const entity_controller& b )
{
  controller::copy_instance_data(b);


  state = b.state;
}



/*!
character_controller::character_controller( character* chr,
                                            character_head_fcs * _head_fcs )
  : entity_controller(chr)
{
  head_fcs = _head_fcs;
  recording = false;
}


character_controller::~character_controller()

{
}



// Control setting functions, which also can record info if move_editing.
// Just like the good old days!

void character_controller::set_target_speed_pct(rational_t v)
{
}



void character_controller::set_target_h_speed_pct(rational_t v)
{
}


void character_controller::set_d_theta_pct(rational_t v)
{
}



void character_controller::set_burst(vector3d dir, time_value_t time)
{
}



void character_controller::set_block_type(int b)
{
}



void character_controller::set_jump_flag(bool torf)
{
}


void character_controller::set_crouch_flag(bool torf)
{
}


void character_controller::set_front_crouch_flag(bool torf)
{
}


void character_controller::set_rear_crouch_flag(bool torf)
{
}


void character_controller::set_flip(bool torf)
{
}



void character_controller::set_neck_target_theta(rational_t t)
{
  if (head_fcs)
    head_fcs->controller_set_neck_target_theta(t);
}


void character_controller::set_neck_target_psi(rational_t t)
{
  if (head_fcs)
    head_fcs->controller_set_neck_target_psi(t);
}


void character_controller::set_neck_target_extend(rational_t t)
{
  if (head_fcs)
    head_fcs->controller_set_neck_target_extend(t);
}



void character_controller::set_head_target_psi(rational_t t)

{
  if (head_fcs)
    head_fcs->controller_set_head_target_psi(t);
}


void character_controller::set_head_target_phi(rational_t t)

{
  if (head_fcs)
    head_fcs->controller_set_head_target_phi(t);
}


void character_controller::set_jaw_target_psi(rational_t t)
{
  if (head_fcs)
    head_fcs->controller_set_jaw_target_psi(t);
}

!*/


////////////////////////////////////////////////////////////////////////////////
// mouselook_controller
////////////////////////////////////////////////////////////////////////////////
mouselook_controller::mouselook_controller(dolly_and_strafe_mcs* _move_cs,
                                           theta_and_psi_mcs* _angle_mcs)
  : controller(),
    move_cs(_move_cs),
    angle_mcs(_angle_mcs)
{
}

// <<<< for consistency, time_inc should be computed in the mcs
void mouselook_controller::frame_advance(time_value_t time_inc)
{
	// @TODO
	PANIC;
}


////////////////////////////////////////////////////////////////////////////////
// joypad_usercam_controller
////////////////////////////////////////////////////////////////////////////////

joypad_usercam_controller::joypad_usercam_controller(dolly_and_strafe_mcs* _move_cs,
                                           theta_and_psi_mcs* _angle_mcs)
  : controller(),
    move_cs(_move_cs),
    angle_mcs(_angle_mcs)
{
}

extern game* g_game_ptr;

extern MenuSystem *menus;

	// producer screen shot hack
extern bool superduperpausehack;


// <<<< for consistency, time_inc should be computed in the mcs
void joypad_usercam_controller::frame_advance(time_value_t time_inc)

{
	PANIC;
}


/*
#if defined(TARGET_PC)

  ////////////////////////////////////////////////////////////////////////////////
  // edit_controller
  ////////////////////////////////////////////////////////////////////////////////
  edit_controller::edit_controller(dolly_and_strafe_mcs* _move_cs,
                                             theta_and_psi_mcs* _angle_mcs) :
      move_cs(_move_cs),
      angle_mcs(_angle_mcs), controller()
    {
    }

  // <<<< for consistency, time_inc should be computed in the mcs

  void edit_controller::frame_advance(time_value_t time_inc)

  {
    if( input_mgr::inst()->get_control_state( JOYSTICK_DEVICE, EDITCAM_PITCH_UP ) == AXIS_MAX )
    {
      if(os_developer_options::inst()->is_flagged(os_developer_options::FLAG_INVERT_MOUSE_Y))

        angle_mcs->set_tilt_for_next_frame( 2.0f / 30.0f );
      else
        angle_mcs->set_tilt_for_next_frame( -2.0f / 30.0f );
    }
    else if( input_mgr::inst()->get_control_state( JOYSTICK_DEVICE, EDITCAM_PITCH_DOWN ) == AXIS_MAX )
    {
      if(os_developer_options::inst()->is_flagged(os_developer_options::FLAG_INVERT_MOUSE_Y))
        angle_mcs->set_tilt_for_next_frame( -2.0f / 30.0f );
      else
        angle_mcs->set_tilt_for_next_frame( 2.0f / 30.0f );
    }


    if( input_mgr::inst()->get_control_state( JOYSTICK_DEVICE, EDITCAM_YAW_LEFT ) == AXIS_MAX )

      angle_mcs->set_pan_for_next_frame( -2.0f / -30.0f );
    else if( input_mgr::inst()->get_control_state( JOYSTICK_DEVICE, EDITCAM_YAW_RIGHT ) == AXIS_MAX )
      angle_mcs->set_pan_for_next_frame( 2.0f / -30.0f );


    rational_t speed = 10;
    if( input_mgr::inst()->get_control_state( JOYSTICK_DEVICE, EDITCAM_FAST ) == AXIS_MAX )
      speed = 50;

    if( input_mgr::inst()->get_control_state( JOYSTICK_DEVICE, EDITCAM_SLOW ) == AXIS_MAX )
      speed = 2;

    if( input_mgr::inst()->get_control_state( JOYSTICK_DEVICE, EDITCAM_FORWARD ) == AXIS_MAX )

      move_cs->set_dolly_for_next_frame( speed*time_inc );

    if( input_mgr::inst()->get_control_state( JOYSTICK_DEVICE, EDITCAM_BACKWARD ) == AXIS_MAX )
      move_cs->set_dolly_for_next_frame( -speed*time_inc );

    if( input_mgr::inst()->get_control_state( JOYSTICK_DEVICE, EDITCAM_UP ) == AXIS_MAX )
      move_cs->set_lift_for_next_frame( speed*time_inc );

    if( input_mgr::inst()->get_control_state( JOYSTICK_DEVICE, EDITCAM_DOWN ) == AXIS_MAX )
      move_cs->set_lift_for_next_frame( -speed*time_inc );

    if( input_mgr::inst()->get_control_state( JOYSTICK_DEVICE, EDITCAM_STRAFE_RIGHT ) == AXIS_MAX )

      move_cs->set_strafe_for_next_frame( speed*time_inc );


    if( input_mgr::inst()->get_control_state( JOYSTICK_DEVICE, EDITCAM_STRAFE_LEFT ) == AXIS_MAX )
      move_cs->set_strafe_for_next_frame( -speed*time_inc );

    }
#endif


*/
