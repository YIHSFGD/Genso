#pragma once
#include"GameObject.h"

namespace Resource {
	void Init(Node* node, Units* units_, Player* pl, AmiUI* _aui);
	void LoadDefaltSound();
	void LoadAmiSound();
	void LoadGalagaSound(Node* node);
	void LoadAllAmiResource(Node* node);
	void LoadYousei(Node* node);
	void LoadBarrier(Node* node);
	void Release();
	Player* GetPlayer();
	void UpdateMousePos(Vec2 pos);
	UINT DeleteOutOfWindow(IUnit* unit, int* tick);
	UINT SpellDeleteActionEnd(IUnit* unit, int* tick);

	void DeleteAllAction(IUnit* unit);
	void ShotToPoint(Bullet* unit);
	UINT BSpellToPoint(Bullet* unit, int* tick);
	UINT DefaltAnimationSpell(IUnit* unit, int tick);
}

namespace AmiResource {
	UINT HpCheck(IUnit* unit);
	void ShotSimple(Bullet* bullet, IUnit* target, IUnit* from, UINT(*BSpell)(Bullet* unit, int* tick));
	void ShotSimple(Bullet* bullet, Vec2 target, IUnit* from, UINT(*BSpell)(Bullet* unit, int* tick));
	void AmiShot1(IUnit* target, IUnit* from);
	void AmiShot1(Vec2 target, IUnit* from);
	void OnBarrir(IUnit* unit, IUnit* barrier, float time = 0);
	void MovePlayer(Vec2 rot);
	//SPELL//
	UINT PlayerSpell(IUnit* unit, int* tick);
	UINT DanmakuDefaltSpell(IUnit* unit, int tick);
	UINT DefaltAnimationFSpell(IUnit* unit, int* tick);
	UINT DeleteAnimationEndFSpell(IUnit* unit, int* tick);
	UINT SpellDoriDori(IUnit* unit, int tick);
	UINT SpellFairy(IUnit* unit, int* tick);

	UINT YouseiAttack();
	void ActivePlayer(Vec2 pos, Animation* playerDefalt, Animation* playerAttack, Player* player);
	UINT PlayerAttackMotion(IUnit* unit, int* tick);
	UINT FairyAttackMotion(IUnit* unit, int* tick);
	UINT PlayerShot(IUnit* unit, int* tick);
	UINT FairyShot(IUnit* unit, int* tick);
	UINT Defence(IUnit* unit, int* tick);

	void PlayerAttackToMousePos();
	void YouseiAttackToPlayer(Enemy* unit);
	void PlayerDefence();
	void SpawnEnemy(Vec2 pos);
}