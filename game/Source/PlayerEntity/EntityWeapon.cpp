#include "stdafx.h"
#include "../headers/PlayerEntity/EntityWeapon.h"
#include "../headers/PlayerEntity/PlayerEntity.h"
#include "../headers/Map.h"
#include "../headers/Physic.h"

void EntityWeapon::init()
{	
	//DAMAGE
	damageBasedOnWeaponType[AXE] = 25;
	damageBasedOnWeaponType[MISSILE] = 50;
	damageBasedOnWeaponType[BOMB] = 65;
	damageBasedOnWeaponType[BANANA] = 45;
	damageBasedOnWeaponType[DYNAMIT] = 55;
	damageBasedOnWeaponType[MINE] = 155;
	damageBasedOnWeaponType[UZI] = 60;
	damageBasedOnWeaponType[BOW] = 50;
	damageBasedOnWeaponType[MAIL] = 25;

	//reset shots and hit effects
	EnemyShotList.delete_all();


	isInAttackDelay = false;
	explosionTimer = 0;
	lastFrametime = 0;

	explosionDamageImg.SetImageFromFile("maxExplodeRadius.jpg");
	isAlldamageDealed = false;
	IsInAttackChargeMode = false;
	AmmoSpriteInit();

	maxChargeAttackTime = 2000;
	chargeAttackTimer = 0;
	localWeaponType == NONE;
 
}

void EntityWeapon::OnUpdate(UINT32 time, std::vector<PlayerEntity*> PlayerEntitys, CSprite& enemySprite, int weaponType, MapGen& Map, float EnemyDirection, float aimAngle)
{
	if (lastFrametime != 0)  dt = time - lastFrametime;
	lastFrametime = time;
	localEnemyDirection = EnemyDirection;
	localEnemySprite = &enemySprite;
	localAllPlayerEntityes = PlayerEntitys;
	localTime = time;
	localMap = &Map;
	localWeaponType = weaponType;
	localAimAngle = aimAngle;
	
	
	if (chargeAttackTimer != 0 && chargeAttackTimer - localTime <= 0)
	{
		LoadingTick.Stop();
		EntityWeapon::attackHandler(chargeAttackTimer, maxChargeAttackTime, chargedAttackBar);
	}
	


	if (localEnemySprite->GetStatus() == INATTACK && localEnemySprite->GetCurrentAnimationFrame() == 3 && weaponType == AXE) meleeAttack();
	else shotsHandler();


 

	//updates
	for (CSprite* explosion : explosionAnimationList) explosion->Update(localTime);
	for (CSprite* shot : EnemyShotList) shot->Update(localTime);
	chargedAttackBar.Update(localTime);

	//deletes
	explosionAnimationList.delete_if(deleted);
	EnemyShotList.delete_if(deleted);
	

 
}

void EntityWeapon::attackHandler( float& chargeAttackTimer, float& maxChargeAttackTime, CSprite& chargedAttackBar)
{
	float fullyChargedAttack = 0;	//calculate projectile speed
	explosionDamageActive = false;
	ProjectileSpeed = 0;
	CSprite* newProjectile = new CSprite();

	explosionTimer = 5000 + localTime;
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
		fullyChargedAttack = 1200;
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

	//pos and time Reset
	newProjectile->SetPosition(localEnemySprite->GetX(), localEnemySprite->GetY() + 20);
	newProjectile->ResetTime(localTime);


	//calculate projectile speed
	IsInAttackChargeMode = false;
	float timeDiference = chargeAttackTimer - localTime;
	if (timeDiference <= 0) ProjectileSpeed = fullyChargedAttack;
	else
	{
		float timePast = maxChargeAttackTime - timeDiference;
		ProjectileSpeed = fullyChargedAttack / maxChargeAttackTime * timePast;
	}
	//reset
	chargedAttackBar.SetSize(10, 30);
	chargeAttackTimer = 0;
	newProjectile->SetStatus(localWeaponType);
	newProjectile->SetSpeed(ProjectileSpeed);

	 

	if (newProjectile->GetStatus() == MAIL) {
		for (int i = 0; i < 4; i++)
		{
			CSprite* copy = newProjectile->Clone();
			copy->SetPosition(mailAttackMousePointer.GetX() + i * 50, 650 + rand() % 150 + 50 );
			EnemyShotList.push_back(copy);
		}
 
		
	}
	else 
	{
		if (localEnemyDirection == -90) newProjectile->SetDirection(localEnemyDirection + localAimAngle);
		else newProjectile->SetDirection(localEnemyDirection - localAimAngle);
	 
		EnemyShotList.push_back(newProjectile);
	}

}


//**** Shots Handler for Update function
void EntityWeapon::shotsHandler()
{
	//DAMAGE APPLY
	DamageApply();
	
	//HITTEST & LOGIC
	for (CSprite* shot : EnemyShotList)
	{
		//**** APPLY WIND FORCE  // GetSpeed to avoid moving object which lended on the ground
		if(shot->GetSpeed() > 30) shot->Accelerate(windStrength);

		//**** APPLY GRAVITY FORCE
		CVector gravity = { 0,-10 };


		//Outside of map -> delete
		if (shot->GetX() < 0 || shot->GetX() > 1920 || shot->GetY() < 0 || shot->GetY() > 1080) {
			shot->Delete();
			break;
		}

		
		if (shot->GetStatus() != BOW && shot->GetStatus() != UZI) shot->Accelerate(gravity);
		if(shot->GetStatus() == MISSILE ) shot->SetRotation(shot->GetDirection());
		if (shot->GetStatus() == BOW || shot->GetStatus() == UZI) shot->SetRotation(shot->GetDirection() - 90);
	 

		//IF MAIL ROTATE - using HOOKES LAW
		if (shot->GetStatus() == MAIL) 
			Physic::UpdateSpriteRotationHookesLaw(*shot, 90, 0.1);
	
	
		
	 
		//IF DINAMIT EXPLODE TIME
		if (explosionTimer < localTime && (shot->GetStatus() == DYNAMIT || shot->GetStatus() == MINE))
		{
			exploditionSetup(shot->GetX(), shot->GetY());
			shot->Delete();
		 
		}

		//IF BOMB OR BANANA
		if ((explosionTimer < localTime ) && (shot->GetStatus() == BOMB || shot->GetStatus() == BANANA)  )
		{
			if (shot->GetStatus() == BANANA)
			{
				explosionTimer += 100; // becouse i have One explodition image , so it can be in trhee places in same time , had to use List!!
			}
				exploditionSetup(shot->GetX(), shot->GetY());
				shot->Delete();
		}

		//AIPlayer HIT TEST if hit player and ammo is not DYNAMIT or Minde -> explode
		if (localWeaponType != DYNAMIT && localWeaponType != MINE) 
		{
			for (auto AIPlayer : localAllPlayerEntityes)
			{
				if (!AIPlayer->isPlayerTurn && shot->HitTest(AIPlayer->enemySprite))
				{
					exploditionSetup(AIPlayer->enemySprite->GetX(), AIPlayer->enemySprite->GetY());
					shot->Delete();
					break;
				}
			}
		}

		

		//BOMB AND BANANA REFLECTION LOGIC
		if (shot->GetStatus() == BOMB || shot->GetStatus() == BANANA)
		{
			for (CSprite* mapPart : localMap->mapList)
			{
				if (shot->GetSpeed() < 5)
				{
					shot->SetOmega(0);
					shot->SetSpeed(0);
					break;
				}
				if (Physic::hitTestHandler(*shot, *mapPart, 0.5, true, dt)) 
				{
					if (shot->GetStatus() == BANANA && !bananaFirstTouch)
					{
						
						for (int i = 0; i < 3; i++)
						{
							bananaFirstTouch = true;
							CSprite* newProjectile = new CSprite();
							newProjectile = bananaSprite->Clone();
							newProjectile->SetOmega(250);
							newProjectile->SetDirection(shot->GetDirection() + (rand() % 360));
							newProjectile->SetSpeed(shot->GetSpeed());
							newProjectile->SetStatus(BANANA);
							newProjectile->SetPosition(shot->GetPosition());

							newProjectile->ResetTime(localTime);

							EnemyShotList.push_back(newProjectile);
						}
						shot->Delete();
						
					}
					break;
				}
			}
			continue;

		}

		


		//MAP HIT TEST
		for (CSprite* mapPart : localMap->mapList)
		{

			if (shot->HitTest(mapPart))
			{
				//IF DINAMIT
				if (shot->GetStatus() == DYNAMIT || shot->GetStatus() == MINE)
				{
					shot->SetSpeed(0);
					continue;
				}

				//ELSE
				exploditionSetup(mapPart->GetX(), mapPart->GetY());
				shot->Delete();
				continue;
			}
		}

 
 
		
	}
	 
}

void EntityWeapon::exploditionSetup(float posX, float posY)
{
 
	//animation

	CSprite* explosion = new CSprite();
	explosion = explosionSprite->Clone();
	explosion->SetPosition(posX, posY);
	explosion->ResetTime(localTime);
	explosion->Die(750);
	explosionAnimationList.push_back(explosion);

	//Explosion Damage Image
	explosionDamageImg.SetPos(posX, posY);
	explosionDamageActive = true;
}

void EntityWeapon::DamageApply()
{
	if (explosionDamageActive)
	{
		for (CSprite* mapPart : localMap->mapList)
		{
		
			if (explosionDamageImg.HitTest(mapPart)) mapPart->Delete();
		}

		for (auto playerEntity : localAllPlayerEntityes)
			if (playerEntity->enemySprite->HitTest(&explosionDamageImg)) playerEntity->EnemyGettingDamage(damageBasedOnWeaponType[localWeaponType]);

		explosionDamageActive = false;
		
		Explodition.Play("Explosion.wav");
		Explodition.Volume(50);

	}
}

void EntityWeapon::AmmoSpriteInit()
{
	missileSprite = new CSprite();
	missileSprite->AddImage("ammo.png", "missileImg", 8, 1, 1, 0, 1, 0, CColor::Black());
	missileSprite->SetAnimation("missileImg", 1);
	 
	bombSprite = new CSprite();
	bombSprite->AddImage("ammo.png", "BOMB", 8, 1, 4, 0, 4, 0 );
	bombSprite->SetAnimation("BOMB", 1);

	bananaSprite = new CSprite();
	bananaSprite->AddImage("ammo.png", "BANANA", 8, 1, 3, 0, 3, 0 );
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


void EntityWeapon::OnDraw(CGraphics* g)
{
	if (IsInAttackChargeMode)
	{
		if(!LoadingTick.IsPlaying()) LoadingTick.Play("chargeAttack.wav");
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

	if (localWeaponType == MAIL) mailAttackMousePointer.Draw(g);

	//Enemy shots
	for (CSprite* shot : EnemyShotList) shot->Draw(g);
	for (CSprite* explosion : explosionAnimationList) explosion->Draw(g);
	
}


void EntityWeapon::meleeAttack()
{
	 
	if(localEnemyDirection == 90) localEnemySprite->SetStatus(STANDRIGHT);
	else localEnemySprite->SetStatus(STANDLEFT);
	for (auto playerEntity : localAllPlayerEntityes)
	{
		if (playerEntity->enemySprite->HitTest(localEnemySprite) && playerEntity->isPlayerTurn == false)
		{
			playerEntity->EnemyGettingDamage(damageBasedOnWeaponType[AXE]);
		}
	}

	

}

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
	AttackSoundPlay();

	if (localWeaponType != AXE )
	{
		if ( sym == SDLK_SPACE && IsInAttackChargeMode)
		{			
			LoadingTick.Stop();
			EntityWeapon::attackHandler( chargeAttackTimer, maxChargeAttackTime, chargedAttackBar);
		}
	}
}

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


void EntityWeapon::OnMouseMove(Uint16 x, Uint16 y)
{
	if (localWeaponType == MAIL) mailAttackMousePointer.SetPos(x, y);

}