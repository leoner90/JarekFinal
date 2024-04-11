#pragma once
#include "headers/Inventory.h"

class Player;
class MapGen;

class PlayerEntity : public Inventory
{
public:
	//constructor & destructor
	PlayerEntity();
	~PlayerEntity();

	//Main Functions
	void init(int xPos, int yPos, int typeofEnemy);
	void OnUpdate(Uint32 t, MapGen& mapGen, bool Dkey, bool Akey, bool Wkey, bool Skey, bool& isTurnFinished);
	void OnDraw(CGraphics* g);
	void OnKeyDown(SDLKey sym);
	void OnKeyUp(SDLKey sym);
	//Damage
	void EnemyGettingDamage(float damageAmount, float t, CSpriteList& DroppedPoutions);
	void  OnLButtonDown(Uint16 x, Uint16 y);
	void OnMouseMove(Uint16 x, Uint16 y);
	void performShot();
	//mainSprite
	CSprite* enemySprite;

	//Is enemy dead , And coundown till insstance is deleted to play animation
	bool* finishTurn;
	bool dead;
	bool DeathTimer;
	int enemyType;
	bool isPlayerTurn;

	bool weaponSelected;
	enum weaponTypes { AXE, MISSILE, BOMB, BANANA, DYNAMIT, MINE, UZI, BOW, MAIL, SKIP };
	int weaponType;
	bool isInventoryOpen;

	float ProjectileSpeed;
	float maxChargeAttackTime;
	bool IsInAttackChargeMode;
	float chargeAttackTimer;

	CSprite chargedAttackBar;
private:
	//Variables Miroring for local use
	MapGen* localMapVar;
 
	Uint32 CurrentTime;

	//Enemie Stats 
	float CurrentEnemyHealth;
	float maxEnemyHealth;

	//Hp bar
	void EnemyInterface();

	CSpriteRect* enemyHpBarRect = new CSpriteRect(0, 500, 60, 35, CColor::Blue(), CColor::Blue(), 1);
	CSpriteRect* enemyHpBarRect2 = new CSpriteRect(0, 500, 60, 35, CColor::Red(), CColor::Red(), 1);


	//Sprite Lists for Hit Effect and Shots List
	CSpriteList hitEffectList;
	CSpriteList EnemyShotList;
	CSpriteList explosionList;
	CSprite explosionSprite;

	//Enemie Conditions
	enum AllEnemies { WARIOR, NINJAGIRL, NINJAGIRLKUNAI, DOG, NINJAGIRLMELEE, BOSS1 };
				
	enum action_state { PATROL , CHASSING };
	enum animation_state { NONE, STANDLEFT, STANDRIGHT, WALKLEFT, WALKRIGHT, INAIR, FALLING, SLIDE, INATTACK, ONHIT, DEAD };
	bool inAttack, inDamage;

	//Combat
	void shotsHandler();




 
 
	void playerPickUpItem();


	int playerState;
	bool hitTestDelay;
	int frameRatesToStop;
	float distToEnemy;
	bool isFriend;
	//Base Stats
	float enemySpeed, attackDistance;
	float EnemyDirection;
	char* throwAttackImgPath;
 
	float meleeDamage, kunaiDamage, fireboltDamage, RangeAttackDamage;


	CSprite aimPointer;
	float aimAngle;

	//Sounds
	CSoundPlayer deadSound;
	
	//patroling
	int action;
	float angle = 0;
	CVector initEnemyPos; 

	//enemie position
	CVector pos;

	//Death
	void enemyDeathHandler();

	//Controller
	void playerControler();

	//Collisions
	void playerCollision();
 

	//Animation Handler
	void EnemyAnimation(int old_animation_status);
	void setDefaultStandAnimation();
	void initAnimations();
};