#include "stdafx.h"
#include "../headers/PlayerEntity/EntityWeapon.h"
#include "../headers/PlayerEntity/PlayerEntity.h"
#include "../headers/Map.h"
#include "../headers/Physic.h"

EntityWeapon::EntityWeapon()
{
	//SPRITES INIT
	explosionDamageImg.SetImageFromFile("maxExplodeRadius.jpg");
	AmmoSpriteInit();

	//DAMAGE
	damageBasedOnWeaponType[AXE] = 25;
	damageBasedOnWeaponType[MISSILE] = 50;
	damageBasedOnWeaponType[BOMB] = 65;
	damageBasedOnWeaponType[BANANA] = 45;
	damageBasedOnWeaponType[DYNAMIT] = 55;
	damageBasedOnWeaponType[MINE] = 50;
	damageBasedOnWeaponType[UZI] = 60;
	damageBasedOnWeaponType[BOW] = 50;
	damageBasedOnWeaponType[MAIL] = 25;
}

//*** INIT
void EntityWeapon::init()
{	
	//RESETS
	EnemyShotList.delete_all();
	explosionTimer = 0;
	lastFrametime = 0;
	chargeAttackTimer = 0;
	localWeaponType = NONE;
	IsInAttackChargeMode = false;
	maxChargeAttackTime = 2000;
}


//*** UPDATE
void EntityWeapon::OnUpdate(UINT32 time, std::vector<PlayerEntity*> PlayerEntitys, CSprite& entitySprite, int weaponType, MapGen& Map, float EnemyDirection, float aimAngle)
{
	//Delta Time
	if (lastFrametime != 0)  dt = time - lastFrametime;
	lastFrametime = time;

	//local Variables-  just miroring to be able use in whole class
	localEnemyDirection = EnemyDirection;
	localEnemySprite = &entitySprite;
	localAllPlayerEntityes = PlayerEntitys;
	localTime = time;
	localMap = &Map;
	localWeaponType = weaponType;
	localAimAngle = aimAngle;
	
	//IF ATTACK CHARGE REACHED MAX POINT RELEASE PROJECTILE
	if (chargeAttackTimer != 0 && chargeAttackTimer - localTime <= 0)
	{
		LoadingTick.Stop();
		EntityWeapon::attackHandler(chargeAttackTimer, maxChargeAttackTime, chargedAttackBar);
	}
	
	//IF IN ATTACK AND Weapon Is Axe call Melee Attack , otherwise Shots Handler
	if (localEnemySprite->GetStatus() == INATTACK && localEnemySprite->GetCurrentAnimationFrame() == 3 && weaponType == AXE) meleeAttack();
	else shotsHandler();

	//Updates
	for (CSprite* explosion : explosionAnimationList) explosion->Update(localTime);
	for (CSprite* shot : EnemyShotList) shot->Update(localTime);
	chargedAttackBar.Update(localTime);

	//Deletes
	explosionAnimationList.delete_if(deleted);
	EnemyShotList.delete_if(deleted);
}


//*** DRAW
void EntityWeapon::OnDraw(CGraphics* g)
{
	//Chargin Attack Sound and Logic
	if (IsInAttackChargeMode)
	{
		if (!LoadingTick.IsPlaying()) LoadingTick.Play("chargeAttack.wav");
		CVector side;
		if (localEnemyDirection == 90)
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

	//if Mail Weapon selected draw Mouse Pointer
	if (localWeaponType == MAIL) mailAttackMousePointer.Draw(g);

	//Player shots
	for (CSprite* shot : EnemyShotList) shot->Draw(g);
	for (CSprite* explosion : explosionAnimationList) explosion->Draw(g);
}


//*** ATTACK HANDLER - Create Projectile
void EntityWeapon::attackHandler( float& chargeAttackTimer, float& maxChargeAttackTime, CSprite& chargedAttackBar)
{
	//resets
	float fullyChargedAttack = 0;	//calculate projectile speed
	explosionDamageActive = false;
	ProjectileSpeed = 0;
	explosionTimer = 5000 + localTime;
	chargedAttackBar.SetSize(10, 30);
	IsInAttackChargeMode = false;
	CSprite* newProjectile = new CSprite();

	//CREATE PROJECTILE BASE ON WEAPON TYPE
	switch (localWeaponType)
	{
	case MISSILE:
		fullyChargedAttack = 1000;
		missileSprite->SetPos(mailAttackMousePointer.GetPosition());
		newProjectile = missileSprite->Clone();
		break;

	case BOMB:
		fullyChargedAttack = 1000;
		newProjectile = bombSprite->Clone();
		newProjectile->SetOmega(250);
		explosionTimer = 2500 + localTime;
		break;

	case BANANA:
		fullyChargedAttack = 1000;
		newProjectile = bananaSprite->Clone();
		newProjectile->SetOmega(250);
		bananaFirstTouch = false;
		explosionTimer = 2500 + localTime;
		break;

	case DYNAMIT:
		fullyChargedAttack = 300;
		newProjectile = dynamitSprite->Clone();
		newProjectile->SetRotation(0);
		explosionTimer = 2000 + localTime;
		break;

	case MINE:
		fullyChargedAttack = 300;
		newProjectile = mineSprite->Clone();
		explosionTimer = 2000 + localTime;
		newProjectile->SetRotation(0);
		break;

	case UZI:
		fullyChargedAttack = 900;
		newProjectile = uziSprite->Clone();
		break;

	case BOW:
		fullyChargedAttack = 700;
		newProjectile = bowSprite->Clone();
		break;

	case MAIL:
		fullyChargedAttack = 300;
		newProjectile = mailSprite->Clone();
		break;

	case SKIP:
		fullyChargedAttack = 300;
		newProjectile = missileSprite->Clone();
		break;

	default:
		break;
	}

	//SETS POSITIONS AND RESETS TIME
	newProjectile->SetPosition(localEnemySprite->GetX(), localEnemySprite->GetY() + 20);
	newProjectile->ResetTime(localTime);


	//CALCULATES PROJECTILE SPEED
	float timeDiference = chargeAttackTimer - localTime;
	if (timeDiference <= 0) ProjectileSpeed = fullyChargedAttack;
	else
	{
		float timePast = maxChargeAttackTime - timeDiference;
		ProjectileSpeed = fullyChargedAttack / maxChargeAttackTime * timePast;
	}

	//reset chargeAttackTimer
	chargeAttackTimer = 0;


	//SET PROJECTILE SPEED AND STATUS
	newProjectile->SetStatus(localWeaponType);
	newProjectile->SetSpeed(ProjectileSpeed);

	//IF MAIL ATTACK 
	if (newProjectile->GetStatus() == MAIL) {
		for (int i = 0; i < 4; i++)
		{
			CSprite* copy = newProjectile->Clone();
			copy->SetPosition(mailAttackMousePointer.GetX() + i * 50, 650 + rand() % 150 + 50 );
			EnemyShotList.push_back(copy);
		}
	}
	//IF REST OF PROJECTILES
	else 
	{
		if (localEnemyDirection == -90) newProjectile->SetDirection(localEnemyDirection + localAimAngle);
		else newProjectile->SetDirection(localEnemyDirection - localAimAngle);
		EnemyShotList.push_back(newProjectile);
	}
}

 
//**** SHOTS HANDLER
void EntityWeapon::shotsHandler()
{
	//IF explosionDamageActive => DAMAGE APPLY
	DamageApply();
	
	//HITTEST & LOGIC
	for (CSprite* shot : EnemyShotList)
	{
		//OUTSIDE OF THE MAP -> DELETE
		if (shot->GetX() < 0 || shot->GetX() > 1920 || shot->GetY() < 0 || shot->GetY() > 1080) {
			shot->Delete();
			continue;
		}

		ApplyBasePhysic(*shot);
		OnExploditionTimerEnd(*shot);

		//BOMB AND BANANA REFLECTION LOGIC "CUSTOME"
		if (shot->GetStatus() == BOMB || shot->GetStatus() == BANANA)
			BombAndBananaLogic(*shot);
		else otherProjectileHandler(*shot); 	// Other PROJECTILE MAP HIT TEST
	}
}


//BASE PHYSIC APPLY
void EntityWeapon::ApplyBasePhysic(CSprite& shot)
{
	//**** APPLY WIND FORCE  // GetSpeed to avoid moving object which lended on the ground
	if (shot.GetSpeed() > 30 && shot.GetStatus() != BOW && shot.GetStatus() != UZI) shot.Accelerate(windStrength);

	//**** APPLY GRAVITY FORCE
	CVector gravity = { 0,-10 };
	if (shot.GetStatus() != BOW && shot.GetStatus() != UZI) shot.Accelerate(gravity);

	//SET DIRECTION BASED ON ROTATION
	if (shot.GetStatus() == MISSILE) shot.SetRotation(shot.GetDirection());
	if (shot.GetStatus() == BOW || shot.GetStatus() == UZI) shot.SetRotation(shot.GetDirection() - 90);


	//IF MAIL ATTACK ROTATE - using HOOKES LAW
	if (shot.GetStatus() == MAIL)
		Physic::UpdateSpriteRotationHookesLaw(shot, 90, 0.1);
}


//WHEN EXPLODITION TIMER < CURRENT TIME => EXPLODE
void EntityWeapon::OnExploditionTimerEnd(CSprite& shot)
{
	//IF DINAMIT EXPLODE TIME < CURRENT TIME -> EXPLODE
	if (explosionTimer < localTime && (shot.GetStatus() == DYNAMIT || shot.GetStatus() == MINE))
	{
		exploditionSetup(shot.GetX(), shot.GetY());
		shot.Delete();
	}

	//IF BOMB OR BANANA
	if ((explosionTimer < localTime) && (shot.GetStatus() == BOMB || shot.GetStatus() == BANANA))
	{
		if (shot.GetStatus() == BANANA)
		{
			explosionTimer += 100; // because I have One explodition image, so it can't be in three places in same time, it should explode in diferent time  -> (probably had to use List!!)
		}
		exploditionSetup(shot.GetX(), shot.GetY());
		shot.Delete();
	}

	//AIPlayer HIT TEST if hits player and ammo is not DYNAMIT or Mine -> explode
	if (localWeaponType != DYNAMIT && localWeaponType != MINE)
	{
		for (auto AIPlayer : localAllPlayerEntityes)
		{
			if (!AIPlayer->isPlayerTurn && Physic::hitTestHandler(shot, *AIPlayer->entitySprite, 0.5, true, dt))
			{
				exploditionSetup(AIPlayer->entitySprite->GetX(), AIPlayer->entitySprite->GetY());
				shot.Delete();
				break;
			}
		}
	}
}


//BOMB AND BANANA REFLECTION LOGIC "CUSTOME"
void EntityWeapon::BombAndBananaLogic(CSprite& shot)
{
	for (CSprite* mapPart : localMap->mapList)
	{
		if (shot.GetSpeed() < 5) // stop rotation and Wind Effect if speed is < 5
		{
			shot.SetOmega(0);
			shot.SetSpeed(0);
			break;
		}

		//Return True if hit test happend and makes reflection IF BOMB JUST REFLECT
		if (Physic::hitTestHandler(shot, *mapPart, 0.5, true, dt))
		{
			if (shot.GetStatus() == BANANA && !bananaFirstTouch)
			{
				//IF BANANA FIRST TOUCH CREATES MULTIPLY BANANAS
				for (int i = 0; i < 3; i++)
				{
					bananaFirstTouch = true;
					CSprite* newProjectile = new CSprite();
					newProjectile = bananaSprite->Clone();
					newProjectile->SetOmega(250);
					newProjectile->SetDirection(shot.GetDirection());
					newProjectile->SetSpeed(shot.GetSpeed() + rand() % 160 + 50);
					newProjectile->SetStatus(BANANA);
					newProjectile->SetPosition(shot.GetPosition());
					newProjectile->ResetTime(localTime);
					EnemyShotList.push_back(newProjectile);
				}
				shot.Delete();
			}
			break;
		}
	}
}

//OTHER PROJECTILE HANDLER
void EntityWeapon::otherProjectileHandler(CSprite& shot)
{
	for (CSprite* mapPart : localMap->mapList)
	{
		if (Physic::hitTestHandler(shot, *mapPart, false, false, dt))
		{
			//IF DYNAMIT
			if (shot.GetStatus() == DYNAMIT || shot.GetStatus() == MINE)
			{
				shot.SetSpeed(0);
				return;
			}

			//Explodition Setup
			exploditionSetup(mapPart->GetX(), mapPart->GetY());
			shot.Delete();
		}
	}
}

//EXPLODITION IMAGE AND ANIMATION SETUP
void EntityWeapon::exploditionSetup(float posX, float posY)
{
	//Animation
	CSprite* explosion = new CSprite();
	explosion = explosionSprite->Clone();
	explosion->SetPosition(posX, posY);
	explosion->ResetTime(localTime);
	explosion->Die(750);
	explosionAnimationList.push_back(explosion);

	//ACTUAL Explosion Image FOR Damage
	explosionDamageImg.SetPos(posX, posY);
	explosionDamageActive = true;
}

//*** DAMAGE APPLY
void EntityWeapon::DamageApply()
{
	if (explosionDamageActive)
	{
		for (CSprite* mapPart : localMap->mapList)
			if (explosionDamageImg.HitTest(mapPart)) mapPart->Delete();

		for (auto playerEntity : localAllPlayerEntityes)
			if (playerEntity->entitySprite->HitTest(&explosionDamageImg)) playerEntity->EtityGettingDamage(damageBasedOnWeaponType[localWeaponType]);

		explosionDamageActive = false;
		
		Explodition.Play("Explosion.wav");
		Explodition.Volume(30);
	}
}

//*** AXE ATTACK
void EntityWeapon::meleeAttack()
{
	if(localEnemyDirection == 90) localEnemySprite->SetStatus(STANDRIGHT);
	else localEnemySprite->SetStatus(STANDLEFT);
	for (auto playerEntity : localAllPlayerEntityes)
	{
		if (playerEntity->entitySprite->HitTest(localEnemySprite) && playerEntity->isPlayerTurn == false)
		{
			playerEntity->EtityGettingDamage(damageBasedOnWeaponType[AXE]);
		}
	}
}

//********* KEYBOARD EVENTS
void EntityWeapon::OnKeyDown(SDLKey sym)
{
	//START ATTACK CHARGING
	if (sym == SDLK_SPACE )
	{
		if (localWeaponType == SKIP) return;

		localEnemySprite->SetSpeed(0);
		localEnemySprite->SetStatus(INATTACK);


		if (localWeaponType == BOW || localWeaponType == UZI)
		{
			//reset animation
			localEnemySprite->SetStatus(NONE);
			attackHandler(chargeAttackTimer, maxChargeAttackTime, chargedAttackBar);
		}

		if (localWeaponType != AXE && localWeaponType != BOW && localWeaponType != UZI)
		{
			//reset animation
			if (localEnemyDirection == 90) chargedAttackBar.SetPosition(localEnemySprite->GetPos() + CVector(18, 18));
			else chargedAttackBar.SetPosition(localEnemySprite->GetPos() + CVector(-15, 18));

			IsInAttackChargeMode = true;
			chargeAttackTimer = localTime + maxChargeAttackTime;
		}
	}
}


void EntityWeapon::OnKeyUp(SDLKey sym)
{
	AttackSoundPlay(); //shot sound Play

	if (localWeaponType != AXE )
	{
		if ( sym == SDLK_SPACE && IsInAttackChargeMode)
		{			
			LoadingTick.Stop();
			EntityWeapon::attackHandler( chargeAttackTimer, maxChargeAttackTime, chargedAttackBar);
		}
	}
}

//*** MOUSE EVENETS
void EntityWeapon::OnMouseMove(Uint16 x, Uint16 y)
{
	if (localWeaponType == MAIL) mailAttackMousePointer.SetPos(x, y);
}


//** SHOTS SOUNDS PLAY
void EntityWeapon::AttackSoundPlay()
{
	if(localWeaponType == AXE) attackSound.Play("axeSlash.wav");
	else if (localWeaponType == MISSILE) attackSound.Play("missileAttack.wav");
	else if (localWeaponType == BOMB) attackSound.Play("HOLYGRENADE.wav");
	else if (localWeaponType == BANANA) attackSound.Play("bananaThrow.wav");
	else if (localWeaponType == MINE || localWeaponType == DYNAMIT) attackSound.Play("mineDynamitSound.wav");
	else if (localWeaponType == UZI) attackSound.Play("uziShot.wav");
	else if (localWeaponType == BOW) attackSound.Play("bowAttack.wav");
	else if (localWeaponType == MAIL) attackSound.Play("Airstrike.wav");
	else if (localWeaponType == SKIP) attackSound.Play("turnSkipSound.wav");
}


 


//PROJECTILE SPRITE INITS
void EntityWeapon::AmmoSpriteInit()
{
	missileSprite = new CSprite();
	missileSprite->AddImage("ammo.png", "missileImg", 8, 1, 1, 0, 1, 0, CColor::Black());
	missileSprite->SetAnimation("missileImg", 1);

	bombSprite = new CSprite();
	bombSprite->AddImage("ammo.png", "BOMB", 8, 1, 4, 0, 4, 0);
	bombSprite->SetAnimation("BOMB", 1);

	bananaSprite = new CSprite();
	bananaSprite->AddImage("ammo.png", "BANANA", 8, 1, 3, 0, 3, 0);
	bananaSprite->SetAnimation("BANANA", 1);

	dynamitSprite = new CSprite();
	dynamitSprite->AddImage("ammo.png", "DYNAMIT", 8, 1, 0, 0, 0, 0, CColor::Black());
	dynamitSprite->SetAnimation("DYNAMIT", 1);

	mineSprite = new CSprite();
	mineSprite->AddImage("ammo.png", "MINE", 8, 1, 2, 0, 2, 0, CColor::Black());
	mineSprite->SetAnimation("MINE", 1);

	uziSprite = new CSprite();
	uziSprite->AddImage("ammo.png", "UZI", 8, 1, 7, 0, 7, 0, CColor::Black());
	uziSprite->SetAnimation("UZI", 1);

	bowSprite = new CSprite();
	bowSprite->AddImage("ammo.png", "BOW", 8, 1, 6, 0, 6, 0, CColor::Black());
	bowSprite->SetAnimation("BOW", 1);

	mailSprite = new CSprite();
	mailSprite->AddImage("ammo.png", "MAIL", 8, 1, 5, 0, 5, 0, CColor::Black());
	mailSprite->SetAnimation("MAIL", 1);

	explosionSprite = new CSprite();
	explosionSprite->AddImage("explosion.bmp", "explosion", 5, 5, 0, 0, 4, 4, CColor::Black());
	explosionSprite->SetAnimation("explosion", 25);

	//chargedAttackBar
	chargedAttackBar.LoadImage("chargedAttackBar.png");
	chargedAttackBar.SetImage("chargedAttackBar.png");
	chargedAttackBar.SetSize(10, 30);

	mailAttackMousePointer.LoadImage("mailAttackPointer.png");
	mailAttackMousePointer.SetImage("mailAttackPointer.png");
	mailAttackMousePointer.SetSize(30, 30);
}