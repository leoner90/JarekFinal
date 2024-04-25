#pragma once
#include "../Physic.h"

//Forward declaration
class MapGen;

class EntityPhysic : public Physic
{
public:
	void playerControler(bool InChartgeMode, int& playerState, CSprite* enemySprite, bool isPlayerTurn, bool isInTurnTransitStatus, bool isInventoryOpen, bool Dkey, bool Akey, bool Wkey, bool Skey, bool Fkey, float& EnemyDirection, float& aimAngle);
 
	void playerCollision(float t, int& playerState, CSprite* enemySprite, CVector pos, MapGen* localMapVar);
 
	CSoundPlayer footsteps;
private:
	enum animation_state { NONE, STANDLEFT, STANDRIGHT, WALKLEFT, WALKRIGHT, INAIR, JUMP, SLIDE, INATTACK, ONHIT, DEAD};
	

	float  dt;
	float lastFrametime = 0;
	 
};
