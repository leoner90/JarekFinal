#pragma once

class EntityAnimation
{
public:
	void EntityAnimationUpdate(int old_animation_status, CSprite* 
		, bool weaponSelected, int weaponType, int EnemyDirection, float angleOfAttack);
	void initAnimations(CSprite* entitySprite);
private:
	enum animation_state { NONE, STANDLEFT, STANDRIGHT, WALKLEFT, WALKRIGHT, INAIR, JUMP, SLIDE, INATTACK, ONHIT, DEAD, WINERTEAM };
	enum weaponTypes { AXE, MISSILE, BOMB, BANANA, DYNAMIT, MINE, UZI, BOW, MAIL, SKIP };
};
 