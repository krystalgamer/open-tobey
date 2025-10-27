void script_strings_deconstruct()
{
	// @TODO - globals suxx
	typedef void (*script_strings_deconstruct_ptr)(void);
	script_strings_deconstruct_ptr script_strings_deconstruct = (script_strings_deconstruct_ptr)0x0051A4B0;

	script_strings_deconstruct();
	/*
  for( int i=0; i<MAX_STATIC_TO_STR_STRINGS; i++ )
  {

    script_strings[ i ] = stringx();
  }
  script_game_string = stringx();
  script_string_none = stringx();
  script_return_string = stringx();
  */
}
