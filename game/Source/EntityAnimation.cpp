#include "stdafx.h"
#include "../headers/PlayerEntity/EntityAnimation.h"


//************ ANIMATION -> Restore Basic Animation
void EntityAnimation::EnemyAnimation(int old_animation_status, CSprite* enemySprite, bool weaponSelected, int weaponType)
{
	if (enemySprite->GetStatus() != old_animation_status)
	{
		if (enemySprite->GetStatus() == WALKLEFT) { enemySprite->SetAnimation("walkLeft", 2); }
		if (enemySprite->GetStatus() == WALKRIGHT) { enemySprite->SetAnimation("walkRight", 2); }

		if (weaponSelected)
		{
			switch (weaponType)
			{
			case AXE:
				if (enemySprite->GetStatus() == STANDRIGHT) { enemySprite->SetAnimation("MeleeAxeRightStand", 1); }
				if (enemySprite->GetStatus() == STANDLEFT) { enemySprite->SetAnimation("MeleeAxeLeftStand", 1); }
				break;
			case MISSILE:
				if (enemySprite->GetStatus() == STANDRIGHT) { enemySprite->SetAnimation("missileRight", 1); }
				if (enemySprite->GetStatus() == STANDLEFT) { enemySprite->SetAnimation("missileLeft", 1); }
				break;
			case BOMB:
				if (enemySprite->GetStatus() == STANDRIGHT) { enemySprite->SetAnimation("bombRightStand", 1); }
				if (enemySprite->GetStatus() == STANDLEFT) { enemySprite->SetAnimation("bombLeftStand", 1); }
				break;
			case BANANA:
				if (enemySprite->GetStatus() == STANDRIGHT) { enemySprite->SetAnimation("bananaRightStand", 1); }
				if (enemySprite->GetStatus() == STANDLEFT) { enemySprite->SetAnimation("bananaLeftStand", 1); }
				break;
			case DYNAMIT:
				if (enemySprite->GetStatus() == STANDRIGHT) { enemySprite->SetAnimation("TNTRight", 1); }
				if (enemySprite->GetStatus() == STANDLEFT) { enemySprite->SetAnimation("TNTLeft", 1); }
				break;
			case MINE:
				if (enemySprite->GetStatus() == STANDRIGHT) { enemySprite->SetAnimation("MineRight", 1); }
				if (enemySprite->GetStatus() == STANDLEFT) { enemySprite->SetAnimation("MineLeft", 1); }
				break;
			case UZI:
				if (enemySprite->GetStatus() == STANDRIGHT) { enemySprite->SetAnimation("uziRight", 1); }
				if (enemySprite->GetStatus() == STANDLEFT) { enemySprite->SetAnimation("uziLeft", 1); }
				break;
			case BOW:
				if (enemySprite->GetStatus() == STANDRIGHT) { enemySprite->SetAnimation("BowRight", 1); }
				if (enemySprite->GetStatus() == STANDLEFT) { enemySprite->SetAnimation("BowLeft", 1); }
				break;
			case MAIL:
				if (enemySprite->GetStatus() == STANDRIGHT) { enemySprite->SetAnimation("MailBombRight", 1); }
				if (enemySprite->GetStatus() == STANDLEFT) { enemySprite->SetAnimation("MailBombLeft", 1); }
				break;
			case SKIP:
				if (enemySprite->GetStatus() == STANDRIGHT) { enemySprite->SetAnimation("SkipTurnRight", 1); }
				if (enemySprite->GetStatus() == STANDLEFT) { enemySprite->SetAnimation("SkipTurnLeft", 1); }
				break;
			default:
				break;
			}
		}

		else
		{
			if (enemySprite->GetStatus() == STANDRIGHT) { enemySprite->SetAnimation("idleRight", 1); }
			if (enemySprite->GetStatus() == STANDLEFT) { enemySprite->SetAnimation("idleLeft", 1); }
		}

	}
}



//**** INITIAL SPRITES DEPENDING ON TYPE OF ENEMY
void EntityAnimation::initAnimations(CSprite* enemySprite)
{

	//iddle
	enemySprite->AddImage("main.png", "idleLeft", 28, 4, 7, 3, 10, 3, CColor::Black());
	enemySprite->AddImage("main.png", "idleRight", 28, 4, 11, 3, 14, 3, CColor::Black());

	//walk
	enemySprite->AddImage("main.png", "walkLeft", 28, 4, 0, 3, 3, 3, CColor::Black());
	enemySprite->AddImage("main.png", "walkRight", 28, 4, 18, 3, 21, 3, CColor::Black());

	//jump
	enemySprite->AddImage("main.png", "jumpLeft", 28, 4, 4, 3, 4, 3, CColor::Black());
	enemySprite->AddImage("main.png", "jumpRight", 28, 4, 17, 3, 17, 3, CColor::Black());

	//fall
	enemySprite->AddImage("main.png", "fallLeft", 28, 4, 5, 3, 7, 3, CColor::Black());
	enemySprite->AddImage("main.png", "fallRight", 28, 4, 15, 3, 16, 3, CColor::Black());

	//victory
	enemySprite->AddImage("main.png", "victory", 28, 4, 22, 3, 23, 3, CColor::Black());


	//**ATTACK ANIMATIONS

	//missile
	enemySprite->AddImage("main.png", "missileLeft", 28, 4, 1, 2, 1, 2, CColor::Black());
	enemySprite->AddImage("main.png", "missileRight", 28, 4, 26, 1, 26, 1, CColor::Black());

	//banana
	enemySprite->AddImage("main.png", "bananaLeftStand", 28, 4, 8, 2, 8, 2, CColor::Black());
	enemySprite->AddImage("main.png", "bananaLeft", 28, 4, 8, 2, 11, 2, CColor::Black());
	enemySprite->AddImage("main.png", "bananaRightStand", 28, 4, 19, 1, 19, 1, CColor::Black());
	enemySprite->AddImage("main.png", "bananaRight", 28, 4, 19, 1, 16, 1, CColor::Black());

	//bomb
	enemySprite->AddImage("main.png", "bombLeftStand", 28, 4, 12, 2, 12, 2, CColor::Black());
	enemySprite->AddImage("main.png", "bombLeft", 28, 4, 12, 2, 16, 2, CColor::Black());
	enemySprite->AddImage("main.png", "bombRightStand", 28, 4, 15, 1, 15, 1, CColor::Black());
	enemySprite->AddImage("main.png", "bombRight", 28, 4, 15, 1, 11, 1, CColor::Black());

	//uzi
	enemySprite->AddImage("main.png", "uziLeft", 28, 4, 19, 2, 19, 2, CColor::Black());
	enemySprite->AddImage("main.png", "uziRight", 28, 4, 8, 1, 8, 1, CColor::Black());

	//Bow
	enemySprite->AddImage("main.png", "BowLeft", 28, 4, 22, 2, 22, 2, CColor::Black());
	enemySprite->AddImage("main.png", "BowRight", 28, 4, 5, 1, 5, 1, CColor::Black());

	//Mail Bomb
	enemySprite->AddImage("main.png", "MailBombLeft", 28, 4, 26, 2, 27, 2, CColor::Black());
	enemySprite->AddImage("main.png", "MailBombRight", 28, 4, 1, 1, 0, 1, CColor::Black());

	//Mine
	enemySprite->AddImage("main.png", "MineLeft", 28, 4, 5, 0, 5, 0, CColor::Black());
	enemySprite->AddImage("main.png", "MineRight", 28, 4, 6, 0, 6, 0, CColor::Black());

	//TNT
	enemySprite->AddImage("main.png", "TNTLeft", 28, 4, 4, 0, 4, 0, CColor::Black());
	enemySprite->AddImage("main.png", "TNTRight", 28, 4, 7, 0, 7, 0, CColor::Black());

	//Melee Axe
	enemySprite->AddImage("melleeAttack.png", "MeleeAxeLeftStand", 4, 2, 0, 1, 0, 1, CColor::Black());
	enemySprite->AddImage("melleeAttack.png", "MeleeAxeLeft", 4, 2, 3, 0, 0, 0, CColor::Black());
	enemySprite->AddImage("melleeAttack.png", "MeleeAxeRightStand", 4, 2, 3, 0, 3, 0, CColor::Black());
	enemySprite->AddImage("melleeAttack.png", "MeleeAxeRight", 4, 2, 3, 0, 0, 0, CColor::Black());


	//in Damage
	enemySprite->AddImage("main.png", "inDamage", 28, 6, 18, 6, 21, 6, CColor::Black());

	//Dead
	enemySprite->AddImage("main.png", "Dead", 28, 4, 18, 6, 21, 6, CColor::Black());

	//Skip turn
	enemySprite->AddImage("main.png", "SkipTurnLeft", 28, 4, 3, 0, 3, 0, CColor::Black());
	enemySprite->AddImage("main.png", "SkipTurnRight", 28, 4, 8, 6, 8, 0, CColor::Black());


}


//*** RESTORE DEFAULT ANIMATION BASE ON DIRECTION
void EntityAnimation::setDefaultStandAnimation(int EnemyDirection, CSprite* enemySprite)
{
	if (EnemyDirection == 90) 	enemySprite->SetStatus(STANDRIGHT);
	if (EnemyDirection == -90) enemySprite->SetStatus(STANDLEFT);
	enemySprite->SetVelocity(0, 0);
}
