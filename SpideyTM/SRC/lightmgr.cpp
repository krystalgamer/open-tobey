// lightmgr.cpp
// Copyright (c) 1999-2000 Treyarch Invention LLC.  ALL RIGHTS RESERVED.

#include "global.h"
#include "lightmgr.h"
#include "entity.h"
#include "light.h"
#include "terrain.h"
#include "pmesh.h" // for MAX_BONES, so we can extern bone_lights
#include "osdevopts.h"

#include "clipflags.h"
#include "game.h"

#include <vector>
#include <algorithm>

#include "errorcontext.h"
#include "osalloc.h"


//#define ENABLE_AFFECT_TERRAIN_LIGHTS
//#define WADE_LIGHTING




static refptr<light_manager> static_light_set=NULL; //NEW light_manager(true);


void LIGHTMGR_StaticInitLightSet( void )
{
	static_light_set=NEW light_manager(true);
}



light_manager* light_manager::get_static_light_set()
{

  static_light_set->lights.resize(0);
  static_light_set->last_ambient=static_light_set->goal_ambient=color_white;
  assert(static_light_set->last_ambient.r==1.0f &&
         static_light_set->last_ambient.g==1.0f &&
         static_light_set->last_ambient.b==1.0f);
  static_light_set->max_lights=1;
  static_light_set->cur_max_lights=1;
  static_light_set->dynamic_only=true;
  static_light_set->allow_omni=true;
  return static_light_set;
}


class scan_light_context
{
public:
  struct light_rec
  {
    float brightness;
    light_source* light;
    light_rec() {}
    light_rec(float _influence, light_source* ls) : brightness(_influence), light(ls) {}
    inline bool operator <(const light_rec& r) const { return brightness > r.brightness; } // brightest lights go first

  };


  std::vector<light_rec> lights;

  color ambient_factor;
  color underwater_ambient_factor;

  scan_light_context()
  {

    lights.reserve(64);
  }

  const color &get_ambient_factor(region *reg, const int playerID);

  void reset_lights(region* my_region, const sphere& bound, bool dynamic_only, const int playerID);
};

static scan_light_context scanlites;



const color &scan_light_context::get_ambient_factor(region *reg, const int playerID )
{
	#ifdef PROJECT_KELLYSLATER
		// this is an ugly last minute hack to change the ambient
		// player lighting under water.  -EO
	if ( UNDERWATER_CameraUnderwater(playerID) )
	{
		int curbeach=g_game_ptr->get_beach_id();
		u_int underwater = BeachDataArray[curbeach].underwaterambient;
		underwater_ambient_factor.a=1.0f;
		underwater_ambient_factor.r=((float) ((underwater >> 16) & 0xFF )) / 255.0f;
		underwater_ambient_factor.g=((float) ((underwater >>  8) & 0xFF )) / 255.0f;
		underwater_ambient_factor.b=((float) ((underwater      ) & 0xFF )) / 255.0f;
#ifdef TARGET_XBOX
	// On Xbox, we override the lighting values from the scene.  The PS2 values aren't appropriate for 
	// Xbox, because of differences in gamma and saturation.  To be fixed in the exporter next year. 

	// (dc 07/11/02)
		float underwater_boost = BeachDataArray[curbeach].underwater_boost;
//		underwater_ambient_factor.a=1.0f;
		underwater_ambient_factor.r+=underwater_boost;
		underwater_ambient_factor.g+=underwater_boost;
		underwater_ambient_factor.b+=underwater_boost;
#endif
  	return underwater_ambient_factor;
	}
	#endif


  return reg->get_ambient();
}


void scan_light_context::reset_lights(region* my_region, const sphere& bound, bool dynamic_only, const int playerID )
{
	PANIC;
}



#ifdef DEBUG

void light_manager::dump_debug_info() const
{
  debug_print("dumping light manager:  %d lights of max %d", lights.size(), max_lights);

  debug_print("  ambient current %f,%f,%f,%f goal %f,%f,%f,%f",
             last_ambient.r,last_ambient.g,last_ambient.b,last_ambient.a,
             goal_ambient.r,goal_ambient.g,goal_ambient.b,goal_ambient.a);

  debug_print("  allow omni %d, dynamic only %d",allow_omni,dynamic_only);
  for (int i=0; i<(int)lights.size(); ++i)
  {
    debug_print("  light %d: source=%p", i, lights[i].source);
    debug_print("    flavor=%s  vec=%f,%f,%f", (lights[i].props.get_flavor()==LIGHT_FLAVOR_POINT) ? "omni" : "dir",
       lights[i].dir_or_pos.x,lights[i].dir_or_pos.y,lights[i].dir_or_pos.z);
    debug_print("    intensity=%f  color=%f,%f,%f",
       lights[i].current_intensity,
       lights[i].props.get_color().r,lights[i].props.get_color().g,lights[i].props.get_color().b);
  }
}
#endif



int light_manager::compare_light(int sli)

{
	PANIC;
	return 0;
}


void light_manager::frame_advance(region* reg, time_value_t t, const int playerID)
{
  ectx e("light_manager::frame_advance");

  // get all lights that can possibly affect us, sorted by brightness
  scanlites.reset_lights(reg, bound, dynamic_only, playerID);

  goal_ambient = scanlites.ambient_factor; //*my_ambient;
  goal_ambient.clamp();

  int num_lites = scanlites.lights.size();

  cur_max_lights = max_lights;


  // this code causes flicker:  you light, your framerate drops, you don't light, your framerate goes up, etc.
  #if defined(TARGET_PC) && 0

  extern game* g_game_ptr;

  if (cur_max_lights > 1)
  {

    if (g_game_ptr->get_total_delta() > 1.0f/20) // skip this minor visual improvement if we're getting slow framerates
      cur_max_lights = 1;
  }
  else if (cur_max_lights > 2)
  {
    if (g_game_ptr->get_total_delta() > 1.0f/27) // skip this minor visual improvement if we're getting slow framerates
      cur_max_lights = 2;
  }
  #endif

  // If an old light doesn't exist in scanlites, zero its source pointer
  int i;
  for (i=lights.size(); --i>=0; )
  {
    light_rec* lite = &lights[i];
    if (!lite->source)
    {
      //if (lite->current_intensity<=0) // this is handled later
      //  lights.erase(lights.begin()+i);
      continue;
    }
    if (i>=(int)cur_max_lights)  // if code somehow decreased the number of max lights, kick extras out
    {

      //debug_print("removing extra light");
      lite->source = NULL;
    }
    int j;
    for (j=num_lites; --j>=0; )
    {
      light_source* ls = scanlites.lights[j].light;
      if (ls==lite->source)
        break;
    }
    if (j<0)

    {
      //debug_print("light no longer available");
      lite->source = NULL; // not found
    }
  }
  // allow stronger lights to replace weaker ones
  for ( i=0; i<num_lites; ++i )
  {
    if (compare_light(i) < 0) // couldn't place it.

      break; // Since strongest lights are first, there's no way any subsequent lights will be placed.
  }

  static const float fadefactor=3.0f; // 1/3 second
  //static const float fadefactor=0.5f; // 2 seconds

  float liteinc = t*fadefactor;

#ifndef PROJECT_KELLYSLATER	// this code causes the lights to shift at the beginning of the level (dc 06/26/02)
  color diff = goal_ambient-last_ambient;

  float dist2=sqr(diff.r)+sqr(diff.g)+sqr(diff.b);


  float maxallowdist = liteinc*0.6667f; // max color delta per second
  if (dist2>sqr(maxallowdist))
  {
    diff *= maxallowdist*fast_recip_sqrt(dist2);
    last_ambient += diff;
  } else
#endif
    last_ambient = goal_ambient;


  for (i=lights.size(); --i>=0; )
  {
    light_rec* lite = &lights[i];
    if (!lite->source)
    {

      lite->current_intensity -= liteinc;
      if (lite->current_intensity <= 0.0f)

      {
        //debug_print("light turned off");
        lights.erase(lights.begin()+i);
      }
    }

    else
    {
      lite->current_intensity += liteinc;
      if ( lite->current_intensity > 1.0f )
        lite->current_intensity = 1.0f;
    }
  }
}

void light_manager::prepare_for_rendering(use_light_context *lites)
{
	PANIC;
}




extern vector3d bone_lights[MAX_BONES][ABSOLUTE_MAX_LIGHTS];



void use_light_context::xform_lights_to_local(const po& world2local, int num_bones, render_flavor_t render_flavor )  // <<<< add num_bones and integrate with render_skin

{
	PANIC;
}

// transform lights to bone space
void use_light_context::transform_lights_to_bone_space(const vector3d& pos,const matrix4x4* bones_world, int num_bones)
{
	PANIC;
}

//////////////////////////////////



void use_light_context::clear_lights()
{
  dir_lights.resize(0);
  point_lights.resize(0);

  point_light_dists.resize(0);
  ambient_factor=color_white;
}


//////////////////////////////////


static light_types lt;

matrix4x4* light_matrices;

/*
void c_light(hw_rasta_vert* src_vert_list,
             int count,

             vector3d lighting_table[][ABSOLUTE_MAX_LIGHTS],
             color ambient_color,
             light_properties dir_light_props[ABSOLUTE_MAX_LIGHTS],
             int num_dir_lights,
             int num_point_lights,

             unsigned flags,
             use_light_context::light_dist* my_light_dists,
             hw_rasta_vert* dest_vert_list
             )
{
  if (num_point_lights||num_dir_lights||(flags&LIGHT_DIFFUSE))
  {

    const hw_rasta_vert* tvlit;
    hw_rasta_vert* dvlit;
    hw_rasta_vert* vert_list_end = src_vert_list+count;
    color color_ambient;
    if ( flags&LIGHT_DIFFUSE )
      color_ambient = ambient_color;
    else
      color_ambient = color_black;

    for (tvlit=src_vert_list,dvlit=dest_vert_list; tvlit!=vert_list_end; ++tvlit,++dvlit)
    {
    #if defined(TARGET_MKS)
      if ( !(tvlit->clip_flags & (FLAG_SELFLIT|CLIP_NOTINLOD) ) )
    #endif
      {
        int bonin = tvlit->specular.c.a;

        color diffuse(color_ambient), additive(color_black);
        vector3d vertex_normal = tvlit->get_normal();

        int j;
        for (j=num_dir_lights; --j>=0; )
        {
          rational_t lighting_factor = -dot(vertex_normal, bone_lights[bonin][j]); // the normal is going the wrong way!
          if(lighting_factor>0)
          {
            diffuse += lites.dir_lights[j].diffuse * lighting_factor;
            additive += lites.dir_lights[j].additive * lighting_factor;
          }
        }
        for(j=num_point_lights; --j>=0; )
        {
          int light_idx = my_light_dists[j].second;
          vector3d* lpl = &bone_lights[bonin][j+num_dir_lights];
          vector3d xyz = tvlit->xyz;
          vector3d light_vector = *lpl - xyz;   // needs transform
          rational_t distance2 = light_vector.length2();
          light_properties* props = &lites.point_light_props[ light_idx ];
          if( distance2 < sqr(props->get_cutoff_range()) )
          {
            rational_t lighting_factor;
            lighting_factor = dot(vertex_normal, light_vector);
            if(lighting_factor>0)
            {
              lighting_factor *= props->get_influence(__fsqrt(distance2));
              diffuse += props->get_color()*lighting_factor;
              additive += props->get_additive_color()*lighting_factor;
            }
          }
        }
        diffuse.clamp();
        additive.clamp();
        // mix vertex color into diffuse
        diffuse.r *= tvlit->diffuse.c.r * (1.0f/255);
        diffuse.g *= tvlit->diffuse.c.g * (1.0f/255);
        diffuse.b *= tvlit->diffuse.c.b * (1.0f/255);
        diffuse.a  = tvlit->diffuse.c.a * (1.0f/255);
        dvlit->diffuse = diffuse.to_color32();
        dvlit->specular = additive.to_color32();
      }
    }
  }
}
*/

// you might notice that while this is pretty it is in fact
// slower than c_light.  All this is is a template for a sega
// assembly function:  it should jamn on the sega.
void sweetlight(hw_rasta_vert* src_vert_list,
              int count,
              vector3d lighting_table[][ABSOLUTE_MAX_LIGHTS],
              light_properties dir_light_props[ABSOLUTE_MAX_LIGHTS],
              int num_dir_lights,
              int num_point_lights,        // dir_lights + point_lights
              unsigned flags,
              use_light_context::light_dist* my_light_dists,
              hw_rasta_vert_lit* dest_vert_list,
              int alpha,
              int num_bones
              )
{
	PANIC;
}


void c_sweetlight_inner(hw_rasta_vert* src_vert_list,
                        hw_rasta_vert_lit* dest_vert_list,

                        matrix4x4* light_matrices,  // note:  the first lighting_matrix is the diffuse lighting table, the second is specular, and
                                                       // from then on you have the light vector matrices for each bone
                        int count,

                        vector3d lighting_table[][ABSOLUTE_MAX_LIGHTS],
                        light_properties dir_light_props[ABSOLUTE_MAX_LIGHTS],
                        light_types lt,

                        unsigned flags)

{
	PANIC;
}



void c_onelight(hw_rasta_vert* src_vert_list,
                int count,
                light_properties* light_info,
                unsigned flags,
                hw_rasta_vert_lit* dest_vert_list,
                color32 ambientc)
{
	PANIC;
}


void prepare_lighting_matrices( color ambient_color,
                                light_properties* light_props_table,
                                int num_dir_lights,
                                int num_point_lights,
                                unsigned flags,
                                int num_bones )


{
	PANIC;
}


void initialize_lighting_matrices()
{
  light_matrices = (matrix4x4*)os_malloc((MAX_BONES+1)*sizeof(matrix4x4));
  memset(light_matrices,0,(MAX_BONES+1)*sizeof(matrix4x4) );
  for (int j=MAX_BONES; --j>=0; )
  {
    light_matrices[MX_BONES+j][3][0] = 1;  // guarantees a 1.0f multiplier on ambient no matter what direction vert is
  }

}
