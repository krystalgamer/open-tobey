#ifndef SINGLETON_H
#define SINGLETON_H
////////////////////////////////////////////////////////////////////////////////

#include "global.h"


// a version of singleton that gives us full control over its
// creation and deletion.

// Note that instance will NOT be available until after the object
// has been fully constructed and operator NEW returns control to
// create_inst()!!  Similar restrictions exist for delete_inst()
class singleton

{
  protected:
    singleton() {}
    virtual ~singleton() {}
  private: // no copying allowed
    singleton(const singleton&);
    singleton& operator=(const singleton&);
};

#endif // SINGLETON_H
