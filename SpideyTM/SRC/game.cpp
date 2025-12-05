////////////////////////////////////////////////////////////////////////////////

// game.cpp
// Copyright (c) 1999-2000 Treyarch Invention LLC.  ALL RIGHTS RESERVED.

////////////////////////////////////////////////////////////////////////////////
#include "global.h"


#include "anim_maker.h"
#include "billboard.h"
#include "colmesh.h"
#include "commands.h"
#include "debug_render.h"
//#include "DemoMode.h"
#include "element.h"
#include "entity_maker.h"
#include "file_finder.h"
//#include "fogmgr.h"
#include "game.h"
#include "game_info.h"
#include "game_process.h"
//#include "gamefile.h"
//#include "GameData.h"
//#include "osGameSaver.h"
#include "geomgr.h"
#include "hwaudio.h"
#include "hwmovieplayer.h"
//#include "interface.h"
#include "localize.h"
#include "lensflare.h"

/*
#include "joystick.h"
#include "keyboard.h"
#include "mouse.h"
#include "MusicMan.h"
#include "msgboard.h"
#include "ksnsl.h"
//#include "WipeTransition.h"
#include "BoardManager.h"
*/
#include "osdevopts.h"

#include "random.h"
#include "pmesh.h"
//#include "beach.h"
#include "profiler.h"
//#include "rumbleManager.h"
#ifdef GCCULL
#include "sound_group.h"
#endif
// BIGCULL #include "spiderman_controller.h"
#include "mustash.h"
#include "stashes.h"
#include "trigger.h"
//#include "tricks.h"
#include "terrain.h"

//#include "waveSound.h"
#include "wds.h"
// @Patch
#include "widget.h"
//#include "widget_script.h"
//#include "VOEngine.h"
#include "script_lib_controller.h"
#include "script_lib_mfg.h"
//#include "SFXEngine.h"
#include "file_manager.h"
#include "global.h"

/*
#include "FrontEndManager.h"
#include "beachdata.h"		// For BEACHDATA_Load
#include "careerdata.h"		// For CAREERDATA_Load
#include "surferdata.h"		// For SURFERDATA_Load
#include "boarddata.h"
#include "camera_tool.h"
#include "ksfx.h"			// For ks_fx_destroy_all
*/
//#include "water.h"	// For WATER_Init
/*	Attempts at screen spray (dc 01/04/02)
#include "refract.h"	// For REFRACT_Init, REFRACT_Update
#include "displace.h"	// For DISPLACE_Init, DISPLACE_Draw

*/
//#include "wavetex.h"

/*
#include "kshooks.h"	// For KSWhatever calls
#include "blur.h"	// For BLUR_Init, BLUR_Draw
#include "wavedata.h"	// For WAVEDATA_Load()
#include "sounddata.h" // For SOUNDDATA_Load()
#include "soundscript.h"
*/


#ifdef TARGET_PC
//#include "winapp.h"
#endif

/*
#include "kellyslater_controller.h"
#include "ksdbmenu.h"
#include "ks_camera.h"
#include "sounddata.h"
*/
#ifdef TARGET_PS2
#include "hwosps2/ps2_input.h"
//#include "libscf.h"
extern ps2_joypad_device *g_pad;

#include "ngl_ps2_internal.h"	// For NGL_VRAM_LOCKED_START only!!! (dc 12/17/01)
#endif


#if _CONSOLE_ENABLE
#include "console.h"

#endif

#if defined(TARGET_XBOX)
#include "region.h"
#include "terrain.h"

// JIV FIXME
//#define nglDestroyTexture(t) nglReleaseTexture(t)
#define NGL_VRAM_LOCKED_START 0 // let's be portable yay
#endif /* TARGET_XBOX JIV DEBUG */

extern profiler_counter profcounter_frame_delta;
extern profiler_counter profcounter_frame_avg_delta;
extern profiler_counter profcounter_frame_min_delta;
extern profiler_counter profcounter_frame_max_delta;

extern int global_frame_counter;  // CTT 04/09/00: this is a stupid kluge to save time at the last minute before E3

int g_wipeout_blur = 1;
bool g_screenshot = false;
const char *g_screenshot_filename = NULL;


Random *g_random_ptr;
Random *g_random_r_ptr;

//--------------------------------------------------------------
void clear_zbuffer()
{
	PANIC;
}



lensflare *sunFlare = NULL;
////////////////////////////////////////////////////////////////////////////////
//  globals
////////////////////////////////////////////////////////////////////////////////

cheat_info_t g_cheats;
debug_info_t g_debug;


game_info::game_info()
{
	reset();
}

bool game_info::set_str(const pstring &att, const stringx &val)
{
#define MAC(type, var, def, str)  { static pstring pstring_set(##str##); if(att == pstring_set) { ##var## = val; return(true); } }
#define GAME_INFO_STRS
#include "game_info_vars.h"
#undef GAME_INFO_STRS
#undef MAC

	return(false);
}

bool game_info::get_str(const pstring &att, stringx &val) const
{
#define MAC(type, var, def, str)  { static pstring pstring_get(##str##); if(att == pstring_get) { val = ##var##; return(true); } }
#define GAME_INFO_STRS
#include "game_info_vars.h"
#undef GAME_INFO_STRS
#undef MAC

	return(false);
}

bool game_info::set_num(const pstring &att, rational_t val)
{

#define MAC(type, var, def, str)  { static pstring pstring_set(##str##); if(att == pstring_set) { ##var## = (##type##)##val##; return(true); } }
#define GAME_INFO_NUMS
#include "game_info_vars.h"
#undef GAME_INFO_NUMS
#undef MAC

	return(false);
}

bool game_info::get_num(const pstring &att, rational_t &val) const
{
#define MAC(type, var, def, str)  { static pstring pstring_get(##str##); if(att == pstring_get) { val = (##type##)##var##; return(true); } }
#define GAME_INFO_NUMS
#include "game_info_vars.h"
#undef GAME_INFO_NUMS
#undef MAC

	return(false);
}

void game_info::reset()
{
#define MAC(type, var, def, str)    var = (##type##)##def##;

#define GAME_INFO_NUMS

#define GAME_INFO_STRS

#include "game_info_vars.h"
#undef GAME_INFO_NUMS
#undef GAME_INFO_STRS
#undef MAC
}


// <<<< DL -- get_hero_name() looks retarded but I had to do it because no easy was to make an array of hero_name_0, hero_name_1
// JIV Stroustrup 15.5 contains info on pointer to member function syntax
const stringx &game_info::get_hero_name(int hero_num)

{
	PANIC;
	return *(NEW stringx(""));
}



vector3d global_ZEROVEC = vector3d(0,0,0); // for being returned by reference
extern game *g_game_ptr;

#ifdef GCCULL
list<sound_stream *> g_stream_play_list;
#endif
file_finder *g_file_finder = NULL;
entity_maker *g_entity_maker = NULL;
anim_maker *g_anim_maker = NULL;


const chunk_flavor CHUNK_WEDGE      ("wedge");
const chunk_flavor CHUNK_NUM_WEDGES ("nowedges");
const chunk_flavor CHUNK_NORMAL     ("normal");

const chunk_flavor CHUNK_NO_WARNINGS  ("nowarn");

// for testing out multiple level loading
char g_scene_name[256];
char g_movie_name[256];
char g_console_command[256];   // for ps2 console (temp)



//For bluring stuff
//static float wipeBlurAlpha = .3;
//static float wipeDeltaY = 0;
//static float wipeDeltaX = 0;
//static float wipeColor[3] = {1, 1, 1};

//int wipeTransition = 0;
//static float wipeX = 512;
//static float wipeY = 512;
//static int wipeTex = 0;

//static float wipeYZero = 0;
//static float wipeXZero = 0;
#ifdef TARGET_PC
static int podump_idx = 0;
#define CAM_IN_MOTION_DELAY 2.0f
#endif


////////////////////////////////////////////////////////////////////////////////
//  game
////////////////////////////////////////////////////////////////////////////////

//bool KSReadFile( char* FileName, nglFileBuf* File, u_int Align );
//void KSReleaseFile( nglFileBuf* File );
//void* KSMemAllocNSL( u_int Size, u_int Align );
//void KSMemFree( void* Ptr );


const int game::SNAPSHOT_WIDTH = 128;

const int game::SNAPSHOT_HEIGHT = 128;

void nslNoPrint( const char * text ) {};
void nslNoErrors(const char * text ) {};

#ifdef TARGET_GC
// Forgive me, Father.
// Defined in gc_main.cpp to indicate necessity

// of displaying PAL60 query screen.
extern bool ksGCQueryPAL60;
#endif

/*** Constructor ***/
game::game()
{
	PANIC;
}



/*** Destructor ***/
game::~game()
{
	PANIC;
}



/*** construct_game_widgets ***/
void game::construct_game_widgets()
{
	PANIC;
}


/*** destroy_game_widgets ***/
void game::destroy_game_widgets()
{
	PANIC;
}

#if defined(TARGET_PS2)
#include <libcdvd.h>
extern bool bad_mc_stack_error;
#endif


/*** frame_advance ***/
void game::frame_advance()
{
	PANIC;
}


extern profiler_timer proftimer_advance;
//extern profiler_timer proftimer_events;

extern profiler_timer proftimer_special_fx;
//extern profiler_timer proftimer_dynamic_ents;

extern profiler_counter profcounter_total_blocks;
extern profiler_counter profcounter_alloced_mem;
extern profiler_counter profcounter_texture_mem;
extern profiler_counter profcounter_audio_mem;

extern profiler_counter profcounter_tri_rend;



bool g_controller_inserted = true;
bool g_controller_never_inserted = false;




extern profiler_timer proftimer_render_interface;
extern profiler_timer proftimer_debug_info;
extern profiler_timer proftimer_profiler;
extern profiler_timer proftimer_draw_prof_info;


/*** render ***/

/*
#include "hwosps2/ps2_m2vplayer.h"
static m2v_player_t* player;
// call m2v_destroy_player (player); when done
*/
/*
static char clock_char = '|';
static int timer = 0;
*/

void game::render_map_screen()
{
	PANIC;
}


void game::render_legal_screen()
{
	PANIC;
}


#ifdef TARGET_GC

extern void system_idle( void );

void game::render_pal60_screen()
{
	PANIC;
}
#endif


void game::render_title_screen()
{
	PANIC;
}

void game::render_igo()

{
	PANIC;
}

void game::render_mem_free_screen()
{
	PANIC;
}

void game::do_profiler_stuff()
{

	// Calculate and display frame rate
#ifdef PROFILING_ON
	STOP_PROF_TIMER( proftimer_cpu_net );
	STOP_PROF_TIMER( proftimer_render_cpu );
	proftimer_profiler.start();
	proftimer_draw_prof_info.start();


	nglListBeginScene();

	nglSetClearFlags(NGLCLEAR_Z | NGLCLEAR_STENCIL);
	profiler::inst()->render();
	nglListEndScene();


	proftimer_draw_prof_info.stop();
	proftimer_profiler.stop();
	START_PROF_TIMER( proftimer_render_cpu );
	START_PROF_TIMER( proftimer_cpu_net );
#endif // PROFILING_ON

}


void game::render_fe()
{
	PANIC;
}



void game::do_autobuild_stuff()
{
	PANIC;
}




void game::render_level_onescreen()
{
	PANIC;
}

void game::render_level_splitscreen()

{
	PANIC;
}
static color32 current, target, old;

void game::render_trippy_cheat()
{
	PANIC;
}


void game::render_shadows()
{
	PANIC;
}





void game::render_level()

{
	PANIC;
}


void game::render()
{
	PANIC;
}

extern void nglDownloadTexture(nglTexture *Dest, nglTexture *Source);
extern void REFRACT_WriteRenderTargetToFile(nglTexture *Texture);

void game::render_snapshot(void)
{
	PANIC;
}

/*
void game::take_snapshot(nglTexture * dest)
{
	PANIC;
}
*/

void game::render_menu_screen()
{
	PANIC;
}


//#include "beachdata.h"


void game::set_level( int lid )
{
	PANIC;
}

void game::set_beach( int bid )

{
	PANIC;
}

void game::set_level(stringx lname, bool career)
{
	PANIC;
}


void game::set_movie( stringx mname )
{
  movie_name = mname;
}


bool game::is_competition_level(void)
{
	PANIC;
	return true;
}

stringx game::get_level_name( void )
{
	PANIC;
	return stringx("");
}

stringx game::get_beach_name( void )
{

	PANIC;
	return stringx("");
}

stringx game::get_beach_stash_name( void )
{
	PANIC;
	return stringx("");
}




//void nglLoadDebugMeshes();
extern int g_debug_num_loads;

/*** load_level_stash ***/
void game::load_level_stash()
{
	PANIC;
}


#if 0

#define load_log(s) { low_level_console_print(s); low_level_console_flush(); }
#else
#define load_log(s) ((void)0)
#endif

int test_big_wave = 0;

/*** load_this_level ***/
void game::load_this_level()
{
	PANIC;
}


/*** unload_current_level ***/
void game::unload_current_level()
{
	PANIC;
}


float game_speed=1.0f;

void game::frame_advance_level()
{
	PANIC;
}




//--------------------------------------------------------------
extern profiler_counter profcounter_lod_tri_estimate;


typedef enum
{
	CHEAT_1,
		CHEAT_2,
		CHEAT_COUNT
} KS_CHEATS;


static control_id_t cheats[CHEAT_COUNT][5] =
{
	{ PSX_X, PSX_CIRCLE, PSX_TRIANGLE, PSX_SQUARE, INVALID_CONTROL_ID },
	{ PSX_SQUARE, PSX_TRIANGLE, PSX_CIRCLE, PSX_X, INVALID_CONTROL_ID },
};

static control_id_t *current_cheat = NULL;
static float cheat_timer;
static bool cheat_release;

static bool get_one_button_down (control_id_t& btn)
{
	input_mgr* inputmgr = input_mgr::inst();
	btn = INVALID_CONTROL_ID;

	for (int i = PSX_X; i <= PSX_SELECT; i++)
	{
		if (inputmgr->get_control_state(ANY_LOCAL_JOYSTICK, i) != 0.0f)
		{
			if (btn != INVALID_CONTROL_ID)
				return false;

			else
				btn = i;
		}

	}

	return true;
}

#define CHEAT_SPEED 1


void game::do_the_cheat_codes(float time_inc)

{
	PANIC;
}

#ifdef GCCULL

sg_entry* game::get_sound_group_entry( const pstring& name )
{
	map<pstring,sound_group*>::iterator it = sound_group_map.find( name );
	if ( it == sound_group_map.end() )
		return 0;
	else
		return ( *it ).second->get_next();
}
#endif

//--------------------------------------------------------------
void game::pause()

{
	PANIC;
}

//--------------------------------------------------------------
void game::unpause()
{
	PANIC;
}


//--------------------------------------------------------------



bool game::is_paused() const
{
	return flag.game_paused; // old school   get_cur_state() == GSTATE_PAUSED;
}

void game::load_new_level( const stringx &new_level_name )
{
	strcpy(g_scene_name, new_level_name.c_str());
	flag.load_new_level = true;
}


// this just isn't right, but what can I do while current_view_camera's in game??

void game::enable_marky_cam( bool enable, bool sync, rational_t priority )
{
	PANIC;
}

//--------------------------------------------------------------


void game::render_interface()
{
	PANIC;
}


//---------------------------------------------------------------
bool game::is_letterbox_active() const
{

	return false; //P (p_letterbox && p_letterbox->active);
}


//--------------------------------------------------------------
void game::show_debug_info()
{
	PANIC;
}


//--------------------------------------------------------------
// MAY BE CALLED MULTIPLE TIMES NOW
//--------------------------------------------------------------

void game::setup_input_registrations()
{
	PANIC;
}

void game::setup_inputs()
{
	// @TODO
	typedef void (__fastcall *func_ptr)(game*);
	func_ptr func = (func_ptr)0x005C9800;

	func(this);
	return;
	PANIC;
}


//-----------------------------------------------------------------
// @Ok
// @Matching
bool game::was_start_pressed() const
{
	// @Patch - removed
	/*
#ifndef TARGET_GC
	return input_mgr::inst()->get_control_state( JOYSTICK_DEVICE, PFE_START ) == AXIS_MAX;
#else
	bool start= input_mgr::inst()->get_control_state( JOYSTICK_DEVICE, GAME_PAUSE ) == AXIS_MAX ;

  bool z= input_mgr::inst()->get_control_state( JOYSTICK_DEVICE, PFE_Z ) == AXIS_MAX ;
  return start && !z;
#endif
*/
	return false;
}

bool game::was_select_pressed() const
{
	PANIC;
	return true;
}

// @Ok
// @Matching
bool game::was_A_pressed() const
{
	// @Patch - removed
	//return input_mgr::inst()->get_control_state( JOYSTICK_DEVICE, PFE_A ) == AXIS_MAX;
	return false;
}

// @Ok
// @Matching
bool game::was_B_pressed() const
{
	// @Patch - removed
	//return input_mgr::inst()->get_control_state( JOYSTICK_DEVICE, PFE_B ) == AXIS_MAX;
	return false;
}
//-----------------------------------------------------------------

void game::freeze_hero( bool freeze )
{
	PANIC;
}

void game::turn_user_cam_on(bool user_cam_status)
{
	PANIC;
}

void game::set_player_camera(int n, camera *cam)
{
	PANIC;
}


void game::set_current_camera(camera *cam)

{
	PANIC;
}

//	move_snapshot_cam()
// Helper function - moves the current camera to a good position for taking a snapshot.
// The old po is saved in snapshotPrevCamPo so that it can be reset after snapshot is taken.
void game::move_snapshot_cam(void)
{
/*
po			newCamPo;

entity *	targetEntity;
vector3d	targ;
vector3d	cam;
vector3d	orient;

  // Save current camera po.
  snapshotPrevCamPo = player_cam[active_player]->get_rel_po();


	// Calculate camera target.

	targetEntity = ((game_camera *)player_cam[active_player])->get_target_entity();
	targ = ((conglomerate *) targetEntity)->get_member("BIP01 SPINE2")->get_abs_position();
	orient = the_world->get_ks_controller(active_player)->get_board_controller().GetForwardDir();
	orient.normalize();

	orient *= 0.4f;
	targ += orient;


	  // Calculate camera directional vector.
	  cam = ((game_camera *)player_cam[active_player])->get_rel_position();
	  orient = cam - targ;

		// Zoom camera.
		orient.normalize();
		if (the_world->get_ks_controller(active_player)->get_super_state() == SUPER_STATE_WIPEOUT)
		orient *= 2.0f;
		else
		orient *= 1.3f;


		  // Translate camera.
		  newCamPo.set_position(targ + orient);
		  newCamPo.set_facing(targ);

			// Use new camera po.
			player_cam[active_player]->set_rel_po(newCamPo);
	*/
}

//--------------------------------------------------------------

void game::end_level( void )
{
	PANIC;
}

//	end_run()

// Call this function when the time runs out on the level.
// In singleplayer modes, the game simply pauses.
// In alternating multiplayer modes, the level restarts with the next player.
void game::end_run(void)
{
	PANIC;
}

//	retry_mode()
// Plays the current game mode again.
void game::retry_mode(const bool fromMap)
{
	PANIC;
}

//	retry_level()
// Plays the wave again, retains game mode state info.
void game::retry_level(const bool fromMap)
{
	PANIC;
}

nslSoundId feMusic=NSL_INVALID_ID;

void game::advance_state_front_end( time_value_t time_inc )
{
	PANIC;
}


// do standard frame advances that almost everyone wants to

void game::frame_advance_game_overlays( time_value_t time_inc )
{
	PANIC;
}


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

#ifdef GCCULL
void game::update_music_applied_volume( time_value_t time_inc, bool force_update )
{
	rational_t old_volume = music_applied_volume;


	rational_t user_volume = old_volume; //P optionsfile->get_option(GAME_OPT_MUSIC_VOLUME) / 10.0f;

	assert( user_volume >= 0.0f && user_volume <= 1.0f );
	assert( music_fade_to_volume >= 0.0f && music_fade_to_volume <= 1.0f );

	if ( force_update )
	{
		music_applied_volume = music_fade_to_volume = user_volume;
		music_fade_time_left = 0.0f;
	}
	else if ( music_fade_time_left > 0.0f )
	{
		if ( music_fade_time_left > time_inc )
		{
			music_applied_volume += ( music_fade_to_volume - music_applied_volume ) * ( time_inc / music_fade_time_left );
			music_fade_time_left -= time_inc;
		}
		else
		{
			music_fade_time_left = 0.0f;
			music_applied_volume = music_fade_to_volume;
		}


		if ( music_applied_volume > user_volume )
		{
			music_applied_volume = user_volume;
		}
	}

	if ( old_volume != music_applied_volume ||
		(old_volume == 0.0f && music_applied_volume == 0.0f) ||
		force_update )
	{
		if ( music_stream )
		{
			music_stream->set_volume( music_applied_volume );
		}
	}
}

#endif

//---------------------------------------------------------------


#ifdef GCCULL
void game::clear_ambients()
{
	ambient_stream.sp_region_node = 0;

	next_ambient_stream.sp_region_node = 0;

	if( ambient_stream.sp_stream )

	{
		ambient_stream.sp_stream->release();
		ambient_stream.sp_stream = 0;

	}
	if( next_ambient_stream.sp_stream )
	{

		next_ambient_stream.sp_stream->release();
		next_ambient_stream.sp_stream = 0;
	}
}

const time_value_t base_ambient_delay = 0.0f;


// NOT TIME-BASED, RATHER HERO-POSITION-BASED
void game::process_ambients( time_value_t delta_t )
{
	entity* hero = g_world_ptr->get_hero_ptr(active_player);	// game::process_ambients(): wtf is this? (multiplayer fixme?)
	if( hero )
	{
		region_graph::node *rnp = hero->get_region();
		if( rnp )
		{
			if( rnp != ambient_stream.sp_region_node )
			{
				// SWAP MAIN REGION WITH SECONDARY
				_ambient_stream ts = ambient_stream;
				ambient_stream = next_ambient_stream;
				next_ambient_stream = ts;
			}
			if( rnp != ambient_stream.sp_region_node )
			{
				stringx refstr = rnp->get_data()->get_region_ambient_sound_name();
				if( (!refstr.length()) || (ambient_stream.sp_stream && refstr.length() && (ambient_stream.sp_stream->get_file_name() != refstr)) )
				{

					// THROW AWAY OLD STREAM
					if( ambient_stream.sp_stream )
						ambient_stream.sp_stream->release();
					ambient_stream.sp_stream = 0;
					ambient_stream.sp_region_node = 0;
				}
				else if( ambient_stream.sp_stream && refstr.length() )

				{
					ambient_stream.sp_region_node = rnp;

				}
				if( !ambient_stream.sp_region_node )
				{
					ambient_stream.sp_region_node = rnp;
					if( refstr.length() )
					{
						//            ambient_stream.sp_stream = sound_device::inst()->create_stream( refstr );

						//            ambient_stream.s_base_volume = ambient_stream.sp_region_node->get_data()->get_region_ambient_sound_volume();
						//            ambient_stream.s_cur_volume = 1.0f;
						//            ambient_stream.sp_stream->request_queue( false, true );
					}
				}

			}


			// NOW SCAN LIST OF CONNECTED REGIONS
			vector3d hv = hero->get_abs_position();
			region_graph::node::iterator rit;
			for( rit = rnp->begin(); rit != rnp->end(); ++rit )
			{
				if( (*rit).get_data()->is_active() )
				{
					vector3d pv = (*rit).get_data()->get_effective_center();

					rational_t d = (pv-hv).length();
					rational_t r2 = (*rit).get_data()->get_effective_radius() *1.0f;
					if( r2 == 0.0f ) r2 = 0.00001f;

					// THIS ASSUMES THAT ONLY ONE PORTAL-SPHERE CAN BE INTERSECTED AT A SINGLE TIME
					if( d <= r2 )
					{
						if( !next_ambient_stream.sp_stream )
						{
							next_ambient_stream.sp_region_node = (*rit).get_data()->get_front();

							if( next_ambient_stream.sp_region_node == ambient_stream.sp_region_node )
								next_ambient_stream.sp_region_node = (*rit).get_data()->get_back();


							stringx refstr = next_ambient_stream.sp_region_node->get_data()->get_region_ambient_sound_name();
							if( refstr.length() )

							{
								//                next_ambient_stream.sp_stream = sound_device::inst()->create_stream( refstr );
								//                next_ambient_stream.s_base_volume = next_ambient_stream.sp_region_node->get_data()->get_region_ambient_sound_volume();
								//                next_ambient_stream.sp_stream->request_queue( false, true );
							}
						}

						ambient_stream.s_cur_volume = 0.5f + d/(r2*2.0f);
						if( ambient_stream.s_cur_volume > 1.0f )

							ambient_stream.s_cur_volume = 1.0f;
						if( ambient_stream.s_cur_volume < 0.0f )
							ambient_stream.s_cur_volume = 0.0f;
						next_ambient_stream.s_cur_volume = 0.5f - d/(r2*2.0f);
						if( next_ambient_stream.s_cur_volume > 1.0f )
							next_ambient_stream.s_cur_volume = 1.0f;

						if( next_ambient_stream.s_cur_volume < 0.0f )
							next_ambient_stream.s_cur_volume = 0.0f;
						break; // for() loop
					}
					else

					{
						ambient_stream.s_cur_volume = 1.0f;
						next_ambient_stream.s_cur_volume = 0.0f;
					}

				}
			}

			if( ambient_stream.sp_stream )
			{
				ambient_stream.sp_stream->set_volume( ambient_stream.s_base_volume * ambient_stream.s_cur_volume );
				if( ambient_stream.s_cur_volume <= 0.0f )
				{
					ambient_stream.sp_stream->release();
					ambient_stream.sp_stream = 0;
					ambient_stream.sp_region_node = 0;
				}

			}

			if( next_ambient_stream.sp_stream )
			{
				next_ambient_stream.sp_stream->set_volume( next_ambient_stream.s_base_volume * next_ambient_stream.s_cur_volume );
				if( next_ambient_stream.s_cur_volume <= 0.0f )
				{
					next_ambient_stream.sp_stream->release();
					next_ambient_stream.sp_stream = 0;

					next_ambient_stream.sp_region_node = 0;
				}
			}
    }
  }
}
#endif

// the actual user-perceived framerate
float game::get_instantaneous_fps() const
{
	assert(total_delta > 0);
	return 1.0F / total_delta;
}

float game::get_min_fps() const
{
	assert(min_delta > 0);
	return 1.0F / min_delta;
}

float game::get_max_fps() const
{
	assert(max_delta > 0);
	return 1.0F / max_delta;

}


float game::get_avg_fps() const  // for last MAX_FRAMES frames
{
	assert(avg_delta > 0);
	return 1.0F / avg_delta;
}

// theoretical max framerate
float game::get_theoretical_fps() const
{
	if (total_delta - flip_delta - limit_delta < 0.0001f) return 10000.0f;
	return 1.0f / (total_delta - flip_delta - limit_delta);
}

// this version is designed for console development
float game::get_console_fps() const

{
	if (total_delta - flip_delta < 0.0001f) return 60.0f;
	// compute what the FPS would be assuming NTSC frame lock
	return floor(60.0f / ceil(60.0f * total_delta));
}

// clears the screen to black
void game::clear_screen()
{
	PANIC;
}


extern void system_idle();


// This function has been changed to run it's own loop while the movie is playing, there's no reason
// to keep running through app::tick() when we're playing a damn movie (except we call system_idle()
// instead now, how lovely).

#ifdef TARGET_PS2
bool game::play_movie(const char *movie_name, bool canskip, int width, int height ) // PATH relative to data is now required to be sent in
#else
bool game::play_movie(const char *movie_name, bool canskip ) // PATH relative to data is now required to be sent in
#endif
{
	PANIC;
	return true;
}


//#define NO_MUSIC

#ifdef GCCULL
// *** NEW music manager ***
bool game::is_music_playing()
{
#ifdef NO_MUSIC
	return true;
#else
	return (music_track[0] != NULL);
#endif

}


bool game::is_music_queued()
{
#ifdef NO_MUSIC
	return true;
#else
	return (music_track[1] != NULL);
#endif
}

bool game::play_music( const pstring &music_name, bool force_it )

{
#ifdef GCCULL
#ifndef NO_MUSIC

	sound_instance *temp_inst = NULL;

	switch (music_state)
	{

	case MUSIC_STATE_QUIET:
		assert(music_track[0] == NULL && music_track[1] == NULL);

		//      music_track[0] = sound_device::inst()->create_sound(music_name);
		if (music_track[0] == NULL)
		{
			warning("Could not find music track %s.", music_name.c_str());

			return false;
		}
		music_track[0]->preload();


		// go to play pending
		music_state = MUSIC_STATE_PLAY_PENDING;
		break;

	case MUSIC_STATE_PLAY_PENDING:

	case MUSIC_STATE_PLAYING:
		// enqueue and preload
		assert(music_track[0] != NULL);


		//      temp_inst = sound_device::inst()->create_sound(music_name);

		if (temp_inst == NULL)
		{
			warning("Could not find music track %s.", music_name.c_str());

			return false;
		}

		if (music_track[1] != NULL)
		{
			music_track[1]->release();

		}
		music_track[1] = temp_inst;
		music_track[1]->preload();
		if (force_it == true)
		{

			music_state = MUSIC_STATE_FORCED_CHANGE_PENDING;

		}
		break;

	case MUSIC_STATE_FORCED_CHANGE_PENDING:
		warning("Scripters tried to force playback too soon after the last forced playback.  Bad scripters.");
		break;
	}
#endif
#endif
	return true;

}


void game::stop_music(bool signal_scripts)
{

#ifdef GCCULL
#ifndef NO_MUSIC
	if (music_track[0] != NULL)
	{
		music_track[0]->release();
		music_track[0] = NULL;
	}
	if (music_track[1] != NULL)

	{
		music_track[1]->release();
		music_track[1] = NULL;
	}
	music_state = MUSIC_STATE_QUIET;
	if (signal_scripts)
		get_script_mfg()->raise_signal(script_mfg::MUSIC_FINISHED);

#endif
#endif
}

void game::update_music( time_value_t time_inc )
{
#ifdef GCCULL
#ifndef NO_MUSIC
	switch (music_state)
	{

	case MUSIC_STATE_PLAY_PENDING:
		assert(music_track[0] != NULL);

		if (music_track[0]->is_ready())
		{
			// begin playback
			music_track[0]->play();
			music_state = MUSIC_STATE_PLAYING;

		}
		break;

	case MUSIC_STATE_PLAYING:
		assert(music_track[0] != NULL);

		// check if we are finished with track0
		if (music_track[0]->is_playing() == false)
		{
			music_track[0]->release();
			if (music_track[1] != NULL)
			{
				// if so, and we have a next track ready, begin its playback
				music_track[0] = music_track[1];
				music_track[1] = NULL;
				music_track[0]->play();
				m_script_mfg.raise_signal(script_mfg::MUSIC_TRACK_SWITCH);
			}
			else

			{

				// if not, go to quiet
				music_track[0] = NULL;
				music_state = MUSIC_STATE_QUIET;

				m_script_mfg.raise_signal(script_mfg::MUSIC_FINISHED);
			}
		}
		break;


	case MUSIC_STATE_FORCED_CHANGE_PENDING:
		assert(music_track[0] != NULL && music_track[1] != NULL);

		if (music_track[1]->is_ready())
		{

			music_track[1]->play();
			music_track[0]->release();
			music_track[0] = music_track[1];
			music_track[1] = NULL;
			music_state = MUSIC_STATE_PLAYING;
			m_script_mfg.raise_signal(script_mfg::MUSIC_TRACK_SWITCH);
			debug_print("Music track switch");
		}
		break;

	case MUSIC_STATE_QUIET:
		// do nothing
		break;
	}
#endif
#endif
}
#endif

void game::update_game_mode(const float time_inc)

{
	PANIC;
}

void game::enable_physics(bool on)
{
	flag.game_paused = !on;

}


//	SetBoardIdx()

// Sets the texture to use for the surfer's board.
// Must be 0 thru 8.
/*
void game::SetBoardIdx(int hero_num, const int idx)
{
	assert(hero_num >= 0 && hero_num < MAX_PLAYERS);

	boardIdx[hero_num] = idx;
}
*/

/*
void game::SetUsingPersonalitySuit(int hero_num, bool val)
{
	PANIC;
}
*/

//	SetSurferIdx()
// Sets the current surfer.
// This method makes setHeroname() obsolete.
/*
void game::SetSurferIdx(int hero_num, int surfer)
{
	PANIC;
}
*/

//	set_game_mode()

// Sets the current gameplay mode.
// Must be called before loading the beach.
/*
void game::set_game_mode(game_mode_t m)
{
	PANIC;
}
*/

//	get_player_viewport()
// Retrieves the rect describing the specified player's render viewport.
/*
const recti & game::get_player_viewport(const int playerIdx)
{
	assert(playerIdx >= 0 && playerIdx < num_players);


	return player_viewports[playerIdx];
}
*/

//	set_active_player()
// Private helper function: sets the specified player to active, and
// makes all other players inactive.
void game::set_active_player(const int n)

{
	PANIC;
}

extern int show_anim_label;
void game::draw_debug_labels()
{
#ifdef DEBUG
	int num_players = this->get_num_players();
	for (int n = 0; n < num_players; n++)
	{
		kellyslater_controller *ksctrl = g_world_ptr->get_ks_controller(n);
		if (ksctrl && ksctrl->is_active())
			ksctrl->draw_debug_labels();
	}
	if (show_anim_label)
	{
		for (beach_object *fobj = g_beach_ptr->my_objects; fobj != NULL; fobj = fobj->next)
		{

			if (!fobj->is_active () || !fobj->spawned)
				continue;

			if (!fobj->is_physical ())
				continue;


			((surfing_object*)fobj)->draw_debug_labels();
		}
	}
#endif
}


void game::LoadingStateInit()
{
	current_loading_state = 0;
	current_loading_sub_state = 0;
	loading_progress = 0;
	last_loading_progress = 0;

	// these are all estimates, which means that the first fe->game loading
	// progress won't be quite right.
	total_sub_states_common = 41;
	total_sub_states_beach = 38;
	total_sub_states_surfer1 = 6;
	total_sub_states_surfer1_aux = 10;
	total_sub_states_surfer2 = 6;
	total_sub_states_surfer2_aux = 10;


	start_drawing_map = false;

}


void game::LoadingStateReset()

{
	current_loading_state = 0;
	loading_progress = 0;
	last_loading_progress = 0;
}

void game::LoadingStateUpdate()
{
	PANIC;
}


void game::LoadingStatePrint()

{
	stringx tmp;
	switch(current_loading_state)
	{

	case LOADING_INITIAL:			tmp = "Initial Stuff"; break;
	case LOADING_WORLD_CREATE:		tmp = "New World"; break;
	case LOADING_RESET_SOUND:		tmp = "Sound Reset"; break;
	case LOADING_COMMON_STASH:		tmp = "Common Stash"; break;
	case LOADING_BEACH_STASH:		tmp = "Beach Stash"; break;

	case LOADING_INIT_DBMENU:		tmp = "Init Debug Menu"; break;
	case LOADING_SCENE:				tmp = "Scene"; break;

	case LOADING_HERO_1_STASH:		tmp = "Hero 1 Stash"; break;
	case LOADING_HERO_1_AUX_STASH:	tmp = "Hero 1 Aux Stash"; break;
	case LOADING_HERO_1_REST:		tmp = "Hero 1 Rest"; break;
	case LOADING_HERO_2_STASH:		tmp = "Hero 2 Stash"; break;
	case LOADING_HERO_2_AUX_STASH:	tmp = "Hero 2 Aux Stash"; break;
	case LOADING_HERO_2_REST:		tmp = "Hero 2 Rest"; break;
	case LOADING_SCENE_END:			tmp = "Scene End"; break;
	case LOADING_GAME_MODE:			tmp = "Game Mode"; break;
	case LOADING_LENS_FLARE:		tmp = "Lens Flare"; break;
	case LOADING_SCRIPT:			tmp = "Script"; break;
	case LOADING_PERFORMANCE:		tmp = "Performance"; break;
	case LOADING_STATE_DONE:		tmp = "Done"; break;
	default:						tmp = "Unknown"; break;
	}
	nglPrintf("Currently loading %s\n", tmp.data());
}

void game::LoadingStateSkipSurfer(bool surfer1)
{
	// put to end of aux stash loading so it will resume the next state on Update

	current_loading_sub_state = -1;
	if(surfer1)
		current_loading_state = LOADING_HERO_1_AUX_STASH;
	else current_loading_state = LOADING_HERO_2_AUX_STASH;
}

void game::LoadingProgressUpdate()
{
	PANIC;
}

void game::SetStashSize(int loading_state, int size)
{
	int sub_states = (int)(size/(1024*128.0f));
	// add a bit to cover truncating errors
	sub_states += 1;

	switch(loading_state)
	{
	case LOADING_COMMON_STASH:		total_sub_states_common = sub_states; break;
	case LOADING_BEACH_STASH:		total_sub_states_beach = sub_states; break;
	case LOADING_HERO_1_STASH:		total_sub_states_surfer1 = sub_states; break;
	case LOADING_HERO_1_AUX_STASH:	total_sub_states_surfer1_aux = sub_states; break;
	case LOADING_HERO_2_STASH:		total_sub_states_surfer2 = sub_states; break;
	case LOADING_HERO_2_AUX_STASH:	total_sub_states_surfer2_aux = sub_states; break;
	default:

		nglPrintf("invalid state\n"); assert(0);
		break;
	}
}



extern float g_takeoff_current_offset;
vector3d game::calc_beach_spawn_pos()
{

	PANIC;
	return vector3d();
}

//	get_player_share()
// Returns the % of the screen that the specified player owns.

float game::get_player_share(const int playerIdx) const
{
	PANIC;
	return 69.0f;
}
int game::get_first_beach()
{
	PANIC;
	return 0;
}

stringx game::get_beach_location_name()
{
	PANIC;
	return stringx("");
}
stringx game::get_beach_board_name(int location)

{
	PANIC;
	return stringx("");
}



int game::get_last_surfer_index()
{
	PANIC;
	return 0;
}

int game::get_first_surfer_index()
{
	PANIC;
	return 0;
}

int game::get_next_surfer_index(int current_index)
{
	PANIC;
	return 0;
}

int game::get_prev_surfer_index(int current_index)
{
	PANIC;
	return 0;
}
void game::set_player_handicap(int hero, int new_handicap)
{
	PANIC;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	GameEventRecipient class
//
//////////////////////////////////////////////////////////////////////////////////////////////////////

/*

  //	GameEventRecipient()
  // Default constructor.
  GameEventRecipient::GameEventRecipient()
  {
  int	i;


	numSnapshots = NULL;


	  for (i = 0; i < MAX_SNAPSHOTS; i++)
	  snapshotScores[i] = NULL;

		recordNextChain = false;
		}


		  //	~GameEventRecipient()

		  // Destructor.
		  GameEventRecipient::~GameEventRecipient()
		  {

			}


			  //	Init()
			  // Sets the pointers to the game's variables that this object modifies.
			  void GameEventRecipient::Init(int * numSnaps, int * snapScores[MAX_SNAPSHOTS])
			  {
			  int i = 0;

				numSnapshots = numSnaps;

				  for (i = 0; i < MAX_SNAPSHOTS; i++)
				  snapshotScores[i] = snapScores[i];
				  }

					//	OnEvent()
					// Responds to appropriate events.
					void GameEventRecipient::OnEvent(const EVENT event, const int param1, const int param2)
					{
					if (recordNextChain)
					{
					if (event == EVT_SCORING_CHAIN_END)
					{
					ScoringManager::CHAININFO *	lastChain;

					  lastChain = g_world_ptr->get_ks_controller(param1)->get_my_scoreManager().GetLastChainInfo();



						  //Press release - re-add this code later.
						  //if (param2)
						  //	*snapshotScores[(*numSnapshots)-1] = lastChain->points;
						  //else
						  //	*snapshotScores[(*numSnapshots)-1] = 0;

						  //


							recordNextChain = false;
							}

							}
							}


*/

// @Ok
// @Matching
int game::is_PAL_allowed(void) const
{
	return 0;
}

// @Ok
// @Matching
int game::is_PAL_enabled(void) const
{
	return 0;
}

// @Ok
// @Matching
int game::is_refresh_at_50hz(void) const
{
	return 0;
}

// @Ok
// @Matching
void game::enable_PAL(bool)
{
}

// @Ok
int g_inside_widescreen;

// @Ok
// @Matching
void game::set_widescreen(bool a1)
{
	g_inside_widescreen = a1;
}

// @Ok
// @Matching
int game::get_widescreen(void) const
{
	return g_inside_widescreen;
}

// @Ok
// @Matching
int game::get_platform(void) const
{
	return 1;
}

// @Ok
// @Matching
void game::set_default_volumes(void)
{
	if (!localize_manager::inst()->get_language_id())
	{
		nslxSetVolume(NSL_SOURCETYPE_MOVIE, 0.75);
		nslxSetVolume(NSL_SOURCETYPE_MUSIC, 0.65);
		nslxSetVolume(NSL_SOURCETYPE_VOICE, 1.0);
		nslxSetVolume(NSL_SOURCETYPE_SFX, 0.65);
	}
	else
	{
		nslxSetVolume(NSL_SOURCETYPE_MOVIE, 0.75);
		nslxSetVolume(NSL_SOURCETYPE_MUSIC, 0.45);
		nslxSetVolume(NSL_SOURCETYPE_VOICE, 1.0);
		nslxSetVolume(NSL_SOURCETYPE_SFX, 0.55);
	}
}

// @Ok
// @Matching
void game::set_fog_color(color c)
{
	this->the_world->set_fog_color(c);
}

// @Ok
// @Matching
void game::set_fog_distance(float start, float end)
{
	this->the_world->set_fog_distance(start, end);
}

// @Ok
// @Matching
color game::get_fog_color(void) const
{
	return this->the_world->get_fog_color();
}

// @Ok
// @Matching
float game::get_fog_start_distance(void) const
{
	return this->the_world->get_fog_start_distance();
}

// @Ok
// @Matching
float game::get_fog_end_distance(void) const
{
	return this->the_world->get_fog_end_distance();
}

// @Ok
// @Matching
bool game::is_music_playing_now(void)
{
	return nslGetSoundStatus(this->field_64) == NSL_SOUNDSTATUS_PLAYING;
}

// @Ok
// @Matching
bool game::is_music_playing(void)
{
	return nslGetSoundStatus(this->field_64) != NSL_SOUNDSTATUS_INVALID;
}

// @Ok
// @Matching
float game::stealth_cheat_enabled(void) const
{
	return 0.0f;
}

// @Ok
// @Matching
void game::reset_control_mappings(void)
{
	input_mgr::inst()->clear_mapping();
	input_mgr::inst()->scan_devices();
	this->setup_inputs();
}

// @Ok
// @Matching
void game::intro_scene_has_started(void)
{
	this->play_intro = false;
}

// @Ok
// @Matching
void game::level_has_intro_scene_anim(void)
{
	this->play_intro = true;
}

// @Ok
// @Matching
void game::enqueue_movie(const char *movie_name)
{
	stringx mov(movie_name);

	this->movie_queue.push_back(mov);
}

// @Ok
// @Matching
void game::stop_music(void)
{
	if (this->field_64)
	{
		nslStopSound(this->field_64);

		if (this->field_58 == 3)
		{
			this->field_60 = 0.0f;
			this->field_58 = 2;
			this->field_5C = 0.0f;
		}

		this->field_68 = "";
	}
}

camera * game::get_current_view_camera(int)
{
	assert (current_view_camera);
	return current_view_camera;
}

void skip_intros(void)
{
	// @TODO
	puts("Skipping intros");
}

#include "my_assertions.h"

void validate_game(void)
{
	VALIDATE_SIZE(game, 0x310);

	VALIDATE(game, the_world, 0x28);

	VALIDATE(game, field_58, 0x58);
	VALIDATE(game, field_5C, 0x5C);
	VALIDATE(game, field_60, 0x60);
	VALIDATE(game, field_64, 0x64);

	VALIDATE(game, field_68, 0x68);

	VALIDATE(game, process_stack, 0x9C);

	VALIDATE(game, movie_queue, 0xA8);

	VALIDATE(game, play_intro, 0x18F);


	VALIDATE_VAL(NSL_SOUNDSTATUS_INVALID, 0);
	VALIDATE_VAL(NSL_SOUNDSTATUS_PLAYING, 3);
}

#include "my_patch.h"

void patch_game(void)
{
	PATCH_PUSH_RET(0x005C1930, skip_intros);

	PATCH_PUSH_RET(0x005C0BE0, game::is_PAL_allowed);
	PATCH_PUSH_RET(0x005C0C00, game::is_PAL_enabled);
	PATCH_PUSH_RET(0x005C0C20, game::is_refresh_at_50hz);
	PATCH_PUSH_RET(0x005C0C40, game::enable_PAL);
	PATCH_PUSH_RET(0x005C0C60, game::set_widescreen);
	PATCH_PUSH_RET(0x005C15F0, game::get_widescreen);
	PATCH_PUSH_RET(0x005C1610, game::get_platform);

	PATCH_PUSH_RET(0x005C23A0, game::set_default_volumes);
	PATCH_PUSH_RET(0x005C2460, game::set_fog_color);
	PATCH_PUSH_RET(0x005C24A0, game::set_fog_distance);
	PATCH_PUSH_RET(0x005C2500, game::get_fog_color);
	PATCH_PUSH_RET(0x005C2540, game::get_fog_start_distance);
	PATCH_PUSH_RET(0x005C2560, game::get_fog_end_distance);
	PATCH_PUSH_RET(0x005CA1C0, game::is_music_playing_now);
	PATCH_PUSH_RET(0x005CA190, game::is_music_playing);
	PATCH_PUSH_RET(0x005CA9A0, game::stealth_cheat_enabled);

	PATCH_PUSH_RET(0x005C9B40, game::was_A_pressed);
	PATCH_PUSH_RET(0x005C9B60, game::was_B_pressed);

	PATCH_PUSH_RET(0x005C9ED0, game::reset_control_mappings);
	PATCH_PUSH_RET(0x005C9B20, game::was_start_pressed);
	PATCH_PUSH_RET(0x005E3FB0, game::intro_scene_has_started);
	PATCH_PUSH_RET(0x005E3FD0, game::level_has_intro_scene_anim);

	PATCH_PUSH_RET(0x005E3D80, game::enqueue_movie);
	PATCH_PUSH_RET(0x005CA410, game::stop_music);
}
