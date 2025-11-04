#include "light.h"

#include "my_assertions.h"

void validate_light_source(void)
{
	VALIDATE(light_source, properties, 0xF4);
}

void validate_light_properties(void)
{
	VALIDATE(light_properties, flags, 0x34);
}

#include "my_patch.h"

void patch_light_source(void)
{
}

void patch_light_properties(void)
{
}
