#pragma once
#include "../headers/Physic.h"

//Forward Class Declaration
class MapGen;
class PlayerEntity;
class Physic;

class EntityWeapon : public Physic
{
public:
	EntityWeapon();

	//Base Functions
	void init();
	void OnUpdate(UINT32 time, std::vector<PlayerEntity*> PlayerEntitys, CSprite& entitySprite, int weaponType, MapGen& Map, float EnemyDirection, float aimAngle);
	void OnDraw(CGraphics* g);

	//KeyBoard & Mouse Handler
	void OnKeyDown(SDLKey sym);
	void OnKeyUp(SDLKey sym);
	void OnMouseMove(Uint16 x, Uint16 y);

	//TO DO GETTERS SETTERS IF WILL HAVE TIME
	CVector windStrength;

	//ALL SHOT LIST
	CSpriteList EnemyShotList;

private:
	//ATTACK SOUNDS LOGIC
	void AttackSoundPlay();

	//ATTACKS HANDLERS
	void attackHandler(float& chargeAttackTimer, float& maxChargeAttackTime, CSprite& chargedAttackBar);
	void meleeAttack();
	void shotsHandler();

	//BASICLY SHOTS HANDLER FUNCTION SPLIT!!
	void ApplyBasePhysic(CSprite& shot);
	void OnExploditionTimerEnd(CSprite& shot);
	void BombAndBananaLogic(CSprite& shot);
	void otherProjectileHandler(CSprite& shot);

	//INIT SPRITES
	void AmmoSpriteInit();

	//Explodition Image And Damage
	void exploditionSetup(float posX, float posY);
	void DamageApply();

	//STATES
	enum weaponTypes { AXE, MISSILE, BOMB, BANANA, DYNAMIT, MINE, UZI, BOW, MAIL, SKIP };
	enum animation_state { NONE, STANDLEFT, STANDRIGHT, WALKLEFT, WALKRIGHT, INAIR, FALLING, SLIDE, INATTACK, ONHIT, DEAD };

	//local Variables-  just miroring to be able use in whole class
	std::vector<PlayerEntity*> localAllPlayerEntityes;
	CSprite* localEnemySprite;
	UINT32 localTime;
	MapGen* localMap;
	CVector localAimPointer;
	int localWeaponType;
	float localEnemyDirection;
	float localAimAngle;

	//Delta Time
	UINT32 lastFrametime;
	UINT32 dt;

	//CHARGE , USED TO CALCULATE PROJECTILE SPEED
	float maxChargeAttackTime;
	float chargeAttackTimer;
	float ProjectileSpeed;

	//DAMAGE BASED O WEAPON TYPE
	int damageBasedOnWeaponType[9];

	//explosion EXPLOSION AND DAMAGE
	CSprite* explosionSprite;
	CSpriteList explosionAnimationList;
	CSprite explosionDamageImg;
	bool explosionDamageActive;
	float explosionTimer;

	//For Banana multiplication on first Hit test
	bool bananaFirstTouch;	

	

	//Spirtes Projectile Prefabs
	CSprite* missileSprite;
	CSprite* bombSprite;
	CSprite* bananaSprite;
	CSprite* dynamitSprite;
	CSprite* mineSprite;
	CSprite* uziSprite;
	CSprite* bowSprite;
	CSprite* mailSprite;

	//SPRITES CHARGE ATTACK AND MOUSE POINTER FOR MAIL ATTACK
	CSprite chargedAttackBar;
	CSprite mailAttackMousePointer;

	//SOUNDS 
	CSoundPlayer LoadingTick;
	CSoundPlayer Explodition;
	CSoundPlayer attackSound;

protected:
	//IS IN CHARGE MODE
	bool IsInAttackChargeMode;
};