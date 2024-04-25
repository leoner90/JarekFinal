#include "stdafx.h"
#include "../headers/PlayerEntity/EntityPhysic.h"
#include "../headers/Map.h"


//**************************** CONTROLLER ****************************
void EntityPhysic::playerControler(bool InChartgeMode, int& playerState, CSprite* enemySprite,
	bool isPlayerTurn,bool isInTurnTransitStatus, bool isInventoryOpen , bool Dkey, bool Akey, bool Wkey , bool Skey, bool Fkey, float& EnemyDirection, float&aimAngle)
{
	//make more natural physic when not in jump  and drop from the edge of a "clif"
	if(enemySprite->GetStatus() != JUMP) enemySprite->SetXVelocity(enemySprite->GetXVelocity() * 0.9);

	if (isPlayerTurn && !isInTurnTransitStatus && enemySprite->GetStatus() != INATTACK && !isInventoryOpen && playerState != INAIR && !InChartgeMode)
	{
		if (Dkey)
		{
			if (!footsteps.IsPlaying())
			{
				footsteps.Play("footsteps.wav", 1);
				footsteps.Volume(0.35);
			}
			enemySprite->SetXVelocity(130);
			enemySprite->SetStatus(WALKRIGHT);
			EnemyDirection = 90;
		}
		else if (Akey)
		{
			if (!footsteps.IsPlaying())
			{
				footsteps.Play("footsteps.wav", 1);
				footsteps.Volume(0.35);
			}
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
		else if(enemySprite->GetStatus() != INATTACK && playerState != INAIR)
		{
			footsteps.Stop();
			if (EnemyDirection == 90) 	enemySprite->SetStatus(STANDRIGHT);
			if (EnemyDirection == -90) enemySprite->SetStatus(STANDLEFT);
			enemySprite->SetVelocity(0, 0);
		}
	}

	if (playerState == INAIR)
	{
		footsteps.Stop();
		enemySprite->SetYVelocity(-350);
	}

}



//**************************** COLLISION ****************************
void EntityPhysic::playerCollision(float t, int& playerState, CSprite* enemySprite, CVector pos, MapGen* localMapVar)
{
	bool touchingPlatform = false;

	for (auto brik : localMapVar->mapList)
	{
		if (enemySprite->HitTest(brik,8))
		{

			CVector displ = (brik->GetPos() + CVector(0, 30)) - enemySprite->GetPos();

			if ((brik->GetTop() + 30 >= enemySprite->GetTop()))
			{
	 
				enemySprite->SetPosition(pos + CVector(0, 5));
			}

			playerState = NONE;
			touchingPlatform = true;
		}
	}


	if (!touchingPlatform)
	{
		playerState = INAIR;
	}
	else enemySprite->SetYVelocity(0);




}


/*

	for (auto brik : localMapVar->mapList)
	{
		enemySprite->SetPos(enemySprite->GetPos() + CVector(0, +25));
		if (Physic::hitTestHandler(*enemySprite, *brik, false, false, dt))
		{
			cout << 22;
			playerState = NONE;
			break;


		}
		else
		{
			enemySprite->SetYVelocity(-20);
		}


		enemySprite->SetPos(enemySprite->GetPos() + CVector(0, 5));





	}

*/