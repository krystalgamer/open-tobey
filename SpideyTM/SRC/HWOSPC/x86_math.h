#ifndef X86_MATH_H
#define X86_MATH_H

#include <math.h>
#include "..\global.h"


typedef float rational_t;  
typedef float angle_t;
typedef float fp;

inline fp fast_recip_sqrt(fp x) 
{
  return static_cast<fp>(fp(1.0)/(rational_t)sqrt(x));
}

#define _FAST_ACOS_LOOKUP_SIZE        (4096*4)
#define _FAST_ACOS_LOOKUP_SIZE_DIV2   (_FAST_ACOS_LOOKUP_SIZE / 2)
#define _FAST_ACOS_LOOKUP_MOD         ((float)_FAST_ACOS_LOOKUP_SIZE_DIV2) 
#define _FAST_ACOS_LOOKUP_MOD_INV     (1.0f / _FAST_ACOS_LOOKUP_MOD)
#define cos_to_lookup(a)              ((int)(((a)+1)*_FAST_ACOS_LOOKUP_MOD))
#define lookup_to_cos(a)              (((float)((a)-_FAST_ACOS_LOOKUP_SIZE_DIV2))*_FAST_ACOS_LOOKUP_MOD_INV)

extern rational_t fast_acos_lookup_table[_FAST_ACOS_LOOKUP_SIZE+1];

inline rational_t fast_acos(rational_t a)
{
  assert(a >= -1.0f && a <= 1.0f);
  return(fast_acos_lookup_table[(int)((a+1.0f)*_FAST_ACOS_LOOKUP_MOD)]);
}



#endif
