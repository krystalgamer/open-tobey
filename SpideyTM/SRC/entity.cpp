#include "entity.h"

// @Ok
// @Matching
INLINE char* strdupcpp(const char* str)
{
  char* retstr;

  retstr = NEW char[ strlen(str)+1 ];
  strcpy( retstr, str );
  return retstr;
}
////////////////////////////////////////////////////////////////////////////////
//  Globals
////////////////////////////////////////////////////////////////////////////////

// These strings must be maintained in a 1-1 correspondence with entity_flavors.
// This is somewhat automatically enforced by the entity_manager constructor, using
// the designated stringx "null" at the end of the list.
// @TODO - not correct
const char* entity_flavor_names[NUM_ENTITY_FLAVORS+1] =
{

  "CAMERA",
  "ENTITY",
  "MARKER",
  "MIC",
  "LIGHT_SOURCE",
  "PARTICLE",
  "PHYSICAL",

  "ITEM",
  "LIGHT",
  "MOBILE",
  "CONGLOMERATE",
  "TURRET",
  "BEAM",
  "SCANNER",
  "MORPHABLE_ITEM",
  "SKY",
  "MANIP",
  "SWITCH",
  "BOX_TRIGGER",
  "POLYTUBE",

  "LENSFLARE",
  "null"
};

// return entity_flavor_t corresponding to given string (NUM_ENTITY_FLAVORS if not found)
entity_flavor_t to_entity_flavor_t( const stringx& s )
{
  int i;
  for ( i=0; i<NUM_ENTITY_FLAVORS; ++i )
  {
    if ( s == entity_flavor_names[i] )
      break;
  }
  return (entity_flavor_t)i;
}


////////////////////////////////////////////////////////////////////////////////
//  entity_id
////////////////////////////////////////////////////////////////////////////////

// @Ok
// @Matching
entity_id::entity_id(const char* name)
{
  set_entity_id(name);
}

// @Ok
// @Matching
INLINE void entity_id::set_entity_id(const char* name)
{
  // from now on, entity_id's must be uppercase.  tools are responsible
  // for converting.
#ifndef NDEBUG
  for(unsigned i=0;i<strlen(name);++i)
  {
    if(islower(name[i]))
    {
      stringx warnmsg = stringx(name) + " is not all uppercase.";
      warning( warnmsg );
      break;  // so we can ignore and find more
    }
  }
#endif
  name_to_number_map& name_to_number = entity_manager::inst()->name_to_number;
  name_to_number_map::iterator it =
    name_to_number.find( charstarwrap( const_cast<char*>(name) ) );
  if( it == name_to_number.end() )
  {
    char* newname = strdupcpp( name );
/*    char* newname = NEW char[strlen(name)+1];
    memcpy(newname,name,strlen(name));
    newname[strlen(name)]=0;*/
	std::pair<name_to_number_map::iterator,bool> result = name_to_number.insert(
      name_to_number_map::value_type( charstarwrap( newname ), entity_manager::inst()->number_server++ ) );
    // will only insert if not in there already
    val = (*result.first).second;
  }
  else
    val = (*it).second;
}


// @Ok
// @Matching
void entity_id::delete_entity_id(entity_id id)
{
  name_to_number_map::iterator found;

  name_to_number_map& name_to_number = entity_manager::inst()->name_to_number;
  for (found = name_to_number.begin(); found != name_to_number.end(); ++found)

  {
    if ((*found).second == id.val)
    {
      delete[] (*found).first.str;
      name_to_number.erase(found);
      return;
    }
  }
  error("Tried to delete unrecognized entity_id: " + itos(id.val)); // obviously it isn't in the list, so we can't get the name
  //error("Tried to delete unrecognized entity_id: " + id.get_val());
  assert(false);
}


// @Ok
// @Matching
stringx entity_id::get_val() const
{
  name_to_number_map::iterator found;

  name_to_number_map& name_to_number = entity_manager::inst()->name_to_number;
  for( found = name_to_number.begin(); found != name_to_number.end(); ++found)
  {
    if( (*found).second == val )
    {
      return stringx((*found).first.str);
    }
  }

  assert(false);
  return stringx("garbage");

}



#define UNIQUE_ENTITY_ID_BASE "_ENTID_"
static int unique_entity_id_idx=0;
#ifdef DEBUG
//int agorra;
#endif
entity_id &entity_id::make_unique_id()
{
  static entity_id ret;
  stringx name( UNIQUE_ENTITY_ID_BASE );
#ifdef DEBUG
  //if (unique_entity_id_idx==258 || unique_entity_id_idx==271)

    //agorra = 1;
#endif
  name += itos( unique_entity_id_idx++ ); //name_to_number.size() );
  ret.set_entity_id(name.c_str());
  return ret;
}


void serial_in(chunk_file& io,entity_id* eid)

{
  stringx in;
  serial_in(io,&in);

  in.to_upper();
  entity_id id_in(in.c_str());
  *eid = id_in;
}

////////////////////////////////////////////////////////////////////////////////
//  entity_manager
////////////////////////////////////////////////////////////////////////////////


// @Test - used for testing asm gen
entity_manager* entity_manager::my_inst;

entity_manager::entity_manager() : number_server( 0 )
{
#if defined(TARGET_XBOX)

  assert( !strcmp(entity_flavor_names[NUM_ENTITY_FLAVORS], "null") );
#else
  assert( entity_flavor_names[NUM_ENTITY_FLAVORS]=="null" );
#endif /* TARGET_XBOX JIV DEBUG */

  name_to_number.clear();
  number_server = 0;
  name_to_number.insert( name_to_number_map::value_type( strdupcpp("UNREG"), -1) );
}

entity_manager::~entity_manager()
{
  name_to_number_map::iterator mi;
  mi = name_to_number.begin();
  for(; mi != name_to_number.end(); ++mi )
  {

    delete[] (*mi).first.str;
  }
  erase( begin(), end() );
  name_to_number = name_to_number_map();
}

entity* entity_manager::find_entity(const entity_id& target_entity,
                                    entity_flavor_t flavor, bool unknown_ok)
{
  entity * outval;

  iterator it = find( target_entity );
  if( ( unknown_ok != FIND_ENTITY_UNKNOWN_OK ) && (it==end() ) )

  {
    stringx composite = stringx("Unable to find entity ") + target_entity.get_val();

    error( composite.c_str() );
  }

  if (it!= end())
  {
    if (flavor!=IGNORE_FLAVOR)
    {
      if((*it).second->get_flavor() != flavor )
      {
        stringx composite = stringx("Entity ") + target_entity.get_val() + " is not a " + entity_flavor_names[flavor];
        error( composite.c_str() );
      }
    }
    outval = (*it).second;
  }
  else
    outval = NULL;
  return outval;
}

#ifdef SPIDEY_SIM
entity* entity_manager::find_nearest_entity_to_line(const vector3d& pos1, const vector3d& pos2, entity_flavor_t flavor )
{
  entity * outval;

  iterator it = begin();
  float best = 0;
  iterator winner = end();
  vector3d unit_ray = pos2 - pos1;
  unit_ray.normalize();
  for( ; it != end(); it++ )
  {
    if( (*it).second->get_flavor() == flavor )
    {
      vector3d entpos = (*it).second->get_abs_position();
      // is in front of spidey?
      float dotty = dot( (entpos-pos1), unit_ray );
      if( dotty > 0 )
      {
        // project entpos onto ray
        vector3d projection = pos1 + dotty * unit_ray;

        // find distance to line
        vector3d ent_to_proj = entpos - projection;
        float distance = ent_to_proj.length();

        // find distance to spidey
        vector3d proj_to_pos1 = projection - pos1;
        float distance2 = proj_to_pos1.length();
        // disqualify anything ridiculous
        if( distance2<50 )  // this will actually vary from level to level
        {
          // quality is length over width

          float quality = distance2 / distance;
          if( quality > best )
          {
            best = quality;
            winner = it;
          }
        }
      }
    }
  }
  if( winner != end() )
    return (*winner).second;
  else
    return NULL;
}

#endif /* SPIDEY_SIM */
void entity_manager::purge()
{
  name_to_number_map::iterator mi;

  mi = name_to_number.begin();
  for(; mi != name_to_number.end(); ++mi )
  {
    delete[] (*mi).first.str;
  }
  erase( begin(), end() );
  name_to_number = name_to_number_map();
  number_server = 0;
  // *weird:  this causes allocations at the end of a heap we're going to delete most of
//  pair<name_to_number_map::iterator,bool> result =  // unused, remove me?
	//nglPrintf("Weird 6 byte leak\n");
  //name_to_number.insert( name_to_number_map::value_type( strdupcpp("UNREG"), -1) );
}


void entity_manager::stl_prealloc(void)
{
	// @TODO
	PANIC;
}


void entity_manager::register_entity(entity* e)

{
  if( !(e->get_id() == ANONYMOUS) )
  {
	  std::pair<iterator,bool> retval = insert( value_type(e->get_id(), e) );

    if(!retval.second)
    {
      stringx composite = stringx("Same entity name appears twice: ")+ e->get_id().get_val();
      error( composite.c_str() );
    }
  }
}

void entity_manager::deregister_entity(entity* e)
{
  if( !(e->get_id() == ANONYMOUS) )
  {
    iterator it = find( e->get_id() );
    entity_id::delete_entity_id(e->get_id());
    erase(it);

  }
}

#include "my_assertions.h"

void validate_entity_manager(void)
{
	VALIDATE_SIZE(entity_manager, 0x20);

	VALIDATE(entity_manager, name_to_number, 0x10);
}

void validate_entity_id(void)
{
	VALIDATE(entity_id, val, 0x0);
}

#include "my_patch.h"

void patch_entity_id(void)
{
	PATCH_PUSH_RET(0x004E3FC0, entity_id::get_val);
	PATCH_PUSH_RET(0x004E3DD0, entity_id::set_entity_id);

	PATCH_PUSH_RET_POLY(0x004E3CF0, entity_id::entity_id(const char*), "??0entity_id@@QAE@PBD@Z");

	PATCH_PUSH_RET(0x004E3EB0, entity_id::delete_entity_id);
}

void patch_entity_manager(void)
{
}

void patch_str(void)
{
	PATCH_PUSH_RET(0x004EBEA0, strdupcpp);
	PATCH_PUSH_RET(0x004EBEA0, strdupcpp);
}
