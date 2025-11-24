#include "mcs.h"



#include "my_assertions.h"
void validate_mcs(void)
{
	VALIDATE_VTABLE(motion_control_system, frame_advance, 3);
}

#include "my_patch.h"
void patch_mcs(void)
{
}
