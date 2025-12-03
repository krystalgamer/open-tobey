#include "game_process.h"
#include "game.h"

int game::get_cur_state() const
{
	typedef int (__fastcall *func_ptr)(const game*);
	func_ptr func = (func_ptr)0x005E4090;

	return func(this);
}

#include "my_assertions.h"
void validate_game_process(void)
{
	VALIDATE_SIZE(game_process, 0x18);
}

#include "my_patch.h"
void patch_game_process(void)
{
}
