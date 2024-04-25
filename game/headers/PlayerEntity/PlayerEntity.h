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
	void OnUpdate(Uint32 t, MapGen& mapGen, bool Dkey, bool Akey, bool Wkey, bool Skey, bool Fkey, bool& isTurnFinished, std::vector<PlayerEntity*> PlayerEntitys);
	void OnDraw(CGraphics* g);
	void OnKeyDown(SDLKey sym);
	void OnKeyUp(SDLKey sym);

	//Damage
	void EnemyGettingDamage(float damageAmount);
	void OnLButtonDown(Uint16 x, Uint16 y);


	void OnMouseMove(Uint16 x, Uint16 y);
	 
	void GameWonPlayerUpdate(float t);
	void showInfoMsg(CGraphics* g);

	//mainSprite
	CSprite* enemySprite;

	//Is enemy dead , And coundown till insstance is deleted to play animation
	bool* finishTurn;
	bool isDead;
	float  DeathTimer, DeathExploditionTimer;
	int enemyType;
	bool isPlayerTurn;

	bool weaponSelected;
	
	int weaponType;
	bool isInventoryOpen;
	bool isFriend;
	float turnEndTimer;
	float turnStartTimer;
	bool beenUsed;
	//Enemie Stats 
	float CurrentEnemyHealth;
	float maxEnemyHealth;

	bool* localIsTurnFinished;
private:

	//Variables Miroring for local use
	MapGen* localMapVar;
	std::vector<PlayerEntity*> localAllPlayersEntitys;
	Uint32 CurrentTime;

	

	//Hp bar
	void EnemyInterface();

	CSpriteRect* enemyHpBarRect = new CSpriteRect(0, 500, 60, 35, CColor::Blue(), CColor::Blue(), 1);
	CSpriteRect* enemyHpBarRect2 = new CSpriteRect(0, 500, 60, 35, CColor::Red(), CColor::Red(), 1);


 
	CSoundPlayer hitSound;
	CSoundPlayer pickUp;
	CSoundPlayer explosionSound;


	//Enemie Conditions
	enum weaponTypes { AXE, MISSILE, BOMB, BANANA, DYNAMIT, MINE, UZI, BOW, MAIL, SKIP };
	enum animation_state { NONE, STANDLEFT, STANDRIGHT, WALKLEFT, WALKRIGHT, INAIR, JUMP, SLIDE, INATTACK, ONHIT, DEAD, WINERTEAM};
	bool inAttack, inDamage;


	void playerPickUpItem();
	void initAnimations();
	void deathHandler();
	void EndOfTurn();


	int playerState;
	bool hitTestDelay;
	int frameRatesToStop;
	float distToEnemy;
	
	float EnemyDirection;

	float aimAngle;
	CSprite aimPointer;

	//Sounds
	CSoundPlayer deadSound;
	CSoundPlayer StartRoundSound;
	CSoundPlayer preDeath;
	CSoundPlayer openInventorySound;
	CSoundPlayer weaponSelectedSound;
	CSoundPlayer jumpSound;

	float angle = 0;
	CVector initEnemyPos; 
	int old_animation_status;
	//enemie position
	CVector pos;


	bool isInTurnTransitStatus;
	float  TurnTransitStatusTimer;


	float inDamageTimer;


	CSpriteList deathexplosionAnimationList;
	CSprite* deathExplosionSprite;

	bool hideTimer;

	//on start Arrow Sprite show
	bool isOnStartOfTheTurn;
	float showTurnArrowTimer;
	CSprite showTurnArrow;

	//onDamage Recived show Damage box
	float infoMsgTimer;
	float infoMsg;
	CSprite infoMsgBg;
};