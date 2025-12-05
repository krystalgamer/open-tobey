// sky.cpp

// This isn't a true skybox because it doesn't follow the camera Y position.
// A true skybox would always have the camera be in the exact center.

#include "global.h"

#include "project.h"
#include "sky.h"
#include "app.h"
#include "game.h"
#include "camera.h"



sky::sky( const entity_id& eid, entity_flavor_t _flavor )
: entity( eid, _flavor )
{
}


sky::sky( chunk_file& fs, const entity_id& _id, entity_flavor_t _flavor, unsigned int _flags )

: entity( fs, _id, _flavor, _flags )
{
}


entity* sky::make_instance( const entity_id& _id,
                            unsigned int _flags ) const
{
  sky* e = NEW sky( _id, ENTITY_SKY );
  e->copy_instance_data( *this );
  return (entity*)e;

}


// @TODO - move if needed, might be used through a macro called get_current_game_camera
int guess_current_game_cam;

// @Ok
// @Matching
void sky::render(rational_t detail, render_flavor_t flavor, rational_t entity_translucency_pct )

{
  // set sky position according to current_view_camera position
	// @Patch - added argument to get_current_view_camera
  vector3d p = app::inst()->get_game()->get_current_view_camera(guess_current_game_cam)->get_abs_position();
  p.y = get_abs_position().y;
  set_rel_position( p );

  // now do the render
  entity::render(detail, flavor, entity_translucency_pct );
}


#include "my_assertions.h"

void validate_sky(void)
{
	VALIDATE_SIZE(sky, 0xF4);

	VALIDATE_VTABLE(sky, render, 15);
}

#include "my_patch.h"

void patch_sky(void)
{
	PATCH_PUSH_RET_POLY(0x0060B6C0, sky::render, "?render@sky@@UAEXMIM@Z");
}
