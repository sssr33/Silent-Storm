#ifndef __aiCollider_H_
#define __aiCollider_H_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BSPCollider.h"
#include "aiCollidersCommon.h"

namespace NAI
{
////////////////////////////////////////////////////////////////////////////////////////////////////
class CCollider : public NCollider::CBSPCollider<SColliderUserInfo>, public IPrepareCollider, public CObjectBase
{
	OBJECT_NOCOPY_METHODS(CCollider);
	unordered_map<CPtr<CObjectBase>,bool,SPtrHash> ignoredObjects;
public:
	virtual void AddConvexHull( const SConvexHull &h );
	virtual void SetBoundAndResolution( const SBound &b, float fLeng );
	void CalcEnclosingBound( SBound *pRes, const vector<SSphere> &spheres, const vector<CVec3> &velocities );
	void CollideCheck( const vector<SSphere> &spheres, const vector<CVec3> &velocities, vector<char> *pRes );
	void CollideInfo( const vector<SSphere> &spheres, const vector<CVec3> &velocities, vector<SCollisionPoint> *pRes );
	void CollideInfo( const vector<CVec3> &points, const vector<CVec3> &velocities, vector<SCollisionPoint> *pRes );
	void CollideInfo( const CVec3 &p, const CVec3 &v, SCollisionPoint *pRes );
	void AddIgnoredUserData( CObjectBase *p ) { ignoredObjects[p]; }
};
////////////////////////////////////////////////////////////////////////////////////////////////////
class IAIMap;
void CollideInfo( IAIMap *pMap, const vector<SSphere> &spheres, const vector<CVec3> &velocities, 
	vector<SCollisionPoint> *pRes, int nFlags );
////////////////////////////////////////////////////////////////////////////////////////////////////
}
#endif
