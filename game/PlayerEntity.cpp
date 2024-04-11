#include "stdafx.h"
#include "headers/PlayerEntity.h"
#include "headers/Map.h"
 
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
	ProjectileSpeed = 0;
	chargeAttackTimer = 0;
 

	if (typeofPlayer == 0) isFriend = false;
	if (typeofPlayer == 1) isFriend = true;
	initEnemyPos = { float(xPos), float(yPos) };

	//reset shots and hit effects
	EnemyShotList.delete_all();
	hitEffectList.delete_all();

	//reset Basic
	dead = DeathTimer = inDamage = inAttack = false;
	
	//Enemy attack distance and speed should be before initAnimations();
	attackDistance = 50;
	enemySpeed = 100;

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
 
 

}

//**************************** UPDATE ****************************
void PlayerEntity::OnUpdate(Uint32 t, MapGen& mapGen, bool Dkey, bool Akey, bool Wkey, bool Skey, bool& isTurnFinished)
{
	finishTurn = &isTurnFinished;

	int old_animation_status = enemySprite->GetStatus();

	if (isPlayerTurn && !IsInAttackChargeMode && !isInventoryOpen && playerState != INAIR)
	{
		if (Dkey )
		{
			enemySprite->SetXVelocity(130);
			enemySprite->SetStatus(WALKRIGHT);
			EnemyDirection = 90;
		}
		else if (Akey)
		{
			enemySprite->SetXVelocity(-130);
			enemySprite->SetSize(250, 200);
			enemySprite->SetStatus(WALKLEFT);
			EnemyDirection = -90;
		}
		else if (Wkey)
		{
			if(aimAngle < 45) aimAngle++;
		}

		else if (Skey)
		{
			if (aimAngle > -45) aimAngle--;
			
		}
		else
		{
			setDefaultStandAnimation();
		}
	}
	// local var usage
	//castToPlayer = &player;
	CurrentTime = t;
	localMapVar = &mapGen;

 
	//*** ON Enemy Dead
	if (CurrentEnemyHealth <= 0) {
		if (enemySprite->GetCurrentAnimationFrame() == frameRatesToStop && sizeof(EnemyShotList) / sizeof(CSprite) == 0)
		{
			enemyHpBarRect2->Delete();
			enemySprite->Delete();
			DeathTimer = true;
		}
		shotsHandler();
		EnemyShotList.delete_if(deleted);
		enemySprite->Update(t);
		return;
	}
	
	
	chargedAttackBar.Update(t);
	if (chargeAttackTimer!= 0 && chargeAttackTimer - CurrentTime <= 0) performShot();

	// ** Enemy Controller
	playerControler();

	// ** COllisions
	playerCollision();

	// ** Animation Handler
	EnemyAnimation(old_animation_status);

	// ** Interface
	EnemyInterface();

	//Hit vfx
	for (CSprite* hitEffect : hitEffectList) hitEffect->Update(t);
	hitEffectList.delete_if(deleted);

	//Enemy Shots
	shotsHandler();
	EnemyShotList.delete_if(deleted);

	//Enemy position
	pos = enemySprite->GetPos();
	enemySprite->Update(t);

	if (weaponSelected && !isInventoryOpen)
	{
		CVector side;
		if (EnemyDirection == 90)	side = { 50, aimAngle };
		else  side = { -50, aimAngle };
		aimPointer.SetPos(enemySprite->GetPos() + side);
	}

	for (CSprite* explosion : explosionList) explosion->Update(t);
	explosionList.delete_if(deleted);
}


//**************************** DRAW ****************************
void PlayerEntity::OnDraw(CGraphics* g)
{
 


	//Hit Effects
	for (CSprite* hitEffect : hitEffectList) hitEffect->Draw(g);




	//HealthBar
	if(CurrentEnemyHealth > 0 && isFriend) enemyHpBarRect->Draw(g);
	else if (CurrentEnemyHealth > 0 && !isFriend)  enemyHpBarRect2->Draw(g);
	
	//Enemy shots
	for (CSprite* shot : EnemyShotList) shot->Draw(g);


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

	for (CSprite* explosion : explosionList) explosion->Draw(g);
	
}
 

//**************************** CONTROLLER ****************************
void PlayerEntity::playerControler()
{
	if(playerState == INAIR) enemySprite->SetVelocity(enemySprite->GetXVelocity(),  - 350);
 
}

//**************************** COLLISION ****************************
void PlayerEntity::playerCollision()
{	
	bool touchingPlatform = false;
	for (auto brik : localMapVar->mapList)
	{
		if (enemySprite->HitTest(brik))
		{

			CVector displ = (brik->GetPos() + CVector(0,30)) - enemySprite->GetPos();

			if ((brik->GetTop() + 20 >= enemySprite->GetTop()))
			{
				//Too High
				//enemySprite->SetPosition(pos + CVector(0, 25));
				//if (enemySprite->HitTest(brik))
				//	enemySprite->SetPosition(pos);
				//else 
				enemySprite->SetPosition(pos + CVector(0, 5));
			}

			playerState = STANDRIGHT;
			touchingPlatform = true;
		}
	}

	if (!touchingPlatform) playerState = INAIR;
}



//************ ANIMATION -> Restore Basic Animation
void PlayerEntity::EnemyAnimation(int old_animation_status)
{
	if (enemySprite->GetStatus() != old_animation_status)
	{
		if (enemySprite->GetStatus() == WALKLEFT) { enemySprite->SetAnimation("walkLeft", 2);  }
		if (enemySprite->GetStatus() == WALKRIGHT) { enemySprite->SetAnimation("walkRight", 2);  }

		if (weaponSelected)
		{
			switch (weaponType)
			{
			case AXE:
				if (enemySprite->GetStatus() == STANDRIGHT) { enemySprite->SetAnimation("MeleeAxeRightStand", 1); }
				if (enemySprite->GetStatus() == STANDLEFT) { enemySprite->SetAnimation("MeleeAxeLeftStand", 1); }
				break;
			case MISSILE:
				if (enemySprite->GetStatus() == STANDRIGHT) { enemySprite->SetAnimation("missileRight", 1); }
				if (enemySprite->GetStatus() == STANDLEFT) { enemySprite->SetAnimation("missileLeft", 1); }
				break;
			case BOMB:
				if (enemySprite->GetStatus() == STANDRIGHT) { enemySprite->SetAnimation("bombRightStand", 1); }
				if (enemySprite->GetStatus() == STANDLEFT) { enemySprite->SetAnimation("bombLeftStand", 1); }
				break;
			case BANANA:
				if (enemySprite->GetStatus() == STANDRIGHT) { enemySprite->SetAnimation("bananaRightStand", 1); }
				if (enemySprite->GetStatus() == STANDLEFT) { enemySprite->SetAnimation("bananaLeftStand", 1); }
				break;
			case DYNAMIT:
				if (enemySprite->GetStatus() == STANDRIGHT) { enemySprite->SetAnimation("TNTRight", 1); }
				if (enemySprite->GetStatus() == STANDLEFT) { enemySprite->SetAnimation("TNTLeft", 1); }
				break;
			case MINE:
				if (enemySprite->GetStatus() == STANDRIGHT) { enemySprite->SetAnimation("MineRight", 1); }
				if (enemySprite->GetStatus() == STANDLEFT) { enemySprite->SetAnimation("MineLeft", 1); }
				break;
			case UZI:
				if (enemySprite->GetStatus() == STANDRIGHT) { enemySprite->SetAnimation("uziRight", 1); }
				if (enemySprite->GetStatus() == STANDLEFT) { enemySprite->SetAnimation("uziLeft", 1); }
				break;
			case BOW:
				if (enemySprite->GetStatus() == STANDRIGHT) { enemySprite->SetAnimation("BowRight", 1); }
				if (enemySprite->GetStatus() == STANDLEFT) { enemySprite->SetAnimation("BowLeft", 1); }
				break;
			case MAIL:
				if (enemySprite->GetStatus() == STANDRIGHT) { enemySprite->SetAnimation("MailBombRight", 1); }
				if (enemySprite->GetStatus() == STANDLEFT) { enemySprite->SetAnimation("MailBombLeft", 1); }
				break;
			case SKIP:
				if (enemySprite->GetStatus() == STANDRIGHT) { enemySprite->SetAnimation("SkipTurnRight", 1); }
				if (enemySprite->GetStatus() == STANDLEFT) { enemySprite->SetAnimation("SkipTurnLeft", 1); }
				break;
			default:
				break;
			}
		}

		else 
		{
			if (enemySprite->GetStatus() == STANDRIGHT) { enemySprite->SetAnimation("idleRight", 1); }
			if (enemySprite->GetStatus() == STANDLEFT) { enemySprite->SetAnimation("idleLeft", 1); }
		}
		
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

 

//**** Shots Handler for Update function
void PlayerEntity::shotsHandler()
{
	CVector gravity = { 0,-10 };
	
	for (CSprite* shot : EnemyShotList)
	{
		shot->Accelerate(gravity);
		if (shot->GetYVelocity() < 0) 	shot->Rotate(3);
	;
		for (CSprite* mapPart : localMapVar->mapList)
		{
			if (shot->HitTest(mapPart))
			{

				mapPart->Delete();
				shot->Delete();

				CSprite* explosion = new CSprite(mapPart->GetX(), mapPart->GetY() + 20, 0, 0, CurrentTime);
				explosion->AddImage("explosion.bmp", "explosion",5,5,0,0,4,4, CColor::Black());
				explosion->SetAnimation("explosion", 25);
				explosion->Die(1000);
				explosionList.push_back(explosion);
			}
		}
		shot->Update(CurrentTime);
	}
}

//*** RESTORE DEFAULT ANIMATION BASE ON DIRECTION
void PlayerEntity::setDefaultStandAnimation()
{
	if (EnemyDirection == 90) 	enemySprite->SetStatus(STANDRIGHT);
	if (EnemyDirection == -90) enemySprite->SetStatus(STANDLEFT);
	enemySprite->SetVelocity(0, 0);
}




 

//**** INITIAL SPRITES DEPENDING ON TYPE OF ENEMY
void PlayerEntity::initAnimations()
{
 
		//iddle
		enemySprite->AddImage("main.png", "idleLeft", 28, 4, 7, 3, 10, 3, CColor::Black());
		enemySprite->AddImage("main.png", "idleRight", 28, 4, 11, 3, 14, 3, CColor::Black());

		//walk
		enemySprite->AddImage("main.png", "walkLeft", 28, 4, 0, 3, 3, 3, CColor::Black());
		enemySprite->AddImage("main.png", "walkRight", 28, 4, 18, 3, 21, 3, CColor::Black());

		//jump
		enemySprite->AddImage("main.png", "jumpLeft", 28, 4, 4, 3, 4, 3, CColor::Black());
		enemySprite->AddImage("main.png", "jumpRight", 28, 4, 17, 3, 17, 3, CColor::Black());

		//fall
		enemySprite->AddImage("main.png", "fallLeft", 28, 4, 5, 3, 7, 3, CColor::Black());
		enemySprite->AddImage("main.png", "fallRight", 28, 4, 15, 3, 16, 3, CColor::Black());
		
		//victory
		enemySprite->AddImage("main.png", "victory", 28, 4, 22, 3, 23, 3, CColor::Black());


		//**ATTACK ANIMATIONS
		
		//missile
		enemySprite->AddImage("main.png", "missileLeft", 28, 4, 1, 2, 1, 2, CColor::Black());
		enemySprite->AddImage("main.png", "missileRight", 28, 4, 26, 1, 26, 1, CColor::Black());

		//banana
		enemySprite->AddImage("main.png", "bananaLeftStand", 28, 4, 8, 2, 8, 2, CColor::Black());
		enemySprite->AddImage("main.png", "bananaLeft", 28, 4, 8, 2, 11, 2, CColor::Black());
		enemySprite->AddImage("main.png", "bananaRightStand", 28, 4, 19, 1, 19, 1, CColor::Black());
		enemySprite->AddImage("main.png", "bananaRight", 28, 4, 19, 1, 16, 1, CColor::Black());

		//bomb
		enemySprite->AddImage("main.png", "bombLeftStand", 28, 4, 12, 2, 12, 2, CColor::Black());
		enemySprite->AddImage("main.png", "bombLeft", 28, 4, 12, 2, 16, 2, CColor::Black());
		enemySprite->AddImage("main.png", "bombRightStand", 28, 4, 15, 1, 15, 1, CColor::Black());
		enemySprite->AddImage("main.png", "bombRight", 28, 4, 15, 1, 11, 1, CColor::Black());

		//uzi
		enemySprite->AddImage("main.png", "uziLeft", 28, 4, 19, 2, 19, 2, CColor::Black());
		enemySprite->AddImage("main.png", "uziRight", 28, 4, 8, 1, 8, 1, CColor::Black());

		//Bow
		enemySprite->AddImage("main.png", "BowLeft", 28, 4, 22, 2, 22, 2, CColor::Black());
		enemySprite->AddImage("main.png", "BowRight", 28, 4, 5, 1, 5, 1, CColor::Black());

		//Mail Bomb
		enemySprite->AddImage("main.png", "MailBombLeft", 28, 4, 26, 2, 27, 2, CColor::Black());
		enemySprite->AddImage("main.png", "MailBombRight", 28, 4, 1, 1, 0, 1, CColor::Black());
		
		//Mine
		enemySprite->AddImage("main.png", "MineLeft", 28, 4, 5, 0, 5, 0, CColor::Black());
		enemySprite->AddImage("main.png", "MineRight", 28, 4, 6, 0, 6, 0, CColor::Black());

		//TNT
		enemySprite->AddImage("main.png", "TNTLeft", 28, 4, 4, 0, 4, 0, CColor::Black());
		enemySprite->AddImage("main.png", "TNTRight", 28, 4, 7, 0, 7, 0, CColor::Black());

		//Melee Axe
		enemySprite->AddImage("melleeAttack.png", "MeleeAxeLeftStand", 4, 2, 0, 1, 0, 1, CColor::Black());
		enemySprite->AddImage("melleeAttack.png", "MeleeAxeLeft", 4, 2, 3, 0, 0, 0, CColor::Black());
		enemySprite->AddImage("melleeAttack.png", "MeleeAxeRightStand", 4, 2, 3, 0, 3, 0, CColor::Black());
		enemySprite->AddImage("melleeAttack.png", "MeleeAxeRight", 4, 2, 3, 0, 0, 0, CColor::Black());


		//in Damage
		enemySprite->AddImage("main.png", "inDamage", 28, 6, 18, 6, 21, 6, CColor::Black());

		//Dead
		enemySprite->AddImage("main.png", "Dead", 28, 4, 18, 6, 21, 6, CColor::Black());

		//Skip turn
		enemySprite->AddImage("main.png", "SkipTurnLeft", 28, 4, 3, 0, 3, 0, CColor::Black());
		enemySprite->AddImage("main.png", "SkipTurnRight", 28, 4, 8, 6, 8, 0, CColor::Black());

		//AMMO
		//Mine
		enemySprite->AddImage("ammo.png", "MineImg", 6, 1, 18, 6, 21, 6, CColor::Black());
		//TNT
		enemySprite->AddImage("ammo.png", "TntImg", 6, 1, 18, 6, 21, 6, CColor::Black());
		//missile
		enemySprite->AddImage("ammo.png", "missileImg", 6, 1, 1, 0, 1, 0, CColor::Black());


		//aim
		aimPointer.LoadImage("mousePointer.png");
		aimPointer.SetImage("mousePointer.png");
		aimPointer.SetSize(30, 30);

		//chargedAttackBar
		chargedAttackBar.LoadImage("chargedAttackBar.png");
		chargedAttackBar.SetImage("chargedAttackBar.png");
		chargedAttackBar.SetSize( 10, 30);

		explosionSprite.LoadImage("firebolt2.png");
		explosionSprite.SetImage("firebolt2.png");
		explosionSprite.SetSize(30, 30);


}


//ENEMY RECEIVING DAMAGE
void PlayerEntity::EnemyGettingDamage(float damageAmount, float t, CSpriteList& DroppedPoutions)
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
}




void PlayerEntity::OnKeyDown(SDLKey sym)
{
	if (!IsInAttackChargeMode && enemySprite->GetStatus() != INAIR)
	{
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

		if (weaponSelected && sym == SDLK_SPACE)
		{
			enemySprite->SetSpeed(0);
			if (EnemyDirection == 90) chargedAttackBar.SetPosition(enemySprite->GetPos() + CVector(18, 18));
			else chargedAttackBar.SetPosition(enemySprite->GetPos() + CVector(-15, 18));
	 
			IsInAttackChargeMode = true;
			chargeAttackTimer = CurrentTime + maxChargeAttackTime;
		}
	}

}

void PlayerEntity::OnKeyUp(SDLKey sym)
{
	if (weaponSelected && sym == SDLK_SPACE && IsInAttackChargeMode)
	{
		performShot();
	}

}


void PlayerEntity::OnMouseMove(Uint16 x, Uint16 y)
{
	if(isInventoryOpen) aimPointer.SetPosition(x, y);
}

void PlayerEntity::performShot()
{

	//calculate projectile speed

	
	float fullyChargedAttack = 0;



	CSprite* newProjectile = new CSprite(enemySprite->GetX(), enemySprite->GetY() + 20, 0, 0, CurrentTime);
	switch (weaponType)
	{
	case AXE:
		fullyChargedAttack = 300;
		newProjectile->AddImage("ammo.png", "DYNAMIT", 8, 1, 0, 0, 0, 0, CColor::Black());
		newProjectile->SetAnimation("DYNAMIT", 1);
		break;

	case MISSILE:
		fullyChargedAttack = 1000;
		newProjectile->AddImage("ammo.png", "missileImg", 8, 1, 1, 0, 1, 0, CColor::Black());
		newProjectile->SetAnimation("missileImg", 1);
		break;
	case BOMB:
		fullyChargedAttack = 1000;
		newProjectile->AddImage("ammo.png", "BOMB", 8, 1, 4, 0, 4, 0, CColor::Black());
		newProjectile->SetAnimation("BOMB", 1);
		newProjectile->SetOmega(250);
		break;
	case BANANA:
		fullyChargedAttack = 1000;
		newProjectile->AddImage("ammo.png", "BANANA", 8, 1, 3, 0, 3, 0);
		newProjectile->SetAnimation("BANANA", 1);
		newProjectile->SetOmega(250);
		break;

	case DYNAMIT:
		fullyChargedAttack = 300;
		newProjectile->AddImage("ammo.png", "DYNAMIT", 8, 1, 0, 0, 0, 0, CColor::Black());
		newProjectile->SetAnimation("DYNAMIT", 1);
		break;

	case MINE:
		fullyChargedAttack = 300;
		newProjectile->AddImage("ammo.png", "MINE", 8, 1, 2, 0, 2, 0, CColor::Black());
		newProjectile->SetAnimation("MINE", 1);
		break;
	case UZI:
		fullyChargedAttack = 1000;
		newProjectile->AddImage("ammo.png", "UZI", 8, 1, 7, 0, 7, 0, CColor::Black());
		newProjectile->SetAnimation("UZI", 1);
		break;
	case BOW:
		fullyChargedAttack = 1000;
		newProjectile->AddImage("ammo.png", "BOW", 8, 1, 6, 0, 6, 0, CColor::Black());
		newProjectile->SetAnimation("BOW", 1);
		break;
	case MAIL:
		fullyChargedAttack = 300;
		newProjectile->AddImage("ammo.png", "MAIL", 8, 1, 5, 0, 5, 0, CColor::Black());
		newProjectile->SetAnimation("MAIL", 1);
		break;

	case SKIP:
		fullyChargedAttack = 300;
		newProjectile->AddImage("ammo.png", "MINE", 6, 1, 4, 0, 4, 0, CColor::Black());
		newProjectile->SetAnimation("MINE", 1);
		break;

	default:
		break;
	}


	//calculate projectile speed
	IsInAttackChargeMode = false;
	float timeDiference = chargeAttackTimer - CurrentTime;
	if (timeDiference <= 0) ProjectileSpeed = fullyChargedAttack;
	else
	{
		float timePast = maxChargeAttackTime - timeDiference;
		ProjectileSpeed = fullyChargedAttack / maxChargeAttackTime * timePast;
	}
	//reset
	chargedAttackBar.SetSize(10, 30);
	chargeAttackTimer = 0;



	if (enemySprite->GetStatus() == STANDLEFT)
	{
		newProjectile->SetDirection(EnemyDirection + aimAngle);
		newProjectile->SetRotation(EnemyDirection + aimAngle);
	}
	else
	{
		newProjectile->SetDirection(EnemyDirection - aimAngle);
		newProjectile->SetRotation(EnemyDirection - aimAngle);
	}

	newProjectile->SetSpeed(ProjectileSpeed);
	newProjectile->Die(1000);
	EnemyShotList.push_back(newProjectile);
	*finishTurn = true;
}
