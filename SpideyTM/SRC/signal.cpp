#include "global.h"
#include <algorithm>
#include "signals.h"
#include "oserrmsg.h"
#include "vm_thread.h"


unsigned int signal_callback::id_counter = 0;


///////////////////////////////////////
// script callback data

// @Ok
// @Matching
INLINE script_callback::script_callback( script_object::instance* _inst, const vm_executable* _func, const char* _parms )
  : signal_callback(),
  inst( _inst ),
  func( _func )
{
  int psize = func->get_parms_stacksize();
  if ( psize )
  {

    parms = NEW char[psize];
    memcpy( parms, _parms, psize );
  }
  else
    parms = NULL;
}


// @Ok
// @Matching
script_callback::~script_callback()
{

	// fix up dangling pointers to me, change this code with care, it fixed a bug once.

	// @Patch - add the g_world_ptr check
	if (GET_G_WORLD_PTR)
	{
		inst->clear_callback_references(this);
	}


	if ( parms )
	{
		delete[] parms;
	}
}


// @Ok
// @Matching
// spawn script callback function
void script_callback::spawn(signaller*sgrptr)
{
    // NB: sgrptr is ignored

  if ( !is_disabled() )

  {
    vm_thread* newvmt;
    if ( is_one_shot() )
      newvmt = inst->add_thread( func, parms );
    else
      newvmt = inst->add_thread( this, func, parms );

    //newvmt->set_suspendable( is_suspendable() );  // TODO: figure this out
  }
}

// @Ok
// @Matching
INLINE const stringx &script_callback::get_func_name()
{
  return(func->get_fullname());
}

///////////////////////////////////////
// script code callback data
// @Ok
// @Matching
INLINE code_callback::code_callback( void (*fn)(signaller*,const char*), const char *cptr )
  : signal_callback()
{

  func = fn;
  parms = const_cast<char*>(cptr);
}

// @Ok
// @Matching
code_callback::~code_callback()
{

}


// @Ok
// @Matching
void code_callback::spawn(signaller*sgrptr)
{
  if( !is_disabled() )

  {
    func( sgrptr, parms );
  }
}

///////////////////////////////////////////////////////////////////////////////
// CLASS signal

// @Ok
// @PartialMatching - thread safety
INLINE signal::signal(signaller*sgrptr)
:   flags( 0 ),
    name( NULL ),
    callbacks()
{
  flavor = SIGNAL;
  owner = sgrptr;

  // @Patch - moved down here
  outputs = NULL;
}



// @Ok
// @PartialMatching - thread safety and counter
signal::signal( const char* _name, signaller*sgrptr )
:   flags( 0 ),
    name( _name ),
    callbacks()
{
  flavor = SIGNAL;
  owner = sgrptr;
  // @Patch - write outputs here
  outputs = NULL;
}


// @Ok
// @PartialMatching - thread safety
signal::~signal()
{
  if ( outputs != NULL )
  {
    delete outputs;
    outputs = NULL;
  }

  clear_callbacks();
}


// @Ok
// @Matching
// add an output link
void signal::link( signal* s )

{
  if ( outputs == NULL )
    outputs = NEW signal_list;
  signal_list::const_iterator i = std::find( outputs->begin(), outputs->end(), s );

  if ( i == outputs->end() )
    outputs->push_front( s );

}



// @Ok
// @PartialMatching - thread safety
// remove an output link
void signal::unlink( signal* s )
{
  if ( outputs )

  {
	  signal_list::iterator i = std::find( outputs->begin(), outputs->end(), s );
    outputs->erase( i );
  }
}


// @Ok
// @NotMatching - it seems to clear during delete, but this is fine
// from what I understand the default implementation of this STL was holding
// into memory longer than it needed, imho not that important
INLINE void signal::clear_links()
{
  if ( outputs != NULL )
  {
	  // @Patch - clear before delete
	outputs->clear();
    delete outputs;
    outputs = NULL;
  }
}


// @Ok
// @Matching
// find an output gated_signal matching the given parameters
INLINE signal* signal::find_AND( const signal* b ) const
{
  if ( outputs )
  {
    signal_list::const_iterator i = outputs->begin();
    signal_list::const_iterator i_end = outputs->end();
    for ( ; i!=i_end; i++ )
    {

      signal* s = *i;
      if ( s->get_flavor() == GATED_SIGNAL )
        {

        if ( ((gated_signal*)s)->match( gated_signal::AND, b ) )
          return s;
      }
    }
  }
  return NULL;
}

// @Ok
// @Matching
INLINE signal* signal::find_OR( const signal* b ) const
{
  if ( outputs )

  {
    signal_list::const_iterator i = outputs->begin();
    signal_list::const_iterator i_end = outputs->end();
    for ( ; i!=i_end; i++ )
    {
      signal* s = *i;
      if ( s->get_flavor() == GATED_SIGNAL )

      {
        if ( ((gated_signal*)s)->match( gated_signal::OR, b ) )
          return s;
      }
    }
  }
  return NULL;
}



// @Ok
// @Matching
// raise this signal!
INLINE void signal::raise()
{

	//if ( !(flags & (DISABLED|RAISED)) )
	if ( !(flags & DISABLED) )
	{
		set_flag( RAISED );
		set_needs_refresh();
		// spawn script callbacks, if any
		do_callbacks();
		// raise the inputs of all linked output signals
		if ( outputs )
		{
			signal_list::const_iterator i = outputs->begin();
			signal_list::const_iterator i_end = outputs->end();
			for ( ; i!=i_end; i++ )
			{
				signal* s = *i;
				s->raise_input( this );
			}
		}
	}
}


// @Ok
// @Note - same as needs_refresh
INLINE void signal::set_needs_refresh()
  {
  if ( !is_flagged( NEEDS_REFRESH ) )  // avoid duplicate entries on the refresh list
  {
    // tell the signal_manager we need a refresh this frame (see below)
    set_flag( NEEDS_REFRESH );
    signal_manager::inst()->needs_refresh( this );

  }
}


// @Ok
// @Matching
// virtual function processes the raising of an input
void signal::raise_input( signal* input, signaller*sgrptr )
{
  // for basic signals, raising an input is equivalent to raising the signal directly
  raise();
}



// This virtual function performs an internal reset (once per frame, initiated by the

// @Ok
// @Matching
// signal_manager; see below) of ephemeral changes accumulated while raising signals
// in the course of a game frame.
INLINE void signal::refresh()
{
  // RAISED protects a signal from being raised more than once per frame;
  // NEEDS_REFRESH protects a signal from being duplicated on the signal_manager's refresh list
  flags &= ~( RAISED | NEEDS_REFRESH );
}



// @Ok
// @Matching
// add a script callback for this signal
unsigned int signal::add_callback( script_object::instance* _inst, vm_executable* _func, char* _parms, bool one_shot )
{
  script_callback* new_callback = NEW script_callback( _inst, _func, _parms );
  new_callback->set_one_shot( one_shot );
  callbacks.push_back( new_callback );

  return(new_callback->get_id());
}

// @Ok
// @Matching
// add code callback for this signal
unsigned int signal::add_callback( void (*fn)(signaller*,const char*), char* _parms, bool one_shot )
{
  code_callback* new_callback = NEW code_callback( fn, static_cast<const char*>(_parms) );
  new_callback->set_one_shot( one_shot );
  callbacks.push_back( new_callback );

  return(new_callback->get_id());
}


// @Ok
// @Patch - thread safety
// clear the script callback list
void signal::kill_callback(unsigned int callback_id)
{

  callback_list::iterator i = callbacks.begin();


  for ( ; i!=callbacks.end(); ++i )

  {
    signal_callback* sc = *i;
    if ( sc != NULL && sc->get_id() == callback_id)
    {
      callbacks.erase(i);
      delete sc;


      return;
    }
  }
}


// @Ok
// @PartialMatching - thread safety
INLINE void signal::clear_callbacks()
{
  callback_list::const_iterator i = callbacks.begin();
  callback_list::const_iterator i_end = callbacks.end();
  for ( ; i!=i_end; ++i )
  {

    signal_callback* sc = *i;
    if ( sc != NULL )
      delete sc;
  }
  callbacks.resize(0);

  // @Patch - add the clear
  callbacks.clear();
}


// @Ok
// @PartialMatch - thread safety
INLINE void signal::clear_script_callbacks()
{
  callback_list::iterator i = callbacks.begin();


  while(i!=callbacks.end())
  {
    signal_callback* sc = *i;
    if ( sc != NULL && sc->is_script_callback())
    {
      i = callbacks.erase(i);
      delete sc;
    }
    else

      ++i;
  }
}

// @Ok
// @PartialMatching - thread safety
INLINE void signal::clear_code_callbacks()
{
  callback_list::iterator i = callbacks.begin();

  while(i!=callbacks.end())
  {
    signal_callback* sc = *i;
    if ( sc != NULL && sc->is_code_callback())
    {
      i = callbacks.erase(i);
      delete sc;
    }
    else

      ++i;
  }
}

// @Ok
// @PartialMatching - thread safety
void signal::clear_script_callback(const stringx &name)
{
  callback_list::iterator i = callbacks.begin();

  for ( ; i!=callbacks.end(); ++i )
  {
    signal_callback* sc = *i;
    if ( sc != NULL && sc->is_script_callback() && ((script_callback *)sc)->get_func_name() == name)

    {
      callbacks.erase(i);
      delete sc;

      return;
    }
  }
}



// @Ok
// @PartialMatching - thread safety
// spawn script callbacks, if any
void signal::do_callbacks()
{
  if ( !is_flagged(CALLBACKS_DISABLED) )
  {
    {
      callback_list::iterator i = callbacks.begin();

      for ( ; i!=callbacks.end(); )
      {
        signal_callback* c = *i;
        c->spawn( owner );
        if ( c->is_one_shot() )
        {

          delete c;
          i = callbacks.erase( i );
        }
        else
          ++i;
      }

    }
  }

}



///////////////////////////////////////////////////////////////////////////////
// CLASS gated_signal

// @Ok
// @Matching
INLINE gated_signal::gated_signal( type_t _type, signal* _input_a, signal* _input_b )
  :   signal(),
      type( _type ),
      flags( 0 ),
      input_a( _input_a ),
      input_b( _input_b )
{
  flavor = GATED_SIGNAL;
}


// @Ok
// @Matching
// virtual function processes the raising of an input
void gated_signal::raise_input( signal* input )
{
  // raise signal if gate condition has been satisfied
  if ( input == input_a )
    set_flag( RAISED_A );
  else if ( input == input_b )
    set_flag( RAISED_B );
  else
    assert( 0 );
  if ( type == AND )
  {
    if ( (flags&(RAISED_A|RAISED_B)) == (RAISED_A|RAISED_B) )

      raise();
  }
  else
    raise();
  // need a refresh in any case
  set_needs_refresh();
}



// return true if parameters match this gated_signal
// @Ok
// @Matching
INLINE bool gated_signal::match( type_t _type, const signal* input ) const
{
  return (_type==type && (input==input_b || input==input_a));
}


// This virtual function performs an internal reset (once per frame, initiated by the
// signal_manager; see below) of ephemeral changes accumulated while raising signals
// in the course of a game frame.
void gated_signal::refresh()
{
  signal::refresh();
  // these flags are used to evaluate whether the gate condition has been satisfied

  // at any time during a single game frame
  flags = 0;
}




///////////////////////////////////////////////////////////////////////////////
// CLASS signaller

// @Ok
// @Matching
signaller::signaller()
  :   flags( 0 ),
      signals( NULL )
{
	this->field_C[0] = 0;
	this->field_C[1] = 0;
}


// @Ok
// @PartialMatching - thread safety and alloc
signaller::~signaller()
{
	this->clear_callbacks();
	this->clear_signals();
}

// @Ok
// @Matching
INLINE void signaller::clear_signals()
{
  if ( signals != NULL )
  {
    signal_list::iterator i = signals->begin();
    signal_list::iterator i_end = signals->end();
    for ( ; i!=i_end; ++i )
    {
      if(*i)
      {
		  delete *i;
      }
    }

	delete signals;
	signals = NULL;

  }
}

// @Ok
// @PartialMatching - thread safefty and alloc
INLINE void signaller::clear_callbacks(void)
{
  if ( signals != NULL )
  {
    signal_list::iterator i = signals->begin();
    signal_list::iterator i_end = signals->end();
    for ( ; i!=i_end; ++i )
    {
      if(*i)
      {
		  (*i)->clear_callbacks();
		  (*i)->clear_links();
      }
    }
  }
}

// @Ok
// @PartialMatching - thread safety
void signaller::clear_script_callbacks()
{
  if ( signals != NULL )
  {

    signal_list::iterator i = signals->begin();

    signal_list::iterator i_end = signals->end();
    for ( ; i!=i_end; ++i )
    {

      if(*i)
      {
        (*i)->clear_script_callbacks();
        (*i)->clear_links();
      }

    }
  }

}


void signaller::clear_code_callbacks()
{
  if ( signals != NULL )
  {
    signal_list::iterator i = signals->begin();
    signal_list::iterator i_end = signals->end();

    for ( ; i!=i_end; ++i )
    {
      if(*i)
        (*i)->clear_code_callbacks();
    }

  }
}

// @Ok
// @Matching
void signaller::clear_script_callback(const stringx &name)
{
  if ( signals != NULL )
  {
    signal_list::iterator i = signals->begin();
    signal_list::iterator i_end = signals->end();
    for ( ; i!=i_end; ++i )

    {
      if(*i)
        (*i)->clear_script_callback(name);

    }
  }

}

// @OK
// @Matching
void signaller::signal_error(unsigned int a2, const stringx& parm)
{
	stringx result(parm);

	if (a2 >= this->signals->size())
	{
		result += stringx(
				stringx::fmt,
				"\nSignal %d out of range (%d max)\n\n\nPossible signals for id %d:\n\n%s",
				a2,
				this->signals->size(),
				a2,
				signal_manager::inst()->get_name(a2).c_str());
	}

	error(result);
}





///////////////////////////////////////////////////////////////////////////////
// CLASS signal_manager


// @Patch
//DEFINE_SINGLETON( signal_manager )



// @Ok
// @AlmostMatching - thread safety
signal_manager::signal_manager()
  :   signal_id_map(),
      refresh_list()
{
	refresh_list.reserve(128);
	gated_refresh_list.reserve(128);
}


// @Ok
// @Matching
// return signaller-local index value for signal
unsigned short signal_manager::get_id( const stringx& name ) const
{
  signal_id_map_t::const_iterator i = signal_id_map.find( name );
  if ( i == signal_id_map.end() )
    error( "signal " + name + " not found" );
  return (*i).second;
}

// @Ok
// @NotMatching - debug function it concatenates a bunch of strings, not important imo
stringx signal_manager::get_name(unsigned short id) const
{
  signal_id_map_t::const_iterator i = signal_id_map.begin();
  signal_id_map_t::const_iterator i_end = signal_id_map.end();

  stringx result;
  for (; i != i_end; ++i)
  {
	  if (id == (*i).second)
	  {
		  stringx empty = stringx("");
		  if((*i).first == empty)
		  {
			  result += empty;
		  }
		  else
		  {
			  result += ("\n" + (*i).first);
		  }
	  }
	 
  }

  if (result == stringx(""))
	  return stringx("SIGNAL_NOT_FOUND");

  return result;
}


// @Ok
// @AlmostMatching - not as much inlined the pair stuff
// insert a NEW signal name and associated signaller-local index
void signal_manager::insert( const stringx& name, unsigned short id )
{
	std::pair< signal_id_map_t::iterator, bool > ib;
	ib = signal_id_map.insert( signal_id_map_t::value_type( name, id ) );
	assert( ib.second );
}


// @Ok
// @AlmostMatching - original was spilling into arguments for some reason
// create NEW signal consisting of logical AND of given signals
signal* signal_manager::signal_AND( signal* a, signal* b )
{
  // see if given signal already exists
  signal* sig = a->find_AND( b );
  if ( !sig )
  {
    // if not, create it
    sig = NEW gated_signal( gated_signal::AND, a, b );

	// @Patch
	needs_refresh(sig);

    a->link( sig );
    b->link( sig );
  }
  return sig;
}


// @Ok
// @AlmostMatching - original was spilling into arguments for some reason
// create NEW signal consisting of logical OR of given signals
signal* signal_manager::signal_OR( signal* a, signal* b )
{
  // see if given signal already exists
  signal* sig = a->find_OR( b );
  if ( !sig )
  {
    // if not, create it
    sig = NEW gated_signal( gated_signal::OR, a, b );

	// @Patch
	needs_refresh(sig);

    a->link( sig );
    b->link( sig );
  }
  return sig;
}


// this function is called when a signal object is affected such that it

// will need to be reset at the end of the game frame

// @Ok
// @PartialMatching - thread safety
INLINE void signal_manager::needs_refresh( signal* s )
{
  refresh_list.push_back( s );
}


// @Ok
// @NotMatching - different resize impl
void signal_manager::delete_gated_signals()
{
	gated_signal_list::const_iterator i = gated_refresh_list.begin();
	gated_signal_list::const_iterator i_end = gated_refresh_list.end();

	for (; i != i_end; ++i)
	{
		delete *i;
	}

	gated_refresh_list.clear();
	gated_refresh_list.resize(0);
}

// @Ok
// @NotMatching - it seems resize is slightly different it zeroes more stuff
// don't really care
//
// this function is called once per game frame to reset any signals that need it
// (see app::tick)
void signal_manager::do_refresh()
{
	signal_list::const_iterator i = refresh_list.begin();
	signal_list::const_iterator i_end = refresh_list.end();
	for ( ; i!=i_end; ++i )
	{
		signal* s = (*i);
		assert( s );
		s->refresh();
	}

	// @Patch - added clear before resize
	refresh_list.clear();

	refresh_list.resize(0);
}

// @Ok
// @NotMatching - same as do_refresh
// PEH BETA LOCK
void signal_manager::purge()
{
	// @Patch
  refresh_list.clear();

  refresh_list.resize(0);
  // @Patch - remove
  //signal_id_map.clear();
}

#include "my_assertions.h"
void validate_signaller(void)
{
	VALIDATE_SIZE(signaller, 0x14);

	VALIDATE(signaller, flags, 0x4);
	VALIDATE(signaller, signals, 0x8);
	VALIDATE(signaller, field_C, 0xC);

	VALIDATE_VTABLE(signaller, is_an_entity, 1);
	VALIDATE_VTABLE(signaller, is_a_trigger, 2);
	VALIDATE_VTABLE(signaller, signal_error, 3);
	VALIDATE_VTABLE(signaller, raise_signal, 4);
	VALIDATE_VTABLE(signaller, construct_signal_list, 5);
	VALIDATE_VTABLE(signaller, get_signal_name, 6);
}

void validate_signal_callback(void)
{
	VALIDATE_SIZE(signal_callback, 0x10);

	VALIDATE(signal_callback, parms, 0x4);
	VALIDATE(signal_callback, disabled, 0x8);
	VALIDATE(signal_callback, one_shot, 0x9);
	VALIDATE(signal_callback, id, 0xC);

	VALIDATE_VTABLE(signal_callback, spawn, 1);
	VALIDATE_VTABLE(signal_callback, is_code_callback, 2);
	VALIDATE_VTABLE(signal_callback, is_script_callback, 3);
}

void validate_script_callback(void)
{
	VALIDATE_SIZE(script_callback, 0x18);

	VALIDATE(script_callback, inst, 0x10);
	VALIDATE(script_callback, func, 0x14);

	VALIDATE_VTABLE(script_callback, spawn, 1);
	VALIDATE_VTABLE(script_callback, is_code_callback, 2);
	VALIDATE_VTABLE(script_callback, is_script_callback, 3);
}

void validate_code_callback(void)
{
	VALIDATE_SIZE(code_callback, 0x14);

	VALIDATE(code_callback, func, 0x10);

	VALIDATE_VTABLE(code_callback, spawn, 1);
	VALIDATE_VTABLE(code_callback, is_code_callback, 2);
	VALIDATE_VTABLE(code_callback, is_script_callback, 3);
}

void validate_signal(void)
{
	VALIDATE_SIZE(signal, 0x1C);

	VALIDATE(signal, flavor, 0x4);
	VALIDATE(signal, flags, 0x8);

	VALIDATE(signal, name, 0xC);

	VALIDATE(signal, outputs, 0x10);
	VALIDATE(signal, callbacks, 0x14);

	VALIDATE(signal, owner, 0x18);

	VALIDATE_VTABLE(signal, refresh, 1);
	VALIDATE_VTABLE(signal, raise_input, 2);
}

void validate_signal_manager(void)
{
	VALIDATE_SIZE(signal_manager, 0x28);

	VALIDATE(signal_manager, refresh_list, 0x10);
	VALIDATE(signal_manager, gated_refresh_list, 0x1C);
}

void validate_gated_signal(void)
{
	VALIDATE_SIZE(gated_signal, 0x28);

	VALIDATE(gated_signal, type, 0x1C);
	VALIDATE(gated_signal, flags, 0x1E);

	VALIDATE(gated_signal, input_a, 0x20);
	VALIDATE(gated_signal, input_b, 0x24);

	VALIDATE_VTABLE(gated_signal, refresh, 1);
	VALIDATE_VTABLE(gated_signal, raise_input, 3);
}

#include "my_patch.h"

void patch_gated_signal(void)
{
	PATCH_PUSH_RET_POLY(0x007D3330, gated_signal::raise_input, "?raise_input@gated_signal@@EAEXPAVsignal@@@Z");
	PATCH_PUSH_RET_POLY(0x007D34F0, gated_signal::refresh, "?refresh@gated_signal@@UAEXXZ");

	PATCH_PUSH_RET_POLY(0x007D2F60, gated_signal::gated_signal, "??0gated_signal@@QAE@W4type_t@0@PAVsignal@@1@Z");
}

void patch_signal_manager(void)
{
	PATCH_PUSH_RET(0x007D4710, signal_manager::needs_refresh);
	PATCH_PUSH_RET(0x007D4850, signal_manager::do_refresh);


	PATCH_PUSH_RET(0x007D4620, signal_manager::delete_gated_signals);
	PATCH_PUSH_RET(0x007D4940, signal_manager::purge);

	PATCH_PUSH_RET(0x007D4270, signal_manager::insert);
	PATCH_PUSH_RET(0x007D3F80, signal_manager::get_id);

	PATCH_PUSH_RET(0x007D4070, signal_manager::get_name);

	PATCH_PUSH_RET(0x007D43B0, signal_manager::signal_AND);
	PATCH_PUSH_RET(0x007D44E0, signal_manager::signal_OR);

	PATCH_PUSH_RET_POLY(0x007D3C40, signal_manager::signal_manager, "??0signal_manager@@AAE@XZ");
}

void patch_signal(void)
{
	PATCH_PUSH_RET_POLY(0x007D2180, signal::signal, "??0signal@@QAE@PBDPAVsignaller@@@Z");

	PATCH_PUSH_RET(0x007D2ED0, signal::do_callbacks);

	PATCH_PUSH_RET(0x007D2D70, signal::clear_code_callbacks);
	PATCH_PUSH_RET_POLY(0x007D2CF0, signal::clear_script_callbacks, "?clear_script_callbacks@signal@@QAEXXZ");
	PATCH_PUSH_RET_POLY(0x007D2DF0, signal::clear_script_callback, "?clear_script_callback@signal@@QAEXABVstringx@@@Z");

	PATCH_PUSH_RET_POLY(0x007D28F0, signal::raise_input, "?raise_input@signal@@EAEXPAV1@PAVsignaller@@@Z");
	PATCH_PUSH_RET(0x007D27B0, signal::set_needs_refresh);
	PATCH_PUSH_RET(0x007D26F0, signal::raise);

	PATCH_PUSH_RET(0x007D2BE0, signal::kill_callback);
	PATCH_PUSH_RET(0x007D2C60, signal::clear_callbacks);

	PATCH_PUSH_RET_POLY(0x007D29B0, signal::refresh, "?refresh@signal@@UAEXXZ");
	PATCH_PUSH_RET_POLY(0x007D2B00, signal::add_callback, "?add_callback@signal@@QAEIP6AXPAVsignaller@@PBD@ZPAD_N@Z");

	PATCH_PUSH_RET_POLY(0x007D29D0, signal::add_callback, "?add_callback@signal@@QAEIPAVinstance@script_object@@PAVvm_executable@@PAD_N@Z");

	PATCH_PUSH_RET(0x007D23D0, signal::link);
	PATCH_PUSH_RET(0x007D24F0, signal::unlink);
	PATCH_PUSH_RET(0x007D25B0, signal::clear_links);

	PATCH_PUSH_RET(0x007D2650, signal::find_AND);
	PATCH_PUSH_RET(0x007D26A0, signal::find_OR);
}

void patch_signaller(void)
{
	PATCH_PUSH_RET_POLY(0x004A09B0, signaller::is_a_trigger, "?is_a_trigger@signaller@@UBE_NXZ");
	PATCH_PUSH_RET_POLY(0x004E4800, signaller::is_an_entity, "?is_an_entity@signaller@@UBE_NXZ");

	PATCH_PUSH_RET_POLY(0x004E4820, signaller::construct_signal_list, "?construct_signal_list@signaller@@EAEPAV?$fast_vector@PAVsignal@@@@XZ");
	PATCH_PUSH_RET_POLY(0x004E4890, signaller::get_signal_name, "?get_signal_name@signaller@@MBEPBDG@Z");

	PATCH_PUSH_RET_POLY(0x004A09D0, signaller::raise_signal, "?raise_signal@signaller@@UAEXI@Z");

	PATCH_PUSH_RET_POLY(0x007D3510, signaller::signaller, "??0signaller@@QAE@XZ");

	PATCH_PUSH_RET(0x007D3680, signaller::clear_signals);
	PATCH_PUSH_RET(0x007D3700, signaller::clear_callbacks);

	PATCH_PUSH_RET_POLY(0x007D3B10, signaller::clear_script_callback, "?clear_script_callback@signaller@@QAEXABVstringx@@@Z");
	PATCH_PUSH_RET(0x007D38B0, signaller::clear_script_callbacks);

	PATCH_PUSH_RET(0x007D3B60, signaller::signal_error);
}

void patch_signal_callback(void)
{
	PATCH_PUSH_RET_POLY(0x007D1B70, signal_callback::signal_callback, "??0signal_callback@@QAE@XZ");

	PATCH_PUSH_RET_POLY(0x007D1BC0, signal_callback::is_code_callback, "?is_code_callback@signal_callback@@UAE_NXZ");
	PATCH_PUSH_RET_POLY(0x007D1BE0, signal_callback::is_script_callback, "?is_script_callback@signal_callback@@UAE_NXZ");
}

void patch_script_callback(void)
{

	PATCH_PUSH_RET_POLY(0x007D1C30, script_callback::script_callback, "??0script_callback@@QAE@PAVinstance@script_object@@PBVvm_executable@@PBD@Z");

	// @Note: don't patch virtual destructors really dumb
	// it might not free the memory and cause issues later on
	//PATCH_PUSH_RET_POLY(0x007D1D10, script_callback::~script_callback, "??1script_callback@@UAE@XZ");

	PATCH_PUSH_RET_POLY(0x007D1DD0, script_callback::spawn, "?spawn@script_callback@@UAEXPAVsignaller@@@Z");
	PATCH_PUSH_RET_POLY(0x007D1CF0, script_callback::is_script_callback, "?is_script_callback@script_callback@@UAE_NXZ");
}

void patch_code_callback(void)
{
	PATCH_PUSH_RET_POLY(0x007D1E30, code_callback::code_callback, "??0code_callback@@QAE@P6AXPAVsignaller@@PBD@Z1@Z");
	PATCH_PUSH_RET_POLY(0x007D1F10, code_callback::spawn, "?spawn@code_callback@@UAEXPAVsignaller@@@Z");

	PATCH_PUSH_RET_POLY(0x007D1EA0, code_callback::is_code_callback, "?is_code_callback@code_callback@@UAE_NXZ");
}
