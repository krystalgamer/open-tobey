#include "warnlvl.h"
#include "app.h"

bool g_master_clock_is_up = false;

DEFINE_SINGLETON(app)


#include "my_assertions.h"

void validate_app(void)
{
	VALIDATE_SIZE(app, 0x24);
}

#include "my_patch.h"
void patch_app(void)
{
	PATCH_PUSH_RET_POLY(0x005B3CE0, app::construct_signal_list, "?construct_signal_list@app@@EAEPAV?$fast_vector@PAVsignal@@@@XZ");
}
