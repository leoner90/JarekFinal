#pragma once


class EntityAnimation
{
public:
	enum animation_state { NONE, STANDLEFT, STANDRIGHT, WALKLEFT, WALKRIGHT, INAIR, FALLING, SLIDE, INATTACK, ONHIT, DEAD };
	enum weaponTypes { AXE, MISSILE, BOMB, BANANA, DYNAMIT, MINE, UZI, BOW, MAIL, SKIP };
	void EnemyAnimation(int old_animation_status, CSprite* enemySprite, bool weaponSelected, int weaponType, int EnemyDirection);
	void initAnimations(CSprite* enemySprite);
	void setDefaultStandAnimation(int EnemyDirection, CSprite* enemySprite);

private:

};
 