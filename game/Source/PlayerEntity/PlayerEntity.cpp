#include "stdafx.h"
#include "../headers/PlayerEntity/PlayerEntity.h"
#include "../headers/Map.h"
 
PlayerEntity::PlayerEntity()
{
	entitySprite = new CSprite();
	initAnimations(); // INIT Animations And Images
}

PlayerEntity::~PlayerEntity()
{
	delete entitySprite;
	delete enemyHpBarRect;
	delete enemyHpBarRect2;
}

//*** INIT (is called on PlayerEntity Creation)
void PlayerEntity::init(int xPos, int yPos, int typeofPlayer)
{
	onTheGround = false;
	//init Resets
	isPlayerTurn = false;
	isInventoryOpen = false;
	weaponSelected = false;
	isDead = false;
	isOnStartOfTheTurn = true;
	turnStartTimer = 0;
	DeathTimer = 0;
	weaponType = -1;
	aimAngle = 0;
	inDamageTimer = 0;
	showTurnArrowTimer = 0;
	infoMsgTimer = 0;
	EnemyDirection = 90;
	TurnTransitStatusTimer = 0;

	//Enemy Stats
	CurrentEnemyHealth = maxEnemyHealth = 100;

	//Team Assignment
	if (typeofPlayer == 0) teamMemberNumber = false;
	if (typeofPlayer == 1) teamMemberNumber = true;
 
	//Sprite Defaul Inits
	entitySprite->SetStatus(STANDRIGHT);
	entitySprite->SetPosition(float(xPos), float(yPos));
	entitySprite->SetAnimation("idleRight", 1);
	entitySprite->SetSpeed(0);

	//Weapon Init
	EntityWeapon::init();
}

//**************************** UPDATE ****************************
void PlayerEntity::OnUpdate(Uint32 t, MapGen& mapGen, bool movement[], bool& isTurnFinished, std::vector<PlayerEntity*> PlayerEntitys)
{
	//localVar just miroring to be able use in whole class
	localTime = t;
	localMapVar = &mapGen;
	localAllPlayersEntitys = PlayerEntitys;
	localIsTurnFinished = &isTurnFinished;


	underWaterDeath(); // if Y < 0
	EndOfTurn();	//ON Turn Finished
	StartOfTurn(); // On Start Of The Turn
	OnHitStatusReset(); // OH hit RESET

	if (isPlayerTurn) turnEndTimer = (int)(turnStartTimer - t) / 1000;

	// ** Weapon AND ANIMTAION UPDATE
	EntityWeapon::OnUpdate(localTime, localAllPlayersEntitys, *entitySprite, weaponType, mapGen, EnemyDirection, aimAngle);
	EntityAnimation::EntityAnimationUpdate(old_animation_status, entitySprite, weaponSelected, weaponType, EnemyDirection, aimAngle);
	old_animation_status = entitySprite->GetStatus();

	// ** COllisions
	EntityController::playerCollision(localTime, playerState, entitySprite,pos, &mapGen);

	// ** Enemy Controller
	EntityController::playerControler(EntityWeapon::IsInAttackChargeMode, playerState, entitySprite, isPlayerTurn, isInTurnTransitStatus, isInventoryOpen, EnemyDirection, aimAngle, movement);

	//Enemy position save
	pos = entitySprite->GetPos();

	//Sprite Update
	entitySprite->Update(localTime);

	//AIMING
	Aiming();
	
	// ** PICK UP HEALTH BOXES
	playerPickUpItem();

	// ** Death Handler
	deathHandler();
 
	// ** Interface (HP BAR)
	EnemyInterface();
}


//**************************** DRAW ****************************
void PlayerEntity::OnDraw(CGraphics* g)
{
	showInfoMsg(g);

	//HealthBar
	if(CurrentEnemyHealth > 0 && teamMemberNumber) enemyHpBarRect->Draw(g);
	else if (CurrentEnemyHealth > 0 && !teamMemberNumber)  enemyHpBarRect2->Draw(g);
	
	//Weapon Draw function
	EntityWeapon::OnDraw(g);

	//Draw Player Sprite If not dead
	if(!isDead) entitySprite->Draw(g);

	//Draw Curent turn Arrow and Animate
	if (showTurnArrowTimer > localTime)
	{
		showTurnArrow.SetPosition(entitySprite->GetX(), entitySprite->GetTop() + 35 + rand() % 10);
		showTurnArrow.Draw(g);
	}

	//Show Turn Timer if isPlayerTurn && not a final cutscene
	if (isPlayerTurn && !hideTimer)
	{
		Inventory::OnDraw(g);
		*g << font(52) << color(turnEndTimer < 6 ? CColor::Red() :CColor::White()) << xy(0, 985) << center <<  turnEndTimer ;
	}

	//DRAW MOUSE POINTER IF NEEDED!! :))
	if (!isDead && isPlayerTurn && (weaponSelected || isInventoryOpen) && (weaponType != MAIL || isInventoryOpen)  && !isInTurnTransitStatus) aimPointer.Draw(g);
}
 

//***** Health Bar UPDATE
void PlayerEntity::EnemyInterface()
{
	float baseHpBarWidth = 50;

	float hpBarSize = baseHpBarWidth * (CurrentEnemyHealth / maxEnemyHealth);
	if (hpBarSize < 0) hpBarSize = 0;

	enemyHpBarRect2->SetSize(hpBarSize, 15);
	enemyHpBarRect2->Update(localTime);

	enemyHpBarRect->SetSize(hpBarSize, 15);
	enemyHpBarRect->Update(localTime);

	enemyHpBarRect2->SetPosition(entitySprite->GetX(), entitySprite->GetTop() + 20);
	enemyHpBarRect->SetPosition(entitySprite->GetX(), entitySprite->GetTop() + 20);
}

//Is the A ground under player on start of the game as map is randomly generated!!
bool PlayerEntity::FindGroundForAllPlayersOnStart(float t, MapGen& mapGen)
{
	//It is Disabled by default in MyGame.cpp, as I haven't check it properly, but I it works fine
	if (onTheGround) return true; // if allready on the ground return true at once
	entitySprite->SetYVelocity(-5000);
 
	//if under screen move left X axis , and reset Y axis
	//TO DO - reset x position if player reaches end of the screen
	if (entitySprite->GetY() < 0)
	{
		entitySprite->SetY(1000);
		entitySprite->SetX(entitySprite->GetX() + 32);
	}

	//hit test with a map
	for (auto mapPart : mapGen.mapList)
	{
		if (entitySprite->HitTest(mapPart))
		{
			onTheGround = true;
			entitySprite->SetSpeed(0);
			return true;
		}
	}

	entitySprite->Update(t);
	return false;
}


//***** Health Box Pick UP Logic
void PlayerEntity::playerPickUpItem()
{
	for (auto loot : localMapVar->lootList)
	{
		if (entitySprite->HitTest(loot))
		{
			pickUp.Play("pickUp.wav");
			CurrentEnemyHealth += 50;
			infoMsg = 50;
			infoMsgTimer = localTime + 2000;
			if (CurrentEnemyHealth > maxEnemyHealth) CurrentEnemyHealth = maxEnemyHealth;
			loot->Delete();
		}
	}
}


//***** Death Handler
void PlayerEntity::deathHandler()
{
	if (entitySprite->GetStatus() == DEAD && DeathTimer < localTime && DeathTimer != 0)
	{
		DeathTimer = 0;
		deadSound.Play("Explosion.wav");
		deadSound.Volume(0.3);
		localMapVar->addGrave(entitySprite->GetX(), entitySprite->GetY());
		isDead = true;
		if (isPlayerTurn) *localIsTurnFinished = true;
	}
}

//***** END OF TURN LOGIC
void PlayerEntity::EndOfTurn()
{
	//return if not all animations been played and if there is some shots left
	if (DeathTimer > 0 || EnemyShotList.size() > 0)
	{
		turnEndTimer = 0;
		return;
	}

	if (((TurnTransitStatusTimer != 0 && TurnTransitStatusTimer < localTime) || turnEndTimer <= 0) && turnStartTimer != 0 && isPlayerTurn)
	{
		//Release Weapon If time is over && player still charging
		if (IsInAttackChargeMode) EntityWeapon::OnKeyUp(SDLK_SPACE);
		//Base Resets
		IsInAttackChargeMode = false;
		entitySprite->SetSpeed(0);
		footsteps.Stop();
		*localIsTurnFinished = true;
		TurnTransitStatusTimer = 0;
		turnStartTimer = 0;
		isInTurnTransitStatus = false;
		isPlayerTurn = false;
		weaponSelected = false;
		weaponType = -1;
		isOnStartOfTheTurn = true;
		showTurnArrowTimer = 0;
		entitySprite->SetStatus(NONE);
	}
}

//***** START OF TURN
void PlayerEntity::StartOfTurn()
{
	//SETS TIMER AND ARROW (to show current player turn)
	if (isOnStartOfTheTurn && isPlayerTurn)
	{
		isOnStartOfTheTurn = false;
		showTurnArrowTimer = localTime + 4000;
		StartRoundSound.Play("StartRound.wav");
		//reset Turn Timer
		turnStartTimer = 20000 + localTime;
	}
}

//***** RESETS ANIMATION AFTER HIT EFFECT
void PlayerEntity::OnHitStatusReset()
{
	if (entitySprite->GetStatus() == ONHIT && inDamageTimer < localTime)
	{
		entitySprite->SetStatus(ONHIT);
		if (EnemyDirection == 90) entitySprite->SetStatus(STANDRIGHT);
		else entitySprite->SetStatus(STANDLEFT);
		inDamageTimer = 0;
	}
}

//***** AIMING
void PlayerEntity::Aiming()
{
	//MOVES CURSORS AND SAVES ANGLE...
	if (weaponSelected && !isInventoryOpen && weaponType != AXE && weaponType != MAIL)
	{
		CVector side;
		if (EnemyDirection == 90)	side = { 60, aimAngle };
		else  side = { -60, aimAngle };
		aimPointer.SetPos(entitySprite->GetPos() + side);
	}
}

//***** IF Dead under Water y < 0
void PlayerEntity::underWaterDeath()
{
	//IF Player GetY < 0 => delete and play UnderWaterDeath sound
	if (entitySprite->GetY() < 0)
	{
		if (isPlayerTurn) *localIsTurnFinished = true;
		isDead = true;
		deadSound.Play("UnderWaterDeath.wav");
		deadSound.Volume(0.15);
		return;
	}
}

//**FINAL WHEN GAME WON SPRITES UPDATE
void PlayerEntity::GameWonPlayerUpdate(float t)
{
	hideTimer = true;
	entitySprite->SetStatus(WINERTEAM);
	EntityAnimation::EntityAnimationUpdate(old_animation_status, entitySprite, weaponSelected, weaponType, EnemyDirection, aimAngle);
	old_animation_status = entitySprite->GetStatus();
	entitySprite->Update(t);
}

//***** INFO MSG WHEN PLAYER GETS DAMAGE OR PICK UPS HEALTH BOX
void PlayerEntity::showInfoMsg(CGraphics* g)
{
	if (infoMsgTimer > localTime)
	{
		float floatingEffect = (infoMsgTimer - localTime) / 50;

		infoMsgBg.SetPosition(entitySprite->GetCenter().GetX(), entitySprite->GetTop() + 65 - floatingEffect);
		infoMsgBg.SetX(entitySprite->GetCenter().GetX());
		infoMsgBg.Draw(g);

		*g << font("AFontPTSerif.ttf") << font(24) << color(infoMsg > 0 ? CColor::Green() : CColor::Red()) << xy(entitySprite->GetCenter().GetX() - 18, entitySprite->GetTop() + 55 - floatingEffect) << infoMsg;
		*g << font("ARIAL.ttf");
	}
}


//ENEMY RECEIVING DAMAGE
void PlayerEntity::EtityGettingDamage(float damageAmount)
{
	if (entitySprite->GetStatus() == DEAD || CurrentEnemyHealth <= 0) return;

	//INFO MSG
	infoMsg = -damageAmount;
	infoMsgTimer = localTime + 2000;

	//DAMAGE APPLY
	CurrentEnemyHealth -= damageAmount;

	// IF Entity DIES 
	if (CurrentEnemyHealth <= 0)
	{
		preDeath.Play("preDeath.wav");
		CurrentEnemyHealth = 0;
		entitySprite->SetStatus(DEAD);
		DeathTimer = localTime + 2000;
		inDamageTimer = 0;
	}
	// OTHERWISE SET ONHIT EFFECT
	else
	{  
		hitSound.Play("hitSound.wav");
		inDamageTimer = 2000 + localTime;
		entitySprite->SetStatus(ONHIT);
		entitySprite->SetVelocity(0, 0);
	}
}


//**** INITIAL SPRITES DEPENDING ON TYPE OF ENEMY
void PlayerEntity::initAnimations()
{
	EntityAnimation::initAnimations(entitySprite);
	//aim
	aimPointer.LoadImage("mousePointer.png");
	aimPointer.SetImage("mousePointer.png");
	aimPointer.SetSize(30, 30);

	//showTurnArrow
	showTurnArrow.LoadImage("showTurnArrow.png");
	showTurnArrow.SetImage("showTurnArrow.png");
	showTurnArrow.SetSize(30, 30);

	//infoMsgBg
	infoMsgBg.LoadImage("infoMsgBg.png");
	infoMsgBg.SetImage("infoMsgBg.png");
	infoMsgBg.SetSize(60, 40);
}


//********** MOUSE & KEYBOARD HANDLER

//SELECT WEAPON WHEN INVENTORY IS OPEN
void PlayerEntity::OnLButtonDown(Uint16 x, Uint16 y)
{
	//Weapon Selection
	if (isInventoryOpen) 
	{
		weaponType = Inventory::selectWeapon(x, y);
		if (weaponType != -1)
		{
			isInventoryOpen = false;
			entitySprite->SetStatus(NONE); // animation reset
			weaponSelected = true;
			weaponSelectedSound.Play("weaponSelected.wav");
			weaponSelectedSound.Volume(75);
		}
	}
}


void PlayerEntity::OnKeyDown(SDLKey sym)
{
	if (!EntityWeapon::IsInAttackChargeMode && entitySprite->GetStatus() != INATTACK && TurnTransitStatusTimer == 0)
	{
		//### OPEN || HIDE INVENTORY
		if (sym == SDLK_TAB && isInventoryOpen)
		{
			isInventoryOpen = false;
			Inventory::CloseInventory();
		}
		else if (sym == SDLK_TAB)
		{
			//stop footsteps and reset animtaion
			footsteps.Stop();
			if (EnemyDirection == 90) entitySprite->SetStatus(STANDRIGHT);
			else entitySprite->SetStatus(STANDLEFT);
			entitySprite->SetSpeed(0);

			//open inventory
			isInventoryOpen = true;
			Inventory::OpenInventory();
			openInventorySound.Play("openInventory.wav");
			openInventorySound.Volume(200);
		}

		//### ATTACK
		if (weaponSelected && sym == SDLK_SPACE)
		{
			//stop footsteps and reset animtaion
			footsteps.Stop();
			entitySprite->SetSpeed(0);

			EntityWeapon::OnKeyDown(sym);
			if (weaponType != AXE)
			{
				if (EnemyDirection == 90) entitySprite->SetStatus(STANDRIGHT);
				else entitySprite->SetStatus(STANDLEFT);
			}
		}

		//### JUMP
		if (sym == SDLK_f && jumpTimer < localTime) // playerState != INAIR)
		{
			jumpTimer = 750 + localTime;
			//Y POS
			entitySprite->SetPosition(entitySprite->GetPosition() + CVector(0, 100)); // use set Y Velocity bettter???

			//X VELOCITY
			//could be better , but there is a bug with constantly changing INAIR STATE
			if (EnemyDirection == 90) entitySprite->SetXVelocity(150);
			else entitySprite->SetXVelocity(-150);


			entitySprite->SetStatus(JUMP);
			jumpSound.Play("jump.wav");
			jumpSound.Volume(0.5);
		}
	}
}

//RELEASE PROJECTILE
void PlayerEntity::OnKeyUp(SDLKey sym)
{
	if (weaponSelected && sym == SDLK_SPACE && TurnTransitStatusTimer == 0)
	{
			EntityWeapon::OnKeyUp(sym);
			TurnTransitStatusTimer = localTime + 4500;
			turnStartTimer = localTime + 4500;
			isInTurnTransitStatus = true;

			//RESET AMMO IN HAND -> becouse it was thrown
			if (weaponType == BOMB || weaponType == BANANA || weaponType == MINE || weaponType == DYNAMIT)
			{
				weaponSelected = false;
				weaponType != -1;
				entitySprite->SetStatus(NONE);
			}
	}
}


//SELECT X POSITION FOR MAIL ATTACK
void PlayerEntity::OnMouseMove(Uint16 x, Uint16 y)
{
	if(isInventoryOpen) aimPointer.SetPosition(x, y);

	if (weaponType == MAIL && !isInventoryOpen) EntityWeapon::OnMouseMove(x,y);  aimPointer.SetPosition(x, y);
}