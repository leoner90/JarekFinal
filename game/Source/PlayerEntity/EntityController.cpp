#include "stdafx.h"
#include "../headers/PlayerEntity/EntityController.h"
#include "../headers/Map.h"


//**************************** CONTROLLER ****************************
void EntityController::playerControler(bool InChargeMode, int& playerState, CSprite* entitySprite, bool isPlayerTurn, bool isInTurnTransitStatus, bool isInventoryOpen , float& EnemyDirection, float&aimAngle, bool movement[])
{
	//make more natural physic when not in jump  and drop from the edge of a "clif"
	if(entitySprite->GetStatus() != JUMP) entitySprite->SetXVelocity(entitySprite->GetXVelocity() * 0.9);
 
	if (isPlayerTurn && !isInTurnTransitStatus && entitySprite->GetStatus() != INATTACK && !isInventoryOpen && playerState != INAIR && !InChargeMode)
	{
		//Move Left
		if (movement[Dkey])
		{
			if (!footsteps.IsPlaying())
			{
				footsteps.Play("footsteps.wav", 1);
				footsteps.Volume(0.35);
			}
			entitySprite->SetXVelocity(130);
			entitySprite->SetStatus(WALKRIGHT);
			EnemyDirection = 90;
		}

		//Move Right
		else if (movement[Akey])
		{
			if (!footsteps.IsPlaying())
			{
				footsteps.Play("footsteps.wav", 1);
				footsteps.Volume(0.35);
			}
			entitySprite->SetXVelocity(-130);
			entitySprite->SetSize(250, 200);
			entitySprite->SetStatus(WALKLEFT);
			EnemyDirection = -90;
		}

		//Aim UP
		else if (movement[Wkey])
		{
			entitySprite->SetXVelocity(0);
			if (aimAngle < 50) aimAngle++;
		}

		//Aim Down
		else if (movement[Skey])
		{
			entitySprite->SetXVelocity(0);
			if (aimAngle > -50) aimAngle--;
		}

		//RESETS => Speed = 0 & sets Default animation if not in Attack & not in the Air
		else if(entitySprite->GetStatus() != INATTACK  )
		{
			footsteps.Stop();
			if (EnemyDirection == 90) 	entitySprite->SetStatus(STANDRIGHT);
			if (EnemyDirection == -90) entitySprite->SetStatus(STANDLEFT);
			entitySprite->SetVelocity(0, 0);
		}
	}

	//Gravity Apply if in the air
	if (playerState == INAIR)
	{
		footsteps.Stop();
		float gravity = -350;
		entitySprite->SetYVelocity(gravity);
	}
}

//**************************** COLLISION ****************************
void EntityController::playerCollision(float t, int& playerState, CSprite* entitySprite, CVector pos, MapGen* localMapVar)
{
	// COLLISION WITH MAP ELEMENTS
	bool touchingPlatform = false;
	for (auto mapPart : localMapVar->mapList)
	{
		if (entitySprite->HitTest(mapPart, 4)) // default
		{
			//GOES UP LOGIC
			if ((mapPart->GetTop() + 30 >= entitySprite->GetTop()))
				entitySprite->SetPosition(pos + CVector(0, 5));

			//IF TOUCHING PLATFORM
			playerState = NONE;
			touchingPlatform = true;
		}
	}

	//IF Not touching platform sets status  INAIR  else sets velocity to 0
	if (!touchingPlatform) playerState = INAIR;
	else entitySprite->SetYVelocity(0);
}