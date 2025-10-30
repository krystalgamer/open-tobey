// vm_thread.h
#ifndef _VM_THREAD_H
#define _VM_THREAD_H

#include "warnlvl.h"

// @TODO
#include "script_library_class.h"
#include "script_object.h"
#include "vm_stack.h"


// @Patch
#include "pcglobals.h"


#include <stack>
#include <set>

// turn this on when you want to profile script thread execution at runtime
#define THREAD_PROFILING 1

class vm_executable;
class region;
//!class character;
class script_callback;

class vm_thread
  {

		friend void validate_vm_thread(void);
		friend void patch_vm_thread(void);
  // Types
  protected:
    union argument_t
      {
      vm_num_t val;
      vm_str_t str;
      short word;
      char* sdr;

      script_library_class::function* lfr;

      vm_executable* sfr;
      unsigned binary;
      };

    enum flags_t
      {
      SUSPENDED =   0x0001,
      SUSPENDABLE = 0x0002,
      };


  // Data

  protected:
    // thread identification
    script_object::instance* inst;

    const vm_executable* ex;
    // thread flags
    unsigned int flags;
    // data stack
    vm_stack dstack;
    // program counter
    const unsigned short* PC;
    // program counter stack

	std::vector<const unsigned short*> PC_stack;
    // used when calling library functions

    script_library_class::function::entry_t entry;
    // if thread was spawned by an event callback, this points to the callback definition
    script_callback* my_callback;




    region * local_region;
//!    character * local_character;
	
    rational_t camera_priority;

    static unsigned int id_counter;

    unsigned int thread_id;

  // Constructors
  public:

    EXPORT vm_thread();
    EXPORT vm_thread(script_object::instance* i,const vm_executable* x,int sa);
    EXPORT vm_thread(script_object::instance* i,const vm_executable* x,int sa,script_callback* cb);
    EXPORT ~vm_thread();
    EXPORT region * get_local_region() const {return local_region;}
    EXPORT void set_local_region(region * reg) {local_region = reg; /*! local_character = NULL; !*/}
    EXPORT void remove_from_local_region();

//!    character * get_local_character() const {return local_character;}
//!    void set_local_character(character * chr) {local_character = chr; local_region = NULL;}
    EXPORT void remove_from_local_character();

  // Methods
  public:
    EXPORT script_object::instance* get_instance() const { return inst; }

    EXPORT const vm_executable* get_executable() const { return ex; }

    EXPORT vm_stack& get_data_stack() { return dstack; }

	// @Ok
	// @Matching
    EXPORT void set_flag( flags_t f, bool v ) { flags = v? (flags|f) : (flags&~f); }
    EXPORT bool is_flagged( flags_t f ) const { return flags & f; }

    EXPORT void set_suspended( bool v );
	// @Ok
	// @Matching
    EXPORT bool is_suspended() const { return is_flagged( SUSPENDED ); }
    
    EXPORT void set_suspendable( bool v );
    EXPORT bool is_suspendable() const { return is_flagged( SUSPENDABLE ); }

    EXPORT void set_camera_priority( rational_t pr );
    EXPORT rational_t get_camera_priority() { return(camera_priority); }

    // Execute this thread until interrupted or terminated.
    // Return true if the thread should be killed.
    EXPORT bool run();

    EXPORT void spawn_subthread(vm_executable const * new_ex);

    EXPORT void slf_error( const stringx& err );
    EXPORT void slf_warning( const stringx& err );

    //unsigned int get_thread_id() const     { return(thread_id); }


#if THREAD_PROFILING
    // counts number of operations performed on this thread in a given frame
    float prof_runtime;
    unsigned int prof_opcount;
#endif

  // Internal Methods
  protected:
    // data stack
    EXPORT void dstack_init(int sa)
	{
		// @TODO
		PANIC;
		//dstack.init(sa);
	}
    // program counter stack

    EXPORT void push_PC() { PC_stack.push_back(PC); }

    EXPORT void pop_PC();

    EXPORT bool call_script_library_function( const argument_t& arg, const unsigned short* oldPC );
    EXPORT void spawn_sub_thread( const argument_t& arg );
    EXPORT void spawn_parallel_thread( const argument_t& arg );
    EXPORT void create_event_callback( const argument_t& arg, bool one_shot );
    EXPORT void create_static_event_callback( const argument_t& arg, bool one_shot );


  // Friends

  friend class script_object::instance;
  friend class script_object;            // remove me on sight -- gt
  friend void validate_vm_thread(void);
  };


#endif  // _VM_THREAD_H
