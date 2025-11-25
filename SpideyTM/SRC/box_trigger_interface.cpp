#include "global.h"
#include "box_trigger_interface.h"

#include "my_assertions.h"

// @Ok
// @Matching
void box_trigger_interface::copy(box_trigger_interface* a2)
{
	this->box = a2->box;
}

void validate_box_trigger_interface(void)
{
	VALIDATE_SIZE(box_trigger_interface, 0x80);

	VALIDATE(box_trigger_interface, box, 8);

	VALIDATE_VTABLE(box_trigger_interface, copy, 7);
}

void validate_bounding_box(void)
{
	VALIDATE_SIZE(bounding_box, 24);
}

#include "my_patch.h"

void patch_box_trigger_interface(void)
{
	PATCH_PUSH_RET_POLY(0x004B9010, box_trigger_interface::copy, "?copy@box_trigger_interface@@UAEXPAV1@@Z");
}

void patch_bounding_box(void)
{
}

