#include "stdafx.h"
#include "../headers/PlayerEntity/EntityAnimation.h"

//************ ANIMATION UPDATE
void EntityAnimation::EntityAnimationUpdate(int old_animation_status, CSprite* entitySprite, bool weaponSelected, int weaponType, int EnemyDirection, float angleOfAttack)
{
	//if status remains the same , do not change animation
	if (entitySprite->GetStatus() != old_animation_status)
	{
		//WALK
		if (entitySprite->GetStatus() == WALKLEFT) { entitySprite->SetAnimation("walkLeft", 11); }
		if (entitySprite->GetStatus() == WALKRIGHT) { entitySprite->SetAnimation("walkRight", 11); }

		//ON HIT
		if(entitySprite->GetStatus() ==  ONHIT)  
		{ 
			if (EnemyDirection == 90)   entitySprite->SetAnimation("inDamageRight", 2);  
			else entitySprite->SetAnimation("inDamageLeft", 2); ;
		}

		//JUMP
		if (entitySprite->GetStatus() == JUMP) 
		{
			if (EnemyDirection == 90)   entitySprite->SetAnimation("InAirRight", 2);
			else entitySprite->SetAnimation("InAirLeft", 2); ;
		}

		//DEAD
		if (entitySprite->GetStatus() == DEAD) {
			if (EnemyDirection == 90)   entitySprite->SetAnimation("deadRight", 1);
			else entitySprite->SetAnimation("deadLeft", 1); ;
		}

		//WINNER TEAM ANIMATION
		if (entitySprite->GetStatus() == WINERTEAM) {
		  entitySprite->SetAnimation("WINERTEAM", 1);
		}

		
		//IF WEAPON SELECTED
		if (weaponSelected)
		{
			switch (weaponType)
			{
				case AXE:
					if (entitySprite->GetStatus() == STANDRIGHT) { entitySprite->SetAnimation("MeleeAxeRightStand", 1); }
					if (entitySprite->GetStatus() == STANDLEFT) { entitySprite->SetAnimation("MeleeAxeLeftStand", 1); }
					if (entitySprite->GetStatus() == INATTACK)
					{
						if (EnemyDirection == 90)  entitySprite->SetAnimation("MeleeAxeLeft", 3); 
						else  entitySprite->SetAnimation("MeleeAxeRight", 3);
					}
					break;
				case MISSILE:
					if (entitySprite->GetStatus() == STANDRIGHT) { entitySprite->SetAnimation("missileRight", 1); }
					if (entitySprite->GetStatus() == STANDLEFT) { entitySprite->SetAnimation("missileLeft", 1); }
					break;
				case BOMB:
					if (entitySprite->GetStatus() == STANDRIGHT) { entitySprite->SetAnimation("bombRightStand", 1); }
					if (entitySprite->GetStatus() == STANDLEFT) { entitySprite->SetAnimation("bombLeftStand", 1); }
					break;
				case BANANA:
					if (entitySprite->GetStatus() == STANDRIGHT) { entitySprite->SetAnimation("bananaRightStand", 1); }
					if (entitySprite->GetStatus() == STANDLEFT) { entitySprite->SetAnimation("bananaLeftStand", 1); }
					break;
				case DYNAMIT:
					if (entitySprite->GetStatus() == STANDRIGHT) { entitySprite->SetAnimation("TNTRight", 1); }
					if (entitySprite->GetStatus() == STANDLEFT) { entitySprite->SetAnimation("TNTLeft", 1); }
					break;
				case MINE:
					if (entitySprite->GetStatus() == STANDRIGHT) { entitySprite->SetAnimation("MineRight", 1); }
					if (entitySprite->GetStatus() == STANDLEFT) { entitySprite->SetAnimation("MineLeft", 1); }
					break;
				case UZI:
					if (entitySprite->GetStatus() == STANDRIGHT) { entitySprite->SetAnimation("uziRight", 1); }
					if (entitySprite->GetStatus() == STANDLEFT) { entitySprite->SetAnimation("uziLeft", 1); }
					break;
				case BOW:
					if (entitySprite->GetStatus() == STANDRIGHT) { entitySprite->SetAnimation("BowRight", 1); }
					if (entitySprite->GetStatus() == STANDLEFT) { entitySprite->SetAnimation("BowLeft", 1); }
					break;
				case MAIL:
					if (entitySprite->GetStatus() == STANDRIGHT) { entitySprite->SetAnimation("MailBombRight", 1); }
					if (entitySprite->GetStatus() == STANDLEFT) { entitySprite->SetAnimation("MailBombLeft", 1); }
					break;
				case SKIP:
					if (entitySprite->GetStatus() == STANDRIGHT) { entitySprite->SetAnimation("SkipTurnRight", 1); }
					if (entitySprite->GetStatus() == STANDLEFT) { entitySprite->SetAnimation("SkipTurnLeft", 1); }
					break;
				default:
					break;
			}
		}
		else
		{
			//IF "NONE" or "STAND" STATUS
			if (entitySprite->GetStatus() == NONE)
			{
				if (EnemyDirection == 90) entitySprite->SetStatus(STANDRIGHT);
				else entitySprite->SetStatus(STANDLEFT);
			}
			if (entitySprite->GetStatus() == STANDRIGHT ) { entitySprite->SetAnimation("idleRight", 1); }
			if (entitySprite->GetStatus() == STANDLEFT) { entitySprite->SetAnimation("idleLeft", 1); }
		}
	}
}



//**** INITIAL SPRITES
void EntityAnimation::initAnimations(CSprite* entitySprite)
{
	//iddle
	entitySprite->AddImage("main.png", "idleLeft", 28, 4, 7, 3, 10, 3, CColor::Black());
	entitySprite->AddImage("main.png", "idleRight", 28, 4, 11, 3, 14, 3, CColor::Black());

	//walk
	entitySprite->AddImage("main.png", "walkLeft", 28, 4, 0, 3, 3, 3, CColor::Black());
	entitySprite->AddImage("main.png", "walkRight", 28, 4, 18, 3, 21, 3, CColor::Black());

	//jump
	entitySprite->AddImage("main.png", "jumpLeft", 28, 4, 4, 3, 4, 3, CColor::Black());
	entitySprite->AddImage("main.png", "jumpRight", 28, 4, 17, 3, 17, 3, CColor::Black());

	//fall
	entitySprite->AddImage("main.png", "fallLeft", 28, 4, 5, 3, 7, 3, CColor::Black());
	entitySprite->AddImage("main.png", "fallRight", 28, 4, 15, 3, 16, 3, CColor::Black());

	//victory
	entitySprite->AddImage("main.png", "victory", 28, 4, 22, 3, 23, 3, CColor::Black());


	//**ATTACK ANIMATIONS

	//missile
	entitySprite->AddImage("main.png", "missileLeft", 28, 4, 1, 2, 1, 2, CColor::Black());
	entitySprite->AddImage("main.png", "missileRight", 28, 4, 26, 1, 26, 1, CColor::Black());

	//banana
	entitySprite->AddImage("main.png", "bananaLeftStand", 28, 4, 8, 2, 8, 2, CColor::Black());
	entitySprite->AddImage("main.png", "bananaLeft", 28, 4, 8, 2, 11, 2, CColor::Black());
	entitySprite->AddImage("main.png", "bananaRightStand", 28, 4, 19, 1, 19, 1, CColor::Black());
	entitySprite->AddImage("main.png", "bananaRight", 28, 4, 19, 1, 16, 1, CColor::Black());

	//bomb
	entitySprite->AddImage("main.png", "bombLeftStand", 28, 4, 12, 2, 12, 2, CColor::Black());
	entitySprite->AddImage("main.png", "bombLeft", 28, 4, 12, 2, 16, 2, CColor::Black());
	entitySprite->AddImage("main.png", "bombRightStand", 28, 4, 15, 1, 15, 1, CColor::Black());
	entitySprite->AddImage("main.png", "bombRight", 28, 4, 15, 1, 11, 1, CColor::Black());

	//uzi
	entitySprite->AddImage("main.png", "uziLeft", 28, 4, 19, 2, 19, 2, CColor::Black());
	entitySprite->AddImage("main.png", "uziRight", 28, 4, 8, 1, 8, 1, CColor::Black());

	//Bow
	entitySprite->AddImage("main.png", "BowLeft", 28, 4, 22, 2, 22, 2, CColor::Black());
	entitySprite->AddImage("main.png", "BowRight", 28, 4, 5, 1, 5, 1, CColor::Black());

	//Mail Bomb
	entitySprite->AddImage("main.png", "MailBombLeft", 28, 4, 26, 2, 27, 2, CColor::Black());
	entitySprite->AddImage("main.png", "MailBombRight", 28, 4, 1, 1, 0, 1, CColor::Black());

	//Mine
	entitySprite->AddImage("main.png", "MineLeft", 28, 4, 5, 0, 5, 0, CColor::Black());
	entitySprite->AddImage("main.png", "MineRight", 28, 4, 6, 0, 6, 0, CColor::Black());

	//TNT
	entitySprite->AddImage("main.png", "TNTLeft", 28, 4, 4, 0, 4, 0, CColor::Black());
	entitySprite->AddImage("main.png", "TNTRight", 28, 4, 7, 0, 7, 0, CColor::Black());

	//Melee Axe
	entitySprite->AddImage("melleeAttack.png", "MeleeAxeLeftStand", 4, 2, 0, 1, 0, 1, CColor::Black());
	entitySprite->AddImage("melleeAttack.png", "MeleeAxeLeft", 4, 2, 3, 0, 0, 0, CColor::Black());
	entitySprite->AddImage("melleeAttack.png", "MeleeAxeRightStand", 4, 2, 3, 0, 3, 0, CColor::Black());
	entitySprite->AddImage("melleeAttack.png", "MeleeAxeRight", 4, 2, 0, 1, 3, 1, CColor::Black());

	//in Damage
	entitySprite->AddImage("main.png", "inDamageLeft", 28, 4, 0, 0, 0, 0, CColor::Black());
	entitySprite->AddImage("main.png", "inDamageRight", 28, 4, 11, 0, 11, 0, CColor::Black());

	//Dead
	entitySprite->AddImage("main.png", "deadLeft", 28, 4, 1, 0, 2, 0, CColor::Black());
	entitySprite->AddImage("main.png", "deadRight", 28, 4, 10, 0, 9, 0, CColor::Black());

	//Skip turn
	entitySprite->AddImage("main.png", "SkipTurnLeft", 28, 4, 3, 0, 3, 0, CColor::Black());
	entitySprite->AddImage("main.png", "SkipTurnRight", 28, 4, 8, 0, 8, 0, CColor::Black());

	//JUMP
	entitySprite->AddImage("main.png", "InAirLeft", 28, 4, 4, 3, 4, 3, CColor::Black());
	entitySprite->AddImage("main.png", "InAirRight", 28, 4, 17, 3, 17, 3, CColor::Black());

	//WINERTEAM
	entitySprite->AddImage("main.png", "WINERTEAM", 28, 4, 22, 3, 23, 3, CColor::Black());
}