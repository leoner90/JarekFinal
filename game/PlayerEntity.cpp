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
	//Enemy Sprite
	enemySprite = new CSprite();
	isPlayerTurn = false;


	if (typeofPlayer == 0) isFriend = false;
	if (typeofPlayer == 1) isFriend = false;
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
	enemySprite->SetStatus(STANDLEFT);
	enemySprite->SetPosition(initEnemyPos);
	enemySprite->SetAnimation("standright", 2);
	enemySprite->SetSpeed(0);
	EnemyDirection = 90;

	enemySprite->AddImage("main.png", "standright", 32, 10, 0, 10, 4, 10, CColor::Black());
	enemySprite->SetAnimation("standright", 2);
	//enemySprite->SetYVelocity(-60);

}

//**************************** UPDATE ****************************
void PlayerEntity::OnUpdate(Uint32 t, MapGen& mapGen, bool Dkey, bool Akey)
{

 
	if (Dkey && isPlayerTurn && playerState != INAIR) enemySprite->SetXVelocity(230);
	else if (Akey && isPlayerTurn && playerState != INAIR) enemySprite->SetXVelocity(-230);
	else enemySprite->SetSpeed(0);
 
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
	
	
	int old_animation_status = enemySprite->GetStatus();
	
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
}


//**************************** DRAW ****************************
void PlayerEntity::OnDraw(CGraphics* g)
{
 

	//Hit Effects
	for (CSprite* hitEffect : hitEffectList) hitEffect->Draw(g);

	//HealthBar
	if(CurrentEnemyHealth > 0) enemyHpBarRect2->Draw(g);
	
	//Enemy shots
	for (CSprite* shot : EnemyShotList) shot->Draw(g);

 
	
	enemySprite->Draw(g);
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
	for (auto brik : localMapVar->m_castle)
	{
		if (enemySprite->HitTest(brik))
		{

			CVector displ = (brik->GetPos() + CVector(0,30)) - enemySprite->GetPos();

			if ((brik->GetTop()+ 20 >= enemySprite->GetTop()))
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
		if (enemySprite->GetStatus() == WALKLEFT) { enemySprite->SetAnimation("walkleft", 10);  }
		if (enemySprite->GetStatus() == WALKRIGHT) { enemySprite->SetAnimation("walkright", 10);  }
		if (enemySprite->GetStatus() == STANDRIGHT) { enemySprite->SetAnimation("standright", 2);  }
		if (enemySprite->GetStatus() == STANDLEFT) { enemySprite->SetAnimation("standleft", 2);   }
	}
}

//***** Health Bar
void PlayerEntity::EnemyInterface()
{
	float baseHpBarWidth = 100;

	enemyHpBarRect2->SetX(enemySprite->GetX());
	enemyHpBarRect2->SetY(enemySprite->GetY() + enemySprite->GetSize().GetY()  - 10);

	float hpBarSize = baseHpBarWidth * (CurrentEnemyHealth / maxEnemyHealth);
	if (hpBarSize < 0) hpBarSize = 0;

	enemyHpBarRect2->SetSize(hpBarSize, 10);
	enemyHpBarRect2->Update(CurrentTime);

 
}

//***** ENEMY ATTACK
void PlayerEntity::EnemyAttack(Player& player)
{
	if (enemySprite->GetStatus() != INATTACK && inAttack == false)
	{
 
	 
	}
}

//**** Shots Handler for Update function
void PlayerEntity::shotsHandler()
{
	for (CSprite* shot : EnemyShotList)
	{
 
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
	if (enemyType == WARIOR)
	{
		//walk / iddle animation

		enemySprite->AddImage("main.png", "standright", 32, 10, 0, 10, 4, 10, CColor::Black());
		enemySprite->AddImage("main.png", "walkright", 32, 10, 0, 0, 9, 0, CColor::Black());
		enemySprite->AddImage("main.png", "standleft", 32, 10, 0, 0, 9, 0, CColor::Black());
		enemySprite->AddImage("main.png", "walkleft", 32, 10, 9, 0, 0, 0, CColor::Black());

		//in damage 
		enemySprite->AddImage("main.png", "inDamageRight", 32, 10, 0, 0, 1, 0, CColor::Black());
		enemySprite->AddImage("main.png", "inDamageLeft", 32, 10, 1, 0, 0, 0, CColor::Black());

	} 
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
		frameRatesToStop = 8;
		if (enemyType == DOG || enemyType == BOSS1) frameRatesToStop = 3;
		if (EnemyDirection == -90) enemySprite->SetAnimation("deadLeft", frameRatesToStop);
		else enemySprite->SetAnimation("deadRight", frameRatesToStop);
		deadSound.Play("EnemyDeath.wav");
		deadSound.Volume(0.3);
		EnemyInterface();

	 
	}
}

 