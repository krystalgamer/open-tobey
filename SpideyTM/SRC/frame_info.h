#ifndef _FRAME_INFO_H
#define _FRAME_INFO_H



#include "ostimer.h"


class frame_info
{
	friend void validate_frame_info(void);
	friend void patch_frame_info(void);
private:
  time_value_t age;
  int ifl_frame_boost;

  int ifl_frame_locked;
  static float ifl_frame_rate;

public:
  EXPORT frame_info();
  EXPORT static void set_frame_rate(float);
  EXPORT static float get_frame_rate ();
  EXPORT time_value_t get_age() const;

  // @Ok
  // @Matching
  EXPORT void set_age(time_value_t a) { age = a; }
  EXPORT int get_ifl_frame_boost () const;
  EXPORT void set_ifl_frame_boost( int boost ) { ifl_frame_boost = boost; }
  EXPORT int get_ifl_frame_locked () const;

  EXPORT void set_ifl_frame_locked(int);
  EXPORT void operator=(const frame_info &source);

  EXPORT void compute_boost_for_play(int period);
  EXPORT int time_to_frame_locked (int period = 0) const;
  EXPORT int time_to_frame (int period = 0) const;
};



#endif  // _FRAME_INFO_H
