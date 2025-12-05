/*-------------------------------------------------------------------------------------------------------
  chunk_file implementation
-------------------------------------------------------------------------------------------------------*/
#include "global.h"

#include "chunkfile.h"
#include "oserrmsg.h"
#include "debug.h"

#include "osdevopts.h"

/*-------------------------------------------------------------------------------------------------------
CF_WRITE_BINARY


If defined, opening a text file for reading will also cause a binary file to be opened for writing.
Subsequent read operations from the text file cause equivalent write operations to the binary file.
This allows the load process to act as a 'converter' from text to binary form.


This is just here so I can test the binary loading code.

Any time the text data or the loading code changes, you must execute "del \steel\data\*.b?? /s /q /f"
and build all the binaries again.
-------------------------------------------------------------------------------------------------------*/
#if defined( TARGET_PC ) && 0
#define CF_WRITE_BINARY
#endif

const chunk_flavor CHUNK_EOF    ("eof");

const chunk_flavor CHUNK_END    ("chunkend"); // only flexible chunks need the chunkend tag.
const chunk_flavor CHUNK_SPF    ("spf");      // seconds-per-frame for animatable files (.tam & .ban)
const chunk_flavor CHUNK_SCALE  ("scale");    // scale factor for meshes or whatever else.

// @Patch - made it const char*
// @Neat - to undo the optimization make it a const char* so the code starts matching
const char* chunkend_label =  "chunkend";

bool chunk_file::noquotes=false;

/*-------------------------------------------------------------------------------------------------------
  chunk_file implementation
-------------------------------------------------------------------------------------------------------*/
chunk_file::chunk_file() : use_stash(false), type(CFT_NONE) {}


#define FBIN_TAG_VALUE 0x4642494E // this actually should be reversed (right now it appears in the file backwards)

void chunk_file::open( const stringx& name, int flags )
{
	// @TODO - removed
	PANIC;
}


void chunk_file::close()
{

  if (type==CFT_BINARY)
  {
    if (use_stash == true)
    {
      use_stash = false;
      the_stash.close();
    }
    else
      binary.close();
  }
  else
  if (type==CFT_TEXT)

  {
    text.close();
#if defined( CF_WRITE_BINARY )
    if ( binary.is_open() )
      binary.close();
#endif
  }
  type=CFT_NONE;
}

bool chunk_file::operator!() const
{
	// @TODO
	PANIC;
	return true;
}

void chunk_file::set_fp( unsigned int pos, os_file::filepos_t base )
{
	// @TODO
	PANIC;
}

bool chunk_file::at_eof()
{
	// @TODO
	PANIC;
}

int chunk_file::get_size(void)

{
  int theOldStash = the_stash.get_current_stash();
  bool retval=0;
  the_stash.set_current_stash(my_stash);

  if (type==CFT_BINARY)
  {
    if (use_stash)
      retval= the_stash.get_size();
    else
      retval= binary.get_size();

  }
  else if (type==CFT_TEXT)
    retval= 0; //text.get_size();
  else
    assert(false);


  the_stash.set_current_stash(theOldStash);
  return retval;
}


bool chunk_file::read( void *buf, int bytes )
{
  int theOldStash = the_stash.get_current_stash();
  bool retval=true;;
  the_stash.set_current_stash(my_stash);

  if (get_type()==chunk_file::CFT_TEXT)
  {
    retval= false; //io.get_text()->read( buf, chunk_flavor::CHUNK_FLAVOR_SIZE );
  }
  else
  {

    if (use_stash)

      the_stash.read(buf,bytes);
    else
      binary.read(buf,bytes);
  }
  the_stash.set_current_stash(theOldStash);

	return retval;
}

stringx chunk_file::get_name()
{
	// @TODO
	PANIC;
}

stringx chunk_file::get_dir()
{
  stringx name=get_name();
  int end = name.rfind('\\');
  if (end < 0)
    return stringx("");
  else
    return name.substr(0,end)+"\\";
}

stringx chunk_file::get_filename()
{
  stringx name=get_name();
	filespec spec=filespec(name);
	stringx fname=spec.name;

  return fname; //filespec(name).name;
}


#ifndef NO_SERIAL_IN
/*-------------------------------------------------------------------------------------------------------
  basic serial_in's
-------------------------------------------------------------------------------------------------------*/

// Macro to make implementing CF_WRITE_BINARY easier.

#if defined( CF_WRITE_BINARY )

#define CF_BINARY_OUT( var )                  \

  if ( io.get_type()==chunk_file::CFT_TEXT )  \
  {                                           \
    io.set_type( chunk_file::CFT_BINARY );    \
    if ( io.get_binary()->is_open() )         \
      serial_out( io, var );                  \
    io.set_type( chunk_file::CFT_TEXT );      \
  }
#else

#define CF_BINARY_OUT(var)

#endif

// @TODO - removed for now
#define CF_SERIAL_IN(io, var)                 

/*
  if (io.get_type()==chunk_file::CFT_TEXT)    \
    io.get_text()->read(var);                 \
  else                                        \
{  if (io.use_stash)                          \
      io.get_stash()->read(var, sizeof(*var)); \
    else                                       \
      io.get_binary()->read(var,sizeof(*var)); \
}
*/

void serial_in(chunk_file& io, chunk_flavor* d)
{
	// @TODO
	PANIC;
}


void serial_in(chunk_file& io, int* d)
{
  CF_SERIAL_IN(io,d);
  CF_BINARY_OUT(*d);
}

void serial_in(chunk_file& io, unsigned short* d)
{
  int i;
  serial_in(io,&i);

  *d=i;
}


// <<<< can't handle extended range of unsigned ints yet.  need to make unsigned int part of text_file.
void serial_in(chunk_file& io, unsigned int* d)
{
  int i;
  serial_in(io,&i);
//  assert(i>=0);
  *d=i;
}

// <<<< would be cool to support true/false for bool.
void serial_in(chunk_file& io, bool* d)
{
  int i;
  serial_in(io,&i);
  *d=(bool)i;
}

void serial_in(chunk_file& io, float* d)
{
	// @TODO
	typedef void (*serial_in_ptr)(chunk_file&, float*);
	serial_in_ptr serial_in_func = (serial_in_ptr)0x0079AEB0;
	serial_in_func(io, d);
}

#if defined(TARGET_XBOX)
void serial_in(chunk_file& io, double* d)
{
  // JIV FIXME probably won't work
  CF_SERIAL_IN(io,d);
  CF_BINARY_OUT(*d);
}
#endif /* TARGET_XBOX JIV DEBUG */

void serial_in(chunk_file& io, stringx* d)
{
	// @TODO
	typedef void (*serial_in_ptr)(chunk_file& io, stringx* d);
	serial_in_ptr serial_in_func = (serial_in_ptr)0x0079AFD0;

	serial_in_func(io, d);
}


#endif

#if !defined(NO_SERIAL_OUT)
/*-------------------------------------------------------------------------------------------------------
  basic serial_out's
-------------------------------------------------------------------------------------------------------*/
void serial_out(chunk_file& io, const chunk_flavor& d)
{
	// @TODO
	PANIC;
}


void serial_out(chunk_file& io,const int& d)
{
	// @TODO
	PANIC;
}

void serial_out(chunk_file& io,const short& d)
{
  int i=d;
  serial_out(io,i);
}

void serial_out(chunk_file& io,const unsigned short& d)
{
  int i=d;
  serial_out(io,i);
}

void serial_out(chunk_file& io,const unsigned int& d)

{
  int i = d;
  serial_out( io, i );

}


void serial_out(chunk_file& io,const float& d)
{
	// @TODO
	PANIC;
}

#if defined(TARGET_XBOX)

void serial_out(chunk_file& io,const double& src)
{
	// @TODO
	PANIC;
}
#endif /* TARGET_XBOX JIV DEBUG */



void serial_out(chunk_file& io,const stringx& d)
{
	// @TODO
	PANIC;
}

#endif


////////////////////////////////////////////////////////////////////////////////////////////////////
/*

  chunk_flavor members
  */
////////////////////////////////////////////////////////////////////////////////////////////////////
const char* chunk_flavor::c_str() const
{
  return flavor;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
stringx chunk_flavor::to_stringx() const
{
  return stringx( flavor );
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool chunk_flavor::operator==( const chunk_flavor& cf ) const
{
  return (strcmp( flavor, cf.flavor )==0);
}

#ifndef TARGET_PS2

bool chunk_flavor::operator!=( const chunk_flavor& cf ) const
{
  return (strcmp( flavor, cf.flavor )!=0);
}

#endif
