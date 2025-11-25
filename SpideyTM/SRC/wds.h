#ifndef WDS_H
#define WDS_H

#include "warnlvl.h"
#include "graph.h"

#include "generator.h"
#include "entity.h"
#include "script_object.h"
#include "conglom.h"
#include "entity_anim.h"
#include "rect.h"
#include "camera.h"
#include "project.h"
#include "scene_anim.h"
#include "path.h"
#include "capsule.h"
#include "render_data.h"
#include "box_trigger_interface.h"
#include "aggvertbuf.h"
#include "file.h"
#include "region_graph.h"
#include "game.h"
#include "inputmgr.h"

#include <vector>


// @Patch
#include "entity_maker.h"

// @TODO - is this needed?
#define MAX_PLAYERS 2


//#include "kellyslater_controller.h"

/*
#include "LipSync_Anim.h"
class LipSyncMngr;
class LipSyncData;
*/

#include <set>

class crawl_box;
class particle_generator;
class light_properties;
class light_source;

class force_generator;
class force_control_system;
class motion_control_system;
class controller;
class item;
class portal;
class terrain;
class region;
class marker;
class mic;
class beam;
class game_camera;
class marky_camera;
class neolight;
class turret;
class variant_descriptor;
//class dread_net;
class render_data;
class scanner;
class sky;
class gun;
class thrown_item;
class melee_item;
class manip_obj;
class switch_obj;
class polytube;
class lensflare;
// BIGCULL class ai_cue_manager;
class ai_polypath;
//class sound_group;

//typedef graph<stringx,region*,portal*> region_graph;
//typedef region_graph::node region_node;


////////////////////////////////////////////////////////////////////////////////
//  forward class declarations
////////////////////////////////////////////////////////////////////////////////
class camera;
class replay_camera;

class theta_and_psi_mcs;

typedef std::map<stringx,entity*> entfile_map;


// I wrote this simply to get around a Metrowerks compiler bug  agh!
class add_physent_struct
{
  public:
	  int size;

		bool active;
		bool stationary;
		bool invisible;
		bool cosmetic;
		bool walkable;

		bool repulsion;
	  entity_id id;
	  stringx* entity_name;
};


// generate error message if entity has invalid po
void check_po( entity* e );


#if 1 //ndef __MSL_STL
typedef std::vector<entity_anim_tree*> pentity_anim_tree_vector;
#else


typedef simplestaticallocator<entity_anim_tree *> pentity_anim_tree_allocator;
typedef std::vector<entity_anim_tree*,pentity_anim_tree_allocator> pentity_anim_tree_vector;
#endif


//typedef vector<entity_anim_tree*> anim_list;
////////////////////////////////////////////////////////////////////////////////
class world_dynamics_system
{
	friend void validate_wds(void);
	friend void validate_entity_preload_pair(void);
	friend void patch_wds(void);
	friend void validate_ent_time_limit(void);

  public:
    friend class view_frustrum_projection;
    friend class KSReplay;

	typedef std::vector<entity*> entity_list;
    typedef std::vector<entity*> entity_archetype_list;
    //typedef vector<entity_anim_tree*> anim_list;
    typedef std::vector<material_set*> material_set_list;
    typedef std::vector<crawl_box*> crawl_box_list;

    EXPORT world_dynamics_system();
    EXPORT ~world_dynamics_system();

    EXPORT int get_num_generators() const { return generators.size(); }
    EXPORT force_generator * get_generator(int i) const { return generators[i]; }

    EXPORT int get_num_entities() const { return entities.size(); }
    EXPORT bool is_entity_valid(int i) const { return entities[i]!=NULL; }
    EXPORT bool is_entity_valid(entity *ent);

	EXPORT replay_camera* get_replay_cam_ptr(void) { return replay_cam_ptr; }

	EXPORT entity* get_usercam_ptr() { return usercam; }

	EXPORT void start_usercam();

    EXPORT int get_num_path_graphs() const { return path_graph_list.size(); }
    EXPORT path_graph *get_path_graph(int i) const { assert(i >= 0 && i < (int)path_graph_list.size()); return path_graph_list[i]; }
    EXPORT path_graph *get_path_graph(stringx id);
    EXPORT void add_path_graph(path_graph *pg);
    EXPORT void remove_path_graph(path_graph *pg);

    EXPORT void add_region_ambient_sound( stringx &regname, stringx &sndname, rational_t volume );

    EXPORT void add_crawl_box( int type, bool forced, const vector3d& where, const convex_box& binfo );

    EXPORT int get_num_crawl_boxes( void ) const { return crawl_boxes.size( ); }
    EXPORT crawl_box* get_crawl_box( int i ) { return crawl_boxes[i]; }

//    dread_net *get_dread_net() const { return dread_network; }
// BIGCULL     ai_cue_manager *get_ai_cue_mgr() const { return ai_cue_mgr; }


    entity *get_entity(const stringx &name);

    const entity_list& get_entities() const { return entities; }
    const std::vector<item*>& get_items() const { return items; }

    int get_num_controllers() const { return controllers.size(); }
    controller * get_controller(int i) const { return controllers[i]; }
    const std::vector<controller*>& get_controllers() const { return controllers; }

    const std::vector<light_source*>& get_lights() const { return lights; }



    // NEWENT File I/O
    light_source* add_light_source( chunk_file& fs,
                                    const entity_id& id,
                                    unsigned int flags );

    particle_generator* add_particle_generator( const stringx& fname,
                                                const entity_id& id,
                                                unsigned int flags );

    item* add_item( chunk_file& fs,
                    const entity_id& id,
                    unsigned int flags );

    polytube* add_polytube( chunk_file& fs,
                  const entity_id& id,
                  unsigned int flags );


    lensflare* add_lensflare( chunk_file& fs, const entity_id& id, unsigned int flags );

	// @Patch - remove cull
    gun* add_gun( chunk_file& fs,
                  const entity_id& id,
                  unsigned int flags );

    melee_item* add_melee( chunk_file& fs,

                  const entity_id& id,
                  unsigned int flags );

    thrown_item* add_thrown_item( chunk_file& fs,
                  const entity_id& id,
                  unsigned int flags );

    manip_obj* add_manip_obj( chunk_file& fs,
                  const entity_id& id,
                  unsigned int flags );

    switch_obj* add_switch_obj( chunk_file& fs,
                  const entity_id& id,
                  unsigned int flags );
    turret* add_turret( chunk_file& fs,
                  const entity_id& id,
                  unsigned int flags );

    scanner* add_scanner( chunk_file& fs,
                          const entity_id& id,

                          unsigned int flags );
    void add_scanner( scanner* e );


    sky* add_sky( chunk_file& fs, const entity_id& id, unsigned int flags );
    void add_sky( sky *e );

    void add_to_entities( entity *e );
    void add_dynamic_instanced_entity( entity* ent );

    // These DO NOT DELETE THE ENTITY.  That is the responsibility of the caller.

    bool remove_entity( entity* ent );
    bool remove_entity( unsigned int i );
    bool remove_item( item * iptr );
    bool remove_item( unsigned int i );

    // remove and delete entity if present; note that some flavors are disallowed
    EXPORT void destroy_entity( entity* e );

    EXPORT void add_light_source( light_source* ls );
    EXPORT void remove_light_source( light_source* ls );

    EXPORT void add_marker( marker* e );
    EXPORT void add_polytube( polytube* e );
    EXPORT void add_camera( camera* e );
    EXPORT void add_mic( mic* e );

    EXPORT void add_beam( beam* e );

    EXPORT void add_particle_generator( particle_generator* pg );
    EXPORT void add_lensflare( lensflare* e );

    EXPORT void add_neolight( neolight* nl );

    EXPORT void add_item( item* it );

    EXPORT void add_turret( turret* cg );

    // add given entity to the list of entities that are guaranteed to be active
    // independent of visibility, region status, etc.

    void guarantee_active( entity* e );

    bool entity_is_preloaded(const stringx& entity_name);

		void add_entity_instance( entfile_map::iterator fi, entity_id id, unsigned int ent_flags, po& my_po );

    entity* create_preloaded_entity_or_subclass( const stringx& entity_name,
                                                 const stringx& scene_root );

    void remove_entity_from_world_processing( entity *e );

    void add_entity( const stringx& entity_name,
                  bool a, bool stationary, bool invisible, bool cosmetic, bool walkable,
                  bool repulsion, bool nonstatic,
                  entity_id & _id, entity_flavor_t flavor = ENTITY_ENTITY);
                  // i put entity_name in a strange place because Metrowerks was generating wacky code:

    void add_item( const stringx& entity_name,
                  bool a, bool stationary, bool invisible, bool cosmetic,

                  entity_id & _id, entity_flavor_t flavor = ENTITY_ITEM);
    void add_morphable_item( const stringx& entity_name,
                  bool a, bool stationary, bool invisible, bool cosmetic,
                  entity_id & _id, entity_flavor_t flavor = ENTITY_ITEM);
    particle_generator *add_particle_generator( const stringx& filename,
                                bool invisible, bool nonstatic,
                                entity_id &_id );
		/*
    light_source* add_light_source( entity_id id,
                                    po const & loc,
                                    const light_properties& props,
                                    const region_node_list forced_regions,
                                    unsigned int scene_flags = entity::ACTIVE );

    light_source* add_light_source( const stringx& filename,
                                    bool invisible, bool nonstatic,
                                    entity_id &_id );
	*/
    entity* add_box_trigger( entity_id id,
                             const po& loc,
                             const convex_box& binfo,
                             //const list<region_node*>* forced_regions,
                             const region_node_list* forced_regions,
                             unsigned int scene_flags = entity::ACTIVE );
    EXPORT void add_box_trigger( entity* e );

    void add_marker( entity_id& _id );

    beam* add_beam( const entity_id& _id, unsigned int _flags );

    int add_joint(joint * j);
    void remove_joint(joint * j);

    void add_anim( entity_anim_tree* new_anim );
    void kill_anim( entity_anim_tree* the_anim );

    ett_manager *get_ett_manager();

    bool eligible_for_frame_advance( entity_anim_tree* anm ) const;

    int add_controller(controller * ctlr);
    int add_mcs(motion_control_system * mcs);
    int add_fcs(force_control_system * fcs);
    int add_generator(force_generator * gen);


    // create a time-limited effect
    entity* add_time_limited_effect( const char* name, const po& loc, time_value_t duration );
    // move given entity into time-limited list
    void make_time_limited( entity* e, time_value_t duration );

    void load_scene( const stringx& scene_name, const stringx& hero_filename );
	void load_fe_scene(const stringx& scene_name, const stringx& filename);
    void parse_scene(chunk_file& fs, const stringx& scene_root, bool loading_sin = false);
    void unload_scene( void );

    void load_hero(device_id_t joystick_num, const int hero_num);
    void setup_cameras();


    void reload_particle_generators();  // option for developers

    void recompute_all_sectors(); // calls compute_sector for each entity
    entity* get_hero_ptr(int n){return hero_ptr[n];}

    void    set_hero_ptr(int n, entity *hptr) {hero_ptr[n]=hptr;}
	/*
    kellyslater_controller *get_ks_controller(int n)  {return ks_controller[n];}

	kellyslater_controller ** get_ks_controllers(void) { return ks_controller; }
    kellyslater_controller *get_ai_controller()  { return (g_game_ptr->get_num_ai_players()?ks_controller[1]:NULL);}
	*/
	
    entity *get_board_ptr(int n) {PANIC; return NULL;}
    entity *get_board_model_ptr(int n) {PANIC; return NULL;}

    //
    void load_ai_hero(device_id_t joystick_num, int surfer_index, bool pers);

    marky_camera* get_marky_cam_ptr() { return marky_cam; }

    bool is_marky_cam_enabled() { return marky_cam_enabled; }

    void render(camera* camera_link, const int heroIdx);
    void render_debug(camera* camera_link);    // render various debugging graphics


    void frame_advance(time_value_t t);

    // these just update the usercam/scene analyzer.  for use when the game is paused.
    void usercam_frame_advance(time_value_t t);
    void scene_analyzer_frame_advance(time_value_t t);

    rational_t compute_total_energy();

    inline script_manager * get_script_manager() { return &scriptman; }


    script_object* get_current_level_global_script_object() const { return cur_global_so; }
    script_object::instance* get_current_level_global_script_object_instance() const { return cur_global_so_inst; }

    void spawn_misc_preload_scripts();

	// @Ok
	// @Matching
    terrain& get_the_terrain() { return *the_terrain; }

    // teleport hero to next malor marker (if any)
    void malor_next();
    void malor_prev();
    bool malor_to(stringx point);
    void malor() { malor_next(); }

    entity * get_origin_entity() { return origin_entity; }

		static bool wds_exists( const stringx& filename, const stringx& extension, int io_flags=os_file::FILE_READ );
    static stringx wds_open( chunk_file& fs, const stringx& filename, const stringx& extension, int io_flags=os_file::FILE_READ );
    static stringx wds_open( app_file& fs, const stringx& filename, const stringx& extension, int io_flags=os_file::FILE_READ );


	// Always read an extra byte by default, so we can add a '\0' to the end of text files (dc 07/03/02)
    static bool wds_readfile( const char *name, unsigned char **buf, unsigned int *len, int alignment, int extra_bytes = 1 );
    static bool wds_readfile_prealloc( const char *name, unsigned char **buf, unsigned int *len, int alignment, int extra_bytes, int max_len );

    static bool wds_releasefile( unsigned char **buf );


	// @Patch -for now
	//std::vector<entity *> const & get_active_entities() const { return active_entities; }

    time_value_t get_cur_time_inc() const { return cur_time_inc; }

	// @Ok
	// @Matching
    entfile_map const & get_entfiles() const { return entfiles; }

    void insert_entfile( entfile_map::value_type v ) { entfiles.insert( v ); }

    #ifndef BUILD_BOOTABLE
    void dump_frame_info();
    #endif

    //bool sphere_in_frustum(const region_node*, const vector3d& center, rational_t r) const;

    float get_surface_effect_duration( int surf_index );
    stringx get_surface_sound_name( int surf_index );
    stringx get_surface_effect_name( int surf_index );

	std::vector<entity *> parented_entities;

/*
	// LipSync System...putting here because of mem allocation, per Jamie's recommendation
    LipSyncMngr* m_pLipSync;
*/


  private:
    // this is called by game::enable_marky_cam.  ack.
    friend class game;
    void enable_marky_cam( bool enable, rational_t priority = MIN_CAMERA_PRIORITY );

    void do_all_collisions(time_value_t t);
    void do_entity_to_entity_collisions(time_value_t t);
    void do_entity_to_terrain_collisions(time_value_t t);
    void do_entity_to_bsp_collisions(time_value_t t);

  public:
    bool entity_entity_collision_check(entity * a1, entity * a2, time_value_t time, cface * hitFace = NULL);
    bool entity_entity_collision(entity * p1,entity * p2, time_value_t t);

  private:
	PADDING(0x20-0xC);
	  std::vector<force_generator *> generators;
    std::vector<force_control_system *> fcs_list;
    std::vector<motion_control_system *> mcs_list;
    std::vector<controller * > controllers;

    friend class EnableBrainsVariable;


    std::vector<variant_descriptor*> variants;

	  std::vector<entity*> moved_ents;

    ett_manager *ett_mgr;
    pentity_anim_tree_vector anims;

    entity_list entities;

	std::vector<path_graph *> path_graph_list;
//    dread_net *dread_network;

// BIGCULL     ai_cue_manager * ai_cue_mgr;

    crawl_box_list crawl_boxes;

    std::vector<item*> items;
    std::vector<entity*> guaranteed_active_entities;


    entity* collision_dummy;
    //render_data to_render;
	
	PADDING(0x10);

	// @Patch - moved around
    std::vector<light_source*> lights;


    // parallel list allowing entities to be destroyed after a given amount of time
    struct ent_time_limit
    {
      entity* ent;
      time_value_t duration;
      ent_time_limit() : ent(0) {}
      ent_time_limit( entity* _ent, time_value_t _duration ) : ent(_ent), duration(_duration) {}
    };
    struct surface_type_info
    {
      stringx sound_name;
      stringx effect_name;
      float   effect_duration;
#ifdef GCCULL
			vector<sound_group> sound_groups;
#endif
    };
    typedef std::map<int, surface_type_info*> surfaceinfo_list_t;
    surfaceinfo_list_t surfaceinfo_list;

	PADDING(0x10C-0xE0);

	// @Patch - moved around
    std::vector<ent_time_limit> time_limited_entities;


	PADDING(0x124-0x10C-0xC);

    terrain * the_terrain;
    entity * hero_ptr[MAX_PLAYERS];

    //kellyslater_controller * ks_controller[MAX_PLAYERS];
    // file instancing

    // scripting
    script_manager scriptman;
    script_object* cur_global_so;

	// @Patch - moved around
    entfile_map entfiles;

    script_object::instance* cur_global_so_inst;

	PADDING(0x1B0-0x174);

    // This flag is useful for filtering operations that we only wish to perform
    // during the initial load of the scene.  Note that this flag will be FALSE
    // during the load of the .sin file.
    bool loading_from_scn_file;
    stringx g_sxfname;

    // The time_increment for the current frame.  Set in frame_advance, and only valid
    // during a frame_advance call.
    enum { T_INC_INVALID=-999 };
    time_value_t t_inc;


  private:
    void _build_render_data_regions( render_data& rd,
                                     region_node* rn,
                                     const rectf& screen_rect,
                                     camera& camera_link );
    void _build_render_data_ents( render_data& rd );

    void _determine_visible_entities( render_data& rd, region_node* rn, /*matrix4x4 const & world2view,*/
                                      po const & campo);

    void clear();

    // malor support

    int current_malor_marker;

    // these pointers allow us to update the usercam when the game is paused.
    motion_control_system * usercam_orient_mcs;
    theta_and_psi_mcs * usercam_move_mcs;
    controller * usercam_controller;
    entity * usercam;

	replay_camera* replay_cam_ptr;

    // these pointers allow us to update the scene_analyzer when the game is paused.
	// @Patch - removed for now
    motion_control_system * scene_analyzer_move_mcs;
    motion_control_system * scene_analyzer_orient_mcs;
	/*
    controller * scene_analyzer_controller;
    entity * scene_analyzer_cam;
	*/

    // marky_camera - script language controlled game camera.


	  // List of all active entities for frame_advance
	  //std::vector<entity *> active_entities;

    // this is to provide a fake parent to colgeoms and visreps who are already in world space.
    entity * origin_entity;

	// @Patch - moved around
    marky_camera* marky_cam;
    bool marky_cam_enabled;


	  std::vector<entity *> collision_entities;

    // this is the last value passed to frame_advance, representing the duration of the current time_slice.
    // It's here so everyone can access it without passing it everywhere as a parameter
    time_value_t cur_time_inc;

    // used to optimize rendering of many small particle systems
    aggregate_vert_buf_list matvertbufs;

	// @Patch - removed
    //std::vector<entity*> dead_ents;

	// @Patch
	//nglLightContext *current_light_context;
  public:
    int setup_time;

    game_clock_t world_clock;

    EXPORT void apply_radius_damage(vector3d center, rational_t radius, int bio_damage, int mechanical_damage);
    EXPORT aggregate_vert_buf_list &get_matvertbufs() { return matvertbufs; }
    EXPORT bool is_loading_from_scn_file() const;

	// @Patch
	/*
	nglLightContext *get_current_light_context() { return current_light_context; }
	nglLightContext *set_current_light_context(nglLightContext *new_lc); 

	*/
    // scene animation stuff
  private:
    typedef std::map<stringx,scene_anim *> scene_anim_map_t;

    scene_anim_handle_t last_snm_handle;

    scene_anim_map_t scene_anim_map;
    scene_anim_list_t scene_anims;


  public:
    void load_scene_anim( const stringx &filename );
    scene_anim_handle_t play_scene_anim( const stringx &filename, bool reverse=false, float start_time=0.0f );
	float get_scene_anim_time(scene_anim_handle_t handle);
	entity_anim_tree* get_scene_anim_tree(scene_anim_handle_t handle);
    void kill_scene_anim( scene_anim_handle_t handle );

  private:

    material_set_list material_sets;

    friend class MaterialDialog;

  public:
    void add_material_set( material_set *mset );
    void add_material_set( const stringx& fname );
    void delete_material_set( material_set *mset );
    void delete_material_set( stringx& set_name );
    material_set *get_material_set( const stringx& set_name );


    friend class SpawnCommand;
    friend class slf_entity_col_check_t;

  public:
    void add_entity_preload_script(entity *e, const stringx &entity_name);


  private:
    ai_polypath *world_path;

  public:
    ai_polypath *get_world_path() const { return(world_path); }

#ifdef PROJECT_KELLYSLATER

  private:
    void render_kelly_slater_stuff(const int heroIdx);
    void process_kelly_slater_stuff ();
	void process_kelly_slater_debugmenus ();

#endif // PROJECT_KELLYSLATER

	// @Patch - added this section
  private:

	int field_3F0;
	int field_3F4;

	PADDING(0x408-0x3F4-4);

    class entity_preload_pair
    {
      public:
				#ifdef TARGET_GC
        entity_preload_pair()   { ent = NULL; name = stringx(""); }
				#endif
        INLINE entity_preload_pair(entity *e, const stringx &ent_name)   { ent = e; name = ent_name; }

        inline void copy(const entity_preload_pair &b)
        {
          ent = b.ent;
          name = b.name;
        }

        inline entity_preload_pair(const entity_preload_pair &b)
		{
			copy(b);
		}

        entity_preload_pair & operator=(const entity_preload_pair &b)
        {

          copy(b);
          return(*this);
        }

        entity *ent;
        stringx name;

		friend void validate_entity_preload_pair(void);
		friend void patch_entity_preload_pair(void);
    };

	// @Patch - moved around
   std::vector<entity_preload_pair> entity_preloads;

	PADDING(0x420-0x408-0xC);

	color fog_color;

	rational_t fog_near;
	rational_t fog_far;
	rational_t fog_min;
	rational_t fog_max;

	rational_t time_dilation;
	

	// @Patch - added this too
  public:
	EXPORT void set_global_time_dilation(rational_t);
	EXPORT void set_fog_range(rational_t, rational_t);

	EXPORT void set_fog_color(color);

	EXPORT bool is_scene_anim_playing(void) const;
};

////////////////////////////////////////////////////////////////////////////////
extern world_dynamics_system * g_world_ptr;

// @TODO - fix when all cases are replaced
//#define GET_WORLD_PTR (g_world_ptr)
#define GET_WORLD_PTR (*reinterpret_cast<world_dynamics_system**>(0x00929D0C))

void build_region_list_radius(std::vector<region_node*> *regs, region_node* rn, const vector3d& pos, rational_t rad, bool only_active_portals = true, std::vector<region_node*> *append = NULL);
void build_region_list(std::vector<region_node*> *regs, region_node *r, const vector3d& o, const vector3d& d, std::vector<region_node*> *append = NULL);

void compute_combat_target( entity* shooter, rational_t time_inc = 0.0f, bool use_los_only = false );

bool visibility_check( const vector3d& p1, const vector3d& p2, entity *ent );

extern bool g_screenshot;	
extern const char *g_screenshot_filename;	
extern int g_render_cube_map;	

// @Patch - added
EXPORT rational_t get_level_time(void);

#endif
