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

// @TODO
void nslSetEmitterPosition(nslEmitterId id, const nlVector3d &newPosition)
{
	typedef void (*func_ptr)(nslEmitterId, const nlVector3d &);
	func_ptr func = (func_ptr)0x0081D370;
	func(id, newPosition);
}

nslSoundStatusEnum nslGetSoundStatus( nslSoundId whichSound )
{
	typedef nslSoundStatusEnum (*func_ptr)(nslSoundId);
	func_ptr func = (func_ptr)0x0081E4A0;
	return func(whichSound);
}
