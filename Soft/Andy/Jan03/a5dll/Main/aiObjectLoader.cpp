#include "StdAfx.h"
#include "aiObjectLoader.h"
#include "MemObject.h"
#include "..\Misc\BasicShare.h"
#include "aiObject.h"
#include "GFileSkin.h"
#include "BSPTree.h"
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace NAI
{
////////////////////////////////////////////////////////////////////////////////////////////////////
static CVec3 CalcMassCenter( vector<SMassSphere> &spheres )
{
	float fMassSum = 0;
	CVec3 massCenter = VNULL3;
	for ( int i = 0; i < spheres.size(); ++i )
	{
		if ( spheres[i].fMass <= 0 )
			spheres[i].fMass = 1;
		fMassSum += spheres[i].fMass;
		massCenter += spheres[i].fMass * spheres[i].ptCenter;
	}
	massCenter /= fMassSum;
	return massCenter;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
// CLoadGeometryInfo
////////////////////////////////////////////////////////////////////////////////////////////////////
struct SStoredPiece
{
	vector<STriangle> tris;
	vector<CVec3> verts;		
	float fVolume;
	vector<SJunction> juncs;
	vector<CPtr<CBSPTree> > trees;

	int operator&( CStructureSaver &f )
	{ 
		f.Add( 1, &verts );
		f.Add( 2, &tris );
		f.Add( 10, &fVolume );
		f.Add( 11, &juncs );
		f.Add( 12, &trees );
		return 0;
	}
};
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef unordered_map<int, SStoredPiece > CStoredPieceMap;
////////////////////////////////////////////////////////////////////////////////////////////////////
void CLoadGeometryInfo::Recalc()
{
	pValue = new CGeometryInfo;
	try
	{
		NGScene::CResourceOpener file( "AIGeometries", GetKey() );
		vector<CVec3> points;
		vector<STriangle> tris;
		vector<CPtr<CBSPTree> > trees;

		CStoredPieceMap pieces;
		file->Add( 1, &points );
		file->Add( 2, &tris );
		file->Add( 4, &pieces );
		file->Add( 6, &pValue->spheres );
		file->Add( 8, &trees );

		if ( pieces.empty() )
		{
			pValue->AddPiece( 0, points, tris, 0, vector<SJunction>(), true, trees );
			ASSERT( !trees.empty() );
		}
		else
		{
			for ( CStoredPieceMap::const_iterator i = pieces.begin(); i != pieces.end(); ++i )
			{
				pValue->AddPiece( i->first, i->second.verts, i->second.tris, i->second.fVolume, i->second.juncs, true, i->second.trees );
				ASSERT( i->second.tris.empty() || (!i->second.trees.empty()) );
			}
		}
		pValue->CalcBound();
		pValue->massCenter = CalcMassCenter( pValue->spheres );
	}
	catch(...)
	{
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////
// CLoadTwoBSPTrees
////////////////////////////////////////////////////////////////////////////////////////////////////
void CLoadTwoBSPTrees::Recalc()
{
	pValue = new CTwoBSPTrees;
	try
	{
		NGScene::CResourceOpener file( "AIBSPTrees", GetKey() );
		file->Add( 1, &pValue->treesOpen );
		file->Add( 2, &pValue->treesClosed );
	}
	catch(...)
	{
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////
// CMemGeometryInfo
////////////////////////////////////////////////////////////////////////////////////////////////////
struct SMergePoints
{
	vector<CVec3> points;
	vector<STriangle> tris;

	int GetPointIndex( const CVec3 &a )
	{
		for ( int k = 0; k < points.size(); ++k )
		{
			if ( points[k] == a )
				return k;
		}
		points.push_back( a );
		return points.size() - 1;
	}
	void AddTriangle( const CVec3 &a, const CVec3 &b, const CVec3 &c )
	{
		tris.push_back( STriangle( GetPointIndex(a), GetPointIndex(b), GetPointIndex(c) ) );
	}
};
////////////////////////////////////////////////////////////////////////////////////////////////////
void CMemGeometryInfo::Recalc()
{
	pValue = new CGeometryInfo;
	SMergePoints p;
	const vector<CVec3> &points = pMemObject->GetPoints();
	const vector<STriangle> &tris = pMemObject->GetTris();
	for ( int k = 0; k < tris.size(); ++k )
		p.AddTriangle( points[tris[k].i1], points[tris[k].i2], points[tris[k].i3] );
	pValue->AddPiece( -1, p.points, p.tris, 0 );
	pValue->CalcBound();
}
////////////////////////////////////////////////////////////////////////////////////////////////////
// CFileSkinPointsLoad
////////////////////////////////////////////////////////////////////////////////////////////////////
struct SStoredSkin
{
	vector<CVec3> points;
	vector<STriangle> tris;
	vector<NGScene::SLoadVertexWeight> weights;

	int operator&( CStructureSaver &f )
	{ 
		f.Add( 1, &points );
		f.Add( 2, &tris );
		f.Add( 3, &weights );
		return 0;
	}
};
typedef unordered_map<int, SStoredSkin> CBodypartsStoredHash;
////////////////////////////////////////////////////////////////////////////////////////////////////
void CFileSkinPointsLoad::Recalc()
{
	pValue = new CFileSkinPoints;

	NGScene::CResourceOpener file( "AIGeometries", GetKey() );
	CBodypartsStoredHash data;
	file->Add( 4, &data );
	file->Add( 6, &pValue->spheres );
	pValue->massCenter = CalcMassCenter( pValue->spheres );
	for ( CBodypartsStoredHash::const_iterator i = data.begin(); i != data.end(); ++i )
	{
		const SStoredSkin &src = i->second;
		CFileSkinPoints::SBodypart &r = pValue->parts[i->first];
		r.points = src.points;
		r.edges.GenerateEdgeList( src.tris, src.points );
		NGScene::ConvertWeights( &r.weights, src.weights, src.points.size() );
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////
// CSkinner
////////////////////////////////////////////////////////////////////////////////////////////////////
void CSkinner::Recalc()
{
	const CFileSkinPoints &src = *pSkin->GetValue();
	if ( !IsValid( pValue ) )
	{
		pValue = new CGeometryInfo;
		for ( CFileSkinPoints::CBodypartsHash::const_iterator i = src.parts.begin(); i != src.parts.end(); ++i )
		{
			CGeometryInfo::SPiece &dst = pValue->pieces[i->first];
			const CFileSkinPoints::SBodypart &src = i->second;
			dst.edges = src.edges;
			ASSERT( src.edges.IsClosed() );
			dst.points.resize( src.points.size() );
		}
	}
	//pValue->points.resize( nVertices );
	typedef CVec3 SVertex;
	using NGScene::SVertexWeight;
	
	for ( CFileSkinPoints::CBodypartsHash::const_iterator i = src.parts.begin(); i != src.parts.end(); ++i )
	{
		CGeometryInfo::SPiece &dst = pValue->pieces[i->first];
		const CFileSkinPoints::SBodypart &src = i->second;
		ASSERT( src.points.size() == dst.points.size() );
		int nVertices = src.points.size();
		SVertex *pRes = &dst.points[0];
		const SVertex *pMesh = &src.points[0];
		const SVertexWeight *pWeight = &src.weights[0];
		const NGScene::SSkeletonMatrices &blends = pAnimation->GetValue();
		memset( pRes, 0, sizeof(SVertex) * nVertices );
		CVec3 p;
		for ( int i = 0; i < nVertices; ++i )
		{
			int j = 0;
			while ( pWeight->fWeights[j] && j < 4 )
			{
				blends[ pWeight->cBoneIndices[j] ].RotateHVector( &p, *pMesh );
				*pRes += pWeight->fWeights[j] * p;
				++j;
			}
			pRes++;
			pMesh++;
			pWeight++;
		}
	}
	pValue->CalcBound();	
	if ( bDoBSP )
		pValue->CalcBSPTrees();
	else if ( !setTrees.empty() )
	{
		pValue->SetBSPTrees( setTrees );
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace
using namespace NAI;
REGISTER_SAVELOAD_CLASS( 0x012c1160, CLoadGeometryInfo )
REGISTER_SAVELOAD_CLASS( 0x012c1161, CMemGeometryInfo )
REGISTER_SAVELOAD_CLASS( 0x014c1110, CFileSkinPointsLoad )
REGISTER_SAVELOAD_CLASS( 0x014c1111, CSkinner )
REGISTER_SAVELOAD_CLASS( 0x73102120, CTwoBSPTrees )
REGISTER_SAVELOAD_CLASS( 0x73102121, CLoadTwoBSPTrees )
