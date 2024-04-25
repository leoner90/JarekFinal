#pragma once
#include "../headers/Physic.h"
class MapGen;
class PlayerEntity;
class Physic;

class EntityWeapon : public Physic
{
public:
	void init();
	void OnUpdate(UINT32 time, std::vector<PlayerEntity*> PlayerEntitys, CSprite& enemySprite, int weaponType, MapGen& Map, float EnemyDirection, float aimAngle);
	void OnDraw(CGraphics* g);

	void attackHandler( float& chargeAttackTimer, float& maxChargeAttackTime, CSprite& chargedAttackBar);
	void shotsHandler();
	void meleeAttack();
 
	bool isInAttackDelay;
 

	void exploditionSetup(float posX, float posY);
	void DamageApply();
	void AmmoSpriteInit();

	void OnKeyDown(SDLKey sym);
	void OnKeyUp(SDLKey sym);
	void AttackSoundPlay();
	void EntityWeapon::OnMouseMove(Uint16 x, Uint16 y);

	CVector windStrength;


private:
	enum weaponTypes { AXE, MISSILE, BOMB, BANANA, DYNAMIT, MINE, UZI, BOW, MAIL, SKIP };
	enum animation_state { NONE, STANDLEFT, STANDRIGHT, WALKLEFT, WALKRIGHT, INAIR, FALLING, SLIDE, INATTACK, ONHIT, DEAD };
	float ProjectileSpeed;

	CSpriteList EnemyShotList;
	CSpriteList explosionAnimationList;
	CSprite explosionDamageImg;
 
 

	CSprite explosionDamageSprite;
	std::vector<PlayerEntity*> localAllPlayerEntityes;
	CSprite* localEnemySprite;
	UINT32 localTime;
	MapGen* localMap;
	int localWeaponType;
	float localEnemyDirection;
	float localAimAngle;
	CVector localAimPointer;
	float explosionTimer;

	UINT32 lastFrametime;
	UINT32 dt;
	bool bananaFirstTouch;


	CSprite* explosion2;
	bool explosionDamageActive;
	bool isAlldamageDealed;
	bool* localFinishTurn;


	bool reflectedRight, reflectedLeft;
	UINT32 angle;
	int damageBasedOnWeaponType[9];
	CSprite* missileSprite;
	CSprite* bombSprite;
	CSprite* bananaSprite;
	CSprite* dynamitSprite;
	CSprite* mineSprite;
	CSprite* uziSprite;
	CSprite* bowSprite;
	CSprite* mailSprite;
	CSprite* explosionSprite;




	float maxChargeAttackTime;
	float chargeAttackTimer;

	CSprite chargedAttackBar;

	CSprite mailAttackMousePointer;

	CSoundPlayer LoadingTick;
	CSoundPlayer Explodition;
	CSoundPlayer attackSound;

	//Physic physic;

protected:
	bool IsInAttackChargeMode;

};