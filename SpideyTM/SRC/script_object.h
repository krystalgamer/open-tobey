// script_object.h
#ifndef _SCRIPT_OBJECT_H
#define _SCRIPT_OBJECT_H

#ifndef __THROW_BAD_ALLOC
#define __THROW_BAD_ALLOC
#endif


// @Note - can use this to make it match but nah, kinda goofy
//#define _NOTHREADS
#define __STL_NO_EXCEPTION_HEADER
#define __STL_NO_EXCEPTIONS
#define __STL_USE_SGI_ALLOCATORS


#include "stringx.h"
#include "vm_symbol_list.h"
#include "vm_executable.h"
#include "so_data_block.h"
#include "chunkfile.h"
#include "ostimer.h"
#include <list>
#include <set>
#include <map>

class classreg;

class vm_thread;
class script_callback;


enum
  {
  USE_SUSPENDED = 0,
  IGNORE_SUSPENDED = 1
  };


class script_object
  {
	friend void validate_script_object(void);
	friend void patch_script_object(void);
  // Types
  public:
    class instance
      {
      // Types

		typedef std::list<vm_thread*> thread_list;

      // Data
      protected:
        stringx name;
        so_data_block data;
        thread_list threads;
        bool suspended;


      // Constructors
      public:
        EXPORT instance(const stringx& n,int sz);
        EXPORT ~instance();
      protected:

        instance();

      // Methods
      public:
        EXPORT const stringx& get_name() const { return name; }
        EXPORT char* get_buffer() const { return data.get_buffer(); }

        // thread management
        //const thread_list& get_threads() const { return threads; }
        EXPORT vm_thread* add_thread(const vm_executable* ex);
        EXPORT vm_thread* add_thread( const vm_executable* ex, const char* parms );

        // spawn a NEW thread via the given event callback
        EXPORT vm_thread* add_thread( script_callback* cb, const vm_executable* ex, const char* parms );

        EXPORT vm_thread* spawn_subthread(const vm_executable* ex);

		// @Patch - added parameter
        EXPORT void kill_thread(const vm_executable* ex, const vm_thread* thr);

		// @Ok
		// @Matching
        EXPORT bool has_threads() const { return threads.size()? true : false; }

        // execute all threads
        EXPORT void run(bool ignore_suspended);
        // run a single thread
        EXPORT void run_single_thread( vm_thread* t, bool ignore_suspended );

        EXPORT void suspend();
        EXPORT void unsuspend();

        // for debugging purposes; dump information on all threads to a file
        EXPORT void dump_threads( host_system_file_handle outfile ) const;
        EXPORT void clear_callback_references( script_callback *remove_me );

        EXPORT bool thread_exists(vm_thread* thread) const;
        EXPORT bool thread_exists(unsigned int thread_id) const;
        EXPORT bool thread_exists(vm_thread* thread, unsigned int thread_id) const;


#if _CONSOLE_ENABLE
        void dump_threads_to_console() const;
#endif


      // Friends
      friend class script_object;
      friend void validate_script_object_instance(void);
      friend void patch_script_object_instance(void);
      };

    // instance management

    typedef std::list<instance*> instance_list;
    typedef std::set<stringx> instance_name_list;

  // Data
  protected:
    stringx name;

    bool global_object;

#ifndef _RELEASE
    // debug info
    vm_symbol_list static_symbols;
    vm_symbol_list symbols;
#endif

    // static data

    so_data_block static_data;
    // non-static data
    int data_blocksize;

    // script functions
	std::vector<vm_executable*> funcs;

    // instance management
    instance_list instances;

    #ifndef BUILD_BOOTABLE
    instance_name_list instance_names;

    #endif

  // Constructors
  public:
    EXPORT script_object();
    //script_object(const script_object& b);
    EXPORT script_object(const classreg& cl,bool debuginfo);  // compiler support
    EXPORT ~script_object();

  // Methods
  public:
EXPORT void check_all_instances(); // debugging code, remove me please!!! -GT

    EXPORT const stringx& get_name() const { return name; }
    EXPORT bool is_global_object() const { return global_object; }
    EXPORT char* get_static_data_buffer() const { return static_data.get_buffer(); }
    EXPORT int get_static_data_size() const { return static_data.size(); }
    EXPORT const vm_executable& get_func(int i) const { return *funcs[i]; }
    EXPORT int find_func(const stringx& func_fullname) const;

    // return index of function corresponding to given PC (-1 if not found)
    EXPORT int find_func_by_address( const unsigned short* PC ) const;

    // Link all SDR, SFR, and LFR references in the script functions to the
    // appropriate run-time addresses found via the given script manager.
    EXPORT void link(const script_manager& sm);


    // succeeds only if script object has a default constructor
    EXPORT instance* create_auto_instance();

    // instance management
    EXPORT instance* find_instance(const stringx& name) const;

    EXPORT instance* add_instance(const stringx& inst_name, chunk_file* fs_ptr=NULL,
                           char* implict_parms_buffer=NULL, int parm_count=0,
                           const vm_executable::parms_list* check_parms=NULL);
    EXPORT instance* add_instance( const stringx& inst_name,
                            char* constructor_parms_buffer );

    EXPORT int get_num_instances() const { return instances.size(); }

    EXPORT vm_thread* add_thread(instance* inst,int fidx);


    EXPORT bool has_threads() const;

    // execute all threads on all instances of this script object
    EXPORT void run(bool ignore_suspended);

    // for debugging purposes; dump information on all threads to a file
    EXPORT void dump_threads( host_system_file_handle outfile ) const;

#if _CONSOLE_ENABLE
    EXPORT void dump_threads_to_console() const;
#endif

  // Internal Methods
  protected:
    EXPORT void destroy();
    EXPORT void _clear();


    EXPORT void add(script_object::instance* inst);

  // Friends
#if !defined(NO_SERIAL_IN)
  friend void serial_in(chunk_file& io,script_object* so);
#endif

#if !defined(NO_SERIAL_OUT)
  friend void serial_out(chunk_file& io,const script_object& so);
#endif
  friend class script_manager;
  };

const chunk_flavor CHUNK_NUM_SCRIPT_OBJECTS   ("Nscrobjs");
const chunk_flavor CHUNK_SCRIPT_OBJECT        ("scrobj");
const chunk_flavor CHUNK_STATIC_SYMBOLS       ("Nstatic");

const chunk_flavor CHUNK_STATIC_BLOCKSIZE     ("statsize");
const chunk_flavor CHUNK_DATA_SYMBOLS         ("Ndata");
const chunk_flavor CHUNK_DATA_BLOCKSIZE       ("datasize");
const chunk_flavor CHUNK_FUNCS                ("Nfuncs");
const chunk_flavor CHUNK_GLOBAL               ("global");
const chunk_flavor CHUNK_STANDARD             ("standard");


// CLASS script_manager:
// Manages a list of script objects; provides a method for linking compiled
// member references to the corresponding run-time addresses.
class script_manager
  {
	friend void patch_script_manager(void);
	friend void validate_script_manager(void);
  // Types
  public:
	  class sobj_less : public std::binary_function<const script_object*,const script_object*,bool>
      {
      public:
        bool operator()(const script_object* a,const script_object* b) const
          {
          return (a->get_name() < b->get_name());

          }
      };
    typedef std::list<script_object*> sobj_list;
    typedef std::map<stringx,script_object*> name_sobj_map;
    typedef std::set<stringx> string_set_t;

  // Data
  protected:
    sobj_list script_objects;
    name_sobj_map script_objects_by_name;
    string_set_t string_set;
    time_value_t time_inc;

    int field_20;
    int field_24;
    int field_28;
    int field_2C;

  // Constructors
  public:
    EXPORT script_manager();
    EXPORT ~script_manager();

  // Methods
  public:
	EXPORT void check_all_objects( ); // debugging code, please remove me!  --GT
    EXPORT script_object* find_object(const stringx& name) const;

    // return pointer to executable that corresponds to given PC (NULL if not found)
    EXPORT const vm_executable* find_function_by_address( const unsigned short* PC ) const;

    EXPORT void load(const char* filename);

    // Link all SDR, SFR, and LFR references found in the managed script object
    // member functions to the appropriate run-time addresses.
    // NOTE:  This should be performed once only, after all script objects
    // have been loaded and added to the manager.  (We are forced to make load
    // and link separate steps in order to allow linking to script object
    // instances.)
    EXPORT void link();

    // execute all threads on all script object instances
    EXPORT void run(time_value_t t = 0.0f, bool ignore_suspended = USE_SUSPENDED);


    inline time_value_t get_time_inc() const { return(time_inc); }

    EXPORT bool has_threads() const;

    // for debugging purposes; dump information on all threads to a file
    void dump_threads() const;

#if _CONSOLE_ENABLE
    void dump_threads_to_console() const;
#endif

    EXPORT stringx const * add_string(stringx const & st);

  // Internal Methods
  protected:
    EXPORT void destroy();
    EXPORT void clear();
    EXPORT void _add(script_object* so);
  };


#define SCRIPT_TIME_INC (g_world_ptr->get_script_manager()->get_time_inc())

#endif  // _SCRIPT_OBJECT_H
