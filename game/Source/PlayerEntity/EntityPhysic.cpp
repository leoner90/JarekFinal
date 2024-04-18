#include "stdafx.h"
#include "../headers/PlayerEntity/EntityPhysic.h"
#include "../headers/Map.h"


//**************************** CONTROLLER ****************************
void EntityPhysic::playerControler(int& playerState, CSprite* enemySprite,bool isPlayerTurn, bool IsInAttackChargeMode, bool isInventoryOpen , bool Dkey, bool Akey, bool Wkey , bool Skey,float& EnemyDirection, float&aimAngle)
{
	if (isPlayerTurn && !IsInAttackChargeMode && !isInventoryOpen && playerState != INAIR)
	{
		if (Dkey)
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
			enemySprite->SetXVelocity(0);
			if (aimAngle < 50) aimAngle++;
		}

		else if (Skey)
		{
			enemySprite->SetXVelocity(0);
			if (aimAngle > -50) aimAngle--;
		}
		else if(enemySprite->GetStatus() != INATTACK)
		{
			if (EnemyDirection == 90) 	enemySprite->SetStatus(STANDRIGHT);
			if (EnemyDirection == -90) enemySprite->SetStatus(STANDLEFT);
			enemySprite->SetVelocity(0, 0);
		}

	}

	if (playerState == INAIR) enemySprite->SetVelocity(enemySprite->GetXVelocity(), -350);

}



//**************************** COLLISION ****************************
void EntityPhysic::playerCollision(int& playerState, CSprite* enemySprite, CVector pos, MapGen* localMapVar)
{
	bool touchingPlatform = false;

	for (auto brik : localMapVar->mapList)
	{
		if (enemySprite->HitTest(brik))
		{

			CVector displ = (brik->GetPos() + CVector(0, 30)) - enemySprite->GetPos();

			if ((brik->GetTop() + 30 >= enemySprite->GetTop()))
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
	else enemySprite->SetYVelocity(0);
}