#ifndef _SIGNALS_H
#define _SIGNALS_H


#include "singleton.h"
#include "fast_vector.h"

class signaller;


// @TODO
class signal
{};


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

  // Methods
  public:
    signaller();
    virtual ~signaller();


    void set_flag( flags_t f ) { flags |= f; }
    void clear_flag( flags_t f ) { flags &= ~f; }
    bool is_flagged( flags_t f ) const { return (flags & f); }

    virtual bool is_an_entity() const { return(false); }
    virtual bool is_a_trigger() const { return(false); }
//    virtual bool is_dread_net() const { return(false); }

    void disable() { set_flag(DISABLED); }
    void enable() { clear_flag(DISABLED); }

    signal_list::size_t n_signals() const { return signals? signals->size() : 0; }

// @TODO
    // given local signal id, return corresponding pointer;
    // this will construct objects as required, and never return NULL
/*
    signal* signal_ptr( signal_list::size_t idx )
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
*/


    // this will raise the given signal, if present (non-NULL)
    virtual void raise_signal( signal_list::size_t idx ) const
      {
      if ( signals && !is_flagged(DISABLED) )
        {
        signal_list& sl = *signals;

// @TODO
/*
        if ( sl[idx] )
          sl[idx]->raise();
*/
        }
      }

    static unsigned short get_signal_id( const char *name )
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
    virtual signal_list* construct_signal_list() { return NEW signal_list( N_SIGNALS, (signal*)NULL ); }

  protected:
    // This virtual function, used only for debugging purposes, returns the
    // name of the given local signal
    virtual const char* get_signal_name( unsigned short idx ) const { return ""; }
  };

#endif
