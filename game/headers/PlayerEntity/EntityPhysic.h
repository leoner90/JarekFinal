#pragma once

//Forward declaration
class MapGen;

class EntityPhysic
{
public:
	void playerControler(int& playerState, CSprite* enemySprite, bool isPlayerTurn, bool IsInAttackChargeMode, bool isInventoryOpen, bool Dkey, bool Akey, bool Wkey, bool Skey, float& EnemyDirection, float& aimAngle);
	void playerCollision(int& playerState, CSprite* enemySprite, CVector pos, MapGen* localMapVar);
 

private:
	enum animation_state { NONE, STANDLEFT, STANDRIGHT, WALKLEFT, WALKRIGHT, INAIR, FALLING, SLIDE, INATTACK, ONHIT, DEAD };
};
