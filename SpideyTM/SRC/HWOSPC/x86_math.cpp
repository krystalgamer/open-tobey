#include "x86_math.h"

rational_t fast_acos_lookup_table[_FAST_ACOS_LOOKUP_SIZE+1];
void init_fast_acos_table()
{
  static bool table_initted = false;

  if(!table_initted)
  {
    table_initted = true;
    for(int i=0; i<=_FAST_ACOS_LOOKUP_SIZE; ++i)
      fast_acos_lookup_table[i] = fp(acos(lookup_to_cos(i)));
  }
}
