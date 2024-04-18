#include "stdafx.h"
#include "../headers/PlayerEntity/PlayerEntity.h"
#include "../headers/Map.h"
 
PlayerEntity::PlayerEntity()
{
	enemySprite = new CSprite();
}

PlayerEntity::~PlayerEntity()
{
	delete enemySprite;
	delete enemyHpBarRect2;
}

void PlayerEntity::init(int xPos, int yPos, int typeofPlayer)
{
	Inventory::init();
	//Enemy Sprite
	enemySprite = new CSprite();
	isPlayerTurn = false;
	isInventoryOpen = false;
	weaponSelected = false;
	IsInAttackChargeMode = false;
	maxChargeAttackTime = 2000;
	weaponType = -1;
	aimAngle = 0;
	chargeAttackTimer = 0;
 

	if (typeofPlayer == 0) isFriend = false;
	if (typeofPlayer == 1) isFriend = true;
	initEnemyPos = { float(xPos), float(yPos) };

	//reset shots and hit effects
	hitEffectList.delete_all();

	//reset Basic
	dead = DeathTimer = inDamage = inAttack = false;
	
	//Enemy Stats
 
	CurrentEnemyHealth = maxEnemyHealth = 100 ;
 
	//Animations and images
	initAnimations();

	//default Statuses
	enemySprite->SetStatus(STANDRIGHT);
	enemySprite->SetPosition(initEnemyPos);
	enemySprite->SetAnimation("idleRight", 1);
	enemySprite->SetSpeed(0);
	EnemyDirection = 90;

	EntityWeapon::init();
}

//**************************** UPDATE ****************************
void PlayerEntity::OnUpdate(Uint32 t, MapGen& mapGen, bool Dkey, bool Akey, bool Wkey, bool Skey, bool& isTurnFinished, std::vector<PlayerEntity*>& PlayerEntitys)
{
	finishTurn = &isTurnFinished;
	// local
	CurrentTime = t;
	localMapVar = &mapGen;
	localAllPlayersEntitys = PlayerEntitys;

	chargedAttackBar.Update(t);

	EntityWeapon::OnUpdate(CurrentTime, localAllPlayersEntitys, enemySprite, weaponType, mapGen);

	if (chargeAttackTimer != 0 && chargeAttackTimer - CurrentTime <= 0) 
		EntityWeapon::attackHandler(IsInAttackChargeMode, chargeAttackTimer,  maxChargeAttackTime,  chargedAttackBar, EnemyDirection,  aimAngle, finishTurn);


	// ** Animation  Reset  after attack
	if (enemySprite->GetStatus() == INATTACK && enemySprite->GetCurrentAnimationFrame() > 2)
	{
		enemySprite->SetStatus(NONE);
		EntityWeapon::isInAttackDelay = false;
	}

	EntityAnimation::EnemyAnimation(old_animation_status, enemySprite, weaponSelected, weaponType, EnemyDirection);


	old_animation_status = enemySprite->GetStatus();

	// ** ON Enemy Dead
	deathHandler();
	// ** Enemy Controller
	EntityPhysic::playerControler(playerState, enemySprite, isPlayerTurn, IsInAttackChargeMode, isInventoryOpen, Dkey, Akey, Wkey, Skey, EnemyDirection,  aimAngle);
	// ** COllisions
	EntityPhysic::playerCollision(playerState, enemySprite,pos, &mapGen);

	// ** Interface
	EnemyInterface();

	//Hit vfx
	for (CSprite* hitEffect : hitEffectList) hitEffect->Update(t);
	hitEffectList.delete_if(deleted);

	//Enemy position
	pos = enemySprite->GetPos();
	enemySprite->Update(t);

	if (weaponSelected && !isInventoryOpen && weaponType != AXE)
	{
		CVector side;
		if (EnemyDirection == 90)	side = { 60, aimAngle };
		else  side = { -60, aimAngle };
		aimPointer.SetPos(enemySprite->GetPos() + side);
	}
}


//**************************** DRAW ****************************
void PlayerEntity::OnDraw(CGraphics* g)
{
	//Hit Effects
	for (CSprite* hitEffect : hitEffectList) hitEffect->Draw(g);

	//HealthBar
	if(CurrentEnemyHealth > 0 && isFriend) enemyHpBarRect->Draw(g);
	else if (CurrentEnemyHealth > 0 && !isFriend)  enemyHpBarRect2->Draw(g);
	

	EntityWeapon::OnDraw(g);

	enemySprite->Draw(g);

	if (isPlayerTurn)Inventory::OnDraw(g);
	if (isPlayerTurn && (weaponSelected || isInventoryOpen)) aimPointer.Draw(g);

	if (IsInAttackChargeMode)
	{
		CVector side;
		if (EnemyDirection == 90)
		{
			chargedAttackBar.SetSize(chargedAttackBar.GetSize() + CVector(1, 0));
			chargedAttackBar.SetPosition(chargedAttackBar.GetPos() + CVector(0.4, 0));
		}
		else 
		{
			chargedAttackBar.SetSize(chargedAttackBar.GetSize() + CVector(1, 0));
			chargedAttackBar.SetPosition(chargedAttackBar.GetPos() - CVector(0.4, 0));
		}
		chargedAttackBar.Draw(g);
	}
}
 
//***** Health Bar
void PlayerEntity::EnemyInterface()
{
	float baseHpBarWidth = 50;

	enemyHpBarRect2->SetX(enemySprite->GetX());
	enemyHpBarRect2->SetY(enemySprite->GetY() + enemySprite->GetSize().GetY()  - 10);

	enemyHpBarRect->SetX(enemySprite->GetX());
	enemyHpBarRect->SetY(enemySprite->GetY() + enemySprite->GetSize().GetY() - 10);

	float hpBarSize = baseHpBarWidth * (CurrentEnemyHealth / maxEnemyHealth);
	if (hpBarSize < 0) hpBarSize = 0;

	enemyHpBarRect2->SetSize(hpBarSize, 15);
	enemyHpBarRect2->Update(CurrentTime);

	enemyHpBarRect->SetSize(hpBarSize, 15);
	enemyHpBarRect->Update(CurrentTime);
}


//**** INITIAL SPRITES DEPENDING ON TYPE OF ENEMY
void PlayerEntity::initAnimations()
{
	EntityAnimation::initAnimations(enemySprite);
	//aim
	aimPointer.LoadImage("mousePointer.png");
	aimPointer.SetImage("mousePointer.png");
	aimPointer.SetSize(30, 30);

	//chargedAttackBar
	chargedAttackBar.LoadImage("chargedAttackBar.png");
	chargedAttackBar.SetImage("chargedAttackBar.png");
	chargedAttackBar.SetSize( 10, 30);


}

//Death Handler
void PlayerEntity::deathHandler()
{
	/*
	if (CurrentEnemyHealth <= 0) {
		if (enemySprite->GetCurrentAnimationFrame() == frameRatesToStop && sizeof(EnemyShotList) / sizeof(CSprite) == 0)
		{
			enemyHpBarRect2->Delete();
			enemySprite->Delete();
			DeathTimer = true;
		}
		EntityWeapon::shotsHandler(EnemyShotList, *localMapVar, explosionList, CurrentTime);
		EnemyShotList.delete_if(deleted);
		enemySprite->Update(CurrentTime);
		return;
	}
	*/
}

//ENEMY RECEIVING DAMAGE
void PlayerEntity::EnemyGettingDamage(float damageAmount)
{
	//Hit Effect
	CSprite* hitEffect = new CSprite(enemySprite->GetX(), enemySprite->GetY(), 0, 0, CurrentTime);
	hitEffect->AddImage("vfxHit.png", "hitEffect", 4, 3, 0, 0, 3, 2, CColor::Black());
	hitEffect->SetAnimation("hitEffect", 8);
	hitEffect->Die(300);
	hitEffectList.push_back(hitEffect);

	// RECEIVING DAMAGE
	CurrentEnemyHealth -= damageAmount;
	inDamage = true;
	enemySprite->SetStatus(inDamage);
	enemySprite->SetVelocity(0, 0);

	//sets proper animation depending on rotation
	if (EnemyDirection == 90) enemySprite->SetAnimation("inDamageRight", 4);
	else enemySprite->SetAnimation("inDamageLeft", 4);
	frameRatesToStop = 1;

	// IF ENEMY DIES , Sets proper Loot and Animations
	if (CurrentEnemyHealth <= 0 && enemySprite->GetStatus() != dead)
	{
		dead = true;
		frameRatesToStop = 2;
		enemySprite->SetAnimation("dead", frameRatesToStop);
		deadSound.Play("EnemyDeath.wav");
		deadSound.Volume(0.3);
		EnemyInterface();	
	}
}

void PlayerEntity::OnLButtonDown(Uint16 x, Uint16 y)
{
	//Weapon Selection
	if (isInventoryOpen) 
	{
		weaponType = Inventory::selectWeapon(x, y);
		if (weaponType != -1)
		{
			isInventoryOpen = false;
			enemySprite->SetStatus(NONE); // animation reset
			weaponSelected = true;
		}
	}


	else if (weaponType == MAIL)
	{
		EntityWeapon::mailAttack(x,y);
	}
}

void PlayerEntity::OnKeyDown(SDLKey sym)
{
	if (!IsInAttackChargeMode && enemySprite->GetStatus() != INAIR)
	{
		//SHOW HIDE INVENTORY
		if (sym == SDLK_TAB && isInventoryOpen)
		{
			isInventoryOpen = false;
			Inventory::CloseInventory();
		}
		else if (sym == SDLK_TAB)
		{
			isInventoryOpen = true;
			Inventory::OpenShop();
		}

		//START ATTACK CHARGING
		if (weaponSelected && sym == SDLK_SPACE)
		{
			enemySprite->SetStatus(INATTACK);
			if (weaponType != AXE)
			{
				enemySprite->SetSpeed(0);
				if (EnemyDirection == 90) chargedAttackBar.SetPosition(enemySprite->GetPos() + CVector(18, 18));
				else chargedAttackBar.SetPosition(enemySprite->GetPos() + CVector(-15, 18));

				IsInAttackChargeMode = true;
				chargeAttackTimer = CurrentTime + maxChargeAttackTime;
			}
		
		}
	}
}

void PlayerEntity::OnKeyUp(SDLKey sym)
{
	if (weaponType != AXE)
	{
		if (weaponSelected && sym == SDLK_SPACE && IsInAttackChargeMode)
		{
			EntityWeapon::attackHandler(IsInAttackChargeMode, chargeAttackTimer, maxChargeAttackTime, chargedAttackBar, EnemyDirection, aimAngle, finishTurn);

		}
	}
}


void PlayerEntity::OnMouseMove(Uint16 x, Uint16 y)
{
	if(isInventoryOpen) aimPointer.SetPosition(x, y);
	if (weaponType == MAIL) aimPointer.SetPosition(x, y);
}