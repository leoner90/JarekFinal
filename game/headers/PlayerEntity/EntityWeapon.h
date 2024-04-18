#pragma once

class MapGen;
class PlayerEntity;
class EntityWeapon
{
public:
	void init();
	void OnUpdate(UINT32 time, std::vector<PlayerEntity*> PlayerEntitys, CSprite* enemySprite, int weaponType, MapGen& Map);
	void OnDraw(CGraphics* g);

	void attackHandler(bool& IsInAttackChargeMode, float& chargeAttackTimer, float& maxChargeAttackTime, CSprite& chargedAttackBar,float EnemyDirection, float aimAngle, bool* finishTurn);
	void shotsHandler();
	void meleeAttack();
	void mailAttack(Uint16 x, Uint16 y);
	bool isInAttackDelay;
	void hitTestHandler(CSprite& mainObj, CSprite& obstacle, float restitution );

private:
	enum weaponTypes { AXE, MISSILE, BOMB, BANANA, DYNAMIT, MINE, UZI, BOW, MAIL, SKIP };
	enum animation_state { NONE, STANDLEFT, STANDRIGHT, WALKLEFT, WALKRIGHT, INAIR, FALLING, SLIDE, INATTACK, ONHIT, DEAD };
	float ProjectileSpeed;
	CSpriteList EnemyShotList;
	CSpriteList explosionList;
	CSprite explosionSprite;
	std::vector<PlayerEntity*> localAllPlayerEntityes;
	CSprite* localEnemySprite;
	UINT32 localTime;
	MapGen* localMap;
	int localWeaponType;

	float explosionTimer;

	UINT32 lastFrametime;
	UINT32 dt;
	bool bananaFirstTouch, mailSelection;

};