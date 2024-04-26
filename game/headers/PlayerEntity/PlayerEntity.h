#pragma once
#include "../headers/PlayerEntity/Inventory.h"
#include "../headers/PlayerEntity/EntityAnimation.h"
#include "../headers/PlayerEntity/EntityController.h"
#include "../headers/PlayerEntity/EntityWeapon.h"

//Forward Class Declaration
class Player;
class MapGen;

class PlayerEntity : public Inventory, public EntityAnimation, public EntityController, public EntityWeapon
{
public:
	//constructor & destructor
	PlayerEntity();
	~PlayerEntity();

	//Main Functions
	void init(int xPos, int yPos, int typeofEnemy);
	void OnUpdate(Uint32 t, MapGen& mapGen, bool movement[], bool& isTurnFinished, std::vector<PlayerEntity*> PlayerEntitys);
	void OnDraw(CGraphics* g);

	//KEYBOAR & MOUSEEVENETS
	void OnKeyDown(SDLKey sym);
	void OnKeyUp(SDLKey sym);
	void OnMouseMove(Uint16 x, Uint16 y);
	void OnLButtonDown(Uint16 x, Uint16 y);

	//Getting Damage
	void EtityGettingDamage(float damageAmount);

	//updates player When Final Cutscene IS Playing
	void GameWonPlayerUpdate(float t);

	//Shows Msg when receives damage or pick up healt boxes
	void showInfoMsg(CGraphics* g);

	//Is the A ground under player on start of the game as map is randomly generated!!
	bool FindGroundForAllPlayersOnStart(float t, MapGen& mapGen);
	bool onTheGround = false;
	
	//TO DO GETTERS AND SETTERS IF WILL HAVE TIME
	//mainSprite
	CSprite* entitySprite;

	//Player Team Number
	bool teamMemberNumber;

	//Player Turn Change Logic Variables
	bool beenUsed;
	bool isPlayerTurn;


	//Death Handler Variables
	bool isDead;
	float  DeathTimer;
 

	//Enemie Stats 
	float CurrentEnemyHealth;
	float maxEnemyHealth;

private:
	//####### FUNCTIONS

	//Pick up Healt logic
	void playerPickUpItem();

	//init Animation
	void initAnimations();

	//Death Handler
	void deathHandler();

	//End Of Turn Logic
	void EndOfTurn();

	//Start OF Turn
	void StartOfTurn();

	//OHIT RESET
	void OnHitStatusReset();

	//AIMING
	void Aiming();

	//IF Y < 0 DEATH HANDLER
	void underWaterDeath();

	//Hp bar update
	void EnemyInterface();


	//####### VARIABLES

	//localVar just miroring to be able use in whole class
	MapGen* localMapVar;
	Uint32 localTime;
	std::vector<PlayerEntity*> localAllPlayersEntitys;

	//Enemie Conditions
	enum weaponTypes { AXE, MISSILE, BOMB, BANANA, DYNAMIT, MINE, UZI, BOW, MAIL, SKIP };
	enum animation_state { NONE, STANDLEFT, STANDRIGHT, WALKLEFT, WALKRIGHT, INAIR, JUMP, SLIDE, INATTACK, ONHIT, DEAD, WINERTEAM };

	//KEYS PRESSED ENUM
	enum movements { Dkey, Akey, Wkey, Skey };

	//Save Old Animation Status
	int old_animation_status;

	//ON Turn Start => SHOW "TURN ARROW"
	bool isOnStartOfTheTurn;
	float showTurnArrowTimer;
	CSprite showTurnArrow;

	//Player Turn Change Logic Variables
	bool* localIsTurnFinished;
	float turnEndTimer;
	float turnStartTimer;

	//Turn Transition Variables
	bool isInTurnTransitStatus;
	float  TurnTransitStatusTimer;
	
	//Aim Poiner & Angle
	float aimAngle;
	CSprite aimPointer;

	//Player Direction (rotation)
	float EnemyDirection;

	//player State
	int playerState;

	//prev frame position
	CVector pos;

	//in Damage Timer (to reset animation back to normal)
	float inDamageTimer;

	//hide Timer When Game over Custcene playinh
	bool hideTimer;

	//check is inventory open or not
	bool isInventoryOpen;

	//Weapon in hand variables
	bool weaponSelected;
	int weaponType;

	//ON Damage Or Heal Shoe Msg Box
	float infoMsgTimer;
	float infoMsg;
	CSprite infoMsgBg;

	//Hp bars
	CSpriteRect* enemyHpBarRect = new CSpriteRect(0, 500, 60, 35, CColor::Blue(), CColor::Blue(), 1);
	CSpriteRect* enemyHpBarRect2 = new CSpriteRect(0, 500, 60, 35, CColor::Red(), CColor::Red(), 1);

	//Sounds
	CSoundPlayer hitSound;
	CSoundPlayer pickUp;
	CSoundPlayer explosionSound;
	CSoundPlayer deadSound;
	CSoundPlayer StartRoundSound;
	CSoundPlayer preDeath;
	CSoundPlayer openInventorySound;
	CSoundPlayer weaponSelectedSound;
	CSoundPlayer jumpSound;

	//Jump Timer Bug  Fixing..
	float jumpTimer;


};