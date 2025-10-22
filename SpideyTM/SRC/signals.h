#ifndef _SIGNALS_H
#define _SIGNALS_H


#include "warnlvl.h"

#include "singleton.h"
#include "fast_vector.h"
#include "script_object.h"


// @Patch
#include "pcglobals.h"

#define DEBUG_SIGNAL_NAMES 1
class signaller;
class signal;

class signal_callback
{
	friend void validate_signal_callback(void);
	friend void patch_signal_callback(void);
 public:

	// @Ok
	// @Matching
  EXPORT signal_callback(){
	  disabled=one_shot=false;
	  // @Patch - infix instead of postfix
	  id=++id_counter;

	  // @Patch - make it non-zero
	  if (!id)
	  {
		  id=++id_counter;
	  }
  }

  // @Ok
  // @Matching
  EXPORT virtual ~signal_callback(){}

  // @Ok
  // @Matching
  EXPORT virtual void spawn(signaller*sgrptr=0) = 0;

  EXPORT void disable() { disabled = true; }
  EXPORT void enable() { disabled = false; }
  // @Ok
  // @Matching
  EXPORT bool is_disabled() const { return disabled; }

  EXPORT void set_one_shot( bool tf ) { one_shot = tf; }
  EXPORT bool is_one_shot() const { return one_shot; }

  EXPORT unsigned int get_id() const { return(id); }

  // @Ok
  // @Matching
  EXPORT virtual bool is_code_callback() { return(false); }
  // @Ok
  // @Matching
  EXPORT virtual bool is_script_callback() { return(false); }

 protected:
    char* parms;
    bool disabled;
    bool one_shot;
    unsigned int id;

    EXPORT static unsigned int id_counter;

};


class script_callback : public signal_callback
  {
	friend void validate_script_callback(void);
	friend void patch_script_callback(void);
  private:
    script_object::instance* inst;
    const vm_executable* func;

  public:
    EXPORT script_callback( script_object::instance* _inst, const vm_executable* _func, const char* _parms );
    EXPORT virtual ~script_callback();

	// @Ok
	// @Matching
    EXPORT virtual bool is_script_callback() { return(true); }
    EXPORT const stringx &get_func_name();

	  EXPORT virtual void spawn(signaller*sgrptr=0);
  };

class code_callback : public signal_callback
  {
	friend void validate_code_callback(void);
	friend void patch_code_callback(void);
  public:
    EXPORT code_callback( void (*fn)(signaller*,const char*), const char *cptr );
    EXPORT virtual ~code_callback();

    EXPORT virtual void spawn(signaller*sgrptr=0);

	// @Ok
	// @Matching
    EXPORT virtual bool is_code_callback() { return(true); }

  private:
    void (*func)(signaller*,const char*);
  };

// @TODO
class signal
  {
	  friend void patch_signal(void);
	  friend void validate_signal(void);
  // Types
  public:
    enum flavor_t
      {
      SIGNAL,
      GATED_SIGNAL,
      N_FLAVORS
      };

    enum flags_t
      {
      RAISED             = 0x0001,  // true when signal has been raised this frame
      NEEDS_REFRESH      = 0x0002,  // true when signal has been added to the refresh list this frame (see signal_manager)
      DISABLED           = 0x0004,  // true when signal is disabled
      CALLBACKS_DISABLED = 0x0008,  // true when signal callbacks are disabled
      };

    typedef std::list<signal*> signal_list;
    typedef std::list<signal_callback*> callback_list;

  // Data
  protected:
    flavor_t flavor;
  private:
    unsigned int flags;

    #if DEBUG_SIGNAL_NAMES
    const char* name;
    #endif



    signal_list* outputs;
    callback_list callbacks;

  // Methods
  public:
    EXPORT signal(signaller*sgrptr=0);
    EXPORT signal( const char* _name, signaller*sgrptr=0 );
    EXPORT virtual ~signal();

	// @Ok
	// @Matching
    EXPORT flavor_t get_flavor() const { return flavor; }

    EXPORT void set_flag( flags_t f ) { flags |= f; }
    EXPORT void clear_flag( flags_t f ) { flags &= ~f; }
    EXPORT bool is_flagged( flags_t f ) const { return (flags & f); }

    // tell the signal_manager that this signal needs to be refreshed this frame
    EXPORT void set_needs_refresh();

    // add an output link
    EXPORT void link( signal* s );
    // remove an output link
    EXPORT void unlink( signal* s );

    EXPORT void clear_links();

    // find an output gated_signal matching the given parameters
    EXPORT signal* find_AND( const signal* b ) const;
    EXPORT signal* find_OR( const signal* b ) const;

    // raise this signal!
    EXPORT void raise();

    // was I raised?
    EXPORT bool raised() { return(is_flagged(RAISED)); }

    // add a script callback for this signal
    EXPORT unsigned int add_callback( script_object::instance* _inst, vm_executable* _func, char* _parms, bool one_shot=false );
    EXPORT unsigned int add_callback( void (*fn)(signaller*,const char*), char* _parms, bool one_shot=false );
    // clear the script callback list
    EXPORT void kill_callback(unsigned int callback_id);

    EXPORT void clear_callbacks();
    EXPORT void clear_script_callbacks();
    EXPORT void clear_code_callbacks();
    EXPORT void clear_script_callback(const stringx &name);

    // This virtual function performs an internal reset (once per frame, initiated by the
    // signal_manager; see below) of ephemeral changes accumulated while raising signals
    // in the course of a game frame.
    EXPORT virtual void refresh();

  private:
    signaller *owner;

    // process the raising of an input
    EXPORT virtual void raise_input( signal* input, signaller*sgrptr=0 );

    // spawn script callbacks, if any
    EXPORT void do_callbacks();
  };



class gated_signal : public signal
  {
  // Types
  public:
    enum type_t
      {
      AND,
      OR
      };
    enum flags_t
      {
      RAISED_A = 0x0001,  // input_a was raised
      RAISED_B = 0x0002,  // input_b was raised
      };

    // Data
  private:
    unsigned short type;
    unsigned short flags;
    signal* input_a;
    signal* input_b;

  // Methods
  public:
    gated_signal( type_t _type, signal* _input_a, signal* _input_b );

    void set_flag( flags_t f ) { flags |= f; }
    void clear_flag( flags_t f ) { flags &= ~f; }
    bool is_flagged( flags_t f ) const { return (flags & f); }

    // return true if parameters match this gated_signal
    bool match( type_t _type, const signal* input ) const;

    // This virtual function performs an internal reset (once per frame, initiated by the
    // signal_manager; see below) of ephemeral changes accumulated while raising signals
    // in the course of a game frame.
    virtual void refresh();

  private:
    // raise given input and return true if gate condition is satisfied
    virtual void raise_input( signal* input );
  };


class signaller
  {
  // Types
  public:
    enum flags_t
      {
      DISABLED = 0x0001,  // true if signaller is disabled (cannot raise signals)
      };

    typedef fast_vector< signal* > signal_list;

    enum signal_id_t
      {
      N_SIGNALS
      };

  // Data
  private:
    unsigned int flags;
    signal_list* signals;

	int field_C[2];

  // Methods
  public:
    EXPORT signaller();
    virtual ~signaller();

    EXPORT void set_flag( flags_t f ) { flags |= f; }
    EXPORT void clear_flag( flags_t f ) { flags &= ~f; }
    EXPORT bool is_flagged( flags_t f ) const { return (flags & f); }

    EXPORT virtual bool is_an_entity() const { return(false); }
    EXPORT virtual bool is_a_trigger() const { return(false); }
//    virtual bool is_dread_net() const { return(false); }

    EXPORT virtual void signal_error(unsigned int, const stringx&);

    EXPORT void disable() { set_flag(DISABLED); }
    EXPORT void enable() { clear_flag(DISABLED); }

    EXPORT signal_list::size_t n_signals() const { return signals? signals->size() : 0; }

    // given local signal id, return corresponding pointer;
    // this will construct objects as required, and never return NULL
    EXPORT signal* signal_ptr( signal_list::size_t idx )
      {
      if ( signals == NULL )
        signals = construct_signal_list();
      signal_list& sl = *signals;
      if ( sl[idx] == NULL )
        {
        #if DEBUG_SIGNAL_NAMES
        sl[idx] = NEW signal( get_signal_name(idx), this );
        #else
        sl[idx] = NEW signal(this);
        #endif
        }
      return sl[idx];
      }

	// this will raise the given signal, if present (non-NULL)
	// @Ok
	// @Matching
	EXPORT virtual void raise_signal( signal_list::size_t idx )
	{
		if (!is_flagged(DISABLED))
		{
			// @Patch - build signal table
			if (!this->signals)
			{
				this->signals = this->construct_signal_list();
			}

			signal_list& sl = *signals;
			if ( sl[idx] )
			{
				sl[idx]->raise();
			}

			// @Patch - add write
			signal_list::size_t v3 = idx;
			if (idx >= 0x20)
			{
				v3 -= 0x20;
			}

			this->field_C[idx < 0x20] |= 0x80000000 >> v3;
		}

	}

    EXPORT static unsigned short get_signal_id( const char *name )
      {
        return (unsigned short)-1;    // return invalid id in order for caller to print complete error msg (with entity name)
      }


    void clear_callbacks();
    void clear_script_callbacks();
    void clear_code_callbacks();
    void clear_script_callback(const stringx &name);

/*
    bool signal_was_raised(signal_list::size_t sig_id)
    {
      if ( signals == NULL )
        signals = construct_signal_list();
      signal_list& sl = *signals;
      return(sl[idx] != NULL && sl[idx]->raised());
    }
*/

  private:
    // Every descendant of signaller that expects to generate signals and has
    // defined its own local list of signal ids should implement this virtual
    // function for the construction of the signal list, so that it will reserve
    // exactly the number of signal pointers required, on demand.

	// @Ok
	// @Matching
    EXPORT virtual signal_list* construct_signal_list() { return NEW signal_list( N_SIGNALS, (signal*)NULL ); }

  protected:
    // This virtual function, used only for debugging purposes, returns the
    // name of the given local signal
	// @Ok
	// @Matching
    EXPORT virtual const char* get_signal_name( unsigned short idx ) const { return ""; }
	
	friend void validate_signaller(void);
	friend void patch_signaller(void);
  };


  class signal_manager : public singleton
  {
	  friend void validate_signal_manager(void);
	  friend void patch_signal_manager(void);
  // SINGLETON
  public:
    // returns a pointer to the single instance
	  // @Patch - manual signgleton
    //DECLARE_SINGLETON( signal_manager )
	static inline signal_manager* inst()
	{
		// @Hardcoded
		return *reinterpret_cast<signal_manager**>(0x00948C28);
	}

  // Constructors (not public in a singleton)
  private:
    signal_manager();
    signal_manager( const signal_manager& );
    signal_manager& operator=( const signal_manager& );

  // Types
  public:
	  typedef std::map< stringx, unsigned short > signal_id_map_t;
	  typedef std::vector< signal* > signal_list;

  // Data
  private:
    signal_id_map_t signal_id_map;
    signal_list refresh_list;

	PADDING(0x28-0x1C);

  // Methods
  public:
    // return signaller-local index value for signal
    unsigned short get_id( const stringx& name ) const;
    // insert a NEW signal name and associated signaller-local index
    void insert( const stringx& name, unsigned short id );

    // create NEW signal consisting of logical AND of given signals
    signal* signal_AND( signal* a, signal* b ) const;
    // create NEW signal consisting of logical OR of given signals
    signal* signal_OR( signal* a, signal* b ) const;

    // this function is called when a signal object is affected such that it
    // will need to be reset at the end of the game frame (see app:tick)
    void needs_refresh( signal* s );
    // this function is called once per game frame to reset any signals that need it
    void do_refresh();

    void purge(); // PEH BETA LOCK
  };


#endif
