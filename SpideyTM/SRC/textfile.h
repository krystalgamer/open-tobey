/*-------------------------------------------------------------------------------------------------------

  TEXTFILE.H - Text file streaming layer on top of binary file I/O

-------------------------------------------------------------------------------------------------------*/
#ifndef TEXTFILE_H
#define TEXTFILE_H

#include "osfile.h"
#include "mustash.h"

#include "file_manager.h"


class text_file
{
  public:

    EXPORT text_file(); 
    EXPORT ~text_file();

    EXPORT void open(const stringx & name, int _flags=os_file::FILE_READ);

    EXPORT void close();

	// @TODO - wait
    //EXPORT inline const stringx get_name() const { return use_stash ? stringx(the_stash.get_name().c_str()) : io.get_name(); }

    #ifdef TARGET_GC

    EXPORT inline bool at_eof() { return ( use_stash ? (bufpos>=bufamt && the_stash.at_eof()) : (bufpos>=bufamt && io.at_eof())) ; }
    #else

	// @TODO - wait
    //EXPORT inline bool at_eof() const { return use_stash ? (bufpos>=bufamt && the_stash.at_eof()) : (bufpos>=bufamt && io.at_eof()); }
    #endif

    // note: read_char and peek_char don't skip whitespace. everything else does.
    EXPORT char peek_char();
    EXPORT char read_char();  

    EXPORT void read(char* c); // one char, -NOT- a string buffer
    EXPORT void read(int* i);
    EXPORT void read(float* f);
#if defined(TARGET_XBOX)
    EXPORT void read(double* f);
#endif /* TARGET_XBOX JIV DEBUG */

    EXPORT void read(stringx* s);
    EXPORT void read(char* s, int maxlen);  // string buffer

	EXPORT int readln(char* s, unsigned int maxlen, char delimiter = '\n', bool* hitDelimiter = NULL);


    EXPORT void write(char c);
    EXPORT void write(int i);
    EXPORT void write(float f);
    EXPORT void write(const stringx & s);

	// @TODO - wait
	/*
    EXPORT void set_fp( unsigned int pos, os_file::filepos_t base )
    {
      if (use_stash)
        the_stash.set_fp( pos, (stash::filepos_t)base );
      else
        io.set_fp( pos, base );
    }
	*/

	// @Patch - remove
    //EXPORT bool operator!() const { return use_stash ? !the_stash.is_open() : !io; }

    // Integrating PFE parsing functionality
    EXPORT int nextchar();
    EXPORT int nextnonwhite();
    EXPORT int skipuntilthischar(char c);
    EXPORT int readuntilthischar(char c,char *buf,int buflen);
    EXPORT int readuntilwhite(char *buf,int buflen);
    EXPORT int readuntilnotdigit(char *buf,int buflen);
    EXPORT int skipuntildigit();
    EXPORT void keypushback(int c);
	// @Patch - remove
    //EXPORT bool is_open() const { return use_stash ? the_stash.is_open() : io.is_open(); }

    EXPORT static bool text_file_exists(const stringx& name);

  private:

    EXPORT void eat_whitespace();

    EXPORT void refill_buf();

	// @Patch - remove most fields
	/*
    os_file io;
    stash   the_stash;
    int     my_stash;
	*/
	file_id_t field_0;
    char *  buf;
    int     bufpos;
    int     bufamt;
	int field_10;

	/*
    bool    use_stash;

    // Integrating PFE parsing functionality
    int pushbackdata;
	*/

	friend void validate_text_file(void);
	friend void patch_text_file(void);
};

#endif
