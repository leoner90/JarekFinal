#pragma once


class EntityAnimation
{
public:
	enum animation_state { NONE, STANDLEFT, STANDRIGHT, WALKLEFT, WALKRIGHT, INAIR, JUMP, SLIDE, INATTACK, ONHIT, DEAD, WINERTEAM};
	enum weaponTypes { AXE, MISSILE, BOMB, BANANA, DYNAMIT, MINE, UZI, BOW, MAIL, SKIP };
	void EnemyAnimation(int old_animation_status, CSprite* enemySprite, bool weaponSelected, int weaponType, int EnemyDirection, float angleOfAttack);
	void initAnimations(CSprite* enemySprite);
 

private:
	float oldAimAngle;
};
 