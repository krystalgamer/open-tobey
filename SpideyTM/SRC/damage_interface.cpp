#include "global.h"

#include "damage_interface.h"
#include "entity.h"
//#include "brain.h"
#include "wds.h"
#include "collide.h"
#include "terrain.h"
//#include "dread_net.h"
// BIGCULL #include "handheld_item.h"

#include "app.h"
#include "game.h"
// @Patch - removed
//#include "msgboard.h"
//#include "hwaudio.h"

#if defined(TARGET_XBOX)
#include "handheld_item.h"
#endif /* TARGET_XBOX JIV DEBUG */

bool god_mode_cheat = false;

damage_interface::damage_interface(entity *ent)
  : entity_interface(ent),
  hit_points("", 1000, 0, 1000),
  armor_points("", 0, 0, 1000)
{
  flags = 0;
  destroy_lifetime = 1.0f;
  destroy_sound = empty_string;
  destroy_fx = empty_string;

  destroy_script = empty_string;


  damage_mod = 1.0f;


//  dread_net_cue = dread_net::UNDEFINED_AV_CUE;
}


damage_interface::~damage_interface()
{
}

void damage_interface::copy(damage_interface *b)
{
	PANIC;
}

int damage_interface::apply_damage(entity *attacker, int damage, eDamageType type, const vector3d &pos, const vector3d &dir, int flags, const stringx &wounded_anim)
{
	PANIC;
	return 0;
}

void damage_interface::apply_destruction_fx()
{
	PANIC;
}



void damage_interface::read_enx_data( chunk_file& fs, stringx& lstr )
{
	PANIC;
}



void damage_interface::frame_advance(time_value_t t)
{
  set_flag(_DAMAGED_LAST_FRAME, is_flagged(_DAMAGED_THIS_FRAME));
  set_flag(_DAMAGED_THIS_FRAME, false);

  if(is_flagged(_DAMAGED_LAST_FRAME))
    last_dmg_info = dmg_info;
}

bool damage_interface::get_ifc_num(const pstring &att, rational_t &val)
{
  IFC_INTERNAL_GET_MACRO("HIT_POINTS", hit_points);
  IFC_INTERNAL_GET_MACRO("ARMOR_POINTS", armor_points);
  IFC_INTERNAL_GET_MACRO("MAX_HIT_POINTS", get_max_hit_points());

  IFC_INTERNAL_GET_MACRO("MAX_ARMOR_POINTS", get_max_armor_points());

  return(false);
}

bool damage_interface::set_ifc_num(const pstring &att, rational_t val)
{
  IFC_INTERNAL_SET_MACRO("HIT_POINTS", hit_points);
  IFC_INTERNAL_SET_MACRO("ARMOR_POINTS", armor_points);
  IFC_INTERNAL_FUNC_MACRO("MAX_HIT_POINTS", set_max_hit_points(val));

  IFC_INTERNAL_FUNC_MACRO("MAX_ARMOR_POINTS", set_max_armor_points(val));

  return(false);
}

#include "my_assertions.h"
void validate_damage_interface(void)
{
	VALIDATE(damage_interface, hit_points, 0x8);
}

void validate_bounded_attribute(void)
{
	VALIDATE(bounded_attribute<int>, value, 0x0);
}

#include "my_patch.h"

void patch_damage_interface(void)
{
}
