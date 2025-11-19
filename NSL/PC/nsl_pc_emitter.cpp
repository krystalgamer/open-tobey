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

void nslPlaySound( nslSoundId soundToPlay )
{
	typedef void (*func_ptr)(nslSoundId);
	func_ptr func = (func_ptr)0x0081E240;
	func(soundToPlay);
}

nslSourceId	nslGetSource( const char* name, bool fatal )
{
	typedef nslSourceId (*func_ptr)(const char*, bool);
	func_ptr func = (func_ptr)0x0081F6A0;

	return func(name, fatal);
}

nslSoundId  nslXAddSound( nslSourceId soundSource )
{
	typedef nslSourceId (*func_ptr)(nslSourceId);
	func_ptr func = (func_ptr)0x007ED8C0;

	return func(soundSource);
}

void nslSetSoundParam( nslSoundId soundToSet, nslSoundParamEnum whichParam, float newVal )
{
	typedef void (*func_ptr)( nslSoundId soundToSet, nslSoundParamEnum whichParam, float newVal );
	func_ptr func = (func_ptr)0x0081E9D0;
	func(soundToSet, whichParam, newVal);
}

void nslSetSoundEmitter( nslEmitterId soundEmitter, nslSoundId soundInstance )
{
	typedef void (*func_ptr)( nslEmitterId soundEmitter, nslSoundId soundInstance);
	func_ptr func = (func_ptr)0x0081D4C0;
	func(soundEmitter, soundInstance);
}

void nslReleaseEmitter( nslEmitterId emitterToRelease )
{
	typedef void (*func_ptr)(nslEmitterId);
	func_ptr func = (func_ptr)0x0081D2B0;

	func(emitterToRelease);
}
