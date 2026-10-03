#ifndef __WACKBASE_H_
#define __WACKBASE_H_
#include "..\DBFormat\DataAck.h"

namespace NDb
{
	class CDBAckSequence;
	class CDBAck;
	class CDBAckInfo;
	class CRPGPers;
}

namespace NWorld
{
////////////////////////////////////////////////////////////////////////////////////////////////////
const int N_ACK_CRITICAL = 0;
const int N_ACK_DEATH = 1;
const int N_ACK_SKILL = 2;
////////////////////////////////////////////////////////////////////////////////////////////////////
class CUnit;
class CWorld;
class IPlayer;
class CUnitServer;
class CDumbUnitServer;
////////////////////////////////////////////////////////////////////////////////////////////////////
// IAck           ( Ack == Acknowledgement )
////////////////////////////////////////////////////////////////////////////////////////////////////
class IAck: virtual public CObjectBase
{
public:
	//
	virtual NDb::CDBAck *GetDBAck() { return 0; }
	// обработчики событий
	virtual void OnSegment() {} // сегмент
	virtual void OnEnemyBecomesVisible( CUnitServer *pWatcher, 
		CUnitServer *pTarget, bool bRealTime ) {}
	virtual void OnLastPieceOfAmmo( CUnitServer *pUnit ) {} // кончились патроны
	virtual void OnWeaponJammed( CUnitServer *pUnit ) {} // оружие заклинило
	virtual void OnOrderConfirmation( CUnitServer *pUnit ) {} // подтверждение приказа
	virtual void OnImpossibleToPerformAction( CUnitServer *pUnit ) {} // невозможно выполнить команду
	virtual void OnSuffersLightDamage( CUnitServer *pUnit ) {} // получил легкие повреждения
	virtual void OnSuffersHardDamage( CUnitServer *pUnit ) {} // получил сильные повреждения
	virtual void OnUnitDied( CUnitServer *pUnit ) {} // unit умер
	virtual void OnTargetHit( CUnitServer *pUnit ) {} // Unit попал в цель
	virtual void OnHardTargetHit( CUnitServer *pUnit ) {} // Unit попал в трудную цель
	virtual void OnTargetMissed( CUnitServer *pUnit ) {} // Unit не попал в цель
	virtual void OnGrenadeExplosion( CUnitServer *pUnit, int nUnitsDestroyed, int nObjectsDestroyed ) {} // повреждения от гранаты
	virtual void OnDoDamage( CUnitServer *pAttacker, CUnitServer *pTarget ) {}
	virtual void OnDoAccidentalDamage( CUnitServer *pAttacker, CUnitServer *pTarget ) {}
	virtual void OnDoCriticalDamage( CUnitServer *pAttacker, CUnitServer *pTarget ) {}
	virtual void OnUnitWasKilled( CUnitServer *pAttacker, CUnitServer *pTarget ) {}
	virtual void OnNewTurnStarted( IPlayer *pPlayer ) {}
	virtual void OnRealTimeStarted() {}
	virtual void OnInterrupt( CUnitServer *pWho ) {}
	virtual void OnSkillIncreased( CUnitServer *pWho ) {}
	virtual void OnCannotFinishHeal( CUnitServer *pHealer, CUnitServer *pTarget ) {}
	virtual void OnHealFinished( CUnitServer *pHealer, CUnitServer *pTarget ) {}
};
////////////////////////////////////////////////////////////////////////////////////////////////////
// CGlobalAck
////////////////////////////////////////////////////////////////////////////////////////////////////
class CGlobalAck: public IAck
{
  OBJECT_BASIC_METHODS(CGlobalAck);
	ZDATA
	vector< CObj<IAck> > vAck;
	list< CDBPtr<NDb::CDBAck> > sequences; // накопленные sequence с максимальным приоритетом от сработавших ack
	ZEND int operator&( CStructureSaver &f ) { f.Add(2,&vAck); f.Add(3,&sequences); return 0; }

	bool IsContainUnit( const list< CPtr<CUnit> > &visibleUnits, int nRPGPersID );
	bool IsSequenceVisible( const list< CPtr<CUnit> > &visibleUnits, NDb::CDBAckSequence *pSequence );
	void RemoveInvisibleSequences( IPlayer *pPlayer );
	void FetchHighestAcks();
public:
	CGlobalAck() {}
	// добавление в vAck
	void AddAck( CUnitServer *pUnit );
	// добавление в sequences
	void AddAckSequence( NDb::CDBAck *pAck ); 
	// получить случайную из накопленных sequence с учетом того, кто смотрит
	virtual NDb::CDBAckSequence *GetSequence( IPlayer *pPlayer ); 
	void RemoveUnitAcks( CUnitServer *pUnit );
	// IAck
	virtual void OnSegment();
	virtual void OnEnemyBecomesVisible( CUnitServer *pWatcher, 
		CUnitServer *pTarget, bool bRealTime );
	virtual void OnLastPieceOfAmmo( CUnitServer *pUnit );
	virtual void OnWeaponJammed( CUnitServer *pUnit );
	virtual void OnOrderConfirmation( CUnitServer *pUnit );
	virtual void OnImpossibleToPerformAction( CUnitServer *pUnit );
	virtual void OnSuffersLightDamage( CUnitServer *pUnit );
	virtual void OnSuffersHardDamage( CUnitServer *pUnit );
	virtual void OnUnitDied( CUnitServer *pUnit );
	virtual void OnTargetHit( CUnitServer *pUnit );
	virtual void OnHardTargetHit( CUnitServer *pUnit );
	virtual void OnTargetMissed( CUnitServer *pUnit );
	virtual void OnGrenadeExplosion( CUnitServer *pUnit, int nUnitsDestroyed, int nObjectsDestroyed );
	virtual void OnDoDamage( CUnitServer *pAttacker, CUnitServer *pTarget );
	virtual void OnDoAccidentalDamage( CUnitServer *pAttacker, CUnitServer *pTarget );
	virtual void OnDoCriticalDamage( CUnitServer *pAttacker, CUnitServer *pTarget );
	virtual void OnUnitWasKilled( CUnitServer *pAttacker, CUnitServer *pTarget );
	virtual void OnNewTurnStarted( IPlayer *pPlayer );
	virtual void OnRealTimeStarted();
	virtual void OnInterrupt( CUnitServer *pWho );
	virtual void OnSkillIncreased( CUnitServer *pWho );
	virtual void OnCannotFinishHeal( CUnitServer *pHealer, CUnitServer *pTarget );
	virtual void OnHealFinished( CUnitServer *pHealer, CUnitServer *pTarget );
	virtual void SayAck( CUnitServer *pWho, int nConditionID );
};
////////////////////////////////////////////////////////////////////////////////////////////////////
// CAckBase
////////////////////////////////////////////////////////////////////////////////////////////////////
class CAckBase: public IAck
{
	ZDATA
	CPtr<CUnitServer> pUnit; // поле необходимое для удаления ack при гибели соответствующего Unit-а
	CDBPtr<NDb::CDBAck> pDBAck; // параметры ack-а
public:
	ZEND int operator&( CStructureSaver &f ) { f.Add(2,&pUnit); f.Add(3,&pDBAck); return 0; }

	CAckBase() : pUnit(0), pDBAck(0) { }
	CAckBase( CUnitServer *_pUnit, NDb::CDBAck *_pDBAck );
	CUnitServer *GetUnit();
	// IAck
	virtual NDb::CDBAck *GetDBAck();
	virtual CWorld *GetWorld();
	virtual void PlayAck();
};
////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif