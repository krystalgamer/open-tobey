#include "global.h"

// @Patch
//#include "file_manager.h"

#include "ini_parser.h"

#include "osfile.h"


char* flag_names[] =
{
#define MAC(x,y,d) y,
#include "devoptflags.h"
#undef MAC
};

int flag_defaults[] =
{
#define MAC(x,y,d) d,
#include "devoptflags.h"
#undef MAC

};

char* string_names[] =
{
#define MAC(x,y,d) y,
#include "devoptstrs.h"
#undef MAC
};

char *string_defaults[] =
{
#define MAC(x,y,d) d,
#include "devoptstrs.h"
#undef MAC
};

char* int_names[] =
{
#define MAC(x,y,d) y,
#include "devoptints.h"
#undef MAC
};

int int_defaults[] =

{
#define MAC(x,y,d) d,
#include "devoptints.h"

#undef MAC
};

#include "ngl.h"

// @Ok
// @Matching
os_developer_options::os_developer_options()
{
  ini_parser game_ini("GAME.INI", this);
}

#include "debug.h"
// @Ok
// @Matching
bool ini_parser::parse(os_developer_options *opts)
{
  assert(filename != NULL);
  int i, j;
  os_file ini_file;
  int curr_group = NO_GROUP;
  char **name_list = NULL;

  int  name_count = 0;

  // set the defaults
  for (i=0; i<os_developer_options::NUM_FLAGS; ++i)
  {
    opts->flags[i] = flag_defaults[i];
  }
  for (i=0; i<os_developer_options::NUM_INTS; ++i)
  {
    opts->ints[i] = int_defaults[i];
  }
  for (i=0; i<os_developer_options::NUM_STRINGS; ++i)
  {
    opts->strings[i] = string_defaults[i];
  }
#ifndef BUILD_FINAL
  ini_file.open(filename, os_file::FILE_READ);
#endif

  if (ini_file.is_open())
  {
    unsigned int buf_size = ini_file.get_size();
    unsigned char *buf = (unsigned char *)arch_malloc(buf_size+1);

    assert(buf != NULL);

    ini_file.read(buf, buf_size);

    buf[buf_size] = '\0';
    ini_file.close();


    unsigned int pos = 0;

    char *curr_token = NULL;
    int token_type;
    int token_num;

    // parse out our file lists
    new_line((char *)&buf[pos]);

    while (get_token(&curr_token, &token_type, &token_num) != NO_TOKEN)
    {
      if (token_type == TOKEN_GROUP)
      {
        strlwr(curr_token);
        if (strcmp("[flags]", curr_token) == 0)
        {
          curr_group = FLAGS_GROUP;
        }
        else if (strcmp("[ints]", curr_token) == 0)
        {
          curr_group = INTS_GROUP;
        }
        else if (strcmp("[strings]", curr_token) == 0)
        {
          curr_group = STRINGS_GROUP;
        }
        else
        {
          debug_print("Mangled INI file, unknown group '%s', attempting to salvage things...", curr_token);
          curr_group = NO_GROUP;
        }

      }
      else if (token_type == TOKEN_STRING)
      {
        // a key value pair, let's set the correct INI setting.
        assert(curr_group != NO_GROUP);
        strupr(curr_token);

        // look for the string in our current group's names
        switch (curr_group)
        {
          case FLAGS_GROUP:   name_list = flag_names;   name_count = os_developer_options::NUM_FLAGS;  break;
          case INTS_GROUP:    name_list = int_names;    name_count = os_developer_options::NUM_INTS;    break;
          case STRINGS_GROUP: name_list = string_names; name_count = os_developer_options::NUM_STRINGS; break;
          default:            name_list = NULL;         name_count = 0;  break;
        }
        for (i=0; i<name_count; ++i)
        {
          if (strcmp(name_list[i], curr_token) == 0)
            break;
        }


        // pull off an equals and the string value.  If no equals, try to salvage things...
        if (get_token(&curr_token, &token_type, &token_num) == TOKEN_EQUALS)

          get_token(&curr_token, &token_type, &token_num);
        else
        {
          nglPrintf("Mangled INI file, expected '=' but found '%s', trying to salvage things...", curr_token);
          unget_token();
          continue;
        }
        assert(token_type == TOKEN_STRING);

        if (i != name_count)
        {
          // set it to the NEW value
          switch (curr_group)
          {
            case FLAGS_GROUP:
              j = atoi(curr_token);

              assert(j == 0 || j == 1);
              opts->flags[i] = j;
//			  nglPrintf("FLAG\t'%d'\n", j);
              break;

            case INTS_GROUP:

              j = atoi(curr_token);
              opts->ints[i] = j;
//			  nglPrintf("INT\t'%d'\n", j);
              break;

            case STRINGS_GROUP:
//			  nglPrintf("STRING\t'%s'\n", curr_token);
              opts->strings[i] = curr_token;
              break;


            default:

              break;
          }
        }
      }
      else if (token_type == TOKEN_EQUALS)
      {
        nglPrintf("Mangled INI file, expected a key value, but found '=', trying to salvage things...");
      }
    }


    arch_free(buf);
  }
  else
    debug_print("Error.  Could not open ini file %s\n", filename);


  return true;
}


// @Ok
// @Matching
INLINE void ini_parser::despacify_token(char *curr_token)
{
  int i;

  for (i=strlen(curr_token)-1; i>=0; --i)
  {

    if (isspace(curr_token[i]))
      curr_token[i] = '\0';
    else
      break;

  }
}


/*** get_token ***/
// token is a pointer which we change to point to the token to use
// @Ok
// @Matching
int ini_parser::get_token(char **curr_token, int *token_type, int *num_value)
{
	int ret;
	if (stored_token)
	{
		*curr_token = token;

		*token_type = stored_type;
		*num_value = stored_num;

		stored_token = 0;
		return *token_type;
	}


	// eat up whitespace
	token[0] = '\0';
	while( (isspace(line[scan_pos]) && (line[scan_pos] != '\0')) ||
		line[scan_pos] == '\n' || line[scan_pos] == '\r' || line[scan_pos] == ';' )
	{
		if (line[scan_pos] == ';')
		{
			// a comment, scan to the end of the line

			while (line[scan_pos] != '\n' && line[scan_pos] != '\r' && (line[scan_pos] != '\0'))
			++scan_pos;
		}
		else
		  ++scan_pos;
	}
	ret = build_token(&line[scan_pos], token);
	scan_pos += ret;

	if (ret == 0)
	{
		*curr_token = NULL;
		return NO_TOKEN;
	}


	*curr_token = token;
	despacify_token(token);

	if (token[0] == '[')
	{
		strlwr(token);
		*token_type = TOKEN_GROUP;
	}
	else if (token[0] == '=')
		*token_type = TOKEN_EQUALS;
	else if (strlen(token) > 0)
		*token_type = TOKEN_STRING;
	else
		*token_type = NO_TOKEN;

	stored_num = *num_value;
	stored_type = *token_type;

	return *token_type;

}


/*** build_token ***/
// @Ok
// @Matching
INLINE int ini_parser::build_token(char *curr_line, char *curr_token)
{
  int i=0;
  if (curr_line[0] == '=')
  {
    curr_token[0] = '=';
    curr_token[1] = '\0';
    return 1;
  }
  while ( (curr_line[i] != '\n') && (curr_line[i] != '\r') && (curr_line[i] != '\0') && (curr_line[i] != '='))
  {
    curr_token[i] = curr_line[i];
    ++i;
  }
  curr_token[i] = '\0';

  return i;
}

/*** unget_token ***/
// pushes back the last token, only works for one-token's worth
// @Ok
// @Matching
INLINE void ini_parser::unget_token()
{
  stored_token = 1;
}

/*** new_line ***/
// passes in a NEW line to get tokens from
// @Ok
// @Matching
INLINE void ini_parser::new_line(char *curr_line)
{

  line = curr_line;
  scan_pos = 0;
}

#include "my_assertions.h"

void validate_os_developer_options(void)
{
	VALIDATE_SIZE(os_developer_options, 0x108);

	VALIDATE(os_developer_options, flags, 0x4);
	VALIDATE(os_developer_options, strings, 0x6C);
	VALIDATE(os_developer_options, ints, 0xBC);


	VALIDATE(os_developer_options, flags[os_developer_options::FLAG_MOVE_EDITOR], 0x24);
	VALIDATE(os_developer_options, flags[os_developer_options::FLAG_DISTANCE_FADING], 0x3B);
}

void patch_os_developer_options(void)
{
	PATCH_PUSH_RET_POLY(0x0079BFD0, os_developer_options::os_developer_options, "??0os_developer_options@@QAE@XZ");
}

void validate_ini_parser(void)
{
	//VALIDATE_SIZE(ini_parser, 0);

	VALIDATE(ini_parser, filename, 0);
	VALIDATE(ini_parser, token, 0x100);


	VALIDATE(ini_parser, scan_pos, 0x200);
	VALIDATE(ini_parser, line, 0x204);

	VALIDATE(ini_parser, stored_token, 0x208);
	VALIDATE(ini_parser, stored_type, 0x209);
	VALIDATE(ini_parser, stored_num, 0x20A);
}

#include "my_patch.h"

void patch_ini_parser(void)
{
	PATCH_PUSH_RET(0x0079C110, ini_parser::parse);

	PATCH_PUSH_RET(0x0079C520, ini_parser::despacify_token);
	PATCH_PUSH_RET(0x0079C570, ini_parser::get_token);

	PATCH_PUSH_RET(0x0079C7C0, ini_parser::build_token);

	PATCH_PUSH_RET(0x0079C840, ini_parser::unget_token);
	PATCH_PUSH_RET(0x0079C860, ini_parser::new_line);
}
