#ifndef __wOSBase_H_
#define __wOSBase_H_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "..\DBFormat\DataAnimation.h"
#include "wInterface.h"
#include "wInterfaceVisitors.h"
#include "RPGAttackMech.h"
#include "wDynObject.h"
namespace NAnimation
{
	class CSkeletonAnimator;
	class CSkeletonState;
	class CAnimation;
}
namespace NDb
{
	class CObject;
	class CDebrisMaterial;
	class CContainerModel;
}
namespace NRPG
{
	class IObject;
}
namespace NWorld
{
////////////////////////////////////////////////////////////////////////////////////////////////////
struct SObjectPlace
{
	CVec3 ptPos, ptScale;
	float fAngle;
	int nFloor;
};
////////////////////////////////////////////////////////////////////////////////////////////////////
class CWorld;
class CObjectServerBase: public IObject, public NRPG::IAttackable, public IVisObj
{
protected:
	ZDATA
	CObj<NRPG::IObject> pRPG;
	CPtr<NDb::CObject> pDbObject;
	int nDestroyStage;
	bool bLightMap;
	CSyncSrcBind<IVisObj> bindGlobal;
	SObjectPlace position;
	CPtr<CWorld> pWorld;
	STime tStageChange;
	STime tLastSound;
	vector<int> vCreateFlags;
	bool bBorder;
public:
	ZEND int operator&( CStructureSaver &f ) { f.Add(2,&pRPG); f.Add(3,&pDbObject); f.Add(4,&nDestroyStage); f.Add(5,&bLightMap); f.Add(6,&bindGlobal); f.Add(7,&position); f.Add(8,&pWorld); f.Add(9,&tStageChange); f.Add(10,&tLastSound); f.Add(11,&vCreateFlags); f.Add(12,&bBorder); return 0; }
protected:
	void Kill( const CVec3 &ptDir );
	bool CreateTransform( SFBTransform *pRes );
	NDb::CDebrisMaterial* GetDebrisMaterial() const;
	void AddLights( IRenderVisitor *p, NDb::CContainerModel *pCont, const SFBTransform &rv );
	void AddEffects( IRenderVisitor *p, NDb::CContainerModel *pCont, const SFBTransform &rv );
	void AddEffects( IAIVisitor *p, NDb::CContainerModel *pCont, const SFBTransform &rv );
	void AddObject( IRenderVisitor *p, NDb::CObject *pO, const SFBTransform &rv );
	void AddObject( IAIVisitor *p, NDb::CObject *pO, const SFBTransform &rv );
	void AddObject( ISoundVisitor *p, NDb::CObject *pO, const SFBTransform &rv );
	CWorld* GetWorld() const { return pWorld; }
	const vector<int>& GetCreateFlags() const { return vCreateFlags; }
	void PrecacheEffects( IAIVisitor *p, NDb::CObject *pO );
	void AddObjectHull( IAIVisitor *pVisitor, 
		NDb::CAIGeometry *pGeometry, const SFBTransform &rv, NDb::CRPGArmor *pArmor, int nFloor );
	int GetDecalID();
public:
	CObjectServerBase() {}
	CObjectServerBase( CWorld *pWorld, const SObjectPlace &pos, bool bLightMap,
		NDb::CObject *pO, NRPG::IObject *pRPG, const vector<int> &vCreateFlags, bool _bBorder = false );
	
	bool CheckStability();
	
	// NRPG::IAttackable
	virtual int ProcessAttack( int nUserID, NRPG::CAttackPortion *pAttack, NDb::CRPGArmor *pArmor );
	// IObject
	virtual bool IsTargetable() const;
	// IVisObj
	virtual void Visit( IRenderVisitor* );
	virtual void Visit( IAIVisitor* );
	virtual void Visit( ISoundVisitor *p );

	virtual bool NeedSegment() const;
	virtual bool Segment();

	virtual void SetPosition( const SObjectPlace &pos );
	const SObjectPlace& GetPosition() const { return position; }

	// for debug purposes:
	int GetDBObjectID() const; 
	virtual void SetDestroyStage( int nStage );
};
////////////////////////////////////////////////////////////////////////////////////////////////////
class IGetApproaches
{
public:
	virtual void GetApproaches( vector<NAI::SPathPlace> *pRes, NAI::IPathNetwork *pNet ) const = 0;
	virtual void GetApproachPts( vector<CVec3> *pRes ) const = 0; // now used only in debug
};
////////////////////////////////////////////////////////////////////////////////////////////////////
class CActionCounter;
class CAnimObjectServerBase: public CObjectServerBase, public IGetApproaches, public IDynamicObject
{
protected:
	ZDATA_(CObjectServerBase)
	CDBPtr<NDb::CSkeleton> pSkeleton;
	CObj<CFuncBase<STime> > pTime;
	CObj<NAnimation::CSkeletonAnimator> pAnimator;
	CObj<NAnimation::CSkeletonState> pState;
	CObj<CActionCounter> pAction;
	STime tEnd;
	vector<CObj<CObjectBase> > destroyAnimations;
public:
	ZEND int operator&( CStructureSaver &f ) { f.Add(1,(CObjectServerBase*)this); f.Add(2,&pSkeleton); f.Add(3,&pTime); f.Add(4,&pAnimator); f.Add(5,&pState); f.Add(6,&pAction); f.Add(7,&tEnd); f.Add(8,&destroyAnimations); return 0; }
protected:
	void IdleOn();
	void IdleOff();
	void PlayAnimation( bool bInstantly = false );
	void PlayCustomAnimation( NAnimation::CAnimation *pAnimation, bool bInstantly = false );
	void AddEffects( IRenderVisitor *p, NDb::CContainerModel *pCont, const SFBTransform &rv );
	void PrecacheAnimations();
	void PrecacheAIGeom( IAIVisitor *p );
public:
	CAnimObjectServerBase() {}
	CAnimObjectServerBase( CWorld *pWorld, const SObjectPlace &pos, bool bLightMap,
		NDb::CObject *pO, NRPG::IObject *pRPG, CFuncBase<STime> *_pTime, const vector<int> &vCreateFlags );

	// IGetApproaches
	virtual void GetApproaches( vector<NAI::SPathPlace> *pRes, NAI::IPathNetwork *pNet ) const;
	virtual void GetApproachPts( vector<CVec3> *pRes ) const;
	// IVisObj
	virtual void Visit( IRenderVisitor* );
	virtual void Visit( IAIVisitor* );
	// IDynamicObject
	virtual bool Segment();
	//
	virtual void SetPosition( const SObjectPlace &pos );
	void PlayDBAnimation( int nDBAnimationID );
	// for script
	bool IsPerformingAction();
	void CancelAction();
	//
};
////////////////////////////////////////////////////////////////////////////////////////////////////
int GetMask( NDb::CAIGeometry *pAIGeometry, NDb::CRPGArmor *pArmor );
}
#endif
