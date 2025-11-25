#include "global.h"
#include "box_trigger_interface.h"

#include "my_assertions.h"

void validate_box_trigger_interface(void)
{
	VALIDATE_SIZE(box_trigger_interface, 0x80);
}

#include "my_patch.h"

void patch_box_trigger_interface(void)
{
}
