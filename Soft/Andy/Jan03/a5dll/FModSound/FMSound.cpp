#include "StdAfx.h"
#include "FMSound.h"

namespace NFMSound
{
CDriversInfo drivers;

bool SearchDevices()
{
	drivers.clear();
	return true;
}

bool Init( const SStartInfo &info )
{
	(void)info;
	return false;
}

void Done()
{
}

bool IsInitialized()
{
	return false;
}

void Update( const SListener &listen )
{
	(void)listen;
}

CSample2D* LoadSample2D( const void *pData, int nLength )
{
	(void)pData;
	(void)nLength;
	return 0;
}

CSample3D* LoadSample3D( const void *pData, int nLength, float fMinDistance, float fMaxDistance, int nPriority )
{
	(void)pData;
	(void)nLength;
	(void)fMinDistance;
	(void)fMaxDistance;
	(void)nPriority;
	return 0;
}

CSample3D* GetDefault3DSound()
{
	return 0;
}

CSound2D* PlaySound( CSample2D *pSample )
{
	(void)pSample;
	return 0;
}

CSound3D* Play3DSound( const SPlayParams &params )
{
	(void)params;
	return 0;
}

CStream* PlayStream( const char *pszName, bool bLoop )
{
	(void)pszName;
	(void)bLoop;
	return 0;
}

CStream* SwitchStream( CStream *pOldStream, const char *pszNameNewStream, bool bLoop )
{
	(void)pOldStream;
	(void)pszNameNewStream;
	(void)bLoop;
	return 0;
}

bool IsPlaying( CStream *pStream )
{
	(void)pStream;
	return false;
}

bool IsPlaying( CSound2D *pSound )
{
	(void)pSound;
	return false;
}

bool IsPlaying( CSound3D *pSound )
{
	(void)pSound;
	return false;
}

void FadeOut( CStream *pStream, float fSec )
{
	(void)pStream;
	(void)fSec;
}

void CancelFadeOut( CStream *pStream )
{
	(void)pStream;
}

void SetSFXMasterVolume( int nSFX )
{
	(void)nSFX;
}

void SetMusicMasterVolume( int nMusic )
{
	(void)nMusic;
}

void SetSpeakerType( ESpeakerType speaker )
{
	(void)speaker;
}

} // namespace NFMSound
