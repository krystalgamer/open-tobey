#include "script_object.h"
#include "vm_thread.h"

#include "errorcontext.h"

#define VM_STACKSIZE 0x180

INLINE script_object::instance::instance(const stringx& n,int sz)
: name( n ),
  data( sz ),
  threads(),
  suspended( false )
{
}

// @Ok
// @PartialMatching - stl
script_object::instance::~instance()
{
  for ( thread_list::iterator i=threads.begin(); i!=threads.end(); ++i )
    delete *i;
  threads.resize(0);
}

// @Ok
// @NotMatching - the thread-safety on lists
INLINE vm_thread* script_object::instance::add_thread(const vm_executable* ex)
{
	vm_thread* nt = NEW vm_thread(this,ex,VM_STACKSIZE);
  assert(nt != NULL);
  threads.push_back(nt);
  // if this script object instance is suspended, we must suspend any thread
  // that is added here
  if ( suspended )
    nt->set_suspended( true );

  return nt;
}


// @Ok
// @Matching
vm_thread* script_object::instance::add_thread( const vm_executable* ex, const char* parms )
{
	vm_thread* nt = add_thread( ex );
	assert(nt != NULL);

	if ( parms && !nt->get_data_stack().push( parms, ex->get_parms_stacksize() ) )
	{
		error( get_name() + ": stack overflow spawning " + ex->get_fullname() );
	}

	nt->PC = ex->get_start();
	return nt;
}

// @Ok
// @Matching
// spawn a NEW thread via the given event callback
vm_thread* script_object::instance::add_thread( script_callback* cb, const vm_executable* ex, const char* parms )
{
	vm_thread* nt = NEW vm_thread( this, ex, VM_STACKSIZE, cb );
	assert(nt != NULL);
	threads.push_back( nt );
	// if this script object instance is suspended, we must suspend any thread
	// that is added here
	if ( suspended )
	{
		nt->set_suspended( true );
	}
	if ( parms && !nt->get_data_stack().push( parms, ex->get_parms_stacksize() ) )
	{
		error( get_name() + ": stack overflow spawning " + ex->get_fullname() );
	}

	nt->PC = ex->get_start();
	return nt;
}

// @Ok
// @NotMatching - list thread safety
void script_object::instance::kill_thread(const vm_executable* ex, const vm_thread* thr)
{
	thread_list::iterator ti = threads.begin();
	while (ti != threads.end())
	{
		vm_thread* t = *ti;
		assert(t != NULL);
		if ( t->get_instance()==this && t->get_executable()==ex && t != thr)
		{
			// found matching thread (same script object instance, same function);
			// kill it dead
			t->remove_from_local_region();
			t->remove_from_local_character();
			//krPrintf("killing thread %s %s\n", t->ex->get_fullname().c_str(), t->inst->get_name().c_str());
			delete t;
			ti = threads.erase( ti );  // erase returns next value for iterator
		}
		else
		{
			++ti;
		}
	}
}

// @Ok
// @NotMatching - list thread safety
void script_object::instance::run(bool ignore_suspended)
{
#if defined(TARGET_PC) && !defined(BUILD_BOOTABLE)
	if(g_script_debugger_running)
	{
		g_sl_debugger.set_new_instance(this, threads.size());
	}
#endif

	thread_list::iterator i = threads.begin();

	while ( i != threads.end() )
	{
		vm_thread* t = *i;
		assert(t != NULL);
		if ( ignore_suspended || !t->is_suspended() )
		{
			// execute thread (will run until finished or interrupted)
#if _CONSOLE_ENABLE && 0
			console_process("console_log c:\\console.txt", 0);

			console_log("execing thread %s %s\n", t->ex->get_fullname().c_str(), t->inst->get_name().c_str());
			console_process("console_log end", 0);
#endif

			if ( t->run() )
			{
				// thread has asked to be killed
				t->remove_from_local_region();
				t->remove_from_local_character();

				//krPrintf("removing thread %s %s\n", t->ex->get_fullname().c_str(), t->inst->get_name().c_str());
				delete t;

				i = threads.erase( i );  // erase returns next value for iterator
			}
			else
			{
				++i;
			}
		}
		else
		{
			++i;
		}
	}
}



// @Ok
// @NotMatching - list thread safety
void script_object::instance::run_single_thread( vm_thread* t, bool ignore_suspended )
{
	if ( ignore_suspended || !t->is_suspended() )
	{
		// execute thread (will run until finished or interrupted)
		if ( t->run() )
		{
			// thread has asked to be killed
			t->remove_from_local_region();
			t->remove_from_local_character();
			delete t;
			// remove thread from list

			//krPrintf("I shouldn't be called - thread %s %s\n", t->ex->get_fullname().c_str(), t->inst->get_name().c_str());

			thread_list::iterator i = threads.begin();

			for ( ; i!=threads.end(); ++i )
			{
				if ( *i == t )
				{
					threads.erase( i );  // erase returns next value for iterator
					break;
				}
			}
		}
	}
}


// @Ok
// @Matching
void script_object::instance::suspend()
{
  thread_list::iterator i = threads.begin();
  thread_list::iterator i_end = threads.end();
  for ( ; i!=i_end; ++i )
    {
    vm_thread* t = *i;
    assert(t != NULL);
    t->set_suspended( true );
    }

  suspended = true;
}

// @Ok
// @Matching
void script_object::instance::unsuspend()
{
	  thread_list::iterator i = threads.begin();
  thread_list::iterator i_end = threads.end();
  for ( ; i!=i_end; ++i )
    {
    vm_thread* t = *i;
    assert(t != NULL);
    t->set_suspended( false );

    }
  suspended = false;
}

// @Ok
// @Matching
// for bug purposes; fixes a dangling pointer
void script_object::instance::clear_callback_references( script_callback *remove_me )
{
	thread_list::const_iterator i = threads.begin();
	thread_list::const_iterator i_end = threads.end();
	for ( ; i!=i_end; ++i )
	{
		vm_thread* t = *i;

		assert(t != NULL);

		if (t->my_callback == remove_me)
		  t->my_callback = NULL;
	}

}

// @Ok
// @Matching
// for debugging purposes; dump information on all threads to a file
void script_object::instance::dump_threads( host_system_file_handle outfile ) const
{

	thread_list::const_iterator i = threads.begin();
	thread_list::const_iterator i_end = threads.end();
	for ( ; i!=i_end; ++i )
	{
		vm_thread* t = *i;

		if (!t->is_suspended())
		{

#if THREAD_PROFILING
			host_fprintf( outfile, "%s %s %f %i\n", name.c_str(), t->get_executable()->get_name().c_str(), t->prof_runtime, t->prof_opcount );
#else
			host_fprintf( outfile, "%s %s\n", name.c_str(), t->get_executable()->get_name().c_str() );
#endif
		}
	}
}

// @Ok
// @Matching
bool script_object::instance::thread_exists(vm_thread* thread) const
{
	thread_list::const_iterator i = threads.begin();
	thread_list::const_iterator i_end = threads.end();
	for ( ; i!=i_end; ++i )
	{
		if((*i) == thread)
			return(true);
	}

	return(false);
}

// @Ok
// @Matching
bool script_object::instance::thread_exists(unsigned int thread_id) const
{

	thread_list::const_iterator i = threads.begin();
	thread_list::const_iterator i_end = threads.end();
	for ( ; i!=i_end; ++i )
	{
		if((*i)->thread_id == thread_id)
			return(true);
	}


	return(false);

}


// @Ok
// @Matching
bool script_object::instance::thread_exists(vm_thread* thread, unsigned int thread_id) const
{
	thread_list::const_iterator i = threads.begin();
	thread_list::const_iterator i_end = threads.end();

	for ( ; i!=i_end; ++i )
	{
		if((*i) == thread)
		return((*i)->thread_id == thread_id);
	}

	return(false);
}

// CLASS script_object

// Constructors

script_object::script_object()

:   name(),
    global_object(false),

#ifndef _RELEASE
    static_symbols(),
    symbols(),
#endif
    static_data(),
    data_blocksize(0),

    funcs(),
    instances()
{
}


// @Ok
// @PartialMatching - stl goofyness
script_object::~script_object()
{
	destroy();
}


// Methods

int script_object::find_func( const stringx& func_fullname ) const
{
	// @TODO
	PANIC;
  int i = 0;
  std::vector<vm_executable*>::const_iterator fi = funcs.begin();
  std::vector<vm_executable*>::const_iterator fi_end = funcs.end();

  for ( ; fi!=fi_end; ++fi,++i )
  {
    if ( (*fi)->get_fullname() == func_fullname )

      return i;
  }
  return -1;

  }

// return index of function corresponding to given PC (-1 if not found)


int script_object::find_func_by_address( const unsigned short* PC ) const
{
	// @TODO
	PANIC;
  int i = 0;
  std::vector<vm_executable*>::const_iterator fi = funcs.begin();
  std::vector<vm_executable*>::const_iterator fi_end = funcs.end();
  for ( ; fi!=fi_end; ++fi,++i )
  {

    vm_executable* ex = *fi;
    if ( (uint32)PC>=(uint32)ex->get_start() && (uint32)PC<(uint32)(ex->get_start()+ex->get_size()) )
      return i;
  }
  return -1;
}


// @Ok
// @Matching
INLINE void script_object::link(const script_manager& sm)
{
	std::vector<vm_executable*>::iterator xi;
	for ( xi=funcs.begin(); xi!=funcs.end(); ++xi )
	{
		(*xi)->link( sm );
	}
}

// @Ok
// @Matching
script_object::instance* script_object::find_instance( const stringx& name ) const
{
	instance_list::const_iterator i = instances.begin();
	instance_list::const_iterator i_end = instances.end();
	for ( ; i!=i_end; ++i )
	{

		instance* inst = *i;
		if ( inst->get_name() == name )
		{
			return inst;
		}
	}
	return NULL;
}

// Internal Methods

INLINE void script_object::destroy()
{
	instance_list::iterator ii;
	for ( ii=instances.begin(); ii!=instances.end(); ++ii )
	{
		delete *ii;
	}

	std::vector<vm_executable*>::iterator vi;
	for ( vi=funcs.begin(); vi!=funcs.end(); ++vi )
	{
		delete *vi;
	}
}

void script_object::_clear()
{
	// @TODO
	PANIC;
  destroy();
  static_symbols.resize(0);

  symbols.resize(0);
  static_data.clear();
  funcs.resize(0);
}


INLINE void script_object::add( instance* inst )
{
#ifndef BUILD_BOOTABLE
	pair<instance_name_list::iterator,bool> iret = instance_names.insert( inst->get_name() );
	if ( !iret.second )
	{
		error( "Duplicate script instance name " + inst->name );
	}
#endif
	instances.push_back( inst );
}

// @Ok
// @NotMatching - thread safety in list
// Build and add a NEW instance of this script object and initialize a
// constructor thread with the instance (implicit THIS) pointer plus the given
// stack data.
script_object::instance*
script_object::add_instance( const stringx& inst_name,
                             char* constructor_parms_buffer )
{

	error_context::inst()->push_context(inst_name);
	instance* inst = NEW instance( inst_name, data_blocksize );

	add( inst );

	// find constructor
	const vm_executable& con = get_func(0);
	assert( con.get_name() == name );
	// create constructor thread
	vm_thread* con_thread = inst->add_thread( &con );
	// push implicit THIS pointer
	con_thread->get_data_stack().push( (char*)&inst, 4 );
	// push additional parameters
	int parmsize = con.get_parms_stacksize();
	if ( !con.is_static() )
	{
		parmsize -= 4;
	}

	con_thread->get_data_stack().push( constructor_parms_buffer, parmsize );

	error_context::inst()->pop_context();

	return inst;
}


// @Ok
// @Matching
// add a thread to run the given member function on the given instance
vm_thread* script_object::add_thread( instance* inst, int fidx )
{
	// create thread for given member function
	assert(fidx<(int)funcs.size());
	vm_thread* nt = inst->add_thread( funcs[fidx] );

	// push implicit THIS parameter (not necessary for static member functions,
	// but since this is a NEW thread it won't hurt anything)
	nt->get_data_stack().push((char*)&inst,4);
	return nt;
}

// @Ok
// @Matching
INLINE bool script_object::has_threads() const
{
	instance_list::const_iterator i = instances.begin();
	instance_list::const_iterator i_end = instances.end();
	for ( ; i!=i_end; ++i )
	{
		instance* inst = *i;
		if ( inst->has_threads() )
		{
			return true;
		}
	}

	return false;
}

// @Ok
// @Matching
// execute all threads on all instances of this object
void script_object::run( bool ignore_suspended )
{
#if defined(TARGET_PC) && !defined(BUILD_BOOTABLE)
	if(g_script_debugger_running)
	g_sl_debugger.set_new_object(this, instances.size());
#endif

	// remove me on sight!!! debugging only
	//check_all_instances();


	for ( instance_list::iterator i=instances.begin(); i!=instances.end(); ++i )
	{
		instance* inst = *i;
		inst->run( ignore_suspended );
	}
}


// CLASS script_manager

// Constructors

// @Ok
// @PartialMatching - stl goofyness
script_manager::script_manager()
: script_objects(),
  script_objects_by_name()
{
  time_inc = 0.0f;

  field_20 = 0;
  field_24 = 0;
  field_28 = 0;
  field_2C = 0;
}

script_manager::~script_manager()
{
	this->clear();
}


// Methods

INLINE void script_manager::clear()
{
	if (this->field_28)
	{
		file_manager::inst()->nullsub_for_script_manager(this->field_28);
		this->field_28 = 0;
		this->field_2C = 0;
	}

  destroy();
}

// @Ok
// @Matching
script_object* script_manager::find_object( const stringx& name ) const
{
  name_sobj_map::const_iterator i = script_objects_by_name.find( name );
  if ( i == script_objects_by_name.end() )
    return NULL;
  else
    return (*i).second;
}


// return pointer to executable that corresponds to given PC (NULL if not found)
const vm_executable* script_manager::find_function_by_address( const unsigned short* PC ) const
{
	// @TODO
	/*
  sobj_list::const_iterator i = script_objects.begin();
  sobj_list::const_iterator i_end = script_objects.end();
  for ( ; i!=i_end; ++i )
  {
    script_object* so = *i;
    int f = so->find_func_by_address( PC );
    if ( f != -1 )
      return &so->get_func( f );
  }
  */
	PANIC;
  return NULL;
}


#ifndef NO_SERIAL_IN
void script_manager::load(const char* filename)
  {
	  // @TODO
	  PANIC;
	  /*
  chunk_file io;

  chunk_flavor cf;
  io.open(filename);
  serial_in( io, &cf );
  if ( cf != chunk_flavor("scrobjs") )
    error( stringx(filename) + ": bad format; file must be updated" );
  for (serial_in(io,&cf); cf!=CHUNK_END; serial_in(io,&cf))
    {
    if (cf == CHUNK_SCRIPT_OBJECT)
      {
      script_object* so = NEW script_object;

if(so==0)
warning(stringx(filename)+" NULL");

      // begin script object chunk
      serial_in( io, &so->name );
      serial_in( io, so );
      _add( so );
      }
    else
      error( stringx(filename) + ": bad format; file must be updated" );
    }
	*/
  }
#endif

// @Ok
// @Matching
void script_manager::link()
{
  sobj_list::const_iterator i = script_objects.begin();
  sobj_list::const_iterator i_end = script_objects.end();
  for ( ; i!=i_end; ++i )
    (*i)->link( *this );
}

// execute all threads on all script object instances
void script_manager::run(time_value_t t, bool ignore_suspended )
{
#ifndef PROJECT_KELLYSLATER

  time_inc = t;


#if defined(TARGET_PC) && !defined(BUILD_BOOTABLE)
  if(g_script_debugger_running)
    g_sl_debugger.set_new_frame(script_objects.size());
#endif


  sobj_list::const_iterator i = script_objects.begin();
  sobj_list::const_iterator i_end = script_objects.end();

  for ( ; i!=i_end; ++i )
  {
    script_object* so = *i;
    if ( so->get_num_instances() )
    {
      so->run( ignore_suspended );

    }
  }

#endif
}

// check all script objects, debugging code, please remove me if I'm still here
void script_manager::check_all_objects( )
{
	// @TODO
	/*
  sobj_list::const_iterator i = script_objects.begin();
  sobj_list::const_iterator i_end = script_objects.end();
  for ( ; i!=i_end; ++i )
  {
    script_object* so = *i;

//    if ( so->get_num_instances() )
    if (so != NULL)
    {
      so->check_all_instances();
    }
  }
  */
	PANIC;
}


bool script_manager::has_threads() const
{
	PANIC;
  sobj_list::const_iterator i = script_objects.begin();
  sobj_list::const_iterator i_end = script_objects.end();
  for ( ; i!=i_end; ++i )
  {
    script_object* so = *i;
    if ( so->has_threads() )
      return true;
  }
  return false;
}


// for debugging purposes; dump information on all threads to a file
void script_manager::dump_threads() const
{
	PANIC;
	// @TODO
	/*
  host_system_file_handle outfile = host_fopen( "\\steel\\dump\\scriptdump.txt", HOST_WRITE );
  host_fprintf( outfile, "instance thread time ops\n" );
  sobj_list::const_iterator i = script_objects.begin();

  sobj_list::const_iterator i_end = script_objects.end();
  for ( ; i!=i_end; ++i )
  {
    script_object* so = *i;
    so->dump_threads( outfile );
  }
  host_fclose( outfile );
  */
}

#if _CONSOLE_ENABLE
void script_manager::dump_threads_to_console() const

{
  console_log("" );
  console_log("instance   thread   time   ops" );

  sobj_list::const_iterator i = script_objects.begin();
  sobj_list::const_iterator i_end = script_objects.end();
  for ( ; i!=i_end; ++i )
  {
    script_object* so = *i;
    so->dump_threads_to_console();
  }
}
#endif

stringx const* script_manager::add_string( const stringx& s)
{
  return &(*(string_set.insert(s).first));
}

// Internal Methods

INLINE void script_manager::destroy()
{
  sobj_list::const_iterator i = script_objects.begin();
  sobj_list::const_iterator i_end = script_objects.end();
  for ( ; i!=i_end; ++i )

  {
    // destroy fake script_library_class entry for this script object
    slc_manager::inst()->destroy( (*i)->name );
    delete *i;
  }

  script_objects.resize(0);
  script_objects_by_name.clear();
  string_set.clear();
}

void script_manager::_add( script_object* so )
{
	//@TODO
	/*
  // add to name (searchable) map
  typedef script_object* soptr_t;

  soptr_t& mapped_so = script_objects_by_name[ so->get_name() ];
  assert( mapped_so == NULL );
  mapped_so = so;
  // add to simple list
  if ( so->is_global_object() )
    script_objects.push_front( so );  // global script object must run first!
  else
    script_objects.push_back( so );
  // add fake script_library_class entry to support script object linkage
  script_library_class* new_slc = NEW slc_script_object_t( *this, so->name.c_str() );
  // do a fake call on the NEW entry to make GNUC happy
  new_slc->get_size();
  */
}



#include "my_assertions.h"
static void compile_time_assertions()
{
	StaticAssert<sizeof(vm_thread) == 0x48>::sass();
	StaticAssert<sizeof(script_object::instance) == 0x18>::sass();
}


void validate_script_object_instance(void)
{
	VALIDATE_SIZE(script_object::instance, 0x18);
	VALIDATE_SIZE(std::list<vm_thread*>, 0x4);

	VALIDATE(script_object::instance, name, 0x0);
	VALIDATE(script_object::instance, data, 0x8);
	VALIDATE(script_object::instance, threads, 0x10);
	VALIDATE(script_object::instance, suspended, 0x14);
}

void validate_script_object(void)
{
	VALIDATE(script_object, funcs, 0x20);
	VALIDATE(script_object, instances, 0x2C);
}

void validate_script_manager(void)
{
	// @TODO - validate this size is fine
	VALIDATE_SIZE(script_manager, 0x30);

	VALIDATE(script_manager, script_objects, 0x0);
	VALIDATE(script_manager, script_objects_by_name, 0x4);
	VALIDATE(script_manager, string_set, 0x10);

	VALIDATE(script_manager, time_inc, 0x1C);

	VALIDATE(script_manager, field_20, 0x20);
	VALIDATE(script_manager, field_24, 0x24);
	VALIDATE(script_manager, field_28, 0x28);
	VALIDATE(script_manager, field_2C, 0x2C);
}

#include "my_patch.h"

void patch_script_manager(void)
{
	PATCH_PUSH_RET(0x007E06A0, script_manager::find_object);
	PATCH_PUSH_RET(0x007E0E30, script_manager::link);

	PATCH_PUSH_RET_POLY(0x007E0260, script_manager::script_manager, "??0script_manager@@QAE@XZ");
}

void patch_script_object(void)
{
	PATCH_PUSH_RET(0x007DF950, script_object::has_threads);
	PATCH_PUSH_RET(0x007DEED0, script_object::link);

	PATCH_PUSH_RET(0x007DF140, script_object::find_instance);

	PATCH_PUSH_RET(0x007DF9A0, script_object::run);
	PATCH_PUSH_RET_POLY(0x007DF880, script_object::add_thread, "?add_thread@script_object@@QAEPAVvm_thread@@PAVinstance@1@H@Z");
	PATCH_PUSH_RET_POLY(0x007DF720, script_object::add_instance, "?add_instance@script_object@@QAEPAVinstance@1@ABVstringx@@PAD@Z");

	PATCH_PUSH_RET_POLY(0x007DEAC0, script_object::script_object, "??0script_object@@QAE@XZ");
}

void patch_script_object_instance(void)
{
	PATCH_PUSH_RET(0x007DE8C0, script_object::instance::suspend);
	PATCH_PUSH_RET(0x007DE900, script_object::instance::unsuspend);

	PATCH_PUSH_RET(0x007DE940, script_object::instance::clear_callback_references);
	PATCH_PUSH_RET(0x007DE980, script_object::instance::dump_threads);

	PATCH_PUSH_RET_POLY(0x007DE340, script_object::instance::add_thread, "?add_thread@instance@script_object@@QAEPAVvm_thread@@PBVvm_executable@@@Z");
	PATCH_PUSH_RET_POLY(0x007DE420, script_object::instance::add_thread, "?add_thread@instance@script_object@@QAEPAVvm_thread@@PBVvm_executable@@PBD@Z");
	PATCH_PUSH_RET_POLY(0x007DE540, script_object::instance::add_thread, "?add_thread@instance@script_object@@QAEPAVvm_thread@@PAVscript_callback@@PBVvm_executable@@PBD@Z");
	PATCH_PUSH_RET(0x007DE800, script_object::instance::run_single_thread);
	PATCH_PUSH_RET(0x007DE6A0, script_object::instance::kill_thread);
	PATCH_PUSH_RET(0x007DE740, script_object::instance::run);

	PATCH_PUSH_RET_POLY(0x007DE9F0, script_object::instance::thread_exists, "?thread_exists@instance@script_object@@QBE_NPAVvm_thread@@@Z");
	PATCH_PUSH_RET_POLY(0x007DEA30, script_object::instance::thread_exists, "?thread_exists@instance@script_object@@QBE_NI@Z");
	PATCH_PUSH_RET_POLY(0x007DEA70, script_object::instance::thread_exists, "?thread_exists@instance@script_object@@QBE_NPAVvm_thread@@I@Z");

}
