#include "..\global.h"
//#pragma hdrstop

#include "..\singleton.h"
#include "..\ostimer.h"

#include <mmsystem.h>
#pragma comment (lib, "winmm")

DEFINE_SINGLETON(master_clock)

// use timeGetTime because QueryPerformanceCounter has problems on some machines

//  const long XB_CLOCK_SPEED = 294912000;
/*	Superseded by actual queried values (dc 03/07/02)
const long XB_CLOCK_SPEED = 733000000L;

const u_int XB_TICK_RATE = XB_CLOCK_SPEED>>16;   // This now has ~.25ms accuracy.
const float SECONDS_PER_TICK = 1.0f/XB_TICK_RATE;
*/
  
time_value_t game_clock::delta = 0.0F;
uint32 game_clock::frames = 0;
uint64 game_clock::ticks = 0;


master_clock::master_clock()
{
  ticks.total = get_cpu_cycle();
}


void game_clock::frame_advance(time_value_t _delta) 
{

  ++frames;

  assert(_delta>0 && delta<10.0F);
  delta = _delta; 
  ticks += uint64(delta*10000.0F);
}

uint64 master_clock::elapsed( void )
{

	// @Patch
	return 0;
	/*
  ticks.total = get_cpu_cycle();


  return uint64(ticks.total);
  */
}

#ifdef FRAMERATE_LOCK
uint64 hires_clock_t::lock_ticks = 0;
#endif


float hires_clock_t::get_frequency( void )
{
  LARGE_INTEGER counts_per_sec;
  
  BOOL noErr = QueryPerformanceFrequency( &counts_per_sec );

  assert( noErr );

#ifdef FRAMERATE_LOCK
  lock_ticks = (17 * counts_per_sec.QuadPart) / 1000;	// keep us from alternating between 60 and 30 (dc 03/08/02)
#endif

  // system clock speed in msec / cycle

  return (float) (1.0 / double( counts_per_sec.QuadPart ));

}

// @TODO - replace with proper
#define GET_TIME_UNK (*reinterpret_cast<bool*>(0x0189D708))
bool timeUnk;

// @Ok
// @Matching
hires_clock_t::hires_clock_t()
{
	if (!GET_TIME_UNK)
	{
		timeBeginPeriod(1u);
	}

	this->time = timeGetTime();
}

time_value_t hires_clock_t::elapsed() const
{
	PANIC;
}


// @Ok
// @Matching
time_value_t hires_clock_t::elapsed() const
{
	return (timeGetTime() - this->time) * 0.001;
}

#ifdef FRAMERATE_LOCK
// Wait until at least lock_ticks have expired since last reset
void hires_clock_t::wait_for_lock() const 
{

  uint64 cur_ticks;

  do {
    cur_ticks = master_clock::inst()->elapsed();

  } while (cur_ticks - last_reset_ticks < lock_ticks);
}
#endif


#include "../my_assertions.h"

void validate_hires_clock_t(void)
{
	VALIDATE_SIZE(hires_clock_t, 0x4);
}

#include "../my_patch.h"

void patch_hires_clock_t(void)
{
	PATCH_PUSH_RET_POLY(0x008310B0, hires_clock_t::hires_clock_t, "??0hires_clock_t@@QAE@XZ");
	PATCH_PUSH_RET(0x00831140, hires_clock_t::elapsed);
}
