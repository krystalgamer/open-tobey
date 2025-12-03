#ifndef GSTATES_H
#define GSTATES_H


enum game_state_e
{
  GSTATE_NONE,
  GSTATE_PLAY_INTRO_MOVIES,
  GSTATE_FRONT_END,
  GSTATE_LOAD_LEVEL,
  GSTATE_PLAY_MOVIE,
  GSTATE_RUNNING,
  GSTATE_PLAY_FE_MOVIE,
  GSTATE_PAUSED,
  GSTATE_EMPTY_STATE,   // for extra rendering cycle required for notice popups
  GSTATE_POP_PROCESS,
  TOTAL_GSTATES,
};


// @Patch
#include "ostimer.h"
#include "my_export.h"

class game_process
{
	friend void validate_game_process(void);
	friend void patch_game_process(void);
public:
  EXPORT game_process();
  EXPORT game_process( const char *_name, const game_state_e *_flow, int _num_states );

  EXPORT ~game_process();

  EXPORT game_state_e get_cur_state() const { return flow[index]; }
  EXPORT void go_next_state();
  EXPORT void reset_index();
  EXPORT void set_index(int i);

  EXPORT void set_timer( time_value_t _timer ) { timer = _timer; }
  EXPORT time_value_t get_timer() const { return timer; }


  EXPORT void set_allow_override( bool _allow_override ) { allow_override = _allow_override; }

  EXPORT bool get_allow_override() const { return allow_override; }

protected:
  const char *name;
  const game_state_e *flow;
  int index;
  int num_states;
  time_value_t timer;
  bool allow_override;
};


extern game_process startup_process;
extern game_process main_process;
extern game_process play_movie_process;
extern game_process pause_process;

#endif  // GSTATES_H
