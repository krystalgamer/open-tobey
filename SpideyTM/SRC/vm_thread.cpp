// vm_thread.cpp
#include "global.h"

#include "vm_thread.h"
#include "vm_executable.h"
#include "opcodes.h"
#include "script_object.h"
#include "oserrmsg.h"
#include "debug.h"
#include "ostimer.h"
#include "terrain.h"
#include "signals.h"
#include "app.h"
//!#include "character.h"
#include "wds.h"

#include "sl_debugger.h"
#include "timer.h"

// CLASS vm_thread

// Constructors

// @TODO - replace this when all usages are using the proper
unsigned int vm_thread::id_counter = 0;
#define GET_ID_COUNTER (*reinterpret_cast<unsigned int*>(0x00B768B0))
//#define GET_ID_COUNTER vm_thread::id_counter

// @Ok
// @PartialMatching - stl on the reserve call
vm_thread::vm_thread()
  : inst(NULL),
    ex(NULL),
    flags( vm_thread::SUSPENDABLE ),
    dstack(),
    PC(NULL),
    PC_stack(),
    entry(script_library_class::function::FIRST_ENTRY)
{
  PC_stack.reserve(8);
  local_region = NULL;
//!  local_character = NULL;
  #if THREAD_PROFILING
  prof_runtime = 0;
  prof_opcount = 0;
  #endif
  my_callback = NULL;
  camera_priority = 0;

  thread_id = ++GET_ID_COUNTER;
}

#ifdef TARGET_XBOX
#pragma warning( disable : 4355 )  // 'this' used in base member initializer list (why is that wrong? -JDF)
// According to VC documentation, "this" is not guaranteed to give a valid pointer until after the constructor

// runs.  But it seems to be working okay.  (dc 05/21/02)
#endif

// @Ok
// @PartialMatching - stl on the reserve call
vm_thread::vm_thread(script_object::instance* i,const vm_executable* x,int sa)
  : inst(i),
    ex(x),
    flags( vm_thread::SUSPENDABLE ),
    dstack(sa,this),
    PC(ex->get_start()),
    PC_stack(),
    entry(script_library_class::function::FIRST_ENTRY)

{
  PC_stack.reserve(8);
  local_region = NULL;
//!  local_character = NULL;
  #if THREAD_PROFILING

  prof_runtime = 0;
  prof_opcount = 0;
  #endif
  my_callback = NULL;
  camera_priority = 0;

  thread_id = ++GET_ID_COUNTER;
}

// create a thread spawned via the given event callback

// @Ok
// @PartialMatching - stl on the reserve call
vm_thread::vm_thread(script_object::instance* i,const vm_executable* x,int sa,script_callback* cb)
  : inst(i),
    ex(x),
    flags( vm_thread::SUSPENDABLE ),
    dstack(sa,this),
    PC(ex->get_start()),
    PC_stack(),
    entry(script_library_class::function::FIRST_ENTRY)
{
  PC_stack.reserve(8);
  local_region = NULL;
//!  local_character = NULL;

  #if THREAD_PROFILING
  prof_runtime = 0;
  prof_opcount = 0;

  #endif
  my_callback = cb;
  // by default, callback is disabled until this thread terminates

  cb->disable();
  camera_priority = 0;

  thread_id = ++GET_ID_COUNTER;
}

// @Ok
// @NotMatching - weird code gen from both regarding freeing memory
vm_thread::~vm_thread()
{
  // if this thread was spawned via an event callback, re-enable the callback here
  if ( my_callback )
    my_callback->enable();
}


// Methods

// @Ok
// @Matching
INLINE void vm_thread::set_suspended(bool v)
{
  if ( !v || is_suspendable() )
    set_flag( SUSPENDED, v );
}

// @Ok
// @Matching
void vm_thread::set_suspendable( bool v )
{
  set_flag( SUSPENDABLE, v);
  if ( !v )
    set_suspended( false );
}

typedef bool (__fastcall *vm_thread_run_ptr)(vm_thread*);

// @TODO
// Execute this thread until interrupted or terminated.
// Return true if the thread should be killed.
bool vm_thread::run()
{
	vm_thread_run_ptr vm_thread_run = (vm_thread_run_ptr)0x007E7750;
	return vm_thread_run(this);
}


bool vm_thread::call_script_library_function( const argument_t& arg, const unsigned short* oldPC )
{
	char* oldSP = dstack.get_SP();
	if ( !((*arg.lfr)(dstack,entry)) )
	{
		// library function has not generated a return value yet;
		// next cycle will call library function again
		PC = oldPC;
		dstack.set_SP( oldSP );
		// flag signifies re-call of library function
		entry = script_library_class::function::RECALL_ENTRY;
		// interrupt thread (will resume next frame)
		return false;
	}

	else
	{
		entry = script_library_class::function::FIRST_ENTRY;  // reset for next library call
		return true;
	}
	return false;
}


void vm_thread::spawn_sub_thread( const argument_t& arg )
{
  // create NEW thread with initial stack data copied from current stack,
  // based on function parameters stack size
  vm_thread* newvmt = inst->add_thread(arg.sfr);

  // spawned thread inherits the suspendability of the parent thread
  newvmt->set_suspendable( is_suspendable() );

  // spawned thread inherits camera priority
  newvmt->set_camera_priority( get_camera_priority() );

  int psize = arg.sfr->get_parms_stacksize();
  if ( psize )
  {
    if ( !newvmt->dstack.push(dstack.get_SP()-psize,psize) )
      slf_error( "stack overflow spawning " + arg.sfr->get_fullname() );
  }
  newvmt->PC = arg.sfr->get_start();
  // pop function parameters from local stack
  dstack.pop( psize );
}


void vm_thread::spawn_parallel_thread( const argument_t& arg )
{
  // create NEW thread with initial stack data copied from current stack,
  // based on function parameters stack size
  script_object::instance* local_inst = (script_object::instance*)dstack.pop_addr();
  if ( local_inst==NULL || (uint32)local_inst==UNINITIALIZED_SCRIPT_PARM )
  {

    slf_error( "(local) spawn " + arg.sfr->get_fullname() + ": invalid local script object instance pointer" );
  }
  vm_thread* newvmt = local_inst->add_thread( arg.sfr );

  // spawned thread inherits the suspendability of the parent thread
  newvmt->set_suspendable( is_suspendable() );


  // spawned thread inherits camera priority
  newvmt->set_camera_priority( get_camera_priority() );

  int psize = arg.sfr->get_parms_stacksize();
  if ( psize )
  {
    if ( !newvmt->dstack.push(dstack.get_SP()-psize,psize) )
      error( local_inst->get_name() + ": stack overflow spawning " + arg.sfr->get_fullname() );
  }
  newvmt->PC = arg.sfr->get_start();
  // pop function parameters from local stack
  dstack.pop( psize );

}



// @Ok
// @Matching
void vm_thread::create_event_callback( const argument_t& arg, bool one_shot )
{
  // pop local script object instance pointer (instance to which NEW thread will be added)
  script_object::instance* local_inst = (script_object::instance*)dstack.pop_addr();
  if ( local_inst==NULL || (uint32)local_inst==UNINITIALIZED_SCRIPT_PARM )
  {
    slf_error( "(local) event callback " + arg.sfr->get_fullname() + ": invalid local script object instance pointer" );
  }
  // pop function parameters
  dstack.pop( arg.sfr->get_parms_stacksize() );
  char* parms = dstack.get_SP();
  // pop signal to which callback applies
  vm_signal_t v = dstack.pop_signal();
  // add script callback with parameters copied from current stack
  v->add_callback( local_inst, arg.sfr, parms, one_shot );
}


// @Ok
// @Matching
void vm_thread::create_static_event_callback( const argument_t& arg, bool one_shot )
{
  // pop function parameters

  dstack.pop( arg.sfr->get_parms_stacksize() );
  char* parms = dstack.get_SP();
  // pop signal to which callback applies
  vm_signal_t v = dstack.pop_signal();
  // add script callback with parameters copied from current stack
  v->add_callback( inst, arg.sfr, parms, one_shot );
}



// Internal Methods

// @Ok
// @Matching
// program counter stack
void vm_thread::pop_PC()
{
  if ( !PC_stack.empty() )
  {
    PC = PC_stack.back();
    PC_stack.pop_back();
  }

  else
    PC = NULL;
}



// @Ok
// @Matching
void vm_thread::remove_from_local_region()
{
  if ( local_region )
  {
    local_region->remove_local_thread(this);
  }
}



// @Ok
// @Matching
void vm_thread::remove_from_local_character()
{
STUBBED(vm_thread_remove_from_local_character, "vm_thread::remove_from_local_character");
/*!  if ( local_character )
  {
    local_character->remove_local_thread(this);
  }
!*/
}


void vm_thread::slf_error( const stringx& err )
{
  // obtain pointer to vm_executable that corresponds to current PC

	// @TODO
	PANIC;
}



void vm_thread::slf_warning( const stringx& err )
{
  // obtain pointer to vm_executable that corresponds to current PC
  const vm_executable* cur_ex = g_world_ptr->get_script_manager()->find_function_by_address( PC );
  if ( cur_ex )
    warning( inst->get_name() + ": " + cur_ex->get_owner()->get_name() + "::" + cur_ex->get_fullname() + ":\n" + err );
  else
    warning( inst->get_name() + ": (UNKNOWN FUNCTION!):\n" + err );
}


// @Ok
// @Matching
void vm_thread::set_camera_priority(rational_t pr)
{
  camera_priority = pr;
}

#include "my_assertions.h"
static void compile_time_assertions()
{
	//StaticAssert<sizeof(vm_thread) == 0x48>::sass();
}

void validate_vm_thread(void)
{
	VALIDATE_SIZE(vm_thread, 0x48);

	VALIDATE(vm_thread, inst, 0x0);
	VALIDATE(vm_thread, ex, 0x4);

	VALIDATE(vm_thread, flags, 0x8);

	VALIDATE(vm_thread, dstack, 0xC);

	VALIDATE(vm_thread, PC, 0x1C);

	VALIDATE(vm_thread, PC_stack, 0x20);

	VALIDATE(vm_thread, entry, 0x2C);

	VALIDATE(vm_thread, my_callback, 0x30);
	VALIDATE(vm_thread, local_region, 0x34);
	VALIDATE(vm_thread, camera_priority, 0x38);

	VALIDATE(vm_thread, prof_runtime, 0x40);
	VALIDATE(vm_thread, prof_opcount, 0x44);
}

void patch_vm_thread(void)
{
	PATCH_PUSH_RET(0x007E76F0, vm_thread::set_suspended);
	PATCH_PUSH_RET(0x007E7720, vm_thread::set_suspendable);

	// @TODO - when region code is done
	//PATCH_PUSH_RET(0x007E93D0, vm_thread::remove_from_local_region);

	PATCH_PUSH_RET(0x007E93F0, vm_thread::remove_from_local_character);

	PATCH_PUSH_RET(0x007E9710, vm_thread::set_camera_priority);

	PATCH_PUSH_RET(0x007E9390, vm_thread::pop_PC);
	PATCH_PUSH_RET(0x007E9340, vm_thread::create_static_event_callback);

	PATCH_PUSH_RET(0x007E9270, vm_thread::create_event_callback);

	PATCH_PUSH_RET(0x007E90B0, vm_thread::set_flag);


	PATCH_PUSH_RET_POLY(0x007E7270, vm_thread::vm_thread, "??0vm_thread@@QAE@XZ");
	PATCH_PUSH_RET_POLY(0x007E73D0, vm_thread::vm_thread, "??0vm_thread@@QAE@PAVinstance@script_object@@PBVvm_executable@@H@Z");
	PATCH_PUSH_RET_POLY(0x007E7540, vm_thread::vm_thread, "??0vm_thread@@QAE@PAVinstance@script_object@@PBVvm_executable@@HPAVscript_callback@@@Z");

	PATCH_PUSH_RET_POLY(0x007E7660, vm_thread::~vm_thread, "??1vm_thread@@QAE@XZ");
}
