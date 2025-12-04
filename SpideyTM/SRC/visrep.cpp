////////////////////////////////////////////////////////////////////////////////
/*
  visrep


  base class for physical_entities 3D visual representations,
  plus functions for managing memory storage
*/
////////////////////////////////////////////////////////////////////////////////
#include "global.h"

#include "visrep.h"

#include "pmesh.h"
#include "billboard.h"
#include "oserrmsg.h"
#include "iri.h"


visual_rep::visual_rep( visrep_t _type, bool _instanced )
  : type(_type),
    min_detail_dist(12), max_detail_dist(1), instanced(_instanced)
{}

void visual_rep::render_skin( render_flavor_t render_flavor,
                          const instance_render_info* iri,
                          const po* bones,
                          int num_bones )
{}


void visual_rep::render_batch(render_flavor_t flavor,
                            instance_render_info* viri,
                            int num_instances)
{
  for ( ; --num_instances>=0; ++viri)
    render_instance(flavor,viri);
}


rational_t visual_rep::compute_xz_radius_rel_center( const po& xform ) { return 0; }


int visual_rep::get_min_faces(time_value_t delta_t) const { return 1; }
int visual_rep::get_max_faces(time_value_t delta_t) const { return 1; }


void visual_rep::set_distance_fade_ok(bool v)
{}

bool visual_rep::get_distance_fade_ok() const
{
  return true;
}

float visual_rep::time_value_to_frame( time_value_t t ) { return t*30.0F; }

bool visual_rep::kill_me() { return false; }

void visual_rep::set_light_method( light_method_t lmt ) {}


int visual_rep::get_anim_length() const
{
  return 1;

}


render_flavor_t visual_rep::render_passes_needed() const
{
  return 0;
}

// utility function for turning file name into visual_rep type:
visrep_t visual_rep_name_to_type( const stringx& visrep_name )
{
	PANIC;

	return *(NEW visrep_t);
}


visual_rep* load_new_visual_rep( chunk_file& fs,
                                 const stringx& visrep_name,
                                 visrep_t _flavor,
                                 unsigned additional_flags )
{
	PANIC;
	return NULL;
}


// for loading visual_reps at the beginning of the level:
visual_rep* load_new_visual_rep( const stringx& visrep_name, unsigned additional_flags )
{
	PANIC;
	return NULL;
}


visual_rep* new_visrep_instance( visual_rep* vrep )
{
	PANIC;
	return NULL;
}

visual_rep* new_visrep_copy( visual_rep* vrep )
{
	PANIC;
	return NULL;
}

void unload_visual_rep( visual_rep* discard )
{
	PANIC;
}

// for finding visual_reps mid-game, where you aren't allowed to
// do disk access.  This function fails if the visual_rep hasn't
// been loaded.
visual_rep* find_visual_rep( const stringx& visrep_name )
{
	typedef visual_rep* (*func_ptr)(const stringx&);
	func_ptr func = (func_ptr)0x005B1580;

	return func(visrep_name);
}

time_value_t visual_rep::get_ending_time() const
{
  return LARGE_TIME;

}



bool visual_rep::is_uv_animated() const
{
  return false;
}

#include "my_assertions.h"

void validate_visual_rep(void)
{
	VALIDATE(visual_rep, type, 4);

	VALIDATE_VTABLE(visual_rep, get_ending_time, 6);

	VALIDATE_VTABLE(visual_rep, get_distance_fade_ok, 14);

	VALIDATE_VTABLE(visual_rep, get_anim_length, 15);
	VALIDATE_VTABLE(visual_rep, is_uv_animated, 16);

}

#include "my_patch.h"

void patch_visual_rep(void)
{
}
