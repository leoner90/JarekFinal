#pragma once
#include "../headers/PlayerEntity/Inventory.h"
#include "../headers/PlayerEntity/EntityAnimation.h"
#include "../headers/PlayerEntity/EntityPhysic.h"
#include "../headers/PlayerEntity/EntityWeapon.h"

//Forward Declaration
class Player;
class MapGen;

class PlayerEntity : public Inventory, public EntityAnimation, public EntityPhysic, public EntityWeapon
{
public:
	//constructor & destructor
	PlayerEntity();
	~PlayerEntity();

	//Main Functions
	void init(int xPos, int yPos, int typeofEnemy);
	void OnUpdate(Uint32 t, MapGen& mapGen, bool Dkey, bool Akey, bool Wkey, bool Skey, bool& isTurnFinished, std::vector<PlayerEntity*>& PlayerEntitys);
	void OnDraw(CGraphics* g);
	void OnKeyDown(SDLKey sym);
	void OnKeyUp(SDLKey sym);

	//Damage
	void EnemyGettingDamage(float damageAmount);
	void OnLButtonDown(Uint16 x, Uint16 y);
	void OnMouseMove(Uint16 x, Uint16 y);
	 

	//mainSprite
	CSprite* enemySprite;

	//Is enemy dead , And coundown till insstance is deleted to play animation
	bool* finishTurn;
	bool dead;
	bool DeathTimer;
	int enemyType;
	bool isPlayerTurn;

	bool weaponSelected;
	
	int weaponType;
	bool isInventoryOpen;

 
	float maxChargeAttackTime;
	bool IsInAttackChargeMode;
	float chargeAttackTimer;

	CSprite chargedAttackBar;
private:

	//Variables Miroring for local use
	MapGen* localMapVar;
	std::vector<PlayerEntity*> localAllPlayersEntitys;
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


	//Enemie Conditions
	enum weaponTypes { AXE, MISSILE, BOMB, BANANA, DYNAMIT, MINE, UZI, BOW, MAIL, SKIP };
	enum AllEnemies { WARIOR, NINJAGIRL, NINJAGIRLKUNAI, DOG, NINJAGIRLMELEE, BOSS1 };
	enum animation_state { NONE, STANDLEFT, STANDRIGHT, WALKLEFT, WALKRIGHT, INAIR, FALLING, SLIDE, INATTACK, ONHIT, DEAD };
	bool inAttack, inDamage;


	void playerPickUpItem();
	void initAnimations();
	void deathHandler();


	int playerState;
	bool hitTestDelay;
	int frameRatesToStop;
	float distToEnemy;
	bool isFriend;
	float EnemyDirection;

	float aimAngle;
	CSprite aimPointer;

	//Sounds
	CSoundPlayer deadSound;
	
	//patroling
	int action;
	float angle = 0;
	CVector initEnemyPos; 
	int old_animation_status;
	//enemie position
	CVector pos;
};