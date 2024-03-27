#include "stdafx.h"
#include "headers/Enemy.h"
#include "headers/Player.h"
#include "headers/mapGen.h"
 
Enemy::Enemy() 
{
	enemySprite = new CSprite();
	BottomHitTestEmnemy = new CSprite();
}

Enemy::~Enemy()
{
	delete enemySprite;
	delete enemyHpBarRect2;
}

void Enemy::init(int xPos, int yPos, int typeofEnemy,int conInit, int strInit, int dexInit, int intellectInit)
{
	//Enemy Sprite
	enemySprite = new CSprite();

	//Local Var. miroring
	enemyType = AllEnemies(typeofEnemy);
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
	con = conInit;
	str = strInit;
	dex = dexInit;
	intellect = intellectInit;
	CurrentEnemyHealth = maxEnemyHealth = 100 * con;
	CurrentEnemyMp = maxEnemyMp = 10 * intellect;
	CurrentEnemyEnergy = maxEnemyEnergy = 10.0f * dex;

	//Damage
	meleeDamage = str * 10;
	kunaiDamage = dex * 20;
	fireboltDamage = intellect * 15;

	//Animations and images
	initAnimations();

	
	//default Statuses
	enemySprite->SetStatus(STANDLEFT);
	enemySprite->SetPosition(initEnemyPos);
	enemySprite->SetAnimation("standright", 2);
	enemySprite->SetSpeed(0);
	EnemyDirection = 90;
}

//**************************** UPDATE ****************************
void Enemy::OnUpdate(Uint32 t, Player& player, MapGen& mapGen)
{
	// local var usage
	castToPlayer = &player;
	CurrentTime = t;
	localMapVar = &mapGen;

	//Return if player is to far Away
	if (Distance(enemySprite->GetPosition(), castToPlayer->playerSprite->GetPosition()) > 1600) return;
	
	
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
	EnemyController();

	// ** COllisions
	EnemyCollision(mapGen);

	// ** Chasing & Patroling
	EnemyChasing(player);

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
void Enemy::OnDraw(CGraphics* g)
{
	//Do not Draw - if too far away from Player
	if (Distance(enemySprite->GetPosition(), castToPlayer->playerSprite->GetPosition()) > 1600) return;

	//Hit Effects
	for (CSprite* hitEffect : hitEffectList) hitEffect->Draw(g);

	//If not in the Pit
	if(enemySprite->GetY() > 0) enemySprite->Draw(g);

	//HealthBar
	if(CurrentEnemyHealth > 0) enemyHpBarRect2->Draw(g);
	
	//Enemy shots
	for (CSprite* shot : EnemyShotList) shot->Draw(g);
}


//**************************** CHASING ****************************
void Enemy::EnemyChasing(Player& player)
{
	//if in damage do nothing -> staged
	if (inDamage) return;

	//** IF Distance < 450 CHASE PLAYER
	distToEnemy = Distance(enemySprite->GetPosition(), player.playerSprite->GetPosition());
	CVector DisplVector = player.playerSprite->GetPosition() - enemySprite->GetPosition();
	float direction = Dot(player.playerSprite->GetPosition(), DisplVector);
	if (distToEnemy < 450 && distToEnemy > attackDistance && enemyType != NINJAGIRL && enemyType != NINJAGIRLKUNAI && player.CurrentPlayerHealth > 0)
	{
		if (direction <= 0)
		{
			action = CHASSING;
			enemySprite->SetVelocity(-enemySpeed, 0);
			enemySprite->SetStatus(WALKLEFT);
			EnemyDirection = -90;
		}
		else
		{
			action = CHASSING;
			EnemyDirection = 90;
			enemySprite->SetVelocity(enemySpeed, 0);
			enemySprite->SetStatus(WALKRIGHT);
		}
	}
	//** IF Distance < attackDistance ->  Attack!
	else if (distToEnemy <= attackDistance && direction >= 0     && player.CurrentPlayerHealth > 0)
	{
		EnemyDirection = 90;
		action = CHASSING;
		EnemyAttack(player);
	}
	else if (distToEnemy <= attackDistance &&  direction < 0    && player.CurrentPlayerHealth > 0)
	{
		EnemyDirection = -90;
		action = CHASSING;
		EnemyAttack( player);
	}
 
	else {
		//if to far Away RETURN TO PATROL POSITION First And set action to Patrol - to start patroling
		//action == CHASSING to return to the possition only if in case the was a chasing before
		if (enemySprite->GetX() <= initEnemyPos.GetX() - 20 && action == CHASSING)
		{
			EnemyDirection = 90;
			enemySprite->SetStatus(WALKRIGHT);
			enemySprite->SetVelocity(200, 0);
		}
		else if (enemySprite->GetX() >= initEnemyPos.GetX()  && action == CHASSING)
		{
			EnemyDirection = -90;
			enemySprite->SetStatus(WALKLEFT);
			enemySprite->SetVelocity(-200, 0);
		}
		else {
			//START TO PATROL
			action = PATROL;
		}
	}
}

//**************************** CONTROLLER ****************************
void Enemy::EnemyController()
{
	//If enought frames , removes in damage state
	if (enemySprite->GetCurrentAnimationFrame() == frameRatesToStop && inDamage == true) {
		setDefaultStandAnimation();
		inDamage = false;
	}

	//If in Damage Return
	if (inDamage) return;

	//if enought frames passed return ability to attack 
	if (enemySprite->GetCurrentAnimationFrame() == frameRatesToStop && inAttack == true) {
		setDefaultStandAnimation();
		inAttack = false;
		hitTestDelay = false;
	}

	//***** Attack Player 
	if (inAttack && !hitTestDelay) {
		if (enemySprite->HitTest(castToPlayer->playerSprite)) {
			if (castToPlayer->playerSprite->GetStatus() != castToPlayer->SLIDE) {
				castToPlayer->PlayerGettingDamage(meleeDamage);
				hitTestDelay = true;
			}
		}
	}

	//***** PATROLING 
	if(action == PATROL)
	{ 
		//move Left and right and stops for a while
		angle += 0.01;
		if (cos(angle) > 0) 
		{
			EnemyDirection = -90;
			if (cos(angle) >= 0.5)
			{
				enemySprite->SetVelocity(0, 0);
				enemySprite->SetStatus(STANDLEFT);
			}
			else {
				enemySprite->SetVelocity(-100, 0);
				enemySprite->SetStatus(WALKLEFT);
			}
		}
		else
		{
			EnemyDirection = 90;
			if (cos(angle) <= -0.5)
			{
				enemySprite->SetStatus(STANDRIGHT);
				enemySprite->SetVelocity(0, 0);
			}
			else {
				enemySprite->SetVelocity(100, 0);
				enemySprite->SetStatus(WALKRIGHT);
			}
		}
	}
}

//**************************** COLLISION ****************************
void Enemy::EnemyCollision(MapGen& mapGen)
{
	//if inDamage => return
	if (inDamage) return;

	// PIT => If enemies will drop into pit , they will Die
	if (enemySprite->GetY() <= 0) EnemyGettingDamage(maxEnemyHealth,CurrentTime, castToPlayer->DroppedPoutions);

	//to avoid moving a player
	if (enemySprite->HitTest(castToPlayer->playerSprite) && enemySprite->GetXVelocity() > 0) enemySprite->SetPosition(pos);

	//Checks Whick Bottom BG hit test to
	float cooficent = enemySprite->GetX() / 2759;
	if (cooficent <= 1 || (cooficent > 3 && cooficent <= 4) || (cooficent > 6 && cooficent <= 7)) BottomHitTestEmnemy = localMapVar->bgBottom;
	else if (cooficent <= 2 || (cooficent > 4 && cooficent <= 5) || (cooficent > 7 && cooficent <= 8)) BottomHitTestEmnemy = localMapVar->bgBottom2;
	else  BottomHitTestEmnemy = localMapVar->bgBottom3;

	//Hit test Going Up
	if (enemySprite->HitTest(BottomHitTestEmnemy))
	{
		if (!inAttack) 
		{
			enemySprite->SetVelocity(0, 0);
			setDefaultStandAnimation();
		}
		
		// To HIGH ->set player posY + 25 and checks is it still hitt test, if so->to higth to walk
		enemySprite->SetPosition(pos + CVector(0, 25));
		if (enemySprite->HitTest(BottomHitTestEmnemy))
			enemySprite->SetPosition(pos);
		//if not to higth set prev. pos Y + 5 , to walk UP
		else enemySprite->SetPosition(pos + CVector(0, 5));
	}
	else
	{
		//Check is there a bottom img in -10px distance , if no moves Enemie Down
		enemySprite->SetY(enemySprite->GetY() - 10);
		if (enemySprite->HitTest(BottomHitTestEmnemy))
		{
			enemySprite->SetY(enemySprite->GetY() + 10);
		}
	}
}

//************ ANIMATION -> Restore Basic Animation
void Enemy::EnemyAnimation(int old_animation_status)
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
void Enemy::EnemyInterface()
{
	float baseHpBarWidth = 100;
	if (enemyType == BOSS1) baseHpBarWidth = 350;

	enemyHpBarRect2->SetX(enemySprite->GetX());
	enemyHpBarRect2->SetY(enemySprite->GetY() + enemySprite->GetSize().GetY()  - 10);
	float hpBarSize = baseHpBarWidth * (CurrentEnemyHealth / maxEnemyHealth);
	if (hpBarSize < 0) hpBarSize = 0;
	enemyHpBarRect2->SetSize(hpBarSize, 10);
	enemyHpBarRect2->Update(CurrentTime);
}

//***** ENEMY ATTACK
void Enemy::EnemyAttack(Player& player)
{
	if (enemySprite->GetStatus() != INATTACK && inAttack == false)
	{
		//set Statuses , and remove speed
		enemySprite->SetStatus(INATTACK);
		enemySprite->SetVelocity(0, 0);
		inAttack = true;

		//  MELEE ATTACKS
		if (enemyType != NINJAGIRL && enemyType != NINJAGIRLKUNAI)
		{
			if (enemyType == DOG) frameRatesToStop = 2;
			else frameRatesToStop = 9;
			if (EnemyDirection == 90) enemySprite->SetAnimation("katanaAttack", frameRatesToStop);
			else enemySprite->SetAnimation("katanaLeft", frameRatesToStop);
			
		}
		//REMOTE ATTACKS
		else {
			hitTestDelay = true; // to avoid melee damage
			if (EnemyDirection == 90) enemySprite->SetAnimation("throwAttack", 7);
			else if (EnemyDirection == -90) enemySprite->SetAnimation("throwAttackLeft", 7);
			frameRatesToStop = 7;
			char * imgpath = throwAttackImgPath;

			//new Shot generator
			CSprite* newShot = new CSprite(enemySprite->GetX(), enemySprite->GetY(), imgpath, CurrentTime);
			newShot->SetDirection(EnemyDirection);
			int shotRotation = 0;
			if (EnemyDirection == -90) shotRotation = 180;
			newShot->SetRotation(shotRotation);
			newShot->SetSpeed(500 + enemySprite->GetSpeed());
			newShot->Die(1000);
			EnemyShotList.push_back(newShot);
		}	
	}
}

//**** Shots Handler for Update function
void Enemy::shotsHandler()
{
	for (CSprite* shot : EnemyShotList)
	{
		if (shot->HitTest(localMapVar->bgBottom) || shot->HitTest(localMapVar->bgBottom2) || shot->HitTest(localMapVar->bgBottom3))
			shot->Delete();

		if (shot->HitTest(castToPlayer->playerSprite) && castToPlayer->playerSprite->GetStatus() != castToPlayer->SLIDE)
		{
			shot->Delete();
			castToPlayer->PlayerGettingDamage(fireboltDamage);
		}
		shot->Update(CurrentTime);
	}
}

//*** RESTORE DEFAULT ANIMATION BASE ON DIRECTION
void Enemy::setDefaultStandAnimation()
{
	if (EnemyDirection == 90) 	enemySprite->SetStatus(STANDRIGHT);
	if (EnemyDirection == -90) enemySprite->SetStatus(STANDLEFT);
	enemySprite->SetVelocity(0, 0);
}

//**** INITIAL SPRITES DEPENDING ON TYPE OF ENEMY
void Enemy::initAnimations() 
{
	if (enemyType == WARIOR)
	{
		//walk / iddle animation
		enemySprite->AddImage("WariorIdleRight.png", "standright", 10, 1, 0, 0, 9, 0, CColor::White());
		enemySprite->AddImage("WariorRunRight.png", "walkright", 10, 1, 0, 0, 9, 0, CColor::White());
		enemySprite->AddImage("WariorIdleLeft.png", "standleft", 10, 1, 0, 0, 9, 0, CColor::White());
		enemySprite->AddImage("WariorRunLeft.png", "walkleft", 10, 1, 9, 0, 0, 0, CColor::White());

		//in damage 
		enemySprite->AddImage("wariorInDamageRight.png", "inDamageRight", 2, 1, 0, 0, 1, 0, CColor::White());
		enemySprite->AddImage("wariorInDamageLeft.png", "inDamageLeft", 2, 1, 1, 0, 0, 0, CColor::White());

		//attack
		enemySprite->AddImage("wariorAttackRight.png", "katanaAttack", 10, 1, 0, 0, 9, 0, CColor::White());
		enemySprite->AddImage("wariorAttackLeft.png", "katanaLeft", 10, 1, 9, 0, 0, 0, CColor::White());

		//death
		enemySprite->AddImage("wariorDeadLeft.png", "deadLeft", 10, 1, 9, 0, 0, 0, CColor::White());
		enemySprite->AddImage("wariorDeadRight.png", "deadRight", 10, 1, 0, 0, 9, 0, CColor::White());
	}

	if (enemyType == BOSS1)
	{
		//walk / iddle animation
		enemySprite->AddImage("narutoIddleRight.png", "standright", 6, 1, 0, 0, 5, 0, CColor::Black());
		enemySprite->AddImage("narutoRunRight.png", "walkright", 10, 1, 0, 0, 9, 0, CColor::Black());
		enemySprite->AddImage("narutoIddleLeft.png", "standleft", 6, 1, 5, 0, 0, 0, CColor::Black());
		enemySprite->AddImage("narutoRunLeft.png", "walkleft", 10, 1, 0, 0, 9, 0, CColor::Black());

		//in damage 
		enemySprite->AddImage("narutoDamageRight.png", "inDamageRight", 2, 1, 0, 0, 1, 0, CColor::Black());
		enemySprite->AddImage("narutoDamageLeft.png", "inDamageLeft", 2, 1, 1, 0, 0, 0, CColor::Black());

		//death
		enemySprite->AddImage("narutoDeadLeft.png", "deadLeft", 4, 1, 3, 0, 0, 0, CColor::Black());
		enemySprite->AddImage("narutoDeadRight.png", "deadRight", 4, 1, 0, 0, 3, 0, CColor::Black());

		//melee attack
		enemySprite->AddImage("narutoAttackRight.png", "katanaAttack", 10, 1, 0, 0, 9, 0, CColor::Black());
		enemySprite->AddImage("narutoAttackLeft.png", "katanaLeft", 10, 1, 9, 0, 0, 0, CColor::Black());
		enemySpeed = 270;
		attackDistance = 40;
	}

	else if (enemyType == DOG) {
		attackDistance = 70;

		//walk / iddle animation
		enemySprite->AddImage("dogIdle.png", "standright", 3, 1, 0, 0, 2, 0, CColor::Black());
		enemySprite->AddImage("dogWalkRight.png", "walkright", 8, 1, 6, 0, 0, 0, CColor::Black());
		enemySprite->AddImage("dogIdleLeft.png", "standleft", 3, 1, 2, 0, 0, 0, CColor::Black());
		enemySprite->AddImage("dogWalkLeft.png", "walkleft", 8, 1, 1, 0, 7, 0, CColor::Black());

		//indamage
		enemySprite->AddImage("dogInDamageLeft.png", "inDamageLeft", 2, 1, 0, 0, 1, 0, CColor::Black());
		enemySprite->AddImage("dogInDamageRight.png", "inDamageRight", 2, 1, 0, 0, 1, 0, CColor::Black());

		//Melee Attack Animation
		enemySprite->AddImage("dogAttackRight.png", "katanaAttack", 3, 1, 2, 0, 0, 0, CColor::Black());
		enemySprite->AddImage("dogAttackLeft.png", "katanaLeft", 3, 1, 0, 0, 2, 0, CColor::Black());

		//Death Animation
		enemySprite->AddImage("dogDeathLeft.png", "deadLeft", 4, 1, 0, 0, 3, 0, CColor::Black());
		enemySprite->AddImage("dogDeathRight.png", "deadRight", 4, 1, 3, 0, 0, 0, CColor::Black());
	}
	else if (enemyType == NINJAGIRL || enemyType == NINJAGIRLMELEE || enemyType == NINJAGIRLKUNAI) {
		if (enemyType == NINJAGIRL || enemyType == NINJAGIRLKUNAI) attackDistance = 450;

		//walk / iddle animation
		enemySprite->AddImage("EnemyGirlIdleRight.png", "standright", 10, 1, 0, 0, 9, 0, CColor::Black());
		enemySprite->AddImage("EnemyGirlRunRight.png", "walkright", 10, 1, 0, 0, 9, 0, CColor::Black());
		enemySprite->AddImage("EnemyGirlIdleLeft.png", "standleft", 10, 1, 0, 0, 9, 0, CColor::Black());
		enemySprite->AddImage("EnemyGirlRunLeft.png", "walkleft", 10, 1, 9, 0, 0, 0, CColor::Black());

		//Melee Attack Animation
		enemySprite->AddImage("EnemyGirlMelleAttackRight.png", "katanaAttack", 10, 1, 0, 0, 9, 0, CColor::Black());
		enemySprite->AddImage("EnemyGirlMelleAttackLeft.png", "katanaLeft", 10, 1, 9, 0, 0, 0, CColor::Black());

		//Remote Attack
		enemySprite->AddImage("EnemyGirlIThrowAttackRight.png", "throwAttack", 8, 1, 0, 0, 7, 0, CColor::Black());
		enemySprite->AddImage("EnemyGirlIThrowAttackLeft.png", "throwAttackLeft", 8, 1, 7, 0, 0, 0, CColor::Black());

		//Death Animation
		enemySprite->AddImage("EnemyGirllDeadLeft.png", "deadLeft", 9, 1, 8, 0, 0, 0, CColor::Black());
		enemySprite->AddImage("EnemyGirllDeadRight.png", "deadRight", 9, 1, 0, 0, 8, 0, CColor::Black());

		//InDamage Animation
		enemySprite->AddImage("EGirrllInDamageLeft.png", "inDamageLeft", 2, 1, 1, 0, 0, 0, CColor::Black());
		enemySprite->AddImage("EGirrllInDamageRight.png", "inDamageRight", 2, 1, 0, 0, 1, 0, CColor::Black());
	}

	if (enemyType == NINJAGIRL)
	{
		throwAttackImgPath = "firebolt2.png";
		RangeAttackDamage = fireboltDamage;
	}
	else
	{
		throwAttackImgPath = "kunai.png";
		RangeAttackDamage = kunaiDamage;
	}
}

//ENEMY RECEIVING DAMAGE
void Enemy::EnemyGettingDamage(float damageAmount, float t, CSpriteList& DroppedPoutions)
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
	if (CurrentEnemyHealth <= 0 && enemySprite->GetStatus() != dead) {

		dead = true;
		frameRatesToStop = 8;
		if (enemyType == DOG || enemyType == BOSS1) frameRatesToStop = 3;
		if (EnemyDirection == -90) enemySprite->SetAnimation("deadLeft", frameRatesToStop);
		else enemySprite->SetAnimation("deadRight", frameRatesToStop);
		deadSound.Play("EnemyDeath.wav");
		deadSound.Volume(0.3);
		EnemyInterface();

		//Drop  - Loot
		enum DropList { HpPoutin, MpPoution, EnergyPoution, kunai };
		int randomDrop = rand() % 4;
		char* imgpath = "";
		if (randomDrop == HpPoutin)  imgpath = "HpBottle.png";
		if (randomDrop == MpPoution)  imgpath = "MpBottle.png";
		if (randomDrop == EnergyPoution)  imgpath = "energyBottle.png";
		if (randomDrop == kunai)  imgpath = "kunaiForBar.png";

		CSprite* newPoution = new CSprite(enemySprite->GetX(), enemySprite->GetY() - enemySprite->GetSize().GetY() + 55, imgpath, CColor::White(), t);
		newPoution->SetSize(30, 30);
		newPoution->SetStatus(randomDrop);

		//Send Experience to player depending on Max Health
		castToPlayer->PlayerGettingExp(maxEnemyHealth);
		DroppedPoutions.push_back(newPoution);
	}
}