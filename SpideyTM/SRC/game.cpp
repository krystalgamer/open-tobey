#include "game.h"
#include "random.h"
Random *g_random_ptr;

entity_maker *g_entity_maker = NULL;
void skip_intros(void)
{
	// @TODO
	puts("Skipping intros");
}

#include "my_assertions.h"

void validate_game(void)
{
}

#include "my_patch.h"

void patch_game(void)
{
	PATCH_PUSH_RET(0x005C1930, skip_intros);
}
