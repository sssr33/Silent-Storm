#ifndef __RPGUNIT_H_
#define __RPGUNIT_H_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "..\DBFormat\DataRPG.h"
#include "..\DBFormat\DataFormat.h"
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "GSkeleton.h"
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace NAI
{
	enum EPose;
}
namespace NDb
{
	class CModel;
	class CRPGClass;
	class CRPGPers;
	class CComplexHead;
	enum EWeaponType;
	enum ESkillType;
	class CAnimWeaponType;
	class CDBDifficulty;
}
namespace NRPG
{
class IFirstAidItem;
////////////////////////////////////////////////////////////////////////////////////////////////////
struct SFirstAid
{
	float fdVP;   // сколько хитов отлечивать за это применение
	int nMaxVP;		// максимальное количество хитов, которое можно отлечить, зависит от мед. скила перса оказывающего первую помощь
};
////////////////////////////////////////////////////////////////////////////////////////////////////
class IInventory;
class CWeaponItem;
class CMeleeWeaponItem;
class CPerksTree;
////////////////////////////////////////////////////////////////////////////////////////////////////
const int N_MAX_SKILL = 140;
const int N_MAX_VP = 250;
const int N_MAX_DC = 300;
////////////////////////////////////////////////////////////////////////////////////////////////////
// Skill который может меняется, это HP, AP
class CDynamicSkill: public CObjectBase
{
	OBJECT_BASIC_METHODS(CDynamicSkill);
private:
	ZDATA
	int nBaseValue; // базовое значение вычесляемое исходя из stat
	int nMaxValue;  // максимально возможное значение скила на этот момент времени, включающие базовую величину
	int nValue;		// текущие значение скила, включающие в себя базовую величину
	float fMultiplier; // текушие критикалы
	float fProgress;	// Прогресс до следующей +1 к скиллу
	ZEND int operator&( CStructureSaver &f ) { f.Add(2,&nBaseValue); f.Add(3,&nMaxValue); f.Add(4,&nValue); f.Add(5,&fMultiplier); f.Add(6,&fProgress); return 0; }
public:
	CDynamicSkill( int nSetValue = 0, int nBaseStatValue = 0 ): 
		nBaseValue(nBaseStatValue), nValue(nSetValue), nMaxValue(nSetValue), fMultiplier( 1 ), fProgress( 0 )
	{
		if ( nBaseValue > nMaxValue )
			nMaxValue = nBaseValue;
		fMultiplier = 1.0f;
	}

	void SetNewMaxValue( int nNewValue );
	void SetNewBaseValue( int nNewValue );
	void Modify( int nModif ) { nValue += nModif; nMaxValue += nModif; }
	bool Upgrade( float fAddToProgress );
	int  GetXPPart() const { return nMaxValue - nBaseValue;} // Значение скила зависящее только от XP
	void SetXPPart( int nNewXPPart ) { nValue += nNewXPPart - GetXPPart(); nMaxValue = nBaseValue + nNewXPPart; }
	void Multiply( float fValue );
	
	float GetProgress() const { return fProgress; }
	int GetMaxValue() const { return nMaxValue; }
	int GetCurrentMaxValue() const { return fMultiplier * nMaxValue; }
	void Reset() { nValue = fMultiplier * nMaxValue; }
	void SetValue( int n ) { nValue = n; }
	void SetProgress( float p ) { fProgress = p; }

	void SetConst( int nConstValue ) { nValue = nMaxValue = nBaseValue = nConstValue; }

	const CDynamicSkill& operator += ( int n ) { nValue += n; if ( nValue > nMaxValue ) nValue = nMaxValue; return *this; }
	const CDynamicSkill& operator -= ( int n ) { nValue -= n; return *this; }
	operator int () const { return nValue; }
};
////////////////////////////////////////////////////////////////////////////////////////////////////
class CSkillModifier: public CObjectBase
{
	OBJECT_NOCOPY_METHODS(CSkillModifier);
	struct SModif
	{
		CPtr<CDynamicSkill> pSkill;
		float fMultiplier; // временный модификатор

		SModif(): fMultiplier(1) {}
		SModif( CDynamicSkill *_pSkill, float fModif ) : pSkill(_pSkill), fMultiplier(fModif) { pSkill->Multiply(fModif); }
		~SModif() { pSkill->Multiply(1.0f/fMultiplier); }
	}data;
public:
	CSkillModifier() {}
	CSkillModifier( CDynamicSkill *pSkill, float fMultiplier ) : data( pSkill, fMultiplier) {}
	void Set( float fMultiplier ) 
	{ 
		data.pSkill->Multiply( 1.0f/data.fMultiplier ); 
		data.fMultiplier = fMultiplier; 
		data.pSkill->Multiply( data.fMultiplier ); 
	}
	float Get() { return data.fMultiplier; }

	int operator&( CStructureSaver &f )
	{
		f.Add( 1, &data.fMultiplier );
		f.Add( 2, &data.pSkill );
		return 0;
	}
};
////////////////////////////////////////////////////////////////////////////////////////////////////
//! CUnit содердит данные которые есть у персонажа в миссии и на базе
class CUnit : public CObjectBase
{
	OBJECT_BASIC_METHODS(CUnit);
public:
	ZDATA
	CDBPtr<NDb::CRPGPers> pPers;
	CDBPtr<NDb::CRPGClass> pClass;
	CDBPtr<NDb::CComplexHead> pHead;
	////
	float fXP;					  // Опыт данного персонажа
	int nCheats;
	int nHealedVP;
	int nRPGPersID;
	vector< CObj<CDynamicSkill> > skills; // его скилы
	////
	wstring wsName;
	CPtr<NDb::CModel> pModel;
	CObj<IInventory> pInventory;
	CPtr<CMeleeWeaponItem> pDefaultWeapon;
	////
	CPtr<CWeaponItem> pCannonItem; // CRAP
private:
	int nDeathVP;
	bool bUnconscious;
	CObj<CPerksTree> pPerksTree;
public:
	////
	CDBPtr<NDb::CRPGPers> pPanzerklein;
	bool bHero;
	ZEND int operator&( CStructureSaver &f ) { f.Add(2,&pPers); f.Add(3,&pClass); f.Add(4,&pHead); f.Add(5,&fXP); f.Add(6,&nCheats); f.Add(7,&nHealedVP); f.Add(8,&nRPGPersID); f.Add(9,&skills); f.Add(10,&wsName); f.Add(11,&pModel); f.Add(12,&pInventory); f.Add(13,&pDefaultWeapon); f.Add(14,&pCannonItem); f.Add(15,&nDeathVP); f.Add(16,&bUnconscious); f.Add(17,&pPerksTree); f.Add(18,&pPanzerklein); f.Add(19,&bHero); return 0; }
	//
	CUnit();
	CUnit( NDb::CRPGPers *pPers, NDb::CComplexHead *pHead = 0, bool _bHero = false );
	
	const wstring& GetName() const;
	void SetName( const wstring &wsName );

	void AddXP( float nXPToAdd );
	CDynamicSkill& Skills( const int eSkill ) { return *skills[eSkill]; }
	bool UseSkill( const int eSkill, const int nAddValue );
	int GetSkillBaseStatValue( const int eSkill );

	NDb::CRPGPers* GetPers() const;
	NDb::CComplexHead* GetHead() const;

	NDb::EWeaponType GetWeaponType() const;
	NDb::CAnimWeaponType* GetDBAnimWeapon() const;
	NDb::ESkillType  GetWeaponSkill() const;
	int GetWeaponAP( CWeaponItem *_pWeapon = 0 ) const;
	int GetWeaponBurstAP( CWeaponItem *_pWeapon = 0 ) const;
	int GetWeaponReloadAP( CWeaponItem *_pWeapon = 0 ) const;
	IInventory *GetInventory() { return pInventory; }
	void SetCannonItem( CWeaponItem *pItem ) { pCannonItem = pItem; }
	CWeaponItem* GetCannonItem() { if ( !IsValid(pCannonItem) ) pCannonItem = 0; return pCannonItem; }
	//
	CWeaponItem* GetWeaponItem() const;
	CMeleeWeaponItem* GetMeleeWeaponItem() const;
	NRPG::IFirstAidItem* GetFirstAidItem() const;

	int GetSkillCap( NDb::ESkillType eSkill, float fXP );
	float GetXPForSkill( NDb::ESkillType eSkill, int nLvl );
	bool IsDead();
	void Kill();
	void SetXPLevel( int nLevel );
	bool IsCheatEnabled( int nCheat );
	void SetCheat( int nCheat, bool bState );
	void UpdateSkills();
	int GetDeathVP() { return nDeathVP; }
	void CalcDeathVP( float _fDeathCoeff );
	//
	void CreateFirstAid( SFirstAid *pRes, int nHealVP, int nSkill ) const;
	bool CreateFirstAid( SFirstAid *pRes, int nMaxSpentAP, float fKitCapacity, 
		IFirstAidItem *pItem, CUnit *pTarget, int *pRequiredAP );
	int GetFirstAidDC( IFirstAidItem *pItem );

	void Heal( const SFirstAid &fa );
	void RegenerateVP( const SFirstAid &fa );
	bool CanHeal( CUnit *pTarget ) const;
	const bool IsUnconscious() const { return bUnconscious; }
	void SetUnconscious( bool _bUnconscious ) { bUnconscious = _bUnconscious; }
	CPerksTree* GetPerksTree() const { return pPerksTree; }
	bool HasPerk( int nPerkID, float *pParam1 = 0, float *pParam2 = 0, float *pParam3 = 0 ) const;
	bool IsHero() const { return bHero; }
};
int GetSkillByCap( int nCap, float fXP );
float GetXPBySkill( int nCap, int nLvl );
////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace
////////////////////////////////////////////////////////////////////////////////////////////////////
#endif