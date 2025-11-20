#include "warnlvl.h"
#include "app.h"

bool g_master_clock_is_up = false;

DEFINE_SINGLETON(app)

/////////////////////////////////////////////////////////////////////////////
// Event signals
/////////////////////////////////////////////////////////////////////////////


// This static function must be implemented by every class which can generate
// signals, and is called once only by the application for each such class;
// the effect is to register the name and local id of each signal with the
// signal_manager.  This call must be performed before any signal objects are
// actually created for this class (via signaller::signal_ptr(); see signal.h).
void app::register_signals()
{

#define MAC(label,str)  signal_manager::inst()->insert( str, label );
#include "global_signals.h"

#undef MAC
}

static const char* signal_names[] =
{
#define MAC(label,str)  str,
#include "global_signals.h"
#undef MAC
};

unsigned short app::get_signal_id( const char *name )
{
	int idx;

	for( idx = 0; idx < (int)(sizeof(signal_names)/sizeof(char*)); ++idx )
	{
		int offset = strlen(signal_names[idx])-strlen(name);

		if( offset > (int)strlen( signal_names[idx] ) )
			continue;


		if( !strcmp(name,&signal_names[idx][offset]) )
			return( idx + PARENT_SYNC_DUMMY + 1 );
	}

	// not found
	return signaller::get_signal_id( name );
}

// This virtual function, used only for debugging purposes, returns the
// name of the given local signal
const char* app::get_signal_name( unsigned idx ) const
{
	assert( idx < N_SIGNALS );
	if ( idx <= (unsigned)PARENT_SYNC_DUMMY )
		return signaller::get_signal_name( idx );
	else
		return signal_names[idx-PARENT_SYNC_DUMMY-1];
}

instance_render_info* app::get_viri()
{
	assert(viri != NULL);
	return viri;
}

void app::set_viri(instance_render_info* new_viri)
{
	assert (new_viri == NULL || (viri == NULL && new_viri != NULL));
	viri = new_viri;
}

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
