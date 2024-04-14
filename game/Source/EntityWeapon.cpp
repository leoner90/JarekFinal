#include "stdafx.h"
#include "../headers/PlayerEntity/EntityWeapon.h"
#include "../headers/Map.h"

void EntityWeapon::performShot(CSprite* enemySprite, int weaponType, bool& IsInAttackChargeMode, float& chargeAttackTimer, float CurrentTime,float& maxChargeAttackTime, CSprite& chargedAttackBar,
	float EnemyDirection, float aimAngle, CSpriteList* EnemyShotList, bool* finishTurn)
{
	//calculate projectile speed
	float fullyChargedAttack = 0;
	ProjectileSpeed = 0;
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
	EnemyShotList->push_back(newProjectile);
	*finishTurn = true;
	
}


//**** Shots Handler for Update function
void EntityWeapon::shotsHandler(CSpriteList& EnemyShotList, MapGen& localMapVar, CSpriteList& explosionList, float CurrentTime)
{
	CVector gravity = { 0,-10 };

	for (CSprite* shot : EnemyShotList)
	{
		shot->Accelerate(gravity);
		if (shot->GetYVelocity() < 0) 	shot->Rotate(3);
		
		for (CSprite* mapPart : localMapVar.mapList)
		{
			if (shot->HitTest(mapPart))
			{

				mapPart->Delete();
				shot->Delete();

				CSprite* explosion = new CSprite(mapPart->GetX(), mapPart->GetY() + 20, 0, 0, CurrentTime);
				explosion->AddImage("explosion.bmp", "explosion", 5, 5, 0, 0, 4, 4, CColor::Black());
				explosion->SetAnimation("explosion", 25);
				explosion->Die(1000);
				explosionList.push_back(explosion);
			}
		}
		shot->Update(CurrentTime);
	}
}