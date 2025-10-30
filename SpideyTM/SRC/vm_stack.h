// vm_stack.h
#ifndef _VM_STACK_H
#define _VM_STACK_H


class stringx;
class vm_thread;
class signal;

typedef float vm_num_t;

typedef const stringx* vm_str_t;
typedef signal* vm_signal_t;



// CTT 06/15/00: This should be turned off for final release.
#if defined(BUILD_DEBUG) || defined(BUILD_FASTDEBUG)
  #define REPORT_OVERFLOW 1
#endif


class vm_stack
{
  // Data
  public:
    int salloc;
    char* buffer;
    char* SP;
  private:
    // For debugging... to be able to print nice error messages
    vm_thread * my_thread;

  protected:
    EXPORT void init(int sa);
    EXPORT void set_SP(char* _sp) { SP = _sp; }

	// @Ok
	// @Matching
    EXPORT void move_SP(int n)
      {
      assert(!(n&3));
      assert(!((unsigned)SP&3));
      SP += n;

      // check for stack underflow / overflow
      assert( size()>=0 && size()<=capacity() );
      }

    EXPORT vm_num_t& top_num() { return *(vm_num_t*)(SP-sizeof(vm_num_t)); }
    EXPORT vm_str_t& top_str() { return *(vm_str_t*)(SP-sizeof(vm_str_t)); }
    EXPORT vm_signal_t& top_signal() { return *(vm_signal_t*)(SP-sizeof(vm_signal_t)); }

  // Constructors
  public:

    EXPORT vm_stack() : salloc(0),buffer(NULL),SP(NULL), my_thread(NULL) {}
    EXPORT vm_stack(int sa, vm_thread * _my_thread);
    EXPORT ~vm_stack();

  // Methods
  public:
    EXPORT char* get_SP() const  { return SP; }
    EXPORT int size() const      { return SP-buffer; }
    EXPORT int capacity() const  { return salloc; }
    EXPORT vm_thread * get_thread() const {return my_thread;}

    EXPORT void pop(int n)
      {
      move_SP( -n );
      }


    EXPORT vm_num_t pop_num();

    EXPORT vm_str_t pop_str()
      {
      pop(sizeof(vm_str_t));
      return *(vm_str_t*)SP;
      }

    EXPORT vm_signal_t pop_signal()
      {
      pop( sizeof(vm_signal_t) );
      return *(vm_signal_t*)SP;
      }

    EXPORT void* pop_addr()
      {
      pop(sizeof(void*));

      return *(void**)SP;
      }

    EXPORT bool push( const char* src, int n );
    EXPORT bool push(vm_num_t v)
      {
      *(vm_num_t*)SP = v;
      move_SP( sizeof(vm_num_t) );
#if REPORT_OVERFLOW
      // check for stack overflow
      if ( size() > capacity() )

        return false;

#endif
      return true;
      }

    EXPORT bool push(vm_str_t v)
      {

      *(vm_str_t*)SP = v;
      move_SP( sizeof(vm_str_t) );
#if REPORT_OVERFLOW

      // check for stack overflow
      if ( size() > capacity() )
        return false;
#endif

      return true;
      }

    EXPORT bool push( vm_signal_t v )

      {
      *(vm_signal_t*)SP = v;
      move_SP( sizeof(vm_signal_t) );
#if REPORT_OVERFLOW
      // check for stack overflow
      if ( size() > capacity() )
        return false;
#endif
      return true;
      }

    EXPORT bool push(unsigned v)
      {
      *(int*)SP = v;
      move_SP( sizeof(int) );
#if REPORT_OVERFLOW
      // check for stack overflow
      if ( size() > capacity() )
        return false;
#endif
      return true;
      }

  // Friends
  friend class vm_thread;
	friend void validate_vm_stack(void);
	friend void patch_vm_stack(void);
  };


#endif  // _VM_STACK_H
