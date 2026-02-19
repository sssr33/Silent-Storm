#ifndef __WEXPLTRACKER_H_
#define __WEXPLTRACKER_H_

#include "wDynObject.h"
#include "aiCollider.h"
#include "aiVoxelRender.h"
#include "..\Misc\HPTimer.h"

namespace NAI
{
	class IAIMap;
}

namespace NDb
{
	class CRPGGrenade;
}

namespace NRPG
{
	class IAttackable;
	class CAttackPortion;
}

namespace NWorld
{
////////////////////////////////////////////////////////////////////////////////////////////////////
class CWorld;
class CVoxelExpl;
class CVoxelExplTracker;
class CUnitServer;
////////////////////////////////////////////////////////////////////////////////////////////////////
const float F_VOXEL_SIZE = 0.2f; // метра
const int N_CUBE_SIZE = 16; //30;//16; // voxels // должно быть кратно 2-м
const float F_CUBE_SIZE = N_CUBE_SIZE * F_VOXEL_SIZE;
const int N_REAL_CUBE_SIZE = N_CUBE_SIZE + 2;
const float F_REAL_CUBE_SIZE = N_REAL_CUBE_SIZE * F_VOXEL_SIZE;
const float F_WAVE_ATTENUATION_COEFF = 0.4f;
////////////////////////////////////////////////////////////////////////////////////////////////////
// CExplCube
////////////////////////////////////////////////////////////////////////////////////////////////////
class CExplCube: public CObjectBase
{
	OBJECT_BASIC_METHODS( CExplCube );
public:
	//
	typedef unsigned char byte;
	//
	struct SExplVoxelCoords
	{
		ZDATA
		byte nX;
		byte nY;
		byte nZ;
		ZEND int operator&( CStructureSaver &f ) { f.Add(2,&nX); f.Add(3,&nY); f.Add(4,&nZ); return 0; }
		//
		SExplVoxelCoords() {}
		SExplVoxelCoords( byte _nX, byte _nY, byte _nZ ):
			nX( _nX ), nY( _nY ), nZ( _nZ ) {}
	};
	//
	enum ENeighbourCube
	{
		NC_LEFT = 0,
		NC_RIGHT,
		NC_UPPER,
		NC_UNDER,
		NC_DISTANT,
		NC_NEAR
	};
	//
	NAI::CExplVoxelRenderer renderer;
	//
	ZDATA
	bool bFinished;
	CVec3 ptCenter;
	CPtr<NAI::IAIMap> pAIMap;
	CPtr<CVoxelExpl> pExplosion;
	vector<SExplVoxelCoords> voxels;
	int nFront;
	int nFrontSize;
	int nEmpty;
	vector< CPtr<CExplCube> > neighborCubes;
	vector< CVec3 > neighborCubesCoords;
	ZEND int operator&( CStructureSaver &f ) { f.Add(2,&bFinished); f.Add(3,&ptCenter); f.Add(4,&pAIMap); f.Add(5,&pExplosion); f.Add(6,&voxels); f.Add(7,&nFront); f.Add(8,&nFrontSize); f.Add(9,&nEmpty); f.Add(10,&neighborCubes); f.Add(11,&neighborCubesCoords); return 0; }
	//
	CVec3 GetDirection( int nX1, int nY1, int nZ1, int nX2, int nY2, int nZ2 );
	CExplCube *GetNeighborCube( ENeighbourCube cube );
	bool ProcessBoundaryVoxel( int nX, int nY, int nZ );
	void SubProcessBoundaryVoxel( CExplCube *pNeighborCube, int nNX, int nNY, int nNZ, bool *bBoundaryVoxel );
	void SubProcessNeighborVoxels( int nPX, int nPY, int nPZ, int nX, int nY, int nZ );
	void ProcessNeighborVoxels( int nX, int nY, int nZ );
	void ExpandFront( int nPX, int nPY, int nPZ, int nX, int nY, int nZ );
public:
	//
	CExplCube() {}
	CExplCube( CVec3 _ptCenter, NAI::IAIMap *_pAIMap, CVoxelExpl *_pExplosion );
	//
	CVec3 GetVoxelCenter( int nX, int nY, int nZ );
	void MakeStep();
	bool IsInCube( CVec3 ptCoords );
	bool IsEmpty( int nX, int nY, int nZ );
	void Wave( int nX, int nY, int nZ );
	void Front( int nX, int nY, int nZ );
	bool IsFinished() { return bFinished; }
};
////////////////////////////////////////////////////////////////////////////////////////////////////
// CVoxelExpl
////////////////////////////////////////////////////////////////////////////////////////////////////
class CVoxelExpl: public CObjectBase
{
	OBJECT_BASIC_METHODS( CVoxelExpl );
public:
	struct SObjectDamageInfo
	{
		bool bDestroyed, bPutDecal;
		int nVolume;
		CRay rDir;
		SObjectDamageInfo() : bDestroyed(false), bPutDecal(false), nVolume(0) {}
	};
private:
	//
	ZDATA
	bool bFinished;
	CVec3 ptCenter;
	CPtr<NAI::IAIMap> pAIMap;
	CPtr<CUnitServer> pThrower;
	CDBPtr<NDb::CRPGGrenade> pGrenade;
	list< CObj<CExplCube> > cubesToAdd;
	CPtr<CVoxelExplTracker> pTracker;
	int nMaxVolume;
	int nWave;
	NHPTimer::STime tOverrun;
	int nCurrentCube;
public:
	vector< CObj<CExplCube> > cubes;
	NAI::CExplVoxelRenderer::CObjectsHash objects; // объект с индексом 0 - зарезервирован, 1 - terrain
	vector<SObjectDamageInfo> damageInfo;
	int nObjectsEnd;
	int nObjectsDestroyed;
	int nEnemyUnitsKilled;
	int nVolume;
	ZEND int operator&( CStructureSaver &f ) { f.Add(2,&bFinished); f.Add(3,&ptCenter); f.Add(4,&pAIMap); f.Add(5,&pThrower); f.Add(6,&pGrenade); f.Add(7,&cubesToAdd); f.Add(8,&pTracker); f.Add(9,&nMaxVolume); f.Add(10,&nWave); f.Add(11,&tOverrun); f.Add(12,&nCurrentCube); f.Add(13,&cubes); f.Add(14,&objects); f.Add(15,&damageInfo); f.Add(16,&nObjectsEnd); f.Add(17,&nObjectsDestroyed); f.Add(18,&nEnemyUnitsKilled); f.Add(19,&nVolume); return 0; }
	//
	void ApplyWaveDamage();
	void CheckWaveResults();
	void ExplodeWave();
	int GetVolume( float fRadius );
	//
public:
	//
	CVoxelExpl() {}
	CVoxelExpl( CVec3 _ptCenter, int _nWave, NDb::CRPGGrenade *_pGrenade, 
		CUnitServer *_pThrower, NAI::IAIMap *_pAIMap, CVoxelExplTracker* _pTracker );
	//
	CExplCube *GetExplCube( CVec3 ptCoords );
	bool IsFinished() { return bFinished; }
	void Segment();
};
////////////////////////////////////////////////////////////////////////////////////////////////////
// CVoxelExplTracker
////////////////////////////////////////////////////////////////////////////////////////////////////
class CActionCounter;
class CVoxelExplTracker: public IDynamicObject
{
	OBJECT_BASIC_METHODS( CVoxelExplTracker );
public:
	typedef unordered_map< CPtr<CObjectBase>, bool, SPtrHash> CDecalsHash;
private:
	ZDATA
	int nWave;
	CVec3 ptCenter;
	CDBPtr<NDb::CRPGGrenade> pGrenade;
	CPtr<CUnitServer> pThrower;
	int nEnemyUnitsKilled;
	int nObjectsDestroyed;
	CObj<CActionCounter> pAction;
	CPtr<CWorld> pWorld;
	CObj<CVoxelExpl> pExpl;
public:
	list< CPtr<CUnitServer> > damagedUnits;
	unordered_map< CPtr<CObjectBase>, list<int>, SPtrHash > damagedObjects;
	CDecalsHash drawDecals;
private:
	ZEND int operator&( CStructureSaver &f ) { f.Add(2,&nWave); f.Add(3,&ptCenter); f.Add(4,&pGrenade); f.Add(5,&pThrower); f.Add(6,&nEnemyUnitsKilled); f.Add(7,&nObjectsDestroyed); f.Add(8,&pAction); f.Add(9,&pWorld); f.Add(10,&pExpl); f.Add(11,&damagedUnits); f.Add(12,&damagedObjects); f.Add(13,&drawDecals); return 0; }
	//
	void ExplodeFragments();
	void ApplyWaveDamageToUnits();
	//
public:
	//
	CVoxelExplTracker() {}
	CVoxelExplTracker( CVec3 _ptCenter, NDb::CRPGGrenade *_pGrenade, CUnitServer *_pThrower, CWorld *_pWorld );
	//
	virtual bool Segment();
};
////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif