#include "game_process.h"
#include "game.h"

game_process::game_process()
{
  name = 0;
  flow = 0;
  index = 0;
  num_states = 0;
  timer = 0;
  allow_override = false;
}

game_process::game_process( const char *_name, const game_state_e *_flow, int _num_states )
  : name(_name), flow(_flow), num_states(_num_states)
{
  index = 0;
  timer = 0.0f;
  allow_override = false;
}

// @Ok
// @Matching
INLINE void game_process::go_next_state()
{
  ++index;
  assert( index < num_states );
}

INLINE void game_process::reset_index()
{
  index = 0;
}

void game_process::set_index(int i)
{
	index = i;
	assert(index < num_states);
}


int game::get_cur_state() const
{
	typedef int (__fastcall *func_ptr)(const game*);
	func_ptr func = (func_ptr)0x005E4090;

	return func(this);
}

////////////////////////////////////////////////////////////////

void game::push_process( game_process &process )
{
  process_stack.insert(process_stack.begin(), process);
  process_stack.front().reset_index();
  process_stack.front().set_timer( 0.0f );
}

void game::pop_process()
{
	/*
  assert( process_stack.size() != 0 );
  process_stack.pop_front();
  */
	PANIC;
}

/*
int game::get_cur_state() const
{
  assert( process_stack.size() != 0 );

  return process_stack.front().get_cur_state();
}
*/

// @Ok
// @Matching
void game::go_next_state()
{
  assert( process_stack.size() != 0 );

  // @Patch - front to back
  process_stack.back().go_next_state();
}

INLINE void game::reset_index()
{
  assert( process_stack.size() != 0 );
  process_stack.front().reset_index();
}


#include "my_assertions.h"
void validate_game_process(void)
{
	VALIDATE_SIZE(game_process, 0x18);

	//VALIDATE(game_process, timer, 0);
	VALIDATE(game_process, index, 0x8);
}

#include "my_patch.h"
void patch_game_process(void)
{
	PATCH_PUSH_RET(0x005E40C0, game::go_next_state);
}
