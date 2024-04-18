#include "stdafx.h"
#include "../headers/PlayerEntity/EntityWeapon.h"
#include "../headers/PlayerEntity/PlayerEntity.h"
#include "../headers/Map.h"

//Rotated Obj Hit Test
void EntityWeapon::hitTestHandler(CSprite& mainObj, CSprite& obstacle, float restitution)
{

	//Uint32 dt = GetDeltaTime();
	float timeFrame = (float)dt / 1000.0f; // time between last frame in sec
	CVector v = mainObj.GetVelocity() * timeFrame;	//next frame velocity?

	//All possible Normals
	float alpha = DEG2RAD(obstacle.GetRotation()); //in our case it's allways will be 0

	CVector normals[4] =
	{
		{  sin(alpha), cos(alpha) }, // top normal
		{ -sin(alpha), -cos(alpha) }, // bottom normal
		{ -cos(alpha), sin(alpha) }, //left normal
		{ cos(alpha), -sin(alpha) } // right normal
	};

	float R = mainObj.GetHeight() / 2; // radius of the ball
	float X = obstacle.GetWidth() / 2; // half of the width of the box
	float Y = obstacle.GetHeight() / 2; //  half the height of the box
	CVector t = obstacle.GetPos() - mainObj.GetPos(); // displacment vector
	CVector n = CVector(sin(alpha), cos(alpha));


	for (int i = 0; i < 2; i++)
	{
		n = normals[i];
		if (Dot(v, n) < 0)
		{
			float vy = Dot(v, n);
			CVector d = t + (Y + R) * n;
			float dy = Dot(d, n);
			float f1 = dy / vy;
			float vx = Cross(v, n);
			float tx = Cross(t, n);
			float f2 = (tx - vx * f1) / (X + R);

			if (f1 >= 0 && f1 <= 1 && f2 >= -1 && f2 <= 1)
				mainObj.SetVelocity(Reflect(mainObj.GetVelocity(), n) * restitution);
		}
	}


	for (int i = 2; i < 4; i++)
	{
		n = normals[i];
		if (Dot(v, n) < 0)
		{
			float vy = Dot(v, n);
			CVector d = t + (X + R) * n;
			float dy = Dot(d, n);

			float vx = Cross(v, n);
			float tx = Cross(t, n);

			float f1 = dy / vy;
			float f2 = (tx - vx * f1) / (Y + R);

			if (f1 >= 0 && f1 <= 1 && f2 >= -1 && f2 <= 1)
				mainObj.SetVelocity(Reflect(mainObj.GetVelocity(), n) * restitution);
		}
	}
}



void EntityWeapon::init()
{
	//reset shots and hit effects
	EnemyShotList.delete_all();

	explosionSprite.LoadImage("firebolt2.png");
	explosionSprite.SetImage("firebolt2.png");
	explosionSprite.SetSize(30, 30);
	isInAttackDelay = false;
	explosionTimer = 0;
	lastFrametime = 0;
}

void EntityWeapon::OnUpdate(UINT32 time, std::vector<PlayerEntity*> PlayerEntitys, CSprite* enemySprite, int weaponType, MapGen& Map)
{
	if (lastFrametime == 0) lastFrametime = time;
	else dt = time - lastFrametime;

	localEnemySprite = enemySprite;
	localAllPlayerEntityes = PlayerEntitys;
	localTime = time;
	localMap = &Map;
	localWeaponType = weaponType;

	if (enemySprite->GetStatus() == INATTACK && enemySprite->GetCurrentAnimationFrame() == 2 && weaponType == AXE) meleeAttack();
	//else if (enemySprite->GetStatus() == INATTACK && enemySprite->GetCurrentAnimationFrame() == 2 && weaponType == MAIL && !mailSelection) mailAttack();
	else shotsHandler();

	lastFrametime = time;
}

void EntityWeapon::attackHandler(bool& IsInAttackChargeMode, float& chargeAttackTimer,float& maxChargeAttackTime, CSprite& chargedAttackBar, float EnemyDirection, float aimAngle, bool* finishTurn)
{
	//calculate projectile speed
	float fullyChargedAttack = 0;
	ProjectileSpeed = 0;
	CSprite* newProjectile = new CSprite(localEnemySprite->GetX(), localEnemySprite->GetY() + 20, 0, 0, localTime);
	explosionTimer = 5000 + localTime;
	switch (localWeaponType)
	{
	case AXE:
		fullyChargedAttack = 300;
		newProjectile->AddImage("ammo.png", "AXE", 8, 1, 0, 0, 0, 0, CColor::Black());
		newProjectile->SetAnimation("AXE", 1);
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
		bananaFirstTouch = false;
		break;

	case DYNAMIT:
		fullyChargedAttack = 300;
		newProjectile->AddImage("ammo.png", "DYNAMIT", 8, 1, 0, 0, 0, 0, CColor::Black());
		newProjectile->SetAnimation("DYNAMIT", 1);
		newProjectile->SetRotation(-180);
		explosionTimer = 2000 + localTime;
	
		break;

	case MINE:
		fullyChargedAttack = 300;
		newProjectile->AddImage("ammo.png", "MINE", 8, 1, 2, 0, 2, 0, CColor::Black());
		newProjectile->SetAnimation("MINE", 1);
		explosionTimer = 2000 + localTime;
		newProjectile->SetRotation(-180);
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
		mailSelection = false;
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

	if (EnemyDirection == -90) newProjectile->SetDirection(EnemyDirection + aimAngle);
	else newProjectile->SetDirection(EnemyDirection - aimAngle);
	
	
	newProjectile->SetSpeed(ProjectileSpeed);
	newProjectile->SetStatus(localWeaponType);
	

	EnemyShotList.push_back(newProjectile);
	*finishTurn = true;
	
}


//**** Shots Handler for Update function
void EntityWeapon::shotsHandler()
{
 

	CVector gravity = { 0,-10 };

	for (CSprite* shot : EnemyShotList)
	{
		if (shot->GetStatus() != BOW && shot->GetStatus() != UZI) shot->Accelerate(gravity);

		if (shot->GetStatus() == BOW && shot->GetStatus() == UZI) shot->Accelerate({ 0,-5 });
		shot->SetRotation(shot->GetDirection());

		if (shot->GetStatus() == DYNAMIT || shot->GetStatus() == MINE) shot->SetRotation(0);
	 
			


		if(explosionTimer < localTime && (shot->GetStatus() == DYNAMIT || shot->GetStatus() == MINE) )
		{
			shot->Delete();
			CSprite* explosion = new CSprite(shot->GetX(), shot->GetY(), 0, 0, localTime);
			explosion->AddImage("explosion.bmp", "explosion", 5, 5, 0, 0, 4, 4, CColor::Black());
			explosion->SetAnimation("explosion", 25);
			explosion->SetStatus(0);
			explosion->Die(1000);
			explosionList.push_back(explosion);
		}



		for (CSprite* mapPart : localMap->mapList)
		{
			
			


			if (shot->HitTest(mapPart))
			{
				if (shot->GetStatus() == BANANA && explosionTimer > localTime)
				{


					if (!bananaFirstTouch)
					{
						CSprite* newProjectile = new CSprite(localEnemySprite->GetX(), localEnemySprite->GetY() + 20, 0, 0, localTime);
						newProjectile->AddImage("ammo.png", "BANANA", 8, 1, 3, 0, 3, 0);
						newProjectile->SetAnimation("BANANA", 1);
						newProjectile->SetVelocity(shot->GetVelocity());
						newProjectile->SetOmega(250);
						hitTestHandler(*newProjectile, *mapPart, 0.5);

						EnemyShotList.push_back(newProjectile);
						bananaFirstTouch = true;
					}
					hitTestHandler(*shot, *mapPart, 0.5);
					continue;
				}


				if (shot->GetStatus() == DYNAMIT || shot->GetStatus() == MINE)
				{
					shot->SetSpeed(0);
				}
				 
				else
				{
					shot->Delete();
					CSprite* explosion = new CSprite(shot->GetX(), shot->GetY(), 0, 0, localTime);
					explosion->AddImage("explosion.bmp", "explosion", 5, 5, 0, 0, 4, 4, CColor::Black());
					explosion->SetAnimation("explosion", 25);
					explosion->SetStatus(0);
					explosion->Die(1000);
					explosionList.push_back(explosion);
				}
			}
		}

		shot->Update(localTime);
	}

 

	for (auto explosion : explosionList)
	{
					/*
			for (auto playerEntity : localAllPlayerEntityes)
			{
				if (playerEntity->enemySprite->HitTest(shot))
				{
					playerEntity->EnemyGettingDamage(22);
				}
			}
			*/


		if (explosion->GetStatus() == 1) continue;
		for (CSprite* mapPart : localMap->mapList)
		{

			if (explosion->HitTest(mapPart))
			{
				mapPart->Delete();
				//xplosion->Delete();
				explosion->SetStatus(1);
			}
		}
	}
	
	for (CSprite* explosion : explosionList) explosion->Update(localTime);
	explosionList.delete_if(deleted);
	
	EnemyShotList.delete_if(deleted);
	
}

void EntityWeapon::meleeAttack()
{
	if (isInAttackDelay) return;
	for (auto playerEntity : localAllPlayerEntityes)
	{
		if (playerEntity->enemySprite->HitTest(localEnemySprite) && playerEntity->isPlayerTurn == false)
		{
			playerEntity->EnemyGettingDamage(22);
		}
	}
	isInAttackDelay = true;
}

void EntityWeapon::mailAttack(Uint16 x , Uint16 y)
{
}


void EntityWeapon::OnDraw(CGraphics* g)
{
	//Enemy shots
	for (CSprite* shot : EnemyShotList) shot->Draw(g);
	for (CSprite* explosion : explosionList) explosion->Draw(g);
}


 
