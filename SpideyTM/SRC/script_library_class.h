// script_library_class.h
#ifndef _SCRIPT_LIBRARY_CLASS_H
#define _SCRIPT_LIBRARY_CLASS_H


#include "stringx.h"
#include "singleton.h"

#include <set>
#include <vector>


class vm_stack;
class script_object;
class script_manager;
class chunk_file;


class script_library_class
{
	friend void patch_script_library_class(void);
	friend void validate_script_library_class(void);
// Types
public:
  // SUBCLASS script_library_class::function
  // Base class for all script library functions, which provide the interface
  // from the scripting language to the runtime application.
  class function
  {
		friend void patch_script_library_class_function(void);
		friend void validate_script_library_class_function(void);
	// Types
	public:
	enum entry_t
	{
	  FIRST_ENTRY,
	  RECALL_ENTRY
	};

	// Data
	protected:
	char* name;


	// Constructors
	public:
		EXPORT function(script_library_class* slc,const char* n);
		EXPORT function(const char* n);
	protected:
		EXPORT function(int dummy);

	EXPORT function(const stringx& n);  // provided for script_library_class::find()
	EXPORT virtual ~function();

	// Methods
	public:
		EXPORT const char* get_name() const { return name; }

		// @Ok
		// @Matching
		// Pure virtual function supplies library function execution.
		// Takes vm_stack for obtaining script input parameters; returns true
		// when library function execution is completed and script return value
		// (if any) has been moved onto stack in place of script input
		// parameters.
		// NOTE:  This function will be called repeatedly (once per application
		// frame) on a given thread until it returns true.  For each false
		// return, the vm_thread caller will restore the program counter and
		// stack pointer to their original values so that the calling
		// instruction can be executed again the following frame.  Thus, the
		// library function must not alter the contents of the vm_stack until it
		// has decided to return true.
		EXPORT virtual bool operator()(vm_stack& stack,entry_t entry){return true;}

		// Friends
		friend class script_library_class;
  };

  class function_cptr_less : public std::binary_function<const function*,const function*,bool>
  {
  public:
    EXPORT bool operator()(const function* a,const function* b) const
    {
      return ( strcmp( a->get_name(), b->get_name() )<0 );
    }
  };
  typedef std::set<const function*,function_cptr_less> function_list;

// Data
protected:
  stringx name;
  int size;
  const char* parent_name;  // used for linking the hierarchy after static initialization
  const script_library_class* parent;  // will be NULL until slc_manager::link_hierarchy() is called (which is performed once only, after static initialization)
  function_list funcs;

// Constructors
public:
  EXPORT script_library_class(const char* n,int sz,const char* p=NULL);
  EXPORT virtual ~script_library_class();
protected:
  EXPORT script_library_class();  // provided for searching (see slc_manager::find())
private:
  EXPORT script_library_class(const script_library_class& src) {}


// Methods
public:
  EXPORT const stringx& get_name() const { return name; }
  EXPORT int get_size() const { return size; }
  EXPORT const script_library_class* get_parent() const { return parent; }

  // return true if b or one of its ancestors points to me;
  // note that this notion of equivalence relies on pointer equivalence,
  // which is the intended paradigm for script library classes
  EXPORT bool is_equal_or_descendent(const script_library_class* b) const
  {
    for(; b && b!=this; b=b->get_parent()) continue;
    return (b? true : false);
  }


  EXPORT void add(const function* f)  { funcs.insert(f); }
  EXPORT const function* find(const char* n) const;

  EXPORT const function* find(const stringx& n) const  { return find(n.c_str()); }

  // @Ok
  // @Matching
  // Virtual function performs lookup for class value references (OP_ARG_CLV)
  // during the run-time link process (see vm_executable::_link()).
  // This operation is only valid for library classes whose vm_stack data
  // size is equal to 4 bytes.
  EXPORT virtual unsigned find_instance(const stringx& n) const  { return 0; }

  // @Ok
  // @Matching
  // Virtual function provides method to read library class value data from a
  // stream to the given buffer.
  EXPORT virtual void read_value(chunk_file& fs,char* buf) {}

  // @Ok
  // @Matching
  EXPORT virtual void purge() {}  // for descendents with managed stuff, such as slc_str_t
  // actually, slc_str_t is the only thing that uses this.  

// Friends
  friend class slc_manager;
};


//extern script_library_class* slc_global;
// @TODO - remove when all patched
#define GET_SLC_GLOBAL (*reinterpret_cast<script_library_class**>(0x00B75964))

///////////////////////////////////////////////////////////////////////////////
// SINGLETON CLASS slc_manager:

// Manages a global list of script library classes, which organize the interface
// functions that scripts call to access the application at runtime.
class slc_manager : public singleton //! auto_singleton
{
	friend void validate_slc_manager(void);
	friend void patch_slc_manager(void);
  // singleton support:
  // Use slc_manager::inst()->function_name(parameters);
public:
//!  DECLARE_AUTO_SINGLETON(slc_manager);
	// @Patch - remove
  //DECLARE_SINGLETON(slc_manager);

	static inline slc_manager* inst(void)
	{
		// @Hardcoded
		return *reinterpret_cast<slc_manager**>(0x00B75924);
	}

  // Types

public:
	class class_less : public std::binary_function<const script_library_class*,const script_library_class*,bool>
  {
  public:
    EXPORT bool operator()(const script_library_class* a,const script_library_class* b) const
    {
      return (a->get_name() < b->get_name());
    }
  };
	typedef std::set<script_library_class*,class_less> class_list;

// Data
protected:
  class_list classes;

// Constructors (not public in a singleton)
protected:
  EXPORT slc_manager();

  EXPORT slc_manager(const slc_manager& b);
  EXPORT slc_manager& operator=(const slc_manager& b);
public:
  EXPORT ~slc_manager();

// Methods
public:
  EXPORT void add(script_library_class* slc);
  EXPORT void link_hierarchy();  // call this after static initialization, to establish parent links
  EXPORT script_library_class* find(const char* n) const;
  EXPORT script_library_class* find(const stringx& n) const { return find(n.c_str()); }
  EXPORT void destroy( const stringx& n );

  EXPORT void purge();
};



///////////////////////////////////////////////////////////////////////////////
// CLASS slc_script_object_t supports dummy slc entries for script objects, so
// that they may be referenced and linked at runtime.
class slc_script_object_t : public script_library_class
{
	friend void validate_slc_script_object_t(void);
	friend void patch_slc_script_object_t(void);

// Data
protected:
  const script_manager& sman;
  script_object* sobj;

public:
  // constructor required

  EXPORT slc_script_object_t(const script_manager& sm,const char* n);

#if !defined(NO_SERIAL_IN)
  // read a script object value (by id) from a stream
  EXPORT virtual void read_value(chunk_file& fs,char* buf);
#endif
  // find named instance of script object
  EXPORT virtual unsigned find_instance(const stringx& n) const;
};


///////////////////////////////////////////////////////////////////////////////
// CLASS slc_num_t supports script language built-in type NUM
class slc_num_t : public script_library_class
{

public:
  // constructor required
  EXPORT slc_num_t(const char* n,int sz,const char* p=NULL);
#if !defined(NO_SERIAL_IN)
  // read a num value from a stream
  EXPORT void read_value(chunk_file& fs,char* buf);
#endif
};

extern slc_num_t* slc_num;


///////////////////////////////////////////////////////////////////////////////
// CLASS slc_str_t supports script language built-in type STR
class slc_str_t : public script_library_class
{
private:
	std::vector<stringx*> strings;
public:
  // constructor required
  EXPORT slc_str_t(const char* n,int sz,const char* p=NULL);
  // destructor needed to delete managed strings
  EXPORT ~slc_str_t();
#if !defined(NO_SERIAL_IN)
  // read a str value from a stream
  EXPORT void read_value(chunk_file& fs,char* buf);
#endif
  EXPORT void purge();
};


extern slc_str_t* slc_str;

///////////////////////////////////////////////////////////////////////////////
// Error message function for script library classes
EXPORT void verify_parms_integrity( script_library_class::function* the_function,
                             vm_stack* the_stack,
                             unsigned int* parms,

                             int parms_size );

// @Patch - different value for spidey
const unsigned int UNINITIALIZED_SCRIPT_PARM = 0x7BAD05CF;


///////////////////////////////////////////////////////////////////////////////
// Useful macros for script library functions


// Use this at the start of any library function that requires persistent local
// vm_stack data (should only apply to functions that use SLF_RECALL; see below).
// NOTE:  This macro must be called before SLF_PARMS.
#define SLF_SDATA \
  assert( (int)stack.size()+(int)sizeof(sdata_t) <= (int)stack.capacity() ); \
  sdata_t* sdata = (sdata_t*)stack.get_SP()

// Use this to set up access to parameters on vm_stack (if any).
#ifdef DEBUG
#define SLF_PARMS \
  assert( (sizeof(parms_t) & 3) == 0 ); \
  stack.pop(sizeof(parms_t)); \
  parms_t* parms = (parms_t*)stack.get_SP(); \
  verify_parms_integrity(this, &stack, (unsigned int *) parms, sizeof(parms_t)/4)
#else
#define SLF_PARMS \
  stack.pop(sizeof(parms_t)); \
  parms_t* parms = (parms_t*)stack.get_SP()
#endif

// Setup stack, but don't create unused local
#define SLF_PARMS_UNUSED \
  stack.pop(sizeof(parms_t));


// Use this to push a return value onto the vm_stack.
#if defined(TARGET_MKS)  

#define SLF_RETURN \
  if (!stack.push((char*)&result,sizeof(result)) ) \
    error( "script stack overflow" )
    
#else 
   
#define SLF_RETURN \
  if (!stack.push((char*)&result,sizeof(result)) ) \
    error( stack.get_thread()->get_instance()->get_name() + ": stack overflow in " + stack.get_thread()->get_executable()->get_fullname() )
    
#endif

// Use this to be re-called next frame.
#define SLF_RECALL \
  return false


// Use this to signal termination.
#define SLF_DONE \
  return true

// Function provided by the application to register all known script library classes.

extern void register_script_libs();

#endif  // _SCRIPT_LIBRARY_CLASS_H
