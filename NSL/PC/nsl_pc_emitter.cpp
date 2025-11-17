#include "../common/nsl.h"
#include "nsl_pc.h"


//////////////////////// nsl emitter api functions ////////////////////////


/*-------------------------------------------------------------------------
  nsl Create Emitter
-------------------------------------------------------------------------*/
// @TODO
nslEmitterId nslCreateEmitter( const nlVector3d &position )
{
	typedef nslEmitterId (*func_ptr)(const nlVector3d &);
	func_ptr func = (func_ptr)0x0081D080;
	return func(position);
}
