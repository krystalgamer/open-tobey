#ifndef ENTITY_BOX_TRIGGER_INTERFACE_CLASS_HEADER
#define ENTITY_BOX_TRIGGER_INTERFACE_CLASS_HEADER


#include "global.h"
#include "entity_interface.h"
#include "bound.h"
#include "convex_box.h"

class box_trigger_interface : public entity_interface
{
	friend void validate_box_trigger_interface(void);
	friend void patch_box_trigger_interface(void);
protected:
  convex_box box;

public:

  EXPORT box_trigger_interface( entity* e ) : entity_interface( e ) { }
  EXPORT virtual ~box_trigger_interface( void ) { }

  EXPORT void set_box_info( const convex_box& _box ) { box = _box; }
  EXPORT convex_box& get_box_info( void ) { return box; }
};

#endif
