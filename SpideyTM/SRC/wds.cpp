//--------------------------------------------------
// WDS.CPP

// Copyright (c) 1999-2000 Treyarch Invention LLC.  ALL RIGHTS RESERVED.
//
// Home of the world_dynamics_system
//--------------------------------------------------

#include "global.h"

#include "project.h"
#include "wds.h"

#include "particle.h"
// BIGCULL #include "ai_cue.h"
#include "ai_interface.h"
#include "anim_maker.h"
#include "app.h"
#include "beam.h"
#include "bsp_collide.h"
#include "camera.h"
#include "capsule.h"

#include "colgeom.h"
#include "collide.h"
#include "commands.h"
#include "controller.h"
// BIGCULL #include "damage_interface.h"
#include "debug_render.h"
#include "element.h" // PEH BETA LOCK

#include "entity_maker.h"

#include "entityflags.h"
#include "fcs.h"
#include "file.h"
#include "file_finder.h"
#include "game.h"
#include "game_info.h"
#include "generator.h"
#include "geomgr.h"
#include "frustum.h"
// BIGCULL #include "gun.h"
#include "hwmath.h"
#include "hwrasterize.h"
#include "inputmgr.h"
#include "iri.h"
#include "item.h"
#include "light.h"
#include "lightmgr.h"
// BIGCULL #include "manip_obj.h"
#include "marker.h"
#include "mcs.h"
#include "mic.h"
#include "osdevopts.h"
#include "oserrmsg.h"
#include "particlecleaner.h"
#include "physical_interface.h"
#include "profiler.h"
#include "projconst.h"
#include "rect.h"

#include "render_data.h"
#include "renderflav.h"
// BIGCULL #include "scanner.h"

#include "script_lib_controller.h"
#include "signals.h"
#include "sky.h"
#include "sound_group.h"
#include "sphere.h"
// BIGCULL #include "spiderman_camera.h"
// BIGCULL #include "spiderman_controller.h"
#include "stringx.h"
#include "switch_obj.h"
#include "polytube.h"
#include "lensflare.h"
#include "terrain.h"
// BIGCULL #include "thrown_item.h"
// BIGCULL #include "melee_item.h"
#include "time_interface.h"
#include "trigger.h"

// @Patch - added
#include "neolight.h"

#include "turret.h"
#include "vm_thread.h"
#include "box_trigger_interface.h"
#include "script_access.h"
#include "crawl_box.h"

#include "ai_polypath.h"
// @Patch
//#include "kellyslater_main.h"

#if defined(TARGET_XBOX)
#include "wave.h"
#include "timer.h"
#include "ks/kellyslater_controller.h"
#include "FrontEndManager.h"
#endif /* TARGET_XBOX JIV DEBUG */

#ifdef TARGET_PS2
#include "archalloc.h"	// for FreeBallocMem (dc 02/05/02)
#endif


// @Patch
//#include "ngl_support.h"
//#include "ksfx.h"
//#include "camera_tool.h"
//#include "unlock_manager.h"


#include <math.h>
#include <numeric>
#include <string.h>

#include <algorithm>
//#include "brain.h"
//#include "dread_net.h"
//P #include "membudget.h"
//P #include "memorycontext.h"

#ifdef PROJECT_KELLYSLATER
#include "ks_camera.h"
#endif // PROJECT_KELLYSLATER


#if _CONSOLE_ENABLE
#include "console.h"
#endif
#if defined(BUILD_DEBUG) || defined(BUILD_FASTDEBUG)
// BIGCULL #include "spiderman_common.h"
#endif

#ifdef TARGET_GC
#ifdef START_PROF_TIMER
#undef START_PROF_TIMER
#endif
#ifdef STOP_PROF_TIMER
#undef STOP_PROF_TIMER

#endif
#define START_PROF_TIMER(s) ((void)0)
#define STOP_PROF_TIMER(s) ((void)0)
#endif




//H extern FILE * debug_log_file;



////////////////////////////////////////////////////////////////////////////////
//  Globals
////////////////////////////////////////////////////////////////////////////////

world_dynamics_system * g_world_ptr = NULL;
vector3d up_vector(0,1,0);
bool g_frame_advance_called_this_game = false;
mouselook_controller * g_mouselook_controller;

#if defined(PROJECT_KELLYSLATER)
joypad_usercam_controller * g_ps2usercam_controller;
#endif

theta_and_psi_mcs* g_theta_and_psi_mcs;

extern profiler_timer proftimer_file_exists;
extern profiler_timer proftimer_file_open;
extern profiler_timer proftimer_file_read;

extern profiler_counter profcounter_ents;
extern profiler_counter profcounter_regions;
extern profiler_counter profcounter_active_ents;
extern profiler_counter profcounter_poss_active_ents;
extern profiler_counter profcounter_anims;

extern instance_bank<entity_track_tree> entity_track_bank;


bool g_debug_slow_ass           = false;
char g_debug_entity_id_name[32] = "GCS_WALLBREAKA";
int  g_debug_entity_id          = 3202;

int g_render_cube_map = 0;

////////////////////////////////////////////////////////////////////////////////
// File reading classes
////////////////////////////////////////////////////////////////////////////////

// @TODO - change when done
rational_t g_level_time;
//#define GET_LEVEL_TIME (g_level_time)
#define GET_LEVEL_TIME (*reinterpret_cast<rational_t*>(0x00929880))


////////////////////////////////////////////////////////////////////////////////
world_dynamics_system::world_dynamics_system()
{
	PANIC;
}


extern void destroy_script_lists();

world_dynamics_system::~world_dynamics_system()
{
	PANIC;
}

void system_idle(); // in w32_main or sy_main

void world_dynamics_system::parse_scene(chunk_file& fs, const stringx& scene_root, bool loading_sin )
{
	PANIC;
}

#if defined(TARGET_PS2)
#include <sifdev.h>
#endif /* TARGET_PS2 JIV DEBUG */

void* KSMemAllocate( u_int Size, u_int Align, const char *file, int line );

bool world_dynamics_system::wds_readfile( const char *name, unsigned char **buf, unsigned int *len, int alignment, int extra_bytes )
{
	PANIC;
	return true;
}

// This is designed to use a preallocated buffer.
bool world_dynamics_system::wds_readfile_prealloc( const char *name, unsigned char **buf, unsigned int *len, int alignment, int extra_bytes, int max_len )
{
	static char Work[256];

#if defined(TARGET_XBOX)
	strcpy( Work, "");

#else

	//  if ( 1 ) //strncmp( name, nglHostPrefix, strlen(nglHostPrefix) ) == 0 )
    strcpy( Work, "" );
	//  else
	//    strcpy( Work, nglHostPrefix );
#endif /* TARGET_XBOX JIV DEBUG */
	strcat( Work, name );


#if 1
	stringx work=Work;

	stringx ext="";


	if ( wds_exists(work,ext) )
	{
#if 1
		int size;
		char tmpbuf[32];

		app_file ChunkU;
		stash_index_entry *hdr;


		if ( !stash::is_stash_open() )
		{
			app_file ChunkU;

			wds_open(ChunkU,work,ext);

			*len = ChunkU.get_size();

			if ( *len )
			{
				if (*len > (u_int) max_len)

					return false;
				size = (*len / 32)*32;
				ChunkU.read( *buf, size );

				assert ((*len - size) < 32);

				ChunkU.read( tmpbuf, 32 );
				memcpy(*buf + size, tmpbuf, *len - size);			}
			ChunkU.close();

		}
		else if ( !ChunkU.get_memory_image( Work, *buf, *len, hdr, alignment ))
		{
			app_file ChunkU;
			int size;
			char tmpbuf[32];
			wds_open(ChunkU,work,ext);

			*len = ChunkU.get_size();

			if ( *len )

			{

				// ps2 only reads in 32-byte chunks, rounding down, so we need to read in the extra on our own.
				if (*len > (u_int) max_len)

					return false;
				size = (*len / 32)*32;
				ChunkU.read( *buf, size );

				assert ((*len - size) < 32);

				ChunkU.read( tmpbuf, 32 );
				memcpy(*buf + size, tmpbuf, *len - size);
			}
			ChunkU.close();
		}

		return (*len)>0;
#else
		app_file ChunkU;
		int size;
		char tmpbuf[32];

		wds_open(ChunkU,work,ext);

		*len = ChunkU.get_size();

		if ( *len )
		{
			// ps2 only reads in 32-byte chunks, rounding down, so we need to read in the extra on our own.
			if (*len > max_len)
				return false;
			size = (*len / 32)*32;
			ChunkU.read( *buf, size );

			assert ((*len - size) < 32);
			ChunkU.read( tmpbuf, 32 );
			memcpy(*buf + size, tmpbuf, *len - size);
		}

		ChunkU.close();


		return (*len)>0;
#endif
	}
	return false;

#else
	int size;
	char tmpbuf[32];

	int fd = sceOpen( Work, SCE_RDONLY );
	if ( fd < 0 )

		return false;

	*len = sceLseek( fd, 0, SCE_SEEK_END );

	sceLseek( fd, 0, SCE_SEEK_SET );

	// ps2 only reads in 32-byte chunks, rounding down, so we need to read in the extra on our own.
	if (*len > max_len)
		return false;
	size = (*len / 32)*32;
	sceRead( fd, *buf, size );

	assert ((*len - size) < 32);

	sceRead( fd, tmpbuf, 32 );
	memcpy(*buf + size, tmpbuf, *len - size);

	sceClose( fd );
#endif

	return true;

}

void KSMemFree( void* Ptr );

bool world_dynamics_system::wds_releasefile( unsigned char **buf )
{
	PANIC;
	return true;
}


stringx world_dynamics_system::wds_open( chunk_file& fs, const stringx& filename, const stringx& extension, int io_flags )

{
	PANIC;
	return stringx();
}

stringx world_dynamics_system::wds_open( app_file& fs, const stringx& filename, const stringx& extension, int io_flags )
{
	PANIC;
	return stringx();
}

bool world_dynamics_system::wds_exists( const stringx& filename, const stringx& extension, int io_flags )
{
	PANIC;
	return true;
}


int g_debug_num_loads = 0;
int g_debug_stop_on_load = 16;

//extern rational_t g_max_character_cam_dist;
void world_dynamics_system::load_scene( const stringx& scene_path, const stringx& hero_filename )
{
	PANIC;
}

// BETH
void world_dynamics_system::load_fe_scene(const stringx& scene_path, const stringx& filename)
{
	PANIC;
}

void world_dynamics_system::start_usercam()
{
/*	Set usercam equal to current cam.  The usercam orientation gets reset by usercam_move_mcs,

so we have to mess with the state of that object.  (dc 04/02/02)
	*/
	// @Patch - for now
	//camera *prevcam = g_game_ptr->get_player_camera(0);
	PANIC;
	/*
	usercam->set_rel_po(prevcam->get_rel_po());
	const po usercam_po = usercam->get_rel_po();
	float theta = fast_acos(usercam_po.get_matrix()[0][0]);
	if (usercam_po.get_matrix()[0][2] < 0) theta = -theta;
	float psi = fast_acos(usercam_po.get_matrix()[1][1]);
	if (usercam_po.get_matrix()[2][1] < 0) psi = -psi;
	usercam_move_mcs->set_pan_for_next_frame(theta);
	usercam_move_mcs->set_tilt_for_next_frame(psi);
	usercam_move_mcs->frame_advance(0);
	usercam_move_mcs->set_pan_for_next_frame(0);
	usercam_move_mcs->set_tilt_for_next_frame(0);
	*/
}



extern struct part_debug PartDebug;

// This gets called after the primary level data has already been loaded;
// this division is necessary for the ability to load_entire_state();
// see game.cpp and app.cpp

//================================================================================
// BIG CHANGES
// This procedure is now called many times to support "async" loading
// g_game_ptr->current_loading_state        The state
// g_game_ptr->current_loading_sub_state    The substate.. -1 when done with a state, >=0 else.
//                                          it is automatically incemented elsewhere.  == 0 on first run
//
// We try to find the stashes already open.. which is why this gets soo messy

//  --KES 02/25/02

//================================================================================


void world_dynamics_system::load_hero(device_id_t joystick_num, const int hero_num)
{
	PANIC;
}

void world_dynamics_system::load_ai_hero(device_id_t joystick_num, int surfer_index, bool pers)
{
	PANIC;
}

/* formerly the "_and_so_forth" of load_hero_and_so_forth() */
void world_dynamics_system::setup_cameras()
{
	PANIC;
}


void world_dynamics_system::unload_scene( void )
{
	PANIC;
}

bool world_dynamics_system::entity_is_preloaded(const stringx& entity_name)
{
	filespec entspec( entity_name );
	entspec.name.to_upper();
	entfile_map::iterator fi = entfiles.find( entspec.name );
	return( fi != entfiles.end() );
}


void world_dynamics_system::remove_entity_from_world_processing( entity *e )
{

	e->remove_from_terrain();
	e->force_region( NULL );  // this effectively removes the entity from the world
	e->set_active( false );
	e->set_rel_position( vector3d(-9000.0f,-9000.0f,-9000.0f) );
}


entity* world_dynamics_system::add_box_trigger( entity_id id,
											   const po& loc,
											   const convex_box& binfo,
											   const region_node_list* forced_regions,
											   unsigned int scene_flags )

{
	PANIC;
	return NULL;
}

void world_dynamics_system::add_box_trigger( entity* e )
{
	g_entity_maker->create_entity( e );

}



//DEBUG <<<<<
unsigned int * g_ptr1;



void world_dynamics_system::add_entity(const stringx& entity_name,
									   bool a, bool stationary, bool invisible, bool cosmetic, bool walkable,

									   bool repulsion, bool nonstatic,
									   entity_id & _id, entity_flavor_t flavor )
{
	entity * ent = NEW entity(entity_name, _id, flavor,DELETE_STREAM);
	if ( nonstatic )
		ent->set_flag( EFLAG_MISC_NONSTATIC, true );

	g_entity_maker->create_entity(ent);
	if (a && !stationary)
	{
		stringx composite;
		composite = stringx("Pure entity ") + entity_name  + stringx(" is active and not stationary.");
		warning (composite.c_str());
	}
	ent->set_active(a);
	ent->set_stationary(stationary);
	ent->set_walkable(walkable);
	ent->set_repulsion(repulsion);
	if (invisible)
		ent->set_visible(false);
	else
		ent->set_visible(true);
	assert(!cosmetic);
}

void world_dynamics_system::add_item( const stringx& entity_name,
									 bool a, bool stationary, bool invisible, bool cosmetic,
									 entity_id & _id, entity_flavor_t flavor )
{
	stringx ent_filename = entity_name;
	item * new_item = NEW item(ent_filename, _id, flavor, a, stationary);
	if (invisible)
		new_item->set_visible(false);
	else
		new_item->set_visible(true);
	new_item->set_flag( EFLAG_MISC_NONSTATIC, true );

	if (cosmetic && new_item->has_physical_ifc())
		new_item->physical_ifc()->set_collision_flags(0x00000000);
	add_item( new_item );
}


void world_dynamics_system::add_morphable_item( const stringx& entity_name,
											   bool a, bool stationary, bool invisible, bool cosmetic,
											   entity_id & _id, entity_flavor_t flavor )
{

	stringx ent_filename = entity_name;
	morphable_item * new_item = NEW morphable_item(ent_filename, _id, flavor, a, stationary);

	if (invisible)

		new_item->set_visible(false);
	else
		new_item->set_visible(true);


	new_item->set_flag( EFLAG_MISC_NONSTATIC, true );

	if (cosmetic && new_item->has_physical_ifc())
		new_item->physical_ifc()->set_collision_flags(0x00000000);


	add_item( new_item );
}

particle_generator* world_dynamics_system::add_particle_generator( const stringx& filename,
																  bool invisible, bool nonstatic,
																  entity_id &_id )
{
	//#pragma fixme( "this filename may need to be munged for the NEW locating system. -mkv 4/6/01" )
	/*
	particle_generator* new_pg = NEW particle_generator( filename, _id );
	new_pg->set_flag( EFLAG_MISC_NONSTATIC, true );
	new_pg->set_flag( EFLAG_MISC_RAW_NONSTATIC, true );

	new_pg->set_visible(!invisible);
	add_particle_generator(new_pg);
	return new_pg;
	*/
	PANIC;
	return NULL;
}



#if defined(TARGET_PC)
#define TWO_PASS_RENDER
#endif

#if defined(TARGET_PC) && _ENABLE_WORLD_EDITOR
extern void frame_advance_all_dialogs(time_value_t t);
#endif

//int g_visible_check_count[2];
entity * g_entity;
//H extern FILE * debug_log_file;

extern rational_t g_level_time;

#ifndef BUILD_BOOTABLE
//H extern bool g_dump_frame_info;
//extern bool g_dump_pete_stuff;

bool g_render_capsule_history = false;
//H extern bool g_render_spheres;
void render_spheres();

bool g_render_frustum_pmesh=false;
bool g_render_lights=false;
bool g_render_box_triggers = false;
#endif


int rc_count=0;



extern rational_t ACTIVATION_RADIUS;
bool g_camera_out_of_world=true;


// generate error message if entity has invalid po
void check_po( entity* e )
{
	if ( e->get_abs_po().has_nonuniform_scaling() )
		warning( e->get_id().get_val() + ": non-uniform scaling is not supported" );

	else if ( e->get_colgeom() )
	{
		rational_t s = e->get_abs_po().get_scale();
		if ( s<0.99f || s>1.01f )
			warning( e->get_id().get_val() + ": scaling is not supported on entities with collision geometry" );

	}
}


bool visibility_check( const vector3d& p1, const vector3d& p2, entity *ent )
{
	// cast a ray from p1 to p2 and see if it hits the world
	vector3d hitp, hitn;
	entity *hit_entity = NULL;

	//  if ( g_world_ptr->get_the_terrain().find_intersection( p1, p2, hitp, hitn ) )

	// takes into account the terrain and beamable entities
	bool ret = find_intersection( p1, p2,

		g_world_ptr->get_hero_ptr(g_game_ptr->get_active_player())->get_region(),  // visibility_check(): debug render vis check only works for 1 player (multiplayer fixme?)
		FI_COLLIDE_WORLD|FI_COLLIDE_BEAMABLE,
		&hitp, &hitn,
		NULL, &hit_entity);
	if(ret && (ent == NULL || ent != hit_entity))
		return false;
	else
		return true;
}


//!extern int num_chars_on_screen;
extern bool g_frame_advance_called_this_game;

char g_dump_activity_info = 0;
char g_dump_render_info = 0;

char g_dump_collide_info = 0;
char g_dump_profile_info = 0;

bool g_render_markers = false;
char g_render_paths = 0;
char g_render_brains = 0;
char g_render_anim_info = 0;
char g_render_scene_anim_info = 0;
char g_render_targeting = 0;
char g_render_vis_spheres = 0;
char g_render_portals = 0;
char g_render_ladders = 0;
char g_render_collisions = 0;
//char g_render_dread_net = 0;
char g_brains_enabled = 1;

#if _ENABLE_WORLD_EDITOR
void tool_rendering();
#endif


#if defined(DEBUG)
//vector<entity *> pass0_entities;
//vector<entity *> pass1_entities;

#endif

bool g_disable_render_ents = false;
bool g_disable_lights = false;
#ifdef PROJECT_KELLYSLATER
bool g_disable_render_rgn  = true;
#else
bool g_disable_render_rgn  = false;
#endif

rational_t g_trans_max_test = 0.07f;
rational_t g_trans_min_test = 0.0f;

#if _ENABLE_WORLD_EDITOR
extern matrix4x4 dlg_w2v;
extern matrix4x4 dlg_v2vp;
#endif

extern profiler_counter profcounter_rgn_tri_rend;

struct TranslucentObj
{
	union
	{
		struct
		{
			float detail;
			entity* ent;
		} ent;
		struct
		{

			visual_rep* vrep;
			region_node* node;
		} reg;
	};
	enum
	{
		kNone,

			kEntity,
			kRegion,
	} kind;
	TranslucentObj() : kind(kNone) {}
	TranslucentObj(float detail,entity* e) : kind(kEntity) { ent.detail=detail; ent.ent=e; }

	TranslucentObj(visual_rep* r,region_node* n) : kind(kRegion) { reg.vrep=r; reg.node=n; }
	void render( camera* camera_link, render_flavor_t flavor, const vector3d& camera_pos, time_value_t world_age = 0.0f ) const
	{
		PANIC;
	}
};


//extern profiler_timer proftimer_render_world;

extern profiler_timer proftimer_build_data;
extern profiler_timer proftimer_opaque_rgn;
extern profiler_timer proftimer_opaque_ent;
extern profiler_timer proftimer_trans_rgn;
extern profiler_timer proftimer_trans_ent;
extern profiler_timer proftimer_render_debug;

#ifdef DEBUG
bool dump_portals_to_console=false;
#endif

#include <algorithm>

float g_xpos = 0.0f;
float g_ypos = 0.0f;
float g_zpos = 20.0f;
int g_use_static_cam = 0;
float g_xup = 0.0f;
float g_yup = 1.0f;
float g_zup = 0.0f;


#ifdef NGL


#if 0
//defined(DEBUG)
static void draw_light_frustum(nglProjectorLightInfo *pProjectorLightInfo)
{

	debug_ngl_render_object.begin();
	debug_ngl_render_object.render_sphere(pProjectorLightInfo->Pos, 0.5f); // the pos of the light
	for(int i = 0; i < 8; i++)
	{
		debug_ngl_render_object.render_sphere(pProjectorLightInfo->Frustum.wp[i], 0.25f); // the 8 frustum points

	}
	debug_ngl_render_object.end();
}
#endif


// next time you change a big header file, fix this.
//   uh... what's wrong with it?
camera* g_camera_link;

nglTexture *WAVETEX_TestScreenTex( void );



void world_dynamics_system::render_debug( camera *camera_link )
{
#if !defined(BUILD_BOOTABLE)
	proftimer_render_debug.start();
	if (!g_disable_render_ents && g_render_frustum_pmesh)
	{

		render_frustum(*g_camera_link,color32(255,255,255,128));
	}

	if (g_render_lights)
	{

		vector3d cp = geometry_manager::inst()->get_camera_pos();

		vector3d cd = geometry_manager::inst()->get_camera_dir();

		for (render_data::region_list::iterator ri=to_render.regions.begin(); ri!=to_render.regions.end(); ++ri)
		{


			if ((*ri).reg->get_data())
			{
				std::vector<light_source*> const& lights=(*ri).reg->get_data()->get_lights();

				for (std::vector<light_source*>::const_iterator li=lights.begin();
				li!=lights.end();
				++li)

				{
					light_source* lp = (*li);
					if (!lp) continue;
					if (lp->has_properties())
					{






						color tmpclr(lp->get_color());
						tmpclr.clamp();
						color32 lcolor = tmpclr.to_color32();
						if (lp->get_properties().get_flavor()==LIGHT_FLAVOR_POINT)
						{
							vector3d pos = lp->get_abs_position();
							lcolor.c.a=160;
							render_sphere(pos,0.33f,lcolor);

							lcolor.c.a=100;
							render_sphere(pos,lp->get_near_range(),lcolor);
							lcolor.c.a=70;
							render_sphere(pos,lp->get_cutoff_range(),lcolor);
							if (lp->get_properties().affects_terrain())
								print_3d_text(pos, color32_white, "terrain");
							if (lp->get_properties().get_additive_color32().c.r!=0 &&
								lp->get_properties().get_additive_color32().c.g!=0 &&
								lp->get_properties().get_additive_color32().c.b!=0)
								print_3d_text(pos+vector3d(0.0f,0.5f,0.0f), color32_white, "additive");

						}
						else if (ri == to_render.regions.begin())
						{ // should only draw directional lights that are in the current camera region!
							vector3d dir = -lp->get_abs_po().get_y_facing();
							rational_t dp = -dot(dir,cd);
							if (dp>-0.5f)
							{
								if (dp<0) dp=0;
								lcolor.c.a=uint8(96*dp+32);
								render_plane(plane(cp-dir*2.0f,dir),
									lcolor);
							}
						}
					}

				}
			}
			// render small quad of current region's ambient color if you look straight down
			if (ri == to_render.regions.begin())
			{
				color tmpclr((*ri).reg->get_data()->get_ambient());
				tmpclr.clamp();
				color32 lcolor = tmpclr.to_color32();
				lcolor.c.a = 216;
				if (dot(cd,vector3d(0,-1,0))>0.9f)
					render_quad(cp+vector3d(-0.1f,-0.6f,-0.1f),
					cp+vector3d(-0.1f,-0.6f, 0.1f),
					cp+vector3d( 0.1f,-0.6f, 0.1f),

					cp+vector3d( 0.1f,-0.6f,-0.1f),
					lcolor, false);
			}
		}
	}

	if(g_render_markers)
	{

		vector3d vec;
		camera* cam = app::inst()->get_game()->get_current_view_camera();
		vector3d cam_face = cam->get_abs_po().get_facing();
		vector3d cam_pos = cam->get_abs_position();
		bool vis_check = g_camera_out_of_world;

		for (render_data::region_list::iterator ri=to_render.regions.begin(); ri!=to_render.regions.end(); ++ri)
		{
			if ((*ri).reg->get_data())
			{
				std::vector<entity*> const& entlist=(*ri).reg->get_data()->get_entities();
				std::vector<entity*>::const_iterator i = entlist.begin();


				while (i != entlist.end())
				{
					entity *mark = (*i);
					++i;


					if(mark && (mark->get_flavor() == ENTITY_MARKER /*! || mark->get_flavor() == ENTITY_CRAWL_MARKER !*/))
					{
						vec = mark->get_abs_position();

						vector3d dir = (vec - cam_pos);
						if(dir != ZEROVEC)
							dir.normalize();
						else
							dir = cam_face;

						if(dot(cam_face, dir) > 0.0f && (vis_check || visibility_check(cam_pos, vec, mark)))

						{
							vec = xform3d_1(geometry_manager::inst()->xforms[geometry_manager::XFORM_EFFECTIVE_WORLD_TO_VIEW], vec);

							if(vec.z > 0.0f)
							{
								vec = xform3d_1_homog(geometry_manager::inst()->xforms[geometry_manager::XFORM_VIEW_TO_SCREEN], vec);

								render_marker(camera_link, mark->get_abs_position(), color32(255, 255, 255, 192), 0.5f);
								render_beam( mark->get_abs_position(), mark->get_abs_position() + (mark->get_abs_po().get_facing() * 0.25f), color32(0, 255, 0, 192), 0.05f);

								hw_rasta::inst()->print( mark->get_name(), vector2di(vec.x, vec.y) );

							}
						}
					}
				}
			}
		}
	}

	if(g_render_paths)
	{
		std::vector<path_graph *>::iterator i = path_graph_list.begin();
		while(i != path_graph_list.end())
		{

			if(*i != NULL)
				(*i)->render(camera_link,color32(0, 255, 0, 128), 0.05f, g_render_paths);

			++i;
		}


		if(world_path)
			world_path->render();
	}

	if(g_render_targeting)
	{
		for (render_data::region_list::iterator ri=to_render.regions.begin(); ri!=to_render.regions.end(); ++ri)

		{
			if ((*ri).reg->get_data())

			{

				std::vector<entity*> const& entlist=(*ri).reg->get_data()->get_entities();
				std::vector<entity*>::const_iterator i = entlist.begin();

				while (i != entlist.end())
				{

					entity *ent = (*i);
					++i;


					if(ent && ent->allow_targeting())
					{
						collision_geometry *cg = ent->get_colgeom();

						if(cg)
						{
							render_colgeom(ent, color32(255, 0, 255, 128));
						}
						else
							render_sphere(ent->get_abs_position(), ent->get_radius() > 0.0f ? ent->get_radius() : 1.0f, color32(255, 0, 255, 128));
					}
				}
			}
		}

		if(get_hero_ptr(g_game_ptr->get_active_player())->get_current_target() != NULL)
			print_3d_text(get_hero_ptr(g_game_ptr->get_active_player())->get_current_target_pos(), color32(255, 0, 0, 128), get_hero_ptr(g_game_ptr->get_active_player())->get_current_target()->get_name().c_str());

		render_sphere(get_hero_ptr(g_game_ptr->get_active_player())->get_current_target_pos(), 0.1f, color32(0, 255, 0, 128));
	}



	if(g_render_vis_spheres)
	{
		//for (render_data::region_list::iterator ri=to_render.regions.begin(); ri!=to_render.regions.end(); ++ri)
		for (render_data::region_list::iterator ri=to_render.regions.begin(); ri==to_render.regions.begin(); ++ri) // just current region
		{
			region_node* node = (*ri).reg;
			region* reg = node->get_data();
			if (reg)
			{
				// to cut down on overlap madness, we only render spheres that are within x units of the camera
				float nearby = 10.0f;
				if (g_render_vis_spheres & 15)
				{
					std::vector<entity*> const& entlist=reg->get_entities();
					std::vector<entity*>::const_iterator i = entlist.begin();

					while (i != entlist.end())
					{
						entity *ent = (*i);
						++i;


						if(ent && ent->is_still_visible() && ent->is_flagged(EFLAG_MISC_IN_VISIBLE_REGION))
						{
							color32 color;
							if (ent->is_a_particle_generator())
							{
								if (!(g_render_vis_spheres & 4)) continue;
								color=color32(40, 200, 40, 96);
							}
							else if (ent->is_a_light_source())
							{
								if (!(g_render_vis_spheres & 8)) continue;
								color=color32(200, 200, 200, 96);
							}
							else
							{
								if (!(g_render_vis_spheres & 1)) continue;
								color=color32(161, 0, 236, 96);
							}
							if ((ent->get_visual_center() - geometry_manager::inst()->get_camera_pos()).length() < nearby + ent->get_visual_radius())

								render_sphere(ent->get_visual_center(), ( ent->get_visual_radius() > 0.0f ) ? ent->get_visual_radius() : 1.0f, color);
						}
					}
				}
				if (g_render_vis_spheres & 16)
				{
#if defined(TARGET_PS2) || defined(TARGET_XBOX) || defined(TARGET_GC)
					// something here?
#else
					for (int j=0; j<reg->get_num_visreps(); ++j)
					{
						visual_rep* vrep = reg->get_visrep(j);

						if ((vrep->get_center() - geometry_manager::inst()->get_camera_pos()).length() < nearby + vrep->get_radius())
							render_sphere(vrep->get_center(), ( vrep->get_radius() > 0.0f ) ? vrep->get_radius() : 1.0f, color32(236, 161, 0, 96));
					}
#endif
				}
			}
		}
	}

	if(g_render_portals)
	{
		for (render_data::region_list::iterator ri=to_render.regions.begin(); ri!=to_render.regions.end(); ++ri)
			//for (render_data::region_list::iterator ri=to_render.regions.begin(); ri==to_render.regions.begin(); ++ri) // just current region
		{
			region_node* node = (*ri).reg;
			region* reg = node->get_data();
			if (reg)
			{
				if (reg->is_active())
				{

					// to cut down on overlap madness, we only render spheres that are within x units of the camera
					for (region_node::iterator rni=node->begin(); rni!=node->end(); ++rni)
					{
						portal* port = (*rni).get_data();

						if (port->is_active()) // don't show closed portals
						{
							rational_t radius = port->get_effective_radius();
							vector3d cylvec = port->get_cylinder_normal()*(port->get_cylinder_depth()+0.01f);


							if ((g_render_portals & 1)!=0)
								render_cylinder(port->get_effective_center()-cylvec,
                                port->get_effective_center()+cylvec,
                                radius, color32(161, 236, 0, 96));
							if ((g_render_portals & 4)!=0)
							{
								std::vector<material*> altmat;
								extern game* g_game_ptr;
								altmat.push_back(g_game_ptr->get_blank_material());
								instance_render_info iri(port->get_max_faces(),
									identity_matrix,
									0,
									app::inst()->get_game()->get_current_view_camera()->get_region(),
									0,
									color32(236, 161, 0, 96),

									FORCE_TRANSLUCENCY,
									0,
									1.0f,

									NULL,
									-1,
									&altmat);
								port->render_instance(RENDER_TRANSLUCENT_PORTION, &iri);
							}
							if ((g_render_portals & 2)!=0)
								render_sphere(port->get_effective_center(), radius, color32(161, 236, 0, 56));
							if ((g_render_portals & 8)!=0)
								print_3d_text(port->get_effective_center(), color32(161, 255, 80, 192),

								"%s <-> %s", port->get_front()->get_data()->get_name().c_str(),
								port->get_back ()->get_data()->get_name().c_str());
						}

					}
				}
			}
		}
	}


	if(g_render_collisions)
	{

		for (render_data::region_list::iterator ri=to_render.regions.begin(); ri!=to_render.regions.end(); ++ri)
		{
			if ((*ri).reg->get_data())
			{
				std::vector<entity*> const& entlist=(*ri).reg->get_data()->get_possible_collide_entities();
				std::vector<entity*>::const_iterator i = entlist.begin();

				while (i != entlist.end())

				{
					entity *ent = (*i);

					++i;

					if ( ent && ent->are_collisions_active() )
					{
						bool scannable = ( ent->is_scannable() && ent->is_visible() && !ent->is_destroyable() && !ent->is_a_crate() );

						if ( g_render_collisions == 5
							|| (ent->has_camera_collision() && (g_render_collisions == 2 || g_render_collisions == 1))

							|| (ent->is_beamable() && (g_render_collisions == 3 || g_render_collisions == 1))
							|| (scannable && (g_render_collisions == 4 || g_render_collisions == 1))
							)
						{
							int r=0, g=0, b=0;

							if ( g_render_collisions == 5 )
							{
								r = ent->has_entity_collision() ? 255 : 0;
								g = 255;

								b = 255;
							}
							else
							{
								if ( ent->has_camera_collision() && (g_render_collisions==1 || g_render_collisions==2) )
									r = 255;
								if ( ent->is_beamable() && (g_render_collisions==1 || g_render_collisions==3) )
									g = 255;
								if ( scannable && (g_render_collisions==1 || g_render_collisions==4) )
									b = 255;

							}

							color32 col = color32(r,g,b,96);
							render_colgeom( ent, col );

							stringx flags = empty_string;

							if ( g_render_collisions <= 5 )
							{
								if ( ent->has_camera_collision() )
									flags += "C";
								if ( ent->is_scannable() )
									flags += "S";
								if ( ent->is_beamable() )
									flags += "B";
								if( ent->has_entity_collision() )
									flags += "E";
							}

							print_3d_text(ent->get_abs_position(), col, "%s (%s)", ent->get_name().c_str(), flags.c_str());
						}
					}
				}
			}
		}
	}

	if ( g_render_brains )

	{
		ai_interface::render_ai(g_render_brains);
	}

#if _VIS_ITEM_DEBUG_HELPER
	extern void render_vis_item_debug_info();
	if(g_render_vis_item_debug_info)
		render_vis_item_debug_info();
#endif

	if ( g_render_anim_info )
	{
		for (render_data::region_list::iterator ri=to_render.regions.begin(); ri!=to_render.regions.end(); ++ri)
		{
			if ((*ri).reg->get_data())
			{
				std::vector<entity*> const& entlist=(*ri).reg->get_data()->get_entities();
				std::vector<entity*>::const_iterator i = entlist.begin();


				while (i != entlist.end())
				{
					entity *ent = (*i);
					++i;

					if(ent && ent->is_visible())
					{

						if(g_render_anim_info == 1)
						{
							if(ent->is_hero() || !ent->has_ai_ifc())
								continue;
						}
						else if(g_render_anim_info == 2)
						{
							if(!ent->has_ai_ifc())
								continue;

						}

						stringx misc_str = empty_string;

						if(!ent->anim_finished(ANIM_PRIMARY))
							misc_str += stringx(stringx::fmt, "\nANIM_PRIMARY: %s (%.2f)", ent->get_anim_tree(ANIM_PRIMARY)->get_name().c_str(), ent->get_anim_tree(ANIM_PRIMARY)->get_floor_offset());
						if(!ent->anim_finished(ANIM_SECONDARY_A))
							misc_str += stringx(stringx::fmt, "\nANIM_SECONDARY_A: %s (%.2f)", ent->get_anim_tree(ANIM_SECONDARY_A)->get_name().c_str(), ent->get_anim_tree(ANIM_SECONDARY_A)->get_floor_offset());
						if(!ent->anim_finished(ANIM_SECONDARY_B))
							misc_str += stringx(stringx::fmt, "\nANIM_SECONDARY_B: %s (%.2f)", ent->get_anim_tree(ANIM_SECONDARY_B)->get_name().c_str(), ent->get_anim_tree(ANIM_SECONDARY_B)->get_floor_offset());
						if(!ent->anim_finished(ANIM_SECONDARY_C))
							misc_str += stringx(stringx::fmt, "\nANIM_SECONDARY_C: %s (%.2f)", ent->get_anim_tree(ANIM_SECONDARY_C)->get_name().c_str(), ent->get_anim_tree(ANIM_SECONDARY_C)->get_floor_offset());

						if(!ent->anim_finished(ANIM_TERTIARY_A))
							misc_str += stringx(stringx::fmt, "\nANIM_TERTIARY_A: %s (%.2f)", ent->get_anim_tree(ANIM_TERTIARY_A)->get_name().c_str(), ent->get_anim_tree(ANIM_TERTIARY_A)->get_floor_offset());
						if(!ent->anim_finished(ANIM_TERTIARY_B))
							misc_str += stringx(stringx::fmt, "\nANIM_TERTIARY_B: %s (%.2f)", ent->get_anim_tree(ANIM_TERTIARY_B)->get_name().c_str(), ent->get_anim_tree(ANIM_TERTIARY_B)->get_floor_offset());
						if(!ent->anim_finished(ANIM_TERTIARY_C))
							misc_str += stringx(stringx::fmt, "\nANIM_TERTIARY_C: %s (%.2f)", ent->get_anim_tree(ANIM_TERTIARY_C)->get_name().c_str(), ent->get_anim_tree(ANIM_TERTIARY_C)->get_floor_offset());
						if(!ent->anim_finished(ANIM_SCENE))
							misc_str += stringx(stringx::fmt, "\nANIM_SCENE: %s", ent->get_anim_tree(ANIM_SCENE)->get_name().c_str());

						if(ent->has_physical_ifc())

						{
							rational_t floor_off = ent->physical_ifc()->get_floor_offset();

							misc_str += stringx(stringx::fmt, "\nCurrent Floor: %.2f", floor_off);

							rational_t y = ent->get_abs_position().y - floor_off + 0.001f;
							vector3d ul = ent->get_abs_position() - (ent->get_abs_po().get_x_facing()*0.5f) - (ent->get_abs_po().get_z_facing()*0.5f);
							vector3d ur = ent->get_abs_position() + (ent->get_abs_po().get_x_facing()*0.5f) - (ent->get_abs_po().get_z_facing()*0.5f);
							vector3d ll = ent->get_abs_position() - (ent->get_abs_po().get_x_facing()*0.5f) + (ent->get_abs_po().get_z_facing()*0.5f);
							vector3d lr = ent->get_abs_position() + (ent->get_abs_po().get_x_facing()*0.5f) + (ent->get_abs_po().get_z_facing()*0.5f);

							ul.y = y;

							ur.y = y;
							ll.y = y;
							lr.y = y;

							render_triangle(ul, ur, lr, color32(0, 255, 0, 128), true);
							render_triangle(ul, ll, lr, color32(0, 255, 0, 128), true);

						}

						print_3d_text(ent->get_abs_position()+YVEC, color32_blue, "%s%s", ent->get_name().c_str(), misc_str.c_str());
					}
				}
			}
		}
	}

	if ( g_render_scene_anim_info )
	{
		for (render_data::region_list::iterator ri=to_render.regions.begin(); ri!=to_render.regions.end(); ++ri)
		{
			if ((*ri).reg->get_data())
			{
				std::vector<entity*> const& entlist=(*ri).reg->get_data()->get_entities();
				std::vector<entity*>::const_iterator i = entlist.begin();


				while (i != entlist.end())
				{
					entity *ent = (*i);
					++i;

					if ( ent && ent->playing_scene_anim() )
					{
						render_beam(ent->get_abs_position(), ent->get_abs_position() + (ent->get_abs_po().get_x_facing() * 2.0f), color32(255, 0, 0, 192), 0.05);
						render_beam(ent->get_abs_position(), ent->get_abs_position() + (ent->get_abs_po().get_y_facing() * 2.0f), color32(0, 255, 0, 192), 0.05);

						render_beam(ent->get_abs_position(), ent->get_abs_position() + (ent->get_abs_po().get_z_facing() * 2.0f), color32(0, 0, 255, 192), 0.05);
					}

				}
			}
		}

	}

	//  render_debug_text();

	proftimer_render_debug.stop();
#endif // !BUILD_BOOTABLE

#if defined (DEBUG)
	if (g_render_capsule_history)
		render_capsule_history();
	if (g_debug.render_spheres)
		render_debug_spheres();

#if 0 //BIGCULL
#if defined(BUILD_DEBUG) || defined(BUILD_FASTDEBUG) && defined(TARGET_PS2)
	extern new_LineInfo g_lineinfo[4];


	render_sphere(g_lineinfo[0].StartCoords, 0.1f, color32(255, 0, 0, 128));
	render_sphere(g_lineinfo[0].EndCoords, 0.1f, color32(0, 0, 255, 128));
	render_beam(g_lineinfo[0].StartCoords, g_lineinfo[0].EndCoords, color32(0, 255, 0, 128), 0.05f);
	if(g_lineinfo[0].collision)
	{
		render_sphere(g_lineinfo[0].hit_pos, 0.1f, color32(255, 255, 255, 128));
		render_beam(g_lineinfo[0].hit_pos, g_lineinfo[0].hit_pos+g_lineinfo[0].hit_norm, color32(0, 0, 0, 128), 0.05f);
	}

	render_sphere(g_lineinfo[1].StartCoords, 0.1f, color32(255, 0, 0, 128));
	render_sphere(g_lineinfo[1].EndCoords, 0.1f, color32(0, 0, 255, 128));
	render_beam(g_lineinfo[1].StartCoords, g_lineinfo[1].EndCoords, color32(255, 255, 0, 128), 0.05f);
	if(g_lineinfo[1].collision)
	{
		render_sphere(g_lineinfo[1].hit_pos, 0.1f, color32(255, 255, 255, 128));
		render_beam(g_lineinfo[1].hit_pos, g_lineinfo[1].hit_pos+g_lineinfo[1].hit_norm, color32(0, 0, 0, 128), 0.05f);
	}

	render_sphere(g_lineinfo[2].StartCoords, 0.1f, color32(255, 0, 0, 128));
	render_sphere(g_lineinfo[2].EndCoords, 0.1f, color32(0, 0, 255, 128));
	render_beam(g_lineinfo[2].StartCoords, g_lineinfo[2].EndCoords, color32(0, 255, 255, 128), 0.05f);
	if(g_lineinfo[2].collision)
	{
		render_sphere(g_lineinfo[2].hit_pos, 0.1f, color32(255, 255, 255, 128));
		render_beam(g_lineinfo[2].hit_pos, g_lineinfo[2].hit_pos+g_lineinfo[2].hit_norm, color32(0, 0, 0, 128), 0.05f);
	}

	render_sphere(g_lineinfo[3].StartCoords, 0.1f, color32(255, 0, 0, 128));
	render_sphere(g_lineinfo[3].EndCoords, 0.1f, color32(0, 0, 255, 128));
	render_beam(g_lineinfo[3].StartCoords, g_lineinfo[3].EndCoords, color32(255, 255, 255, 128), 0.05f);
	if(g_lineinfo[3].collision)
	{
		render_sphere(g_lineinfo[3].hit_pos, 0.1f, color32(255, 255, 255, 128));
		render_beam(g_lineinfo[3].hit_pos, g_lineinfo[3].hit_pos+g_lineinfo[3].hit_norm, color32(0, 0, 0, 128), 0.05f);
	}


	// BIGCULL g_spiderman_controller_ptr->render_lookaround_reticle();
#endif
#endif// BIGCULL

#endif

}


//nglVector &WAVETEX_GetSunPos(void);
//void WAVE_SaveTex( camera &cam );

//float WDS_lightscale=3.75;
//extern matrix4x4 WAVETEX_suntolit;

bool g_dump_rendered_ents = false;
//bool WAVETEX_ProjectThisLight( light_source *lp );
void WAVETEX_ProjectLight( light_source *lp );
//nglVector WDS_projScale;

void world_dynamics_system::render(camera* camera_link, const int heroIdx ) //, const bool underwaterflag )
{
	PANIC;
}

/*
nglLightContext *world_dynamics_system::set_current_light_context(nglLightContext *new_lc) {
	PANIC;
	return NULL;
}
*/

#else


//
//
//  DEAD CODE?
//

//


void world_dynamics_system::render(camera* camera_link)
{
	extern bool g_environment_maps_enabled;
	extern bool g_decal_maps_enabled;
	extern bool g_detail_maps_enabled;

	g_environment_maps_enabled = !os_developer_options::inst()->is_flagged(os_developer_options::FLAG_DISABLE_ENV_PASS);

	g_decal_maps_enabled = !os_developer_options::inst()->is_flagged(os_developer_options::FLAG_DISABLE_DECAL_PASS);
	g_detail_maps_enabled = !os_developer_options::inst()->is_flagged(os_developer_options::FLAG_DISABLE_DETAIL_PASS);


	to_render.resize(0);

	// build rendering lists starting in region containing camera

	// ***temporary solution; this should really be handled at the level of

	// class entity, and accessed through the camera entity instead of the
	// geometry_manager
	camera_link.compute_sector(*the_terrain);
	if (os_developer_options::inst()->is_flagged(os_developer_options::FLAG_BSP_SPRAY_PAINT) )
	{
		entity * sf = entity_manager::inst()->find_entity("SPACE_FILLER", IGNORE_FLAVOR);

		sf->set_rel_position(get_hero_ptr()->get_abs_position());
		sf->compute_sector(*the_terrain);

		sf->set_active(true);
		sf->set_visible(true);
	}
	region_node* camera_region = camera_link.get_region();
	if ( camera_region == NULL )
		camera_region = get_hero_ptr()->get_region();

	if ( camera_region )
	{
		proftimer_build_data.start();


		the_terrain->deactivate_all_regions();
		region::prepare_for_visiting();

		// clear all entities' IN_VISIBLE_REGION flag
		entity_list::iterator ei = entities.begin();
		entity_list::iterator ei_end = entities.end();
		for ( ; ei!=ei_end; ++ei)
		{
			if (*ei)
				(*ei)->set_flag( EFLAG_MISC_IN_VISIBLE_REGION, false );

		}
		std::vector<item*>::iterator ii = items.begin();
		std::vector<item*>::iterator ii_end = items.end();

		for ( ; ii!=ii_end; ++ii)
		{
			if (*ii)
				(*ii)->set_flag( EFLAG_MISC_IN_VISIBLE_REGION, false );
		}

		/*
		entity_list::iterator si = special_fx.begin();

		entity_list::iterator si_end = special_fx.end();
		for ( ; si!=si_end; ++si)

		{
		assert(*si);
		if (*si)
        (*si)->set_flag( EFLAG_MISC_IN_VISIBLE_REGION, false );

		}
		*/

		rectf screen_rect( -1, -1, 1, 1 );

		/*
		if(g_spiderman_ptr->is_ceiling_camera())
		{
		render_data::region_info temp_ri;
		temp_ri.reg = g_spiderman_ptr->get_primary_region();


		  render_data::region_list::iterator rii = find( to_render.regions.begin(), to_render.regions.end(), temp_ri );
		  if ( rii == to_render.regions.end() ) // first time seeing this region

		  {
		  // add NEW region to list (along with associated screen rect)
		  render_data::region_info ris;
		  ris.reg=temp_ri.reg;
		  ris.screen_rect = screen_rect;
		  to_render.regions.push_back(ris);
		  }
		  }
		  else
		*/
		_build_render_data_regions( to_render, camera_region, screen_rect, camera_link );

#ifdef DEBUG
		dump_portals_to_console=false;
#endif

		_build_render_data_ents( to_render );

		proftimer_build_data.stop();

		// render all opaque polygons first
		hw_rasta::inst()->send_start( hw_rasta::PT_OPAQUE_POLYS );

		// Setting up stuff for use by render batch
		// this viewport matrix isn't the same as the one in the d3d docs
		// but it actually works and makes sense
		//w2v  = geometry_manager::inst()->xforms[geometry_manager::XFORM_EFFECTIVE_WORLD_TO_VIEW];
		//v2vp = geometry_manager::inst()->xforms[geometry_manager::XFORM_VIEW_TO_VIEWPORT];

#if _ENABLE_WORLD_EDITOR
		dlg_w2v  = geometry_manager::inst()->xforms[geometry_manager::XFORM_EFFECTIVE_WORLD_TO_VIEW];

		dlg_v2vp = geometry_manager::inst()->xforms[geometry_manager::XFORM_VIEW_TO_SCREEN];
#endif

#ifdef TWO_PASS_RENDER
		typedef multimap<float,TranslucentObj> TranslucentMap;
		TranslucentMap translucents; // used to sort translucent entities and terrain back to front

#endif

		vector3d campos = camera_link.get_abs_position();

		vector3d cam2hero = get_hero_ptr()->get_abs_position() - campos;
		rational_t cam2hero_len2 = cam2hero.length2();
		cam2hero.normalize();

		vector3d hit_loc;
		g_camera_out_of_world = !in_world(campos, 0.25f, ZEROVEC, camera_link.get_region(), hit_loc);

		proftimer_opaque_rgn.start();
		if( !g_disable_render_rgn )

		{
			for (render_data::region_list::iterator ri=to_render.regions.begin(); ri!=to_render.regions.end(); ++ri)
			{
				region_node* regnode=(*ri).reg;
				region* reg=regnode->get_data();


				for (int i=reg->get_num_visreps(); --i>=0; )
				{
					visual_rep* vr = reg->get_visrep(i);
					// This is ok because the terrain is all already in world coordinates.  ;)
					if (/*!g_spiderman_ptr->is_ceiling_camera() && */!reg->frustum().includes(sphere(vr->get_center(),vr->get_radius())))
						continue;

					TranslucentObj tobj(vr,regnode);
#ifdef TWO_PASS_RENDER
					render_flavor_t passes=vr->render_passes_needed();

					if (passes&RENDER_TRANSLUCENT_PORTION)
					{
						// we need to sort these translucent regions by distance to camera squared.
						vector3d ctr=vr->get_center(0.0F);
						float dist2=(ctr-campos).length2();
						translucents.insert(TranslucentMap::value_type(-dist2,tobj));

					}
					if (passes&RENDER_OPAQUE_PORTION)
#endif
						tobj.render(RENDER_OPAQUE_PORTION
#ifndef TWO_PASS_RENDER
						| RENDER_TRANSLUCENT_PORTION
#endif
						, campos, world_clock.elapsed() );

				}
			}
		}


		proftimer_opaque_rgn.stop();

		if (!g_disable_render_ents)
		{
			proftimer_opaque_ent.start();
			render_data::entity_list::iterator pei = to_render.entities.begin();
			render_data::entity_list::iterator pei_end = to_render.entities.end();


			//      int sz = 0;
			for ( ; pei!=pei_end; ++pei)
			{
				entity* ent = (*pei).ent;
				if(ent)
				{
#ifdef DEBUG
					if( g_debug_slow_ass )
					{

						if ( stricmp( ent->get_id().get_val().c_str(), g_debug_entity_id_name )==0 )
						{
							debug_print("Rendering entity");
						}
						if ( ent->get_id().get_numerical_val()==g_debug_entity_id )
						{
							debug_print("Rendering entity");
						}
					}
#endif


					//          ++sz;
					rational_t detail = (*pei).extent;

					if ( ent->get_vrep() )
						detail = min(detail,(float)ent->get_max_polys());
					else
						detail = 1.0f;

#if 0  // this code fades conglomerates between the hero and the camera, causing sorting problems - LZ 7/19/01

					if(ent->is_alive() && ent->has_ai_ifc())
					{
						color32 new_color = ent->get_render_color();

						if(!ent->is_hero() && !ent->playing_scene_anim() && !marky_cam_enabled && os_developer_options::inst()->get_camera_state() == 0)
						{
							vector3d cam2ent = get_hero_ptr()->get_abs_position() - ent->get_abs_position();
							rational_t len2 = cam2ent.length2();

							if(len2 > 0.1f)// && len2 < cam2hero_len2)
							{
								len2 = __fsqrt(len2);
								cam2ent /= len2;


								rational_t dotp = dot(cam2ent, cam2hero);
								if(dotp > 0.75f)
								{
									float mod = 1.75f - dotp;
									mod *= mod;
									new_color.set_alpha((unsigned char)((255.0f*mod)+0.5f));
								}
								else
									new_color.set_alpha(255);
							}

							else
								new_color.set_alpha(255);
						}
						else // if (!ent->is_hero())
							new_color.set_alpha(255);

						ent->set_render_color(new_color);
					}
#endif

#ifdef TWO_PASS_RENDER
					render_flavor_t passes=ent->render_passes_needed();
					vector3d vectocamera = ent->get_visual_center() - campos;

					if(ent->get_flavor() != ENTITY_POLYTUBE)

					{
						visual_rep * my_visrep = ent->get_vrep();
						// I'd like to put this stuff into render_passes_needed.  --Sean

						// Well, then you should have joined the Spider-Man team.  --jdf
						if (ent->get_distance_fade_ok())
						{

							rational_t d = vectocamera.xz_length();
							rational_t diff =  PROJ_FAR_PLANE_D - d;

							if (diff > PROJ_FAR_PLANE_D*my_visrep->get_distance_fade_min_pct())
								passes|=RENDER_TRANSLUCENT_PORTION;
						}
					}

					if (passes&RENDER_TRANSLUCENT_PORTION)
						translucents.insert(TranslucentMap::value_type(-(vectocamera.length2()/*-sqr(ent->get_radius())*/), TranslucentObj(detail,ent)));
					if (passes&RENDER_OPAQUE_PORTION)
#endif
					{
						render_flavor_t flavor=0;

						if ((ent->get_flags()&EFLAG_MISC_NO_CLIP_NEEDED)==0)
							flavor|=RENDER_CLIPPED_FULL_DETAIL;

						ent->render(detail,
							flavor | RENDER_OPAQUE_PORTION
#ifndef TWO_PASS_RENDER
							| RENDER_TRANSLUCENT_PORTION
#endif
							, 1.0f);
					}
				}
			}


			proftimer_opaque_ent.stop();

#ifdef TWO_PASS_RENDER
			hw_rasta::inst()->send_start( hw_rasta::PT_TRANS_POLYS );

			// now render translucent stuff
			//proftimer_trans_rgn.start();
			proftimer_trans_ent.start();

			for (TranslucentMap::iterator ti=translucents.begin();
			ti!=translucents.end();
			++ti)
			{
				const TranslucentObj& to=(*ti).second;

#ifdef DEBUG
				/*        if( g_debug_slow_ass )
				if ( stricmp( to.ent.ent->get_id().get_val().c_str(), g_debug_entity_id_name )==0 )
				{
				debug_print("Rendering entity");
				}
				*/
#endif
				to.render(RENDER_TRANSLUCENT_PORTION,campos);
			}

			proftimer_trans_ent.stop();

#endif // TWO_PASS_RENDER


			// BIGCULL g_spiderman_controller_ptr->update_lookaround_reticle();


#if !defined(BUILD_BOOTABLE)
			proftimer_render_debug.start();
			if (!g_disable_render_ents && g_render_frustum_pmesh)
			{
				render_frustum(camera_link,color32(255,255,255,128));
			}

			if (g_render_lights)
			{
				vector3d cp = geometry_manager::inst()->get_camera_pos();
				vector3d cd = geometry_manager::inst()->get_camera_dir();

				for (render_data::region_list::iterator ri=to_render.regions.begin(); ri!=to_render.regions.end(); ++ri)

				{
					if ((*ri).reg->get_data())
					{
						std::vector<light_source*> const& lights=(*ri).reg->get_data()->get_lights();
						for (std::vector<light_source*>::const_iterator li=lights.begin();
						li!=lights.end();
						++li)
						{

							light_source* lp = (*li);

							if (!lp) continue;
							if (lp->has_properties())
							{
								// begin stolen verbatim from MR
								nglProjectorLightInfo plinfo;

								if (1) //lp->get_properties().get_flavor() == LIGHT_FLAVOR_PROJECTED_DIRECTIONAL)
								{
									plinfo.Type = NGLLIGHT_PROJECTED_DIRECTIONAL;

									//#pragma fixme("***** Streaming textures for projected lights needs to be fixed ASAP Wade!!! *****")


									//nglSetTexturePath( "textures\\ps2\\" );

									plinfo.Texture = WAVETEX_TestScreenTex();  //nglGetTextureA(name);
#if 0
									vector3d pos = lp->get_abs_position();

									LightPos[0] = pos.x;

									LightPos[1] = pos.y;
									LightPos[2] = pos.z;
									LightPos[3] = 1.0f;


									vector3d z_axis = lp->get_properties().get_z_axis();//lp->get_abs_po().get_z_facing();

									LightDir[0] = z_axis.x;
									LightDir[1] = z_axis.y;
									LightDir[2] = z_axis.z;
									LightDir[3] = 0.0f;

									vector3d x_axis = lp->get_properties().get_x_axis();

									linfo.Xaxis[0] = x_axis.x;
									linfo.Xaxis[1] = x_axis.y;
									linfo.Xaxis[2] = x_axis.z;
									linfo.Xaxis[3] = 0.0f;

									vector3d y_axis = lp->get_properties().get_y_axis();


									linfo.Yaxis[0] = y_axis.x;
									linfo.Yaxis[1] = y_axis.y;
									linfo.Yaxis[2] = y_axis.z;
									linfo.Yaxis[3] = 0.0f;

									linfo.Zaxis[0] = z_axis.x;
									linfo.Zaxis[1] = z_axis.y;
									linfo.Zaxis[2] = z_axis.z;
									linfo.Zaxis[3] = 0.0f;
#endif
									vector3d scale = vector3d(1,1,1); //lp->get_properties().get_scale();


									plinfo.Scale[0] = scale.x;
									plinfo.Scale[1] = scale.y;
									plinfo.Scale[2] = scale.z;

									plinfo.Scale[3] = 1.0f;

									nglListAddProjectorLight( &plinfo, (nglMatrix)&lp->get_abs_po_ptr()->get_matrix() );

#if defined(DEBUG)
									//draw_light_frustum(&plinfo);
#endif
								}

								// end stolen verbatim from MR

								color tmpclr(lp->get_color());
								tmpclr.clamp();
								color32 lcolor = tmpclr.to_color32();
								if (lp->get_properties().get_flavor()==LIGHT_FLAVOR_POINT)
								{
									vector3d pos = lp->get_abs_position();
									lcolor.c.a=160;
									render_sphere(pos,0.33f,lcolor);
									lcolor.c.a=100;
									render_sphere(pos,lp->get_near_range(),lcolor);
									lcolor.c.a=70;
									render_sphere(pos,lp->get_cutoff_range(),lcolor);
									if (lp->get_properties().affects_terrain())
										print_3d_text(pos, color32_white, "terrain");
									if (lp->get_properties().get_additive_color32().c.r!=0 &&
										lp->get_properties().get_additive_color32().c.g!=0 &&

										lp->get_properties().get_additive_color32().c.b!=0)
										print_3d_text(pos+vector3d(0.0f,0.5f,0.0f), color32_white, "additive");

								}
								else if (ri == to_render.regions.begin())
								{ // should only draw directional lights that are in the current camera region!
									vector3d dir = -lp->get_abs_po().get_y_facing();
									rational_t dp = -dot(dir,cd);
									if (dp>-0.5f)
									{
										if (dp<0) dp=0;
										lcolor.c.a=uint8(96*dp+32);
										render_plane(plane(cp-dir*2.0f,dir),
											lcolor);
									}
								}

              }
            }
          }
          // render small quad of current region's ambient color if you look straight down

          if (ri == to_render.regions.begin())
          {
			  color tmpclr((*ri).reg->get_data()->get_ambient());
			  tmpclr.clamp();
			  color32 lcolor = tmpclr.to_color32();
			  lcolor.c.a = 216;
			  if (dot(cd,vector3d(0,-1,0))>0.9f)

				  render_quad(cp+vector3d(-0.1f,-0.6f,-0.1f),
				  cp+vector3d(-0.1f,-0.6f, 0.1f),
				  cp+vector3d( 0.1f,-0.6f, 0.1f),
				  cp+vector3d( 0.1f,-0.6f,-0.1f),
				  lcolor, false);
          }
        }
      }

      if( g_render_box_triggers ) {
		  // for now, render boxes

		  render_data::region_list::iterator ri;

		  for( ri = to_render.regions.begin( ); ri != to_render.regions.end( ); ri++ ) {
			  // yoiks, this syntax is goofy
			  region* reg = (*ri).reg->get_data( );
			  const region::entity_list& ents = reg->get_entities( );
			  region::entity_list::const_iterator ei;

			  for( ei = ents.begin( ); ei != ents.end( ); ei++ ) {
				  entity* e = (*ei);

				  if( !e ) {

					  continue;
				  }

				  if( e->has_box_trigger_ifc( ) ) {
					  // render_beam_box
					  box_trigger_interface* bti = e->box_trigger_ifc( );
					  convex_box& box = bti->get_box_info( );
					  vector3d min = box.bbox.vmin;
					  vector3d max = box.bbox.vmax;
					  vector3d pos = e->get_abs_position( );
					  min += pos;

					  max += pos;
					  render_beam_box( min, max, color32_white, 0.1f );
				  }

			  }

		  }

      }


      if(g_render_markers)
      {

		  vector3d vec;
		  camera* cam = app::inst()->get_game()->get_current_view_camera();
		  vector3d cam_face = cam->get_abs_po().get_facing();

		  vector3d cam_pos = cam->get_abs_position();

		  bool vis_check = g_camera_out_of_world;

		  for (render_data::region_list::iterator ri=to_render.regions.begin(); ri!=to_render.regions.end(); ++ri)
		  {
			  if ((*ri).reg->get_data())
			  {
				  std::vector<entity*> const& entlist=(*ri).reg->get_data()->get_entities();
				  std::vector<entity*>::const_iterator i = entlist.begin();

				  while (i != entlist.end())
				  {
					  entity *mark = (*i);
					  ++i;

					  if(mark && (mark->get_flavor() == ENTITY_MARKER /*! || mark->get_flavor() == ENTITY_CRAWL_MARKER !*/))
					  {
						  vec = mark->get_abs_position();


						  vector3d dir = (vec - cam_pos);

						  if(dir != ZEROVEC)
							  dir.normalize();
						  else
							  dir = cam_face;


						  if(dot(cam_face, dir) > 0.0f && (vis_check || visibility_check(cam_pos, vec, mark)))
						  {

							  vec = xform3d_1(geometry_manager::inst()->xforms[geometry_manager::XFORM_EFFECTIVE_WORLD_TO_VIEW], vec);


							  if(vec.z > 0.0f)
							  {
								  vec = xform3d_1_homog(geometry_manager::inst()->xforms[geometry_manager::XFORM_VIEW_TO_SCREEN], vec);


								  render_marker(mark->get_abs_position(), color32(255, 255, 255, 192), 0.5f);
								  render_beam(mark->get_abs_position(), mark->get_abs_position() + (mark->get_abs_po().get_facing() * 0.25f), color32(0, 255, 0, 192), 0.05f);

								  hw_rasta::inst()->print( mark->get_name(), vector2di(vec.x, vec.y) );
							  }
						  }
					  }
				  }
			  }

		  }
      }

      if(g_render_paths)
      {
		  std::vector<path_graph *>::iterator i = path_graph_list.begin();
		  while(i != path_graph_list.end())
		  {
			  if(*i != NULL)
				  (*i)->render(color32(0, 255, 0, 128), 0.05f, g_render_paths);

			  ++i;
		  }


		  if(world_path)

			  world_path->render();
      }

      if(g_render_targeting)
      {
		  for (render_data::region_list::iterator ri=to_render.regions.begin(); ri!=to_render.regions.end(); ++ri)
		  {
			  if ((*ri).reg->get_data())
			  {
				  std::vector<entity*> const& entlist=(*ri).reg->get_data()->get_entities();

				  std::vector<entity*>::const_iterator i = entlist.begin();


				  while (i != entlist.end())

				  {
					  entity *ent = (*i);
					  ++i;

					  if(ent && ent->allow_targeting())
					  {
						  collision_geometry *cg = ent->get_colgeom();

						  if(cg)
						  {
							  render_colgeom(ent, color32(255, 0, 255, 128));
						  }
						  else
							  render_sphere(ent->get_abs_position(), ent->get_radius() > 0.0f ? ent->get_radius() : 1.0f, color32(255, 0, 255, 128));
					  }
				  }
			  }
		  }

		  if(get_hero_ptr()->get_current_target() != NULL)
			  print_3d_text(get_hero_ptr()->get_current_target_pos(), color32(255, 0, 0, 128), hero_ptr->get_current_target()->get_name().c_str());


		  render_sphere(get_hero_ptr()->get_current_target_pos(), 0.1f, color32(0, 255, 0, 128));

      }


      if(g_render_vis_spheres)
      {
		  //for (render_data::region_list::iterator ri=to_render.regions.begin(); ri!=to_render.regions.end(); ++ri)
		  for (render_data::region_list::iterator ri=to_render.regions.begin(); ri==to_render.regions.begin(); ++ri) // just current region
		  {
			  region_node* node = (*ri).reg;

			  region* reg = node->get_data();
			  if (reg)
			  {

				  // to cut down on overlap madness, we only render spheres that are within x units of the camera

				  float nearby = 10.0f;
				  if (g_render_vis_spheres & 15)
				  {
					  std::vector<entity*> const& entlist=reg->get_entities();
					  std::vector<entity*>::const_iterator i = entlist.begin();

					  while (i != entlist.end())
					  {
						  entity *ent = (*i);
						  ++i;

						  if(ent && ent->is_still_visible() && ent->is_flagged(EFLAG_MISC_IN_VISIBLE_REGION))

						  {
							  color32 color;
							  if (ent->is_a_particle_generator())
							  {
								  if (!(g_render_vis_spheres & 4)) continue;
								  color=color32(40, 200, 40, 96);
							  }

							  else if (ent->is_a_light_source())
							  {
								  if (!(g_render_vis_spheres & 8)) continue;
								  color=color32(200, 200, 200, 96);
							  }
							  else
							  {

								  if (!(g_render_vis_spheres & 1)) continue;

								  color=color32(161, 0, 236, 96);
							  }
							  if ((ent->get_visual_center() - geometry_manager::inst()->get_camera_pos()).length() < nearby + ent->get_visual_radius())
								  render_sphere(ent->get_visual_center(), ( ent->get_visual_radius() > 0.0f ) ? ent->get_visual_radius() : 1.0f, color);
						  }
					  }
				  }
				  if (g_render_vis_spheres & 16)

				  {
#if defined(TARGET_PS2) || defined(TARGET_XBOX) || defined(TARGET_GC)
					  // do something here?
#else
					  for (int j=0; j<reg->get_num_visreps(); ++j)
					  {

						  visual_rep* vrep = reg->get_visrep(j);

						  if ((vrep->get_center() - geometry_manager::inst()->get_camera_pos()).length() < nearby + vrep->get_radius())
							  render_sphere(vrep->get_center(), ( vrep->get_radius() > 0.0f ) ? vrep->get_radius() : 1.0f, color32(236, 161, 0, 96));
					  }
#endif
				  }
			  }

		  }
      }

      if(g_render_portals)

      {
		  for (render_data::region_list::iterator ri=to_render.regions.begin(); ri!=to_render.regions.end(); ++ri)
			  //for (render_data::region_list::iterator ri=to_render.regions.begin(); ri==to_render.regions.begin(); ++ri) // just current region
		  {
			  region_node* node = (*ri).reg;
			  region* reg = node->get_data();
			  if (reg)

			  {
				  if (reg->is_active())
				  {
					  // to cut down on overlap madness, we only render spheres that are within x units of the camera
					  for (region_node::iterator rni=node->begin(); rni!=node->end(); ++rni)

					  {
						  portal* port = (*rni).get_data();

						  if (port->is_active()) // don't show closed portals
						  {
							  rational_t radius = port->get_effective_radius();
							  vector3d cylvec = port->get_cylinder_normal()*(port->get_cylinder_depth()+0.01f);

							  if ((g_render_portals & 1)!=0)
								  render_cylinder(port->get_effective_center()-cylvec,
								  port->get_effective_center()+cylvec,
								  radius, color32(161, 236, 0, 96));
							  if ((g_render_portals & 4)!=0)
							  {
								  std::vector<material*> altmat;

								  extern game* g_game_ptr;
								  altmat.push_back(g_game_ptr->get_blank_material());
								  instance_render_info iri(port->get_max_faces(),
									  identity_matrix,
									  0,
									  app::inst()->get_game()->get_current_view_camera()->get_region(),
									  0,
									  color32(236, 161, 0, 96),
									  FORCE_TRANSLUCENCY,
									  0,
									  1.0f,
									  NULL,
									  -1,
									  &altmat);
								  port->render_instance(RENDER_TRANSLUCENT_PORTION, &iri);
							  }

							  if ((g_render_portals & 2)!=0)
								  render_sphere(port->get_effective_center(), radius, color32(161, 236, 0, 56));
							  if ((g_render_portals & 8)!=0)
								  print_3d_text(port->get_effective_center(), color32(161, 255, 80, 192),
								  "%s <-> %s", port->get_front()->get_data()->get_name().c_str(),

								  port->get_back ()->get_data()->get_name().c_str());

						  }
					  }
				  }
			  }
		  }
      }

      if(g_render_collisions)
      {
		  for (render_data::region_list::iterator ri=to_render.regions.begin(); ri!=to_render.regions.end(); ++ri)
		  {
			  if ((*ri).reg->get_data())

			  {
				  std::vector<entity*> const& entlist=(*ri).reg->get_data()->get_possible_collide_entities();
				  std::vector<entity*>::const_iterator i = entlist.begin();

				  while (i != entlist.end())
				  {
					  entity *ent = (*i);
					  ++i;

					  if ( ent && ent->are_collisions_active() )
					  {
						  bool scannable = ( ent->is_scannable() && ent->is_visible() && !ent->is_destroyable() && !ent->is_a_crate() );
						  if ( g_render_collisions == 5
							  || (ent->has_camera_collision() && (g_render_collisions == 2 || g_render_collisions == 1))
							  || (ent->is_beamable() && (g_render_collisions == 3 || g_render_collisions == 1))
							  || (scannable && (g_render_collisions == 4 || g_render_collisions == 1))
							  )
						  {
							  int r=0, g=0, b=0;
							  if ( g_render_collisions == 5 )
							  {
								  r = ent->has_entity_collision() ? 255 : 0;
								  g = 255;
								  b = 255;
							  }
							  else
							  {
								  if ( ent->has_camera_collision() && (g_render_collisions==1 || g_render_collisions==2) )
									  r = 255;
								  if ( ent->is_beamable() && (g_render_collisions==1 || g_render_collisions==3) )
									  g = 255;
								  if ( scannable && (g_render_collisions==1 || g_render_collisions==4) )
									  b = 255;

							  }

							  color32 col = color32(r,g,b,96);
							  render_colgeom( ent, col );

							  stringx flags = empty_string;

							  if ( g_render_collisions <= 5 )
							  {
								  if ( ent->has_camera_collision() )
									  flags += "C";
								  if ( ent->is_scannable() )
									  flags += "S";

								  if ( ent->is_beamable() )
									  flags += "B";
								  if( ent->has_entity_collision() )

									  flags += "E";
							  }


							  print_3d_text(ent->get_abs_position(), col, "%s (%s)", ent->get_name().c_str(), flags.c_str());
						  }
					  }
				  }

			  }
		  }
      }

      if ( g_render_brains )
      {
		  ai_interface::render_ai(g_render_brains);
      }

#if _VIS_ITEM_DEBUG_HELPER
	  extern void render_vis_item_debug_info();
	  if(g_render_vis_item_debug_info)
		  render_vis_item_debug_info();
#endif

      if ( g_render_anim_info )
      {
		  for (render_data::region_list::iterator ri=to_render.regions.begin(); ri!=to_render.regions.end(); ++ri)
		  {
			  if ((*ri).reg->get_data())

			  {
				  std::vector<entity*> const& entlist=(*ri).reg->get_data()->get_entities();
				  std::vector<entity*>::const_iterator i = entlist.begin();

				  while (i != entlist.end())
				  {
					  entity *ent = (*i);
					  ++i;

					  if(ent && ent->is_visible())
					  {

						  if(g_render_anim_info == 1)
						  {

							  if(ent->is_hero() || !ent->has_ai_ifc())
								  continue;

						  }
						  else if(g_render_anim_info == 2)
						  {
							  if(!ent->has_ai_ifc())
								  continue;
						  }

						  stringx misc_str = empty_string;

						  if(!ent->anim_finished(ANIM_PRIMARY))
							  misc_str += stringx(stringx::fmt, "\nANIM_PRIMARY: %s (%.2f)", ent->get_anim_tree(ANIM_PRIMARY)->get_name().c_str(), ent->get_anim_tree(ANIM_PRIMARY)->get_floor_offset());
						  if(!ent->anim_finished(ANIM_SECONDARY_A))
							  misc_str += stringx(stringx::fmt, "\nANIM_SECONDARY_A: %s (%.2f)", ent->get_anim_tree(ANIM_SECONDARY_A)->get_name().c_str(), ent->get_anim_tree(ANIM_SECONDARY_A)->get_floor_offset());
						  if(!ent->anim_finished(ANIM_SECONDARY_B))
							  misc_str += stringx(stringx::fmt, "\nANIM_SECONDARY_B: %s (%.2f)", ent->get_anim_tree(ANIM_SECONDARY_B)->get_name().c_str(), ent->get_anim_tree(ANIM_SECONDARY_B)->get_floor_offset());
						  if(!ent->anim_finished(ANIM_SECONDARY_C))

							  misc_str += stringx(stringx::fmt, "\nANIM_SECONDARY_C: %s (%.2f)", ent->get_anim_tree(ANIM_SECONDARY_C)->get_name().c_str(), ent->get_anim_tree(ANIM_SECONDARY_C)->get_floor_offset());
						  if(!ent->anim_finished(ANIM_TERTIARY_A))
							  misc_str += stringx(stringx::fmt, "\nANIM_TERTIARY_A: %s (%.2f)", ent->get_anim_tree(ANIM_TERTIARY_A)->get_name().c_str(), ent->get_anim_tree(ANIM_TERTIARY_A)->get_floor_offset());
						  if(!ent->anim_finished(ANIM_TERTIARY_B))

							  misc_str += stringx(stringx::fmt, "\nANIM_TERTIARY_B: %s (%.2f)", ent->get_anim_tree(ANIM_TERTIARY_B)->get_name().c_str(), ent->get_anim_tree(ANIM_TERTIARY_B)->get_floor_offset());

						  if(!ent->anim_finished(ANIM_TERTIARY_C))
							  misc_str += stringx(stringx::fmt, "\nANIM_TERTIARY_C: %s (%.2f)", ent->get_anim_tree(ANIM_TERTIARY_C)->get_name().c_str(), ent->get_anim_tree(ANIM_TERTIARY_C)->get_floor_offset());
						  if(!ent->anim_finished(ANIM_SCENE))

							  misc_str += stringx(stringx::fmt, "\nANIM_SCENE: %s", ent->get_anim_tree(ANIM_SCENE)->get_name().c_str());


						  if(ent->has_physical_ifc())

						  {
							  rational_t floor_off = ent->physical_ifc()->get_floor_offset();

							  misc_str += stringx(stringx::fmt, "\nCurrent Floor: %.2f", floor_off);


							  rational_t y = ent->get_abs_position().y - floor_off + 0.001f;
							  vector3d xface = ent->get_abs_po().get_x_facing();
							  vector3d zface = ent->get_abs_po().get_z_facing();

							  vector3d ul = ent->get_abs_position() - (ent->get_abs_po().get_x_facing()*0.5f) - (ent->get_abs_po().get_z_facing()*0.5f);
							  vector3d ur = ent->get_abs_position() + (ent->get_abs_po().get_x_facing()*0.5f) - (ent->get_abs_po().get_z_facing()*0.5f);
							  vector3d ll = ent->get_abs_position() - (ent->get_abs_po().get_x_facing()*0.5f) + (ent->get_abs_po().get_z_facing()*0.5f);
							  vector3d lr = ent->get_abs_position() + (ent->get_abs_po().get_x_facing()*0.5f) + (ent->get_abs_po().get_z_facing()*0.5f);


							  ul.y = y;

							  ur.y = y;
							  ll.y = y;
							  lr.y = y;

							  render_triangle(ul, ur, lr, color32(0, 255, 0, 128), true);
							  render_triangle(ul, ll, lr, color32(0, 255, 0, 128), true);
						  }

						  print_3d_text(ent->get_abs_position()+YVEC, color32_blue, "%s%s", ent->get_name().c_str(), misc_str.c_str());

					  }
				  }
			  }
		  }
      }


      if ( g_render_scene_anim_info )
      {
		  for (render_data::region_list::iterator ri=to_render.regions.begin(); ri!=to_render.regions.end(); ++ri)
		  {
			  if ((*ri).reg->get_data())
			  {
				  std::vector<entity*> const& entlist=(*ri).reg->get_data()->get_entities();
				  std::vector<entity*>::const_iterator i = entlist.begin();

				  while (i != entlist.end())
				  {
					  entity *ent = (*i);

					  ++i;


					  if ( ent && ent->playing_scene_anim() )
					  {
						  render_beam(ent->get_abs_position(), ent->get_abs_position() + (ent->get_abs_po().get_x_facing() * 2.0f), color32(255, 0, 0, 192), 0.05);
						  render_beam(ent->get_abs_position(), ent->get_abs_position() + (ent->get_abs_po().get_y_facing() * 2.0f), color32(0, 255, 0, 192), 0.05);
						  render_beam(ent->get_abs_position(), ent->get_abs_position() + (ent->get_abs_po().get_z_facing() * 2.0f), color32(0, 0, 255, 192), 0.05);
					  }
				  }

			  }
		  }
      }

	  /*
      if(g_render_dread_net && dread_network)
      {
	  dread_network->render(color32(0, 0, 255, 127), g_render_dread_net);
      }
	  */

	  //      render_debug_text();

      proftimer_render_debug.stop();
#endif // !BUILD_BOOTABLE

#if _ENABLE_WORLD_EDITOR
      tool_rendering();
#endif


    }
  }
#if defined (DEBUG)
  if (g_render_capsule_history)

	  render_capsule_history();
  if (g_debug.render_spheres)
	  render_debug_spheres();


#if 0 // BIGCULL
#if defined(BUILD_DEBUG) || defined(BUILD_FASTDEBUG)
  extern new_LineInfo g_lineinfo[4];

  render_sphere(g_lineinfo[0].StartCoords, 0.1f, color32(255, 0, 0, 128));
  render_sphere(g_lineinfo[0].EndCoords, 0.1f, color32(0, 0, 255, 128));
  render_beam(g_lineinfo[0].StartCoords, g_lineinfo[0].EndCoords, color32(0, 255, 0, 128), 0.05f);
  if(g_lineinfo[0].collision)
  {
	  render_sphere(g_lineinfo[0].hit_pos, 0.1f, color32(255, 255, 255, 128));
	  render_beam(g_lineinfo[0].hit_pos, g_lineinfo[0].hit_pos+g_lineinfo[0].hit_norm, color32(0, 0, 0, 128), 0.05f);
  }

  render_sphere(g_lineinfo[1].StartCoords, 0.1f, color32(255, 0, 0, 128));
  render_sphere(g_lineinfo[1].EndCoords, 0.1f, color32(0, 0, 255, 128));
  render_beam(g_lineinfo[1].StartCoords, g_lineinfo[1].EndCoords, color32(255, 255, 0, 128), 0.05f);

  if(g_lineinfo[1].collision)
  {
	  render_sphere(g_lineinfo[1].hit_pos, 0.1f, color32(255, 255, 255, 128));
	  render_beam(g_lineinfo[1].hit_pos, g_lineinfo[1].hit_pos+g_lineinfo[1].hit_norm, color32(0, 0, 0, 128), 0.05f);

  }

  render_sphere(g_lineinfo[2].StartCoords, 0.1f, color32(255, 0, 0, 128));
  render_sphere(g_lineinfo[2].EndCoords, 0.1f, color32(0, 0, 255, 128));
  render_beam(g_lineinfo[2].StartCoords, g_lineinfo[2].EndCoords, color32(0, 255, 255, 128), 0.05f);
  if(g_lineinfo[2].collision)
  {
	  render_sphere(g_lineinfo[2].hit_pos, 0.1f, color32(255, 255, 255, 128));
	  render_beam(g_lineinfo[2].hit_pos, g_lineinfo[2].hit_pos+g_lineinfo[2].hit_norm, color32(0, 0, 0, 128), 0.05f);
  }

  render_sphere(g_lineinfo[3].StartCoords, 0.1f, color32(255, 0, 0, 128));
  render_sphere(g_lineinfo[3].EndCoords, 0.1f, color32(0, 0, 255, 128));
  render_beam(g_lineinfo[3].StartCoords, g_lineinfo[3].EndCoords, color32(255, 255, 255, 128), 0.05f);
  if(g_lineinfo[3].collision)
  {
	  render_sphere(g_lineinfo[3].hit_pos, 0.1f, color32(255, 255, 255, 128));
	  render_beam(g_lineinfo[3].hit_pos, g_lineinfo[3].hit_pos+g_lineinfo[3].hit_norm, color32(0, 0, 0, 128), 0.05f);
  }

  // BIGCULL g_spiderman_controller_ptr->render_lookaround_reticle();
#endif
#endif// BIGCULL


#endif


#ifndef BUILD_BOOTABLE

  if (g_debug.dump_frame_info)
  {
	  dump_frame_info();
  }

#endif
}

#endif

void world_dynamics_system::usercam_frame_advance(time_value_t t)
{
	PANIC;
}

void world_dynamics_system::scene_analyzer_frame_advance(time_value_t t)
{
	PANIC;
}




#define _TARGETING_NEAR_THRESHOLD       3.0f
#define _TARGETING_FAR_THETA            0.98f // ~11.5 degrees

#define _TARGETING_NEAR_THETA           0.6f  // ~53.1 degrees

#define _TARGETING_FAR_STICKY_THETA     0.8f  // ~36.8 degrees
#define _TARGETING_NEAR_STICKY_THETA    0.4f  // ~66.4 degrees
#define _TARGETING_STICKY_DISTANCE_MOD  0.75f
#define _TARGETING_HEIGHT_TOLERANCE     0.0f


void build_region_list_radius( std::vector<region_node*> *regs, region_node* rn, const vector3d& pos, rational_t rad, bool only_active_portals, std::vector<region_node*> *append )
{
	PANIC;
}



void build_region_list( std::vector<region_node*> *regs, region_node *r, const vector3d& o, const vector3d& d, std::vector<region_node*> *append )
{
	if( std::find(regs->begin(), regs->end(), r) == regs->end() )
	{
		if (!r) return; // laser grenades can end up here with a null primary region
		regs->push_back( r );

		if(append && std::find(append->begin(), append->end(), r) == append->end())

			append->push_back(r);

		for( edge_iterator tei = r->begin(); tei != r->end(); ++tei )
		{
			region_node* dest = (*tei).get_dest();

			if( std::find(regs->begin(), regs->end(), dest) == regs->end() )
			{
				rational_t  rad, depth;
				vector3d    hit_point, norm, cent;
				portal      *port = (*tei).get_data();
				if (port->is_active())
				{
					rad = port->get_effective_radius();
					depth = port->get_cylinder_depth();
					cent = port->get_effective_center();
					norm = port->get_cylinder_normal();

					if( collide_segment_cylinder( o, d, cent, norm, rad, depth, hit_point ) )
					{
						build_region_list( regs, dest, hit_point, (o + d) - hit_point, append );
					}
				}
			}

		}
	}
}



#define MAX_AUTO_AIM_DIST 15.0f





#ifdef DEBUG
rational_t debug_delay = 0.0f;


//static int g_debug;

#endif

float g_radius = 0;
float g_frame_by_frame = 0.0f;

extern int g_iflrand_counter;
int global_frame_counter = 0;

extern profiler_timer proftimer_physics;
extern profiler_timer proftimer_adv_controllers;
extern profiler_timer proftimer_adv_player_controller;
extern profiler_timer proftimer_adv_fcs;
extern profiler_timer proftimer_adv_generators;
extern profiler_timer proftimer_adv_entities;
extern profiler_timer proftimer_adv_mcs;
extern profiler_timer proftimer_phys_render;
extern profiler_timer proftimer_adv_anims;
extern profiler_timer proftimer_adv_anims_ents;
extern profiler_timer proftimer_get_elevation;
extern profiler_timer proftimer_adv_ent_timed;
extern profiler_timer proftimer_adv_scripts;

extern profiler_timer proftimer_adv_ent_setup;


extern profiler_timer proftimer_adv_AI;
extern profiler_timer proftimer_adv_AI_cue_mgr;


#include <algorithm>


#if defined(PROJECT_KELLYSLATER) && defined(DEBUG)
entity * gde = NULL;

char gde_name[256];
#endif


void world_dynamics_system::frame_advance( time_value_t t )
{
	PANIC;
}

extern bool loading_from_scn_file;  // see wds.cpp for comments
#ifdef DEBUG
//H extern bool g_dump_frame_info;
#endif

void world_dynamics_system::add_marker( entity_id& _id )
{
	marker* marker_ptr = NEW marker(_id);
	marker_ptr->set_flag( EFLAG_MISC_NONSTATIC, true );
	add_marker( marker_ptr );
}

beam* world_dynamics_system::add_beam( const entity_id& _id, unsigned int _flags )
{
	beam* b = NEW beam(_id,_flags);
	add_beam( b );
	return b;

}


entity* world_dynamics_system::create_preloaded_entity_or_subclass( const stringx& entity_name,
																   const stringx& entity_dir )
{
	PANIC;
	return NULL;
}

float world_dynamics_system::get_surface_effect_duration( int surf_index )
{

	surfaceinfo_list_t::value_type::second_type v = surfaceinfo_list[ surf_index ];
	return v ? v->effect_duration : 0.0f;
}

stringx world_dynamics_system::get_surface_sound_name( int surf_index )
{
	surfaceinfo_list_t::value_type::second_type v = surfaceinfo_list[ surf_index ];

	return v ? v->sound_name : stringx();
}

stringx world_dynamics_system::get_surface_effect_name( int surf_index )
{
	surfaceinfo_list_t::value_type::second_type v = surfaceinfo_list[ surf_index ];
	return v ? v->effect_name : stringx();
}

void world_dynamics_system::load_scene_anim( const stringx &filename )

{
	//P  int alloc0 = memtrack::get_total_alloced();

	//P  g_memory_context.push_context( "ANIMS" );

	scene_anim* snm_pt = NEW scene_anim;
	snm_pt->load( filename );
	scene_anim_map[ filename ] = snm_pt;


	/*P
	g_memory_context.pop_context();
	membudget()->use( membudget_t::ANIMS, memtrack::get_total_alloced()-alloc0 );
	P*/
}


// @Ok
// @AlmostMatching - stl goofyness
void world_dynamics_system::add_light_source( light_source* ls )
{
	// @Patch
	GET_ENTITY_MAKER->create_entity(ls);
	lights.push_back( ls );
}

// @Ok
// @Matching
void world_dynamics_system::remove_light_source( light_source* ls )
{
	std::vector<light_source*>::iterator lit;
	lit = std::find( lights.begin(), lights.end(), ls );
	if (lit!=lights.end())
	{
		lights.erase( lit );
	}

	// @Patch
	if (remove_entity(ls))
	{
		delete ls;
	}
}


// @Ok
// @Matching
void world_dynamics_system::add_marker( marker* e )

{
	GET_ENTITY_MAKER->create_entity( e );
}

// @Ok
// @Matching
void world_dynamics_system::add_beam( beam* e )
{
	GET_ENTITY_MAKER->create_entity( e );
}

void world_dynamics_system::add_camera( camera* e )
{
	GET_ENTITY_MAKER->create_entity( e );
}

void world_dynamics_system::add_mic( mic* e )
{
	GET_ENTITY_MAKER->create_entity( e );

}


// @Ok
// @Matching
void world_dynamics_system::add_particle_generator( particle_generator* pg )
{
	GET_ENTITY_MAKER->create_entity( pg );
}

// @Ok
// @Matching
void world_dynamics_system::add_lensflare( lensflare* e )
{
	GET_ENTITY_MAKER->create_entity( e );
}

// @Ok
// @Matching
void world_dynamics_system::add_neolight( neolight* nl )
{
	GET_ENTITY_MAKER->create_entity( nl );
}

void world_dynamics_system::add_item( item* it )
{
	items.push_back( it );
	// this needs to be called to set up last-frame info
	it->frame_done();

}


// @Ok
// @Matching
void world_dynamics_system::add_turret( turret* cg )
{
	GET_ENTITY_MAKER->create_entity( cg );
}

// @Ok
// @NotMatching - uses 4 more bytes of stack and the alloc is different
// This creates an instance of the named effect (assumed to be an .ent file
// located in the fx\ directory) and adds it to the list of effects to be
// destroyed after expiration.
entity* world_dynamics_system::add_time_limited_effect( const char* name,
													   const po& loc,
													   time_value_t duration )
{
	entity* e = GET_ENTITY_MAKER->create_entity_or_subclass(
			name,
			entity_id::make_unique_id(),
			loc,
			"fx\\",
			ACTIVE_FLAG|NONSTATIC_FLAG );

	assert(e != NULL);


	time_limited_entities.push_back( ent_time_limit(e,duration) );
	e->set_time_limited(true);

	// this needs to be called to set up last-frame info
	e->frame_done();
	return e;
}


// @Ok
// @Matching
void world_dynamics_system::make_time_limited( entity* e, time_value_t duration )
{
	assert(e != NULL);

	time_limited_entities.push_back( ent_time_limit(e,duration) );
	e->set_time_limited(true);
}



// @Ok
// @Matching
// The world assumes responsibility for deleting anims that get added here.
// NOTE: this function assumes the given anim has not previously been added
void world_dynamics_system::add_anim( entity_anim_tree* new_anim )
{
	pentity_anim_tree_vector::iterator i = std::find( anims.begin(), anims.end(), (entity_anim_tree*)NULL );

	if ( i == anims.end() )
		anims.push_back( new_anim );
	else
		(*i) = new_anim;
}

// @Ok
// @Matching
// This deconstructs the given anim.
void world_dynamics_system::kill_anim( entity_anim_tree* the_anim )
{

	pentity_anim_tree_vector::iterator i = std::find( anims.begin(), anims.end(), the_anim );
	if ( i != anims.end() )
	{
		entity_anim_tree* a = *i;
		a->get_entity()->clear_anim( a );
		*i = NULL;
	}
}

ett_manager *world_dynamics_system::get_ett_manager()
{
  return ett_mgr;
}


bool world_dynamics_system::eligible_for_frame_advance( entity_anim_tree* anm ) const
{
	if ( anm->is_suspended() || (anm->is_finished() && anm->is_done_tween()) )  // don't advance suspended or finished anim
		return false;

	entity* e = anm->get_entity();


	if(e == NULL || !e->is_visible())	// don't frame_advance our despawned water objects (dc 10/15/01)
		return(false);

	return (
        ( !anm->is_relative_to_start()
		|| anm->is_noncosmetic()                          // always advance NONCOSMETIC anims
		|| e->is_flagged( EFLAG_MISC_IN_VISIBLE_REGION )  // or if entity is currently visible
		|| (e->has_ai_ifc() && e->ai_ifc()->is_active() && !e->ai_ifc()->cosmetic() && !e->ai_ifc()->is_disabled())
		|| (!e->is_flagged(EFLAG_GRAPHICS) && e->is_in_active_region())  // or, if non-graphical, is in an active region

		|| !anm->is_looping()                             // or anim is not looping
		|| anm->get_time() == 0.0f                        // or at start of looping anim
		|| !anm->is_done_tween()
		)
        );
}


// @Ok
// @AlmostMatching - different inlined for size stuff
// these are created elsewhere.  Will need to become like add_joint if
// we add a remove_controller.  Also in frame_advance.
int world_dynamics_system::add_controller(controller * new_controller)

{
	controllers.push_back(new_controller);
	return controllers.size();
}


// @Ok
// @AlmostMatching - different inlined for size stuff
// these are created elsewhere.  Will need to become like add_joint if
// we add a remove_mcs.  Also in frame_advance.
int world_dynamics_system::add_mcs(motion_control_system * new_mcs)
{

	mcs_list.push_back(new_mcs);
	return mcs_list.size();
}



// @Ok
// @AlmostMatching - different inlining
// these are created elsewhere.  Will need to become like add_joint if
// we add a remove_fcs.  Also in frame_advance.
int world_dynamics_system::add_fcs(force_control_system * new_fcs)
{
	fcs_list.push_back(new_fcs);

	return fcs_list.size();
}


// @Ok
// @AlmostMatching - different inlining
// these are created elsewhere.  Will need to become like add_joint if
// we add a remove_generator.  Also in frame_advance.
int world_dynamics_system::add_generator(force_generator * new_generator)
{
	generators.push_back(new_generator);
	return generators.size();
}



// @Ok
// @Matching
void world_dynamics_system::enable_marky_cam( bool enable, rational_t priority )
{
	assert(marky_cam);

	if((enable && priority >= marky_cam->get_priority()) || (!enable && priority == marky_cam->get_priority()))
	{
		marky_cam_enabled = enable;
		marky_cam->camera_set_roll(0.0f);


		if(enable)
		{
			marky_cam->set_priority(priority);

		}
		else

		{
			marky_cam->reset_priority();
		}
	}
}



// @Ok
// @Matching
void world_dynamics_system::add_to_entities( entity *e )
{
	std::vector<entity*>::iterator it;
	assert(std::find( entities.begin(), entities.end(), e) == entities.end());	// don't add the same entity twice
	it = std::find( entities.begin(), entities.end(), (entity*)NULL);

	if ( it == entities.end() )
	{
		assert(e->get_bone_idx() < 0);
		entities.push_back( e );
	}
	else
		*it = e;

}



void world_dynamics_system::add_dynamic_instanced_entity( entity* e )
{
	std::vector<entity*>::iterator it;

	assert(std::find( entities.begin(), entities.end(), e) == entities.end());	// don't add the same entity twice
	it = std::find( entities.begin(), entities.end(), (entity*)NULL);
	if ( it == entities.end() )
	{
		assert(e->get_bone_idx() < 0);
		entities.push_back( e );
	}
	else
		*it = e;
	// non-uniform scaling is not allowed;
	// uniform scaling is allowed only on entities that have no collision geometry
	check_po( e );
	e->compute_sector( *the_terrain, loading_from_scn_file );
	// some flavors of entity are inactive by default
	switch ( e->get_flavor() )
	{
    case ENTITY_ENTITY:
    case ENTITY_MARKER:
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

bool world_dynamics_system::remove_entity( unsigned int i )
{
	PANIC;
	return true;
	// remove it from file
	assert( i < entities.size() );

	return remove_entity(entities[i]);
}


bool world_dynamics_system::remove_entity( entity *e )
{
	typedef bool (__fastcall *func_ptr)(world_dynamics_system*, int, entity*);
	func_ptr func = (func_ptr)0x0062C1D0;

	return func(this, 0, e);
	PANIC;
	return true;

	std::vector<entity*>::iterator it;

	bool success = false;
	it = std::find( entities.begin(), entities.end(), e );
	if ( it != entities.end() )
	{
		*it = NULL;
		success = true;
	}
	it = std::find( active_entities.begin(), active_entities.end(), e );

	if ( it != active_entities.end() )
		*it = NULL;
	it = std::find( guaranteed_active_entities.begin(), guaranteed_active_entities.end(), e );
	if ( it != guaranteed_active_entities.end() )
		*it = NULL;

	// remove it from file list...this is weird, because you could
	// add item a to the list, add item b, and then remove item a,

	// and the game would load the file rather than instance it.

	// generally, we shouldn't be loading/unloading stuff on the fly,
	// but that's currently the way spells are designed
	entfile_map::iterator fi;
	for( fi = entfiles.begin(); fi != entfiles.end(); ++fi )
	{
		if( (*fi).second == e )

		{
			entfiles.erase( fi );
			break;

		}
	}
	return success;

}



bool world_dynamics_system::remove_item( item * iptr )
{
	std::vector<item *>::iterator it;
	bool outval = false;
	int i;

	for (it = items.begin(), i=0; it!=items.end(); ++it, ++i)

	{
		if (iptr==(*it))

		{
			remove_item(i);
			outval = true;
			break;
		}
	}
	return outval;
}


bool world_dynamics_system::remove_item( unsigned int i )
{
	items[i] = NULL;
	return true;
}


// remove and delete entity if present; note that some flavors are disallowed
void world_dynamics_system::destroy_entity( entity* e )
{
	switch ( e->get_flavor() )
	{
    case ENTITY_ITEM:
		error( "Unsupported flavor for destruction: %s", entity_flavor_names[e->get_flavor()] );

    case ENTITY_LIGHT_SOURCE:
		// @Patch
		remove_light_source( reinterpret_cast<light_source*>(e) );
		break;

    default:
		if ( remove_entity( e ) )

			delete e;
		break;
	}

}



entity *world_dynamics_system::get_entity(const stringx &name)
{
	return(entity_manager::inst()->find_entity(entity_id::make_entity_id(name.c_str()),IGNORE_FLAVOR,FIND_ENTITY_UNKNOWN_OK));

}



#ifndef BUILD_BOOTABLE
// for debugging purposes; dump information on all threads to a file
void world_dynamics_system::dump_frame_info()
{
	static unsigned dump_frame_info_counter = ~0;
	static hires_clock_t xTPS;

	if(dump_frame_info_counter == (unsigned)~0)
	{
		dump_frame_info_counter = 0;
		xTPS.reset();
	}

	else

	{
		time_value_t delta = xTPS.elapsed_and_reset();
		dump_frame_info_counter += (unsigned)(delta*1000.0f);
	}

	if(dump_frame_info_counter > 3000)
	{

		unsigned i;

		host_system_file_handle outfile = host_fopen( "frameinfo.txt", HOST_WRITE );
		host_fprintf( outfile, "Render Data\n" );


		host_fprintf( outfile, "  Regions\n" );
		for (i=0;i<to_render.regions.size();++i)
		{
			host_fprintf( outfile, "    %s\n", to_render.regions[i].reg->get_data()->get_name().c_str());
		}


		host_fprintf( outfile, "  Entities\n" );
		for (i=0;i<to_render.entities.size();++i)
		{

			entity * e = to_render.entities[i].ent;
			host_fprintf( outfile, "    (%s) %s : %s\n",
				e->get_region()?e->get_region()->get_data()->get_name().c_str():
			"",entity_flavor_names[e->get_flavor()], e->get_id().get_val().c_str() );
		}

		host_fprintf( outfile, "\n" );


		host_fprintf( outfile, "Active entities\n" );
		std::vector<entity *>::const_iterator en;
		for (en = (get_active_entities()).begin(); en!=(get_active_entities()).end(); ++en)
		{
			entity * e  = (*en);
			host_fprintf( outfile, "  %s : %s \n", entity_flavor_names[e->get_flavor()],
				e->get_id().get_val().c_str());
		}

		host_fprintf( outfile, "\nActive animations\n" );

		pentity_anim_tree_vector::iterator ani;
		for ( ani=anims.begin(); ani!=anims.end(); ++ani )
		{
			if ( (*ani) )
				if( (!(*ani)->get_entity()->has_valid_sector()) || (*ani)->get_entity()->is_in_active_region() ||
					((*ani)->get_flags()&ANIM_COMPUTE_SECTOR))
					if (!(*ani)->is_suspended() && !(*ani)->is_finished() )
					{
						entity * e = (*ani)->get_entity();
						host_fprintf( outfile, "  Anim : %s \n",  e->get_id().get_val().c_str()); //(*ani)->get_name().c_str(),
					}
		}

		host_fprintf( outfile, "\n" );

#ifdef PROFILING_ON
		profiler::inst()->write_to_host_file(outfile);
#endif

		host_fclose( outfile );


		g_debug.dump_frame_info = false;

		dump_frame_info_counter = ~0;
	}
}
#endif

path_graph *world_dynamics_system::get_path_graph(stringx id)

{
	std::vector<path_graph *>::iterator i = path_graph_list.begin();
	while(i != path_graph_list.end())
	{

		if(*i != NULL && (*i)->id == id)
			return((*i));

		++i;
	}

	return(NULL);
}

void world_dynamics_system::add_path_graph(path_graph *pg)

{
	if(pg && get_path_graph(pg->id) == NULL)
		path_graph_list.push_back(pg);
}

void world_dynamics_system::remove_path_graph(path_graph *pg)

{
	std::vector<path_graph *>::iterator i = path_graph_list.begin();
	while(i != path_graph_list.end())
	{

		if(*i != NULL && *i == pg)
		{
			path_graph *pg_del = (*i);
			i = path_graph_list.erase(i);
			delete pg_del;
		}
		else
			++i;
	}
}

void world_dynamics_system::add_material_set( material_set *mset )
{
	material_set_list::iterator msi;
	for( msi = material_sets.begin(); msi != material_sets.end(); ++msi )
		if( *msi == mset )
			return;
		material_sets.push_back( mset );
}



void world_dynamics_system::add_material_set( const stringx& fname )

{
	PANIC;
}



void world_dynamics_system::delete_material_set( material_set *mset )

{
	material_set_list::iterator msi;
	for( msi = material_sets.begin(); msi != material_sets.end(); ++msi )
		if( *msi == mset ) break;
		if( msi != material_sets.end() ) material_sets.erase( msi );
}


void world_dynamics_system::delete_material_set( stringx& set_name )

{
	material_set_list::iterator msi;
	for( msi = material_sets.begin(); msi != material_sets.end(); ++msi )
		if( *((*msi)->name) == set_name ) break;

		if( msi != material_sets.end() ) material_sets.erase( msi );
}


material_set *world_dynamics_system::get_material_set( const stringx& set_name )
{
	material_set_list::iterator msi;
	for( msi = material_sets.begin(); msi != material_sets.end(); ++msi )
		if( *((*msi)->name) == set_name ) break;
		if( msi != material_sets.end() ) return *msi;
		return NULL;
}


extern profiler_timer proftimer_collide;
extern profiler_timer proftimer_collide_entity_entity;
extern profiler_timer proftimer_collide_entity_entity_int;

void world_dynamics_system::do_all_collisions(time_value_t t)
{
	proftimer_collide.start();

	// must go back and recalculate with NEW position
	// This code is only being done on characters, so why not just use the 'active_characters' list? (JDB 9/7/00)

	std::vector<entity *>::const_iterator ei = active_entities.begin();
	std::vector<entity *>::const_iterator ei_end = active_entities.end();
	for ( ; ei!=ei_end; ++ei )
	{
		if((*ei) && (*ei)->get_colgeom() && (*ei)->get_colgeom()->get_type()==collision_geometry::CAPSULE)
		{
			(*ei)->update_colgeom();

			if((*ei)->has_physical_ifc())
			{
				if(!(*ei)->playing_scene_anim() && (*ei)->physical_ifc()->is_enabled() && !(*ei)->physical_ifc()->is_suspended()/* && !(*ei)->is_hero()*/)
					(*ei)->physical_ifc()->manage_standing();

					/*

					if((*ei)->physical_ifc()->is_standing())
					{
					// cancel xz vel (manage standing used to do this when standing)
					vector3d vel = (*ei)->physical_ifc()->get_velocity();
					vel.x = vel.z = 0.0f;
					(*ei)->physical_ifc()->set_velocity(vel);
					}
					else

					{
					//          if((*ei)->physical_ifc()->get_collided_last_frame())
					//            (*ei)->physical_ifc()->set_standing(true);
					}
				*/

			}

		}
	}


	do_entity_to_entity_collisions(t);

	// make sure characters don't tunnel through the world
	do_entity_to_bsp_collisions(t);

	//*
	ei = active_entities.begin();
	for ( ; ei!=ei_end; ++ei )
	{
		if((*ei) && (*ei)->get_colgeom() && (*ei)->get_colgeom()->get_type()==collision_geometry::CAPSULE && (*ei)->has_physical_ifc() && !(*ei)->playing_scene_anim() && (*ei)->physical_ifc()->is_enabled() && !(*ei)->physical_ifc()->is_suspended())
			(*ei)->physical_ifc()->manage_standing((*ei)->physical_ifc()->is_effectively_standing());
	}
	//*/

	proftimer_collide.stop();
}



void world_dynamics_system::do_entity_to_bsp_collisions(time_value_t t)
{
	std::vector<entity *>::const_iterator ei = collision_entities.begin();
	std::vector<entity *>::const_iterator ei_end = collision_entities.end();
	for ( ; ei != ei_end; ++ei )

	{
		entity *ent = *ei;

		assert(ent && ent->is_active() && ent->are_collisions_active() && ent->has_entity_collision());
		//    if ( !ent ) continue;
		if ( !ent->has_physical_ifc() ) continue;
		//    if ( !ent->is_flagged(EFLAG_PHYSICS) ) continue;
		//    if ( !ent->is_active() ) continue;

		//    if ( !ent->are_collisions_active() ) continue;
		//    if ( !ent->has_entity_collision() ) continue;
		if ( ent->is_stationary() ) continue;

		vector3d pi, n;

		bool bad_collision = false;
		vector3d collision_delta = ZEROVEC;
		if ( ent->is_hero() )
		{
			// hero does a more rigorous check that includes collidable entities
			vector3d oldp = ent->get_last_position();
			vector3d newp = ent->get_abs_position();
			newp -= oldp;
			rational_t d = newp.length2();
			if ( d > 0.000001f )

			{

				d = __fsqrt( d );
				newp *= (d + ent->get_colgeom()->get_core_radius()) / d;

			}

			newp += oldp;
			bool coll_active = ent->are_collisions_active();

			ent->set_collisions_active(false, false);
			if ( find_intersection( oldp, newp,
				ent->get_primary_region(),
				FI_COLLIDE_WORLD|FI_COLLIDE_ENTITY,
				&pi, &n ) )

			{

				bad_collision = true;
				collision_delta = newp;
			}
			ent->set_collisions_active(coll_active, false);
		}
		else if ( get_the_terrain().find_intersection( ent->get_last_position(), ent->get_abs_position(), pi, n) )

		{
			collision_delta = ent->get_abs_position() - ent->get_last_position();
			collision_delta.normalize();
			bad_collision = true;
		}


		if ( bad_collision && n.length2() > 0.0f )
		{
			//      collision_delta
			po newpo = ent->get_abs_po();
			newpo.set_position( ent->get_last_position() );

			if(ent->has_parent())
				fast_po_mul(newpo, newpo, ent->link_ifc()->get_parent()->get_abs_po().inverse());


			ent->set_rel_po( newpo );
			if ( ent->has_physical_ifc() )
			{
				ent->physical_ifc()->set_velocity( ZEROVEC );
				ent->physical_ifc()->set_collided_last_frame( true );
				ent->physical_ifc()->set_last_collision_normal( n );

				ent->physical_ifc()->set_ext_collided_last_frame( true );
			}
		}
	}

}



//#define COLLISION_NAMES
#if defined(BUILD_DEBUG) && defined(COLLISION_NAMES)
stringx ent_i_name;
stringx ent_j_name;
#endif

void world_dynamics_system::do_entity_to_entity_collisions(time_value_t t)
{
	PANIC;
}

// *permanent:

#ifdef _DEBUG
stringx g_colla1, g_colla2;
bool g_stationary_a1, g_stationary_a2;
#endif


// shared data for the entity_entity_collision_check and the entity_entity_collision fn's
static bool terrain_collision = false;
static vector3d abs_base_point;


// does the checking portion of an ent/ent collision, but no resolving
bool world_dynamics_system::entity_entity_collision_check(entity * a1, entity * a2, time_value_t time, cface * hitFace)
{

	// following should be removed when fixed up. will slow things down badly.....
	terrain_collision = false;
	entity * b1, * b2;

	if (a1->is_active() && a1->is_sticky() && !a2->is_active())
	{
		terrain_collision = true;
	}

	po old_a1_rel_po = a1->get_rel_po();
	po old_a2_rel_po = a2->get_rel_po();

	entity * old_a1_parent = (entity *)a1->link_ifc()->get_parent();
	entity * old_a2_parent = (entity *)a2->link_ifc()->get_parent();

	// If either a1 or a2 is stationary, resolve the collision in the local space of that entity.

	// To facilitate this,  we create a fake entity for the non-stationary entity and mangle its data instead,
	// of corrupting the actual entity.

	b1 = a1;
	b2 = a2;

	bool character_collision_hack = false;

	if (!b1->is_stationary() && !b2->is_stationary())
	{
		// This flag will cause this routing to use the damage capsules rather than the normal collision
		// geometries for two colliding entities.  It assumes that any two such have valid damage_capsules,
		// i.e. are characters.  This causes them to have longer capsules (covering the
		// characters' feet) so that characters will better retard each other.
		character_collision_hack = true;
	}

	if (b1->is_stationary())
	{
		if ( b1->get_colgeom() )
		{
			// b1 has instanced collision geometry, so we need to set its owner to myself for now.
			b1->get_colgeom()->set_owner(b1);

			b1->get_colgeom()->validate();
			origin_entity->set_radius(b1->get_radius());

			// Now we move b2 into b1's local space.  We do this by creating a temporary entity with the
			// same collision_geometry, position and radius.  This gets around a sticky situation created
			// when we attached character's colgeom's to theiw waist instead of to the character itself.
			// (see character::get_colgeom_root_po())
			po b2_to_b1,last_b2_to_b1;
			fast_po_mul(b2_to_b1, b2->get_colgeom_root_po(), b1->get_abs_po().inverse());
			fast_po_mul(last_b2_to_b1, b2->get_last_po(), b1->get_abs_po().inverse());

			collision_dummy->set_rel_po(b2_to_b1);
			collision_dummy->set_last_po(last_b2_to_b1);
			collision_dummy->set_colgeom(b2->get_colgeom());
			collision_dummy->invalidate_colgeom();
			collision_dummy->set_radius(b2->get_radius());
			b2 = collision_dummy;
			b2->get_colgeom()->set_owner(b2);
			b2->invalidate_colgeom();
			b1->link_ifc()->clear_parent();
			b1->set_rel_po(po_identity_matrix);
		}
		else
		{
			nglPrintf("Crash averted but problem not fixed\n");
			return false;
		}
	}

	// mirror of above for b1, b2 role reversal...
	if (b2->is_stationary())
	{
		b2->get_colgeom()->set_owner(b2);
		b2->get_colgeom()->validate();
		origin_entity->set_radius(b2->get_radius());

		po b1_to_b2, last_b1_to_b2;
		fast_po_mul(b1_to_b2, b1->get_colgeom_root_po(), b2->get_abs_po().inverse());
		fast_po_mul(last_b1_to_b2, b1->get_last_po(), b2->get_abs_po().inverse());


		collision_dummy->set_rel_po(b1_to_b2);
		collision_dummy->set_last_po(last_b1_to_b2);

		collision_dummy->set_colgeom(b1->get_colgeom());
		collision_dummy->invalidate_colgeom();
		collision_dummy->set_radius(b1->get_radius());
		b1 = collision_dummy;
		b1->get_colgeom()->set_owner(b1);
		b1->invalidate_colgeom();
		b2->link_ifc()->clear_parent();
		b2->set_rel_po(po_identity_matrix);

	}

	rational_t radius1 = a1->get_colgeom()->get_core_radius(), radius2 = a2->get_colgeom()->get_core_radius();
	rational_t scale1 = 1.0f, scale2 = 1.0f;

	vector3d a1vel, a2vel;
	a1->get_velocity(&a1vel);
	if (time != 0.0f)
	{
		if ( a1->has_parent() && a1->is_frame_delta_valid() )
		{
			// adjust for non-physical motion (e.g., animation)
			a1vel += a1->get_frame_delta().get_position() * (1.0f / time);
		}
		rational_t delta = time * a1vel.length();
		if ( delta > 2.0f * radius1 )
			scale1 = (delta - radius1) / radius1;

		a2->get_velocity(&a2vel);

		if ( a2->has_parent() && a2->is_frame_delta_valid() )

		{
			// adjust for non-physical motion (e.g., animation)
			a2vel += a2->get_frame_delta().get_position() * (1.0f / time);
		}
		delta = time * a2vel.length();
		if ( delta > 2.0f * radius2 )
			scale2 = (delta - radius2) / radius2;
	}

	if ( character_collision_hack )
	{
		scale1 = a1->get_inter_capsule_radius_scale();
		scale2 = a2->get_inter_capsule_radius_scale();
	}

#if defined(VERBOSE_COLLISIONS)
	debug_print("cg1: scale: " + ftos(scale1) );
#endif
	collision_geometry *cg1 = b1->get_updated_colgeom( NULL, scale1 );
#if defined(VERBOSE_COLLISIONS)
	debug_print("cg2: scale: " + ftos(scale2) );
#endif
	collision_geometry *cg2 = b2->get_updated_colgeom( NULL, scale2 );

	unsigned int col_flags = 0;
	if ( !b2->is_stationary() )
		col_flags |= PP_FULL_MESH;

	if ( terrain_collision )
		col_flags |= ONE_HIT_PER_M2_POLY | PP_REAR_CULL;


	// We aren't using the massive list of collision vectors - this flag
	//   turns the check into a boolean check.
	col_flags |= ONE_HIT_PER_MESH;

	bool hit = (collision_geometry::collides(cg1,cg2,&hit_list,&normal_list1,&normal_list2,col_flags,a1vel-a2vel, hitFace));

	/*
	b1->get_updated_colgeom( NULL, (1.0f / scale1) );
	b2->get_updated_colgeom( NULL, (1.0f / scale2) );
	*/
	/*
	// default to ent 1's collision capsule (if both are non stationary)
	if (a1->get_colgeom() && a1->get_colgeom()->get_type()==collision_geometry::CAPSULE)
    abs_base_point = ((collision_capsule *)a1->get_colgeom())->get_base();
	else if (a2->get_colgeom() && a2->get_colgeom()->get_type()==collision_geometry::CAPSULE)
    abs_base_point = ((collision_capsule *)a2->get_colgeom())->get_base();
	else
    assert(0);
	*/

	// Restore positions afterwards...
	if (a2->is_stationary())
	{
		a2->link_ifc()->set_parent(old_a2_parent);

		a2->set_rel_po(old_a2_rel_po);
		a1->get_colgeom()->set_owner(a1);
		a1->invalidate_colgeom();
		po last_po = a1->get_last_po();
		a1->get_updated_colgeom( &last_po, scale1 );
		abs_base_point = ((collision_capsule *)a1->get_colgeom())->get_base();

		a1->invalidate_colgeom();
		last_po = a1->get_colgeom_root_po();
		a1->get_updated_colgeom( &last_po, scale1 );
	}


	if (a1->is_stationary())
	{
		a1->link_ifc()->set_parent(old_a1_parent);
		a1->set_rel_po(old_a1_rel_po);
		a2->get_colgeom()->set_owner(a2);
		a2->invalidate_colgeom();
		po last_po = a2->get_last_po();
		a2->get_updated_colgeom( &last_po, scale2 );
		abs_base_point = ((collision_capsule *)a2->get_colgeom())->get_base();
		a2->invalidate_colgeom();
		last_po = a2->get_colgeom_root_po();
		a2->get_updated_colgeom( &last_po, scale2 );
	}

	collision_dummy->set_colgeom(NULL);

	// This list seemed to never get cleared before, and eventually it started eating up memory.
	// Hope this is the right place! (dc 01/28/02)
	hit_list.resize(0);
	normal_list1.resize(0);
	normal_list2.resize(0);

	return hit;
}


// does the resolving portion of an ent/ent collision and calls the check fn (above) to do the
// checking itself
bool world_dynamics_system::entity_entity_collision(entity * a1, entity * a2, time_value_t time)
{
	proftimer_collide_entity_entity_int.start();

	bool hit = entity_entity_collision_check(a1, a2, time);

	//  if (collision_geometry::collides(cg1,cg2,&hit_list,&normal_list1,&normal_list2,terrain_collision?ONE_HIT_PER_M2_POLY|PP_REAR_CULL:0,a1->get_velocity()-a2->get_velocity()))
	if (hit)
	{
		vector3d old_a1_pos = a1->get_last_position();
		vector3d old_a2_pos = a2->get_last_position();
		// Treat collisions with static objects like terrain collisions
#if defined(VERBOSE_COLLISIONS)
		debug_print("collision detected: " + v3tos(a1->get_abs_position()) );
#endif
		if ((a1->is_active()==false || a1->get_flavor()==ENTITY_ENTITY || a1->is_stationary()) &&
			a1->get_colgeom()->get_type()==collision_geometry::MESH)
		{
			for (unsigned i=0;i<normal_list1.size();++i)
			{
				vector3d swap = normal_list2[i];
				normal_list2[i] = normal_list1[i];
				normal_list1[i] = swap;
			}


			if(a1->is_stationary())
				hit = physical_interface::resolve_collision_with_terrain(a2, time, a1->get_abs_po(), abs_base_point);
			else

				hit = physical_interface::resolve_collision_with_terrain(a2, time, po_identity_matrix, abs_base_point);
		}
		else if ((a2->is_active()==false || a2->get_flavor()==ENTITY_ENTITY || a2->is_stationary()) &&
			a2->get_colgeom()->get_type()==collision_geometry::MESH)
		{
			if(a2->is_stationary())
				hit = physical_interface::resolve_collision_with_terrain(a1, time, a2->get_abs_po(), abs_base_point);
			else
				hit = physical_interface::resolve_collision_with_terrain(a1, time, po_identity_matrix, abs_base_point);
		}
		else
			physical_interface::resolve_collision(a1, a2, time, terrain_collision);
		//      debug_print( "after collision response: " + v3tos(a1->get_abs_position()) );


		vector3d dir = a2->get_abs_position() - a1->get_abs_position();
		dir.normalize();


		if(a1->has_physical_ifc())
		{

			a1->physical_ifc()->set_collided_last_frame(true);
			a1->physical_ifc()->set_ext_collided_last_frame( true );
			a1->physical_ifc()->set_last_collision_normal( -dir );
		}


		//*
		if(a1->is_frame_delta_valid())
		{
			vector3d delta = (a1->get_abs_position() - old_a1_pos);
			a1->get_movement_info()->frame_delta.set_position(delta);
		}
		//*/
		if(a2->has_physical_ifc())
		{
			a2->physical_ifc()->set_collided_last_frame(true);
			a2->physical_ifc()->set_ext_collided_last_frame( true );
			a2->physical_ifc()->set_last_collision_normal( dir );
		}
		//*
		if(a2->is_frame_delta_valid())
		{
			vector3d delta = (a2->get_abs_position() - old_a2_pos);
			a2->get_movement_info()->frame_delta.set_position(delta);
		}
		//*/


		hit_list.resize(0);
		normal_list1.resize(0);
		normal_list2.resize(0);
	}

	proftimer_collide_entity_entity_int.stop();
	return hit;
}



// add given entity to the list of entities that are guaranteed to be active
// independent of visibility, region status, etc.
void world_dynamics_system::guarantee_active( entity* e )
{

	std::vector<entity*>::iterator i = std::find( guaranteed_active_entities.begin(), guaranteed_active_entities.end(), e );
	if ( i == guaranteed_active_entities.end() )
	{
		std::vector<entity*>::iterator i = std::find( guaranteed_active_entities.begin(), guaranteed_active_entities.end(), (entity*)NULL );
		if ( i == guaranteed_active_entities.end() )
		{

			guaranteed_active_entities.push_back( e );
		}
		else
			*i = e;
	}
}



///////////////////////////////////////////////////////////////////////////////
// NEWENT File I/O
///////////////////////////////////////////////////////////////////////////////

// TODO: check all NEWENT stuff against g_entity_maker->create_entity_or_subclass()



light_source*
world_dynamics_system::add_light_source( chunk_file& fs,
										const entity_id& id,
										unsigned int flags )
{
	light_source* ent = NEW light_source( fs, id, ENTITY_LIGHT_SOURCE, flags );
	add_light_source( ent );
	return ent;
}


particle_generator*
world_dynamics_system::add_particle_generator( const stringx& fname,
											  const entity_id& id,
											  unsigned int flags )
{
	PANIC;
	return NULL;
}


item*
world_dynamics_system::add_item( chunk_file& fs,
								const entity_id& id,
								unsigned int flags )
{
	item* ent = NEW item( fs, id, ENTITY_ITEM, flags );
	add_item( ent );
	return ent;
}

polytube*
world_dynamics_system::add_polytube( chunk_file& fs,
                                    const entity_id& id,
                                    unsigned int flags )
{
	polytube* ent = NEW polytube( fs, id, ENTITY_POLYTUBE, flags );
	add_to_entities( ent );
	return ent;
}

lensflare*
world_dynamics_system::add_lensflare( chunk_file& fs,

									 const entity_id& id,
									 unsigned int flags )
{
	lensflare* ent = NEW lensflare( fs, id, ENTITY_LENSFLARE, flags );
	add_to_entities( ent );
	return ent;
}

#if 0 //BIGCULL
gun*
world_dynamics_system::add_gun( chunk_file& fs,

							   const entity_id& id,
							   unsigned int flags )
{
	gun* ent = NEW gun( fs, id, ENTITY_ITEM, flags );

	add_item( ent );
	return ent;
}

melee_item*
world_dynamics_system::add_melee( chunk_file& fs,
								 const entity_id& id,
								 unsigned int flags )
{
	melee_item* ent = NEW melee_item( fs, id, ENTITY_ITEM, flags );

	add_item( ent );
	return ent;
}

thrown_item*
world_dynamics_system::add_thrown_item( chunk_file& fs,
									   const entity_id& id,
									   unsigned int flags )
{

	thrown_item* ent = NEW thrown_item( fs, id, ENTITY_ITEM, flags );

	add_item( ent );
	return ent;
}

manip_obj*
world_dynamics_system::add_manip_obj( chunk_file& fs,
									 const entity_id& id,
									 unsigned int flags )
{

	manip_obj* ent = NEW manip_obj( fs, id, ENTITY_MANIP, flags );
	add_to_entities( ent );
	return ent;
}

switch_obj*
world_dynamics_system::add_switch_obj( chunk_file& fs,
									  const entity_id& id,
									  unsigned int flags )
{
	switch_obj* ent = NEW switch_obj( fs, id, ENTITY_SWITCH, flags );
	add_to_entities( ent );
	return ent;
}

turret*
world_dynamics_system::add_turret( chunk_file& fs,
								  const entity_id& id,
								  unsigned int flags )
{
	turret* ent = NEW turret( fs, id, ENTITY_TURRET, flags );

	add_turret( ent );


	return ent;
}

scanner* world_dynamics_system::add_scanner( chunk_file& fs,
											const entity_id& id,
											unsigned int flags )
{
	scanner* e = NEW scanner( fs, id, ENTITY_SCANNER, flags );

	add_scanner( e );

	return e;
}

void world_dynamics_system::add_scanner( scanner* e )
{
	g_entity_maker->create_entity( e );
}

#endif //BIGCULL
sky* world_dynamics_system::add_sky( chunk_file& fs, const entity_id& id, unsigned int flags )
{
	sky* ent = NEW sky( fs, id, ENTITY_SKY, flags );

	ent->set_flag( EFLAG_MISC_NONSTATIC, true );

	add_sky( ent );
	return ent;
}

void world_dynamics_system::add_sky( sky *e )
{
	g_entity_maker->create_entity( e );
}

// ok, I'm cleaning this portal stuff up.  Things are getting a lot simpler around here.

// Portals, once loaded, are always converted into convex quads in a counterclockwise

// orientation with the top/bottom edges aligned with the XZ plane and the side edges
// essentially vertical.
// This is so we won't have lots of vertices to deal with later on.
// If the original portal was non-planar, we will use artificially inflate the portal quad.

// Now that we have nice simple portals, all we do when we check portals for visibility
// is this:


//  A) Check portal bounding sphere against frustum planes.  If completely outside, skip it
//  B) IF   the portal cylinder touches the near plane rectangle
//     THEN consider portal too complicated to clip the frustum properly at this time

//          so we just leave the frustum as-is
//     ELSE we can build a screen-space bounding rectangle from the portal mesh
//          and use that to clip down the view frustum,
//  C) if view frustum is nonempty, recurse into the portal dest region


//  --Sean

#ifdef DEBUG
extern bool dump_portals_to_console;

#endif

#include <algorithm>

void world_dynamics_system::_build_render_data_regions( render_data& rd, region_node* rn, const rectf& screen_rect, camera& camera_link )

{
	PANIC;
}




int g_rendered_actors_last_frame = 0;

#define _JDB_ENHANCED_LOD_METHOD 1


void world_dynamics_system::_build_render_data_ents( render_data& rd )

{
	const matrix4x4& world2view = geometry_manager::inst()->xforms[ geometry_manager::XFORM_WORLD_TO_VIEW ];
	po view2world(world2view);
	view2world.invert();


	const po& campo = app::inst()->get_game()->get_current_view_camera()->get_abs_po();   // Changed from get_current_game_cam()
	// because I'm eliminating that variable, not used in KSPS. 10/22/01 -DL

	rd.cam = campo.get_position();


	render_data::region_list::iterator ri;

	render_data::region_list::iterator ri_end = rd.regions.end();
	for (ri=rd.regions.begin(); ri!=ri_end; ++ri)
	{
		region_node* rn = (*ri).reg;

		_determine_visible_entities(rd, rn, campo);
	}


	g_rendered_actors_last_frame = 0;
}


extern profiler_timer proftimer_det_vis_ents;


void world_dynamics_system::_determine_visible_entities( render_data& rd, region_node* rn,
														po const & campo)

{
	PANIC;
}





// teleport hero to next malor marker (if any)
void world_dynamics_system::malor_next()
{
	// find next malor marker

	int n = entities.size();
	unsigned idx = current_malor_marker;

	while (n)
	{
		--n;
		++idx;
		if (idx == entities.size())
			idx = 0;
		if (entities[idx])
			if (stringx("MALOR")==entities[idx]->get_id().get_val().substr(0,5))
			{
				current_malor_marker = idx;
				break;
			}
	}
	// perform malor
	if (current_malor_marker != -1)
	{
		if (get_hero_ptr(g_game_ptr->get_active_player()))
		{

#if _CONSOLE_ENABLE
			console_log("Maloring to: %s", entities[current_malor_marker]->get_name().c_str());

#endif

			po the_po = get_hero_ptr(g_game_ptr->get_active_player())->get_abs_po();
			the_po.set_position(entities[current_malor_marker]->get_abs_position());
			if(get_hero_ptr(g_game_ptr->get_active_player())->has_parent())
				fast_po_mul(the_po, the_po, get_hero_ptr(g_game_ptr->get_active_player())->link_ifc()->get_parent()->get_abs_po().inverse());
			get_hero_ptr(g_game_ptr->get_active_player())->set_rel_po(the_po);


			// BIGCULL chase_cam_ptr->set_rel_position(hero_ptr->get_abs_po().fast_8byte_xform(vector3d(0,1,-2)));
			// BIGCULL chase_cam_ptr->invalidate();
		}
	}
}

// teleport hero to next malor marker (if any)
void world_dynamics_system::malor_prev()
{
	// find next malor marker
	int n = entities.size();
	int idx = current_malor_marker;
	while (n)

	{
		--n;
		--idx;
		if (idx < 0)
			idx = entities.size()-1;

		if (entities[idx])
			if (stringx("MALOR")==entities[idx]->get_id().get_val().substr(0,5))

			{
				current_malor_marker = idx;
				break;
			}
	}

	// perform malor
	if (current_malor_marker != -1)
	{
		if (get_hero_ptr(g_game_ptr->get_active_player()))

		{
#if _CONSOLE_ENABLE
			console_log("Maloring to: %s", entities[current_malor_marker]->get_name().c_str());
#endif

			po the_po = get_hero_ptr(g_game_ptr->get_active_player())->get_abs_po();
			the_po.set_position(entities[current_malor_marker]->get_abs_position());
			if(get_hero_ptr(g_game_ptr->get_active_player())->has_parent())
				fast_po_mul(the_po, the_po, get_hero_ptr(g_game_ptr->get_active_player())->link_ifc()->get_parent()->get_abs_po().inverse());
			get_hero_ptr(g_game_ptr->get_active_player())->set_rel_po(the_po);

			// BIGCULL chase_cam_ptr->set_rel_position(hero_ptr->get_abs_po().fast_8byte_xform(vector3d(0,1,-2)));

			// BIGCULL chase_cam_ptr->invalidate();
		}

	}
}

bool world_dynamics_system::malor_to(stringx point)
{

	// find next malor marker
	std::vector<entity*>::iterator i = entities.begin();
	std::vector<entity*>::iterator i_end = entities.end();

	int old = current_malor_marker;

	for( current_malor_marker = 0; i != i_end; ++i, ++current_malor_marker)
	{
		entity *ent = (*i);

		if (ent && stringx("MALOR") == ent->get_name().substr(0,5) && (point == ent->get_name() || (stringx("MALOR_")+point) == ent->get_name() || (stringx("MALOR")+point) == ent->get_name()))
		{
			if (get_hero_ptr(g_game_ptr->get_active_player()))
			{
#if _CONSOLE_ENABLE
				console_log("Maloring to: %s", ent->get_name().c_str());
#endif

				po the_po = get_hero_ptr(g_game_ptr->get_active_player())->get_abs_po();
				the_po.set_position(ent->get_abs_position());
				if(get_hero_ptr(g_game_ptr->get_active_player())->has_parent())
					fast_po_mul(the_po, the_po, get_hero_ptr(g_game_ptr->get_active_player())->link_ifc()->get_parent()->get_abs_po().inverse());
				get_hero_ptr(g_game_ptr->get_active_player())->set_rel_po(the_po);


				// BIGCULL chase_cam_ptr->set_rel_position(hero_ptr->get_abs_po().fast_8byte_xform(vector3d(0,1,-2)));
				// BIGCULL chase_cam_ptr->invalidate();

				return true;
			}
		}
	}

	current_malor_marker = old;


	return(false);
}


void world_dynamics_system::apply_radius_damage(vector3d center, rational_t radius, int bio_damage, int mechanical_damage)
{
	error ("Apply damage radius not supported in KS.");
#if 0 //BIGCULL
	if(bio_damage != 0 || mechanical_damage != 0)

	{
		// apply the damage to everyone nearby.
		vector<entity*> ents = get_entities();
		vector<entity*>::const_iterator i = ents.begin();
		vector<entity*>::const_iterator i_end = ents.end();

		for ( ; i<i_end; ++i )
		{
			entity* e = *i;

			if ( e && (e->is_hero() || (e->is_visible() && e->allow_targeting())) )
			{
				vector3d vec = center - e->get_abs_position();
				rational_t len = vec.length();

				if(len <= radius)
				{
					switch ( e->get_target_type() )
					{
					case TARGET_TYPE_BIO:
						if(bio_damage != 0)
							e->apply_damage( bio_damage, center, vec, DAMAGE_EXPLOSIVE, NULL );

						break;

					case TARGET_TYPE_MECHANICAL:
						if(mechanical_damage != 0)
							e->apply_damage( mechanical_damage, center, vec, DAMAGE_EXPLOSIVE, NULL );
						break;


					default:
						assert(0);
						break;
					}
				}
			}
		}
	}

#endif //BIGCULL
}



void world_dynamics_system::reload_particle_generators()
{
	PANIC;
}

/*
vm_thread* entity::spawn_entity_script_function( const stringx& function_name ) const

{
vm_thread* nt = NULL;

  if ( function_name.length() > 0 )
  {
  stringx actual_name = function_name + "(entity)";
  actual_name.to_lower();

    script_object* so = g_world_ptr->get_current_level_global_script_object();
    script_object::instance* inst = g_world_ptr->get_current_level_global_script_object_instance();

	  if ( so!=NULL && inst!=NULL )
	  {
      int fidx = so->find_func( actual_name );
      if ( fidx >= 0 )
      {
	  nt = inst->add_thread( &so->get_func(fidx) );

	  // push entity parameter
	  const entity* e = this;
	  nt->get_data_stack().push( (char*)&e, 4 );
      }
      else
	  warning( get_name() + ": script function '" + actual_name + "' not found" );
	  }
	  }

		return nt;
		}


		  void entity::exec_preload_function(const stringx &preload_func)
		  {
		  if(preload_func.length() > 0)
		  {
		  // there might be a script function for preloading additional assets needed by item
		  stringx preload_func_name = preload_func + "()";
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
*/

// @Ok
// @AlmostMatching - can't get the preload pair thing to inline
void world_dynamics_system::add_entity_preload_script(entity *e, const stringx &entity_name)
{
	switch(e->get_flavor())
	{

		case ENTITY_CAMERA:

		case ENTITY_MARKER:

		case ENTITY_MIC:
		case ENTITY_LIGHT_SOURCE:
		case ENTITY_PARTICLE_GENERATOR:
		case ENTITY_LIGHT:
		//    case ENTITY_BEAM:
		//    case ENTITY_SCANNER:
		case ENTITY_BOX_TRIGGER:
		break;

		default:
		{
			stringx name = entity_name;
			name.to_lower();
			filespec spec(name);

			entity_preloads.push_back(entity_preload_pair(e, spec.name));
		}
		break;

	}
}

// this function is called immediately after a level load, to run any required
// item preload scripts
void world_dynamics_system::spawn_misc_preload_scripts()
{
	PANIC;
}



rational_t world_dynamics_system::compute_total_energy()
{
	rational_t total_kinetic_energy = 0;

	int entities_size = entities.size();
	for (int i=0;i<entities_size;++i)
	{
		if ( entities[i] && entities[i]->has_physical_ifc() && !entities[i]->is_stationary() )
			total_kinetic_energy += entities[i]->physical_ifc()->compute_energy();
	}
	return total_kinetic_energy;
}



// @Ok
// @Matching
bool world_dynamics_system::is_entity_valid(entity *ent)
{
	std::vector<entity*>::iterator it;
	it = std::find( entities.begin(), entities.end(), ent );

	return( it != entities.end() );
}


scene_anim_handle_t world_dynamics_system::play_scene_anim( const stringx &filename, bool reverse, float start_time )
{
	scene_anim_map_t::iterator si = scene_anim_map.find( filename );

	if( si != scene_anim_map.end() )
	{
		last_snm_handle++;

		(*si).second->play( scene_anims, last_snm_handle, reverse, start_time );
	}
	else
		error( "world_dynamics_system::play_scene_anim() : Invalid filename" );

	return last_snm_handle;
}

float world_dynamics_system::get_scene_anim_time(scene_anim_handle_t handle)
{
	scene_anim_list_t::iterator i = scene_anims.begin();

	for ( ; i!=scene_anims.end(); i++ )
	{
		if ( (*i).handle == handle && (*i).ent != NULL)
		{
			entity_anim_tree *tree = (*i).ent->get_anim_tree(ANIM_SCENE);
			return tree->get_time();
		}
	}

	return -1;
}

entity_anim_tree* world_dynamics_system::get_scene_anim_tree(scene_anim_handle_t handle)

{
	scene_anim_list_t::iterator i = scene_anims.begin();


	for ( ; i!=scene_anims.end(); i++ )
	{
		if ( (*i).handle == handle && (*i).ent != NULL)

			return (*i).ent->get_anim_tree(ANIM_SCENE);
	}
	return NULL;
}

void world_dynamics_system::kill_scene_anim( scene_anim_handle_t handle )
{
	scene_anim_list_t::iterator i = scene_anims.begin();

	for ( ; i!=scene_anims.end(); ++i )
	{
		if ( (*i).handle == handle && (*i).ent != NULL)
		{
			entity_anim_tree *tree = (*i).ent->get_anim_tree(ANIM_SCENE);
			tree->set_time(tree->get_duration() - 0.10f);
			//      tree->frame_advance(0.0f);
		}
	}
}

// @Ok
// @Matching
bool world_dynamics_system::is_scene_anim_playing(void) const
{
	// @TODO - fix with proper type
	bool* tmp = reinterpret_cast<bool*>(input_mgr::inst());
	return tmp[0x24];
}

// @Ok
// @Matching
void world_dynamics_system::add_region_ambient_sound( stringx &id, stringx &id2, rational_t volume )
{
	region_node* fr = the_terrain->find_region( id );
	if( fr )
	{
		(*fr).get_data()->set_region_ambient_sound( id2 );
		(*fr).get_data()->set_region_ambient_sound_volume( volume );

	}
}


void world_dynamics_system::add_crawl_box( int type, bool forced, const vector3d& where, const convex_box& binfo )
{
	crawl_box* crawl = NEW crawl_box( type, forced, where, binfo );

	// wds 'owns' all crawl boxes for memory tracking purposes
	crawl_boxes.push_back( crawl );

	//#pragma fixme( "crawl box has volume, so it can straddle regions. -mkv 4/6/01" )
	sector* sector = the_terrain->find_sector( where );
	region_node* rnode = sector != NULL ? sector->get_region() : NULL;
	region* reg = rnode != NULL ? rnode->get_data() : NULL;

	if(reg != NULL)
		reg->add( crawl );
	else
		warning("Crawl box placed outside world at <%.3f, %.3f, %.3f>", where.x, where.y, where.z);
}


// @Ok
// @Matching
void world_dynamics_system::recompute_all_sectors() // calls compute_sector for each entity
{
	for (
		entity_list::iterator i = entities.begin();
		i != entities.end();
		++i)
	{
		// @Patch - removed null check

		(*i)->last_compute_sector_position_hash*=2.0f; // should make this a member of entity, such as invalidate_last_sector_position_hash()
		(*i)->compute_sector(*the_terrain,true);
	}

}



// @Ok
// @Matching
bool world_dynamics_system::is_loading_from_scn_file() const
{
	return loading_from_scn_file;
}

// @Ok
// @Matching
void world_dynamics_system::set_fog_color(color f_color)
{
	this->fog_color = f_color;
	nglSetFogColor(f_color.r, f_color.g, f_color.b);
}

// @Ok
// @Matching
void world_dynamics_system::set_fog_range(rational_t f_min, rational_t f_max)
{
	this->fog_min = f_min;
	this->fog_max = f_max;

	nglSetFogRange(this->fog_near, this->fog_far, this->fog_min, this->fog_max);
}

// @Ok
// @Matching
void world_dynamics_system::set_global_time_dilation(rational_t a2)
{
	if (this->field_3F0 == this->field_3F4)
	{
		this->time_dilation = a2;
		return;
	}

	error("Cannot set time dilation during a scene anim.");
}

// @Ok
// @Matching
rational_t get_level_time(void)
{
	return GET_LEVEL_TIME;
}

#include "my_assertions.h"

void validate_wds(void)
{
	VALIDATE_SIZE(world_dynamics_system, 0x444);

	VALIDATE(world_dynamics_system, generators, 0x20);
	VALIDATE(world_dynamics_system, fcs_list, 0x2C);
	VALIDATE(world_dynamics_system, mcs_list, 0x38);
	VALIDATE(world_dynamics_system, controllers, 0x44);

	VALIDATE(world_dynamics_system, anims, 0x6C);

	VALIDATE(world_dynamics_system, entities, 0x78);

	VALIDATE(world_dynamics_system, lights, 0xC8);

	VALIDATE(world_dynamics_system, time_limited_entities, 0x10C);

	VALIDATE(world_dynamics_system, the_terrain, 0x124);

	VALIDATE(world_dynamics_system, loading_from_scn_file, 0x1B0);


	VALIDATE(world_dynamics_system, marky_cam, 0x1E4);
	VALIDATE(world_dynamics_system, marky_cam_enabled, 0x1E8);

	VALIDATE(world_dynamics_system, field_3F0, 0x3F0);
	VALIDATE(world_dynamics_system, field_3F4, 0x3F4);

	VALIDATE(world_dynamics_system, entity_preloads, 0x408);

	VALIDATE(world_dynamics_system, fog_color, 0x420);

	VALIDATE(world_dynamics_system, fog_near, 0x430);
	VALIDATE(world_dynamics_system, fog_far , 0x434);
	VALIDATE(world_dynamics_system, fog_min , 0x438);
	VALIDATE(world_dynamics_system, fog_max , 0x43C);

	VALIDATE(world_dynamics_system, time_dilation, 0x440);
	
	VALIDATE_VAL(0x81, ACTIVE_FLAG|NONSTATIC_FLAG);
}

void validate_entity_preload_pair(void)
{
	VALIDATE_SIZE(world_dynamics_system::entity_preload_pair, 0xC);

	VALIDATE(world_dynamics_system::entity_preload_pair, ent, 0x0);
	VALIDATE(world_dynamics_system::entity_preload_pair, name, 0x4);
}

void validate_ent_time_limit(void)
{
	VALIDATE_SIZE(world_dynamics_system::ent_time_limit, 0x8);

	VALIDATE(world_dynamics_system::ent_time_limit, ent, 0x0);
	VALIDATE(world_dynamics_system::ent_time_limit, duration, 0x4);
}


#include "my_patch.h"

void patch_entity_preload_pair(void)
{
}

void patch_wds(void)
{
	PATCH_PUSH_RET(0x00637A20, world_dynamics_system::set_global_time_dilation);
	PATCH_PUSH_RET(0x00637A60, get_level_time);

	PATCH_PUSH_RET(0x006379D0, world_dynamics_system::set_fog_range);
	PATCH_PUSH_RET(0x006378C0, world_dynamics_system::set_fog_color);

	PATCH_PUSH_RET(0x00636E10, world_dynamics_system::is_loading_from_scn_file);
	PATCH_PUSH_RET(0x00636DC0, world_dynamics_system::recompute_all_sectors);

	PATCH_PUSH_RET(0x00636A50, world_dynamics_system::add_region_ambient_sound);

	PATCH_PUSH_RET(0x00636A30, world_dynamics_system::is_scene_anim_playing);
	PATCH_PUSH_RET_POLY(0x00636750, world_dynamics_system::is_entity_valid, "?is_entity_valid@world_dynamics_system@@QAE_NPAVentity@@@Z");
	PATCH_PUSH_RET(0x00636150, world_dynamics_system::add_entity_preload_script);

	PATCH_PUSH_RET(0x0062BBA0, world_dynamics_system::add_to_entities);

	PATCH_PUSH_RET(0x0062BB10, world_dynamics_system::enable_marky_cam);

	PATCH_PUSH_RET(0x0062B9F0, world_dynamics_system::add_generator);
	PATCH_PUSH_RET(0x0062B670, world_dynamics_system::add_controller);
	PATCH_PUSH_RET(0x0062B7B0, world_dynamics_system::add_mcs);
	PATCH_PUSH_RET(0x0062B8D0, world_dynamics_system::add_fcs);

	PATCH_PUSH_RET(0x0062B420, world_dynamics_system::kill_anim);
	PATCH_PUSH_RET(0x0062B2B0, world_dynamics_system::add_anim);
	PATCH_PUSH_RET(0x0062B120, world_dynamics_system::make_time_limited);
	PATCH_PUSH_RET(0x0062AF10, world_dynamics_system::add_time_limited_effect);

	PATCH_PUSH_RET_POLY(0x0062AEF0, world_dynamics_system::add_turret, "?add_turret@world_dynamics_system@@QAEXPAVturret@@@Z");
	PATCH_PUSH_RET(0x0062ACD0, world_dynamics_system::add_neolight);
	PATCH_PUSH_RET_POLY(0x0062ACB0, world_dynamics_system::add_lensflare, "?add_lensflare@world_dynamics_system@@QAEXPAVlensflare@@@Z");
	PATCH_PUSH_RET_POLY(0x0062AC90, world_dynamics_system::add_particle_generator, "?add_particle_generator@world_dynamics_system@@QAEXPAVparticle_generator@@@Z");
	PATCH_PUSH_RET_POLY(0x0062AC10, world_dynamics_system::add_beam, "?add_beam@world_dynamics_system@@QAEXPAVbeam@@@Z");
	PATCH_PUSH_RET_POLY(0x0062ABF0, world_dynamics_system::add_marker, "?add_marker@world_dynamics_system@@QAEXPAVmarker@@@Z");

	PATCH_PUSH_RET(0x0062AB60, world_dynamics_system::remove_light_source);
	PATCH_PUSH_RET(0x0062A9F0, world_dynamics_system::add_light_source, "?add_light_source@world_dynamics_system@@QAEXPAVlight_source@@@Z");
}
