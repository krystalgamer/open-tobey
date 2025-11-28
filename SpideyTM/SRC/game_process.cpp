#include "game.h"

int game::get_cur_state() const
{
	typedef int (__fastcall *func_ptr)(const game*);
	func_ptr func = (func_ptr)0x005E4090;

	return func(this);
}
