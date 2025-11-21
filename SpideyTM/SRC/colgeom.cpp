#include "colgeom.h"

////////////////////////////////////////////////////////////////////////
//  Globals
////////////////////////////////////////////////////////////////////////
vectorvector hit_list;
vectorvector normal_list1;
vectorvector normal_list2;

#include "my_assertions.h"

void validate_colgeom(void)
{
	VALIDATE(collision_geometry, valid, 8);

	VALIDATE_VTABLE(collision_geometry, get_closest_point_along_dir, 10);
}


#include "my_patch.h"

void patch_colgeom(void)
{
}
