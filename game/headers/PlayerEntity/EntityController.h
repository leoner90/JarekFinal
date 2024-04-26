#pragma once
#include "../Physic.h"

//Forward Class Declaration
class MapGen;

class EntityController : public Physic
{
public:
	void playerControler(bool InChargeMode, int& playerState, CSprite* entitySprite, bool isPlayerTurn, bool isInTurnTransitStatus, bool isInventoryOpen, float& EnemyDirection, float& aimAngle, bool movement[]);
	void playerCollision(float t, int& playerState, CSprite* entitySprite, CVector pos, MapGen* localMapVar);
 
	//Getters & Setter needed, Sorry No Time.
	CSoundPlayer footsteps;
private:
	enum animation_state { NONE, STANDLEFT, STANDRIGHT, WALKLEFT, WALKRIGHT, INAIR, JUMP, SLIDE, INATTACK, ONHIT, DEAD};
	enum movements {  Dkey,  Akey,  Wkey,  Skey, Fkey };
	//DeltaTime
	float  dt;
	float lastFrametime = 0;
};
