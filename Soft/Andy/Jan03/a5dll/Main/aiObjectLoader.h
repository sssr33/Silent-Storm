#ifndef __aiObjectLoader_H_
#define __aiObjectLoader_H_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GFileSkin.h"
#include "GResource.h"
#include "aiObject.h"
#include "GSkeleton.h"
////////////////////////////////////////////////////////////////////////////////////////////////////
class CMemObject;
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace NAI
{
class CGeometryInfo;
////////////////////////////////////////////////////////////////////////////////////////////////////
class CLoadGeometryInfo : public NGScene::CResourceLoader<int, CGeometryInfo>
{
	OBJECT_BASIC_METHODS(CLoadGeometryInfo);
protected:
	virtual void Recalc();
};
////////////////////////////////////////////////////////////////////////////////////////////////////
class CBSPTree;
class CTwoBSPTrees : public CObjectBase
{
	OBJECT_BASIC_METHODS(CTwoBSPTrees);
public:
	vector<NAI::CBSPPieces> treesOpen;
	vector<NAI::CBSPPieces> treesClosed;
};
////////////////////////////////////////////////////////////////////////////////////////////////////
class CLoadTwoBSPTrees : public NGScene::CResourceLoader<int, CTwoBSPTrees>
{
	OBJECT_BASIC_METHODS(CLoadTwoBSPTrees);
protected:
	virtual void Recalc();
};
////////////////////////////////////////////////////////////////////////////////////////////////////
class CMemGeometryInfo : public CPtrFuncBase<CGeometryInfo>
{
	OBJECT_BASIC_METHODS(CMemGeometryInfo);
protected:
	CPtr<CMemObject> pMemObject;
	virtual void Recalc();
public:
	CMemGeometryInfo( CMemObject *p = 0 ): pMemObject(p) {}
};
////////////////////////////////////////////////////////////////////////////////////////////////////
class CFileSkinPoints : public CObjectBase
{
	OBJECT_BASIC_METHODS(CFileSkinPoints);
public:
	vector<SMassSphere> spheres;
	CVec3 massCenter;
	struct SBodypart
	{
		vector<CVec3> points;
		CEdgesInfo edges;
		vector<NGScene::SVertexWeight> weights;
	};
	typedef unordered_map<int, SBodypart> CBodypartsHash;
	CBodypartsHash parts;
};
////////////////////////////////////////////////////////////////////////////////////////////////////
class CFileSkinPointsLoad : public NGScene::CResourceLoader<int, CFileSkinPoints>
{
	OBJECT_BASIC_METHODS(CFileSkinPointsLoad);
protected:
	virtual void Recalc();
};
////////////////////////////////////////////////////////////////////////////////////////////////////
class CSkinner: public CPtrFuncBase<CGeometryInfo>
{
	OBJECT_BASIC_METHODS(CSkinner);
	ZDATA
	CDGPtr< CFuncBase<NGScene::SSkeletonMatrices> > pAnimation;
	CDGPtr< CPtrFuncBase<CFileSkinPoints> > pSkin;
	bool bDoBSP;
	CBSPPieces setTrees;
	ZEND int operator&( CStructureSaver &f ) { f.Add(2,&pAnimation); f.Add(3,&pSkin); f.Add(4, &bDoBSP); f.Add(5, &setTrees); return 0; }
protected:
	virtual bool NeedUpdate() { return pAnimation.Refresh() | pSkin.Refresh(); }
	virtual void Recalc();
public:
	CSkinner() {}
	CSkinner( CPtrFuncBase<CFileSkinPoints> *_pSkin, CFuncBase<NGScene::SSkeletonMatrices> *_pAnimation )
		:pSkin(_pSkin), pAnimation(_pAnimation), bDoBSP(false) {}
	void CreateBSPTrees() { bDoBSP = true; }
	void SetBSPTrees( const CBSPPieces &trees ) { bDoBSP = false; setTrees = trees; }
};
////////////////////////////////////////////////////////////////////////////////////////////////////
// create cube hull
//void MakeCube( CConvexHull *pRes, const CVec3 &base, const CVec3 &size );
}
#endif
