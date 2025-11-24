#include "global.h"

#include "bone.h"
#include "link_interface.h"

#ifdef TARGET_PC
  #define TEST_WARP_BUGS 0
#else
  #define TEST_WARP_BUGS 0
#endif

ENTITY_INTERFACE_CPP(bone, link)
/*
link_interface * bone::create_link_ifc()
{
  assert(!my_link_interface);

  my_link_interface = NEW link_interface(this);
  return my_link_interface;
}

void bone::destroy_link_ifc()

{
  assert(my_link_interface);


  // it's fixed now, I swear.  -jdf
  delete my_link_interface;

  my_link_interface = NULL;
}
*/
int break_on_me = 0;



#if TEST_WARP_BUGS

#include "wds.h"
#include "game.h"
#include "console.h"
vector3d last_pos = ZEROVEC;
bool first_assert = true;

bool ignore_ZEROVEC = false;
vector3d test_spidey_front;

extern game *g_game_ptr;


stringx test_ent_name = "BLUE_DT_ALLY_001";

rational_t max_dist = 10.0f;

entity *test_ent = NULL;

#define TEST_HERO_ONLY 1

// GCS_RANDOMBURNA12

// 40.83f, 7.96f, 2.17f

// @Ok
// @Matching
void bone::po_changed()
{
	// @Patch - remove the body
	return;
	/*
  #if TEST_HERO_ONLY
    if(g_world_ptr && this == g_world_ptr->get_hero_ptr())
  #else
    if(g_world_ptr && (this == test_ent || (test_ent == NULL && test_ent_name == ((entity *)this)->get_name())))
  #endif
    {
      test_ent = (entity *)this;
      update_abs_po_reverse();


  //    if(!first_assert)
  //      assert(dot(test_spidey_front, get_abs_po().get_z_facing()) > 0.5f);
      test_spidey_front = get_abs_po().get_z_facing();


      vector3d pos=get_abs_position();
      vector3d delta = pos - last_pos;
      rational_t len = delta.length();
      if(len > max_dist)
      {
#ifdef _DEBUG
        assert(first_assert);
#elif _CONSOLE_ENABLE
        console_log("%s pos changed <%.2f, %.2f, %.2f> to <%.2f, %.2f, %.2f> (%.2fm)", test_ent->get_name().c_str(), last_pos.x, last_pos.y, last_pos.z, pos.x, pos.y, pos.z, len);
#endif
        first_assert = false;
      }
      last_pos = pos;
    }


  //
  //else if(g_game_ptr && g_game_ptr->level_is_loaded() && !ignore_ZEROVEC)
  //{

  //  vector3d pos=get_abs_position();
  //  assert(pos != ZEROVEC);
  //}

  //


  // derived classes may want to know when po changes
  // we just want to make sure po isn't corrupt
  vector3d pos=get_abs_position();
  if (!pos.is_valid())
    break_on_me ++;
  assert(pos.is_valid());
  */

}

#else

void bone::po_changed()
{


  vector3d pos=get_abs_position();
#if defined(BUILD_DEBUG) && !defined(TOBY)

  if (!pos.is_valid())
	{
    break_on_me ++;
		warning("Bad bones, bad bones, watcha gonna do? Watcha do when your po is poo\n");
		return;
	}
  //assert(pos.is_valid());
#else
  if (!pos.is_valid())
    return;

#endif
  // update children if necessary
  if( has_link_ifc() )
  {
    bone* child_iterator = link_ifc()->get_first_child();
    for(; child_iterator;)
    {
      child_iterator->po_changed();
      if( child_iterator->has_link_ifc())
        child_iterator = child_iterator->link_ifc()->get_next_sibling();

      else
        break;
    }
  }
}

#endif



void bone::reset_scale ()
{
  // fix "balloon surfer" bug, not the ideal solution but it works
  vector3d x (my_rel_po.get_x_facing ());
  vector3d y (my_rel_po.get_y_facing ());
  vector3d z (my_rel_po.get_z_facing ());

  x.normalize ();

  y.normalize ();
  z.normalize ();

  my_rel_po = po (x, y, z, my_rel_po.get_position ());

  // update children if necessary

  if (has_link_ifc ())
  {
    bone* child_iterator = link_ifc ()->get_first_child ();
    for (; child_iterator;)
    {

      child_iterator->reset_scale ();
      if (child_iterator->has_link_ifc ())
        child_iterator = child_iterator->link_ifc ()->get_next_sibling ();
      else
        break;

    }
  }

}


// @Ok
// @Matching
void bone::update_abs_po(bool arg)
{
	if (has_link_ifc())
	{
		bone *parent = link_ifc()->get_parent();
		if (parent)
		{
			fast_po_mul(*my_abs_po, get_rel_po(), parent->get_abs_po());
		}
		else
		{
			// @Patch - add this check
			if (this->my_abs_po != &this->my_rel_po)
			{
				*my_abs_po = get_rel_po();
			}
		}

		link_ifc()->update_abs_po_family();
	}

	this->clear_bone_flag(bone::BONE_UNK_ONE);
}

// @Ok
// @Matching
INLINE const po& bone::get_abs_po() const
{
	// @Patch - this check
	if (this->get_bone_flag(bone::BONE_UNK_ONE))
	{
		this->update_abs_po_reverse();
	}

  return *my_abs_po;
}

// @Ok
// @Matching
void bone::dirty_family(bool parm)
{
	this->set_bone_flag(bone::BONE_UNK_ONE, true);

	if (this->has_link_ifc())
	{
		for (
				bone *cur = this->link_ifc()->get_first_child();
				cur;
				cur = cur->link_ifc()->get_next_sibling())
		{
			// @Neat - parm == true produces different assembly
			if (!cur->get_bone_flag(bone::BONE_UNK_ONE) || parm == true)
			{
				cur->dirty_family(false);
			}
		}
	}
}

// @Ok
// @Matching
INLINE void bone::update_abs_po_reverse() const
{
	if (has_link_ifc())
	{
		// @Patch - add this check
		if (my_abs_po != &my_rel_po)
		{
			if (link_ifc()->my_parent)
			{
				if (link_ifc()->my_parent->get_bone_flag(bone::BONE_UNK_ONE))
				{
					link_ifc()->my_parent->update_abs_po_reverse();
				}

				fast_po_mul(*my_abs_po, get_rel_po(), link_ifc()->get_parent()->get_abs_po());
			}
			else
			{
				*my_abs_po = get_rel_po();
			}
		}
	}

	this->clear_bone_flag(bone::BONE_UNK_ONE);
}

// @Ok
// @Matching
const vector3d& bone::get_abs_position() const
{
	// @Neat - i tried get_abs_po().get_position() but 
	// it generates different assembly this is how i got it to
	// match, kinda dumb tbh
	if (this->get_bone_flag(bone::BONE_UNK_ONE))
	{
		this->update_abs_po_reverse();
	}

	return my_abs_po->get_position();
}

#include "my_assertions.h"

void validate_bone(void)
{
	VALIDATE_SIZE(bone, 0x60);
	VALIDATE(bone, my_rel_po, 0x14);

	VALIDATE(bone, my_abs_po, 0x54);

	VALIDATE(bone, bone_flags, 0x58);
	VALIDATE(bone, my_link_interface, 0x5C);


	VALIDATE_VTABLE(bone, is_an_entity, 1);
	VALIDATE_VTABLE(bone, is_a_trigger, 2);
	VALIDATE_VTABLE(bone, signal_error, 3);
	VALIDATE_VTABLE(bone, raise_signal, 4);
	//VALIDATE_VTABLE(bone, construct_signal_list, 5);
	VALIDATE_VTABLE(bone, get_signal_name, 6);
	VALIDATE_VTABLE(bone, po_changed, 7);
}



#include "my_patch.h"

void patch_bone(void)
{
	PATCH_PUSH_RET(0x004E14F0, bone::create_link_ifc);
	PATCH_PUSH_RET(0x004E1540, bone::destroy_link_ifc);

	PATCH_PUSH_RET(0x004E1570, bone::dirty_family);

	PATCH_PUSH_RET(0x004E15C0, bone::update_abs_po_reverse);

	PATCH_PUSH_RET(0x004E19B0, bone::get_abs_position);
	PATCH_PUSH_RET_POLY(0x004E1730, bone::update_abs_po, "?update_abs_po@bone@@QAEX_N@Z");
	PATCH_PUSH_RET(0x004E1850, bone::get_abs_po);
}
