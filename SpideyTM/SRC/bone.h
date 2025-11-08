#ifndef BONE_CLASS_HEADER
#define BONE_CLASS_HEADER


#include "link_interface.h"
#include "signals.h"

/*** ENT_INTERFACE macro ***/

class po;
class sector;
class skeleton_interface;

/*** class bone ***/
class bone : public signaller
{
  protected:
    po my_rel_po;
    po *my_abs_po;

	unsigned int bone_flags;
  ENTITY_INTERFACE(link)

	// @Patch - added missing enum
	// @Neat
	enum bone_flags_t
	{
		BONE_UNK_ONE = 1,
	};

  public:
    EXPORT bone()
    {
		// @TODO
		PANIC;
    }

    EXPORT virtual ~bone() {
		// @TODO
		PANIC;
    }

	EXPORT virtual int get_bone_idx(void) { return -1; }

  /*** has_parent ***/
    EXPORT bool has_parent() const { return (has_link_ifc() && link_ifc()->get_parent()); }

  /*** has_children ***/
    EXPORT bool has_children() const { return (has_link_ifc() && link_ifc()->get_first_child()); }


	// @Ok
	// @Patch - added functions
	EXPORT bool get_bone_flag(bone_flags_t f) const
	{
		return this->bone_flags & f;
	}

	// @Ok
	// @Patch - added functions
	EXPORT void set_bone_flag(bone_flags_t f, bool b) const
	{
		// @Neat - wtf why is this as thing
		bone *pBone = const_cast<bone*>(this);
		if (b)
			pBone->bone_flags |= f;
		else
			pBone->bone_flags &= ~f;
	}

	// @Patch - added functions
	EXPORT void clear_bone_flag(bone_flags_t f) const
	{
		// @Neat - wtf why is this as thing
		bone *pBone = const_cast<bone*>(this);

		pBone->bone_flags &= ~f;
	}

  /*** po ***/
    EXPORT const po& get_rel_po() const { return my_rel_po; }


    EXPORT const po *get_rel_po_ptr() const { return &my_rel_po; }

    EXPORT const po& get_abs_po() const;

    EXPORT po *get_abs_po_ptr()
    {
      return my_abs_po;
    }

	// @Patch - arg
    EXPORT void update_abs_po(bool);

	EXPORT void set_handed_axis(int axis)
	{
		// @TODO
		PANIC;
	}

	EXPORT void UpdateHandedness(void)
	{
		// @TODO
		PANIC;
	}

    EXPORT void update_abs_po_no_children()
    {
      if (has_link_ifc())
      {
        if (link_ifc()->get_parent())
        {
          fast_po_mul(*my_abs_po, get_rel_po(), link_ifc()->get_parent()->get_abs_po());
//          *my_abs_po = get_rel_po() * link_ifc()->get_parent()->get_abs_po();
        }
        else
          *my_abs_po = get_rel_po();
      }
    }

	// @Patch - add const
    // Used in very specific cases
    EXPORT void update_abs_po_reverse() const;

    EXPORT void set_rel_po(const po & the_po)
    {
#if defined(TARGET_XBOX)
      assert( the_po.get_position().is_valid() );
#endif /* TARGET_XBOX JIV DEBUG */

      my_rel_po = the_po;
	  // @Patch - arg
      update_abs_po(true);
      po_changed();

#if defined(TARGET_XBOX)

      assert( my_rel_po.get_position().is_valid() );
      assert( my_abs_po->get_position().is_valid() );
#endif /* TARGET_XBOX JIV DEBUG */
    }

    EXPORT void set_rel_po_no_children(const po & the_po)
    {
      assert(has_link_ifc());
      my_rel_po = the_po;
      po_changed();
    }


    EXPORT void reset_scale ();
    EXPORT virtual void po_changed(); // called whenever po changes

  /*** position ***/

    EXPORT const vector3d& get_rel_position() const { return my_rel_po.get_position(); }

    EXPORT void set_rel_position(const vector3d &p)
    {
      my_rel_po.set_position(p);
	  // @Patch - arg
      update_abs_po(true);
      po_changed();
    }
    EXPORT void set_rel_position_no_children(const vector3d &p)
    {
      assert(has_link_ifc());
      my_rel_po.set_position(p);
      po_changed();
    }

    EXPORT const vector3d& get_abs_position() const;

  /*** orientation ***/
    EXPORT const orientation get_rel_orientation() const { return my_rel_po.get_orientation(); }
    EXPORT void set_rel_orientation(const orientation& o)
    {
      my_rel_po.set_orientation(o);
	  // @Patch - arg
      update_abs_po(true);
      po_changed();

    }
    EXPORT void set_rel_orientation_no_children(const orientation& o)
    {
      assert(has_link_ifc());

      my_rel_po.set_orientation(o);
      po_changed();
    }


    EXPORT const orientation get_abs_orientation() const

    {
      return my_abs_po->get_orientation();
    }


  public:
	// @Patch - had to add
	EXPORT void dirty_family(bool);

  friend link_interface::~link_interface();
  friend void link_interface::set_parent(bone *new_parent);
  friend class skeleton_interface;

  friend void validate_bone(void);
  friend void patch_bone(void);
};

#endif//BONE_CLASS_HEADER
