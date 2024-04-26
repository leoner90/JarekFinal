#pragma once

class Physic
{
public:
	//HIT TEST AND REFLECTION
	bool hitTestHandler(CSprite& mainObj, CSprite& obstacle, float restitution, bool isReflectNeeded, float dt);

	//HOOKS ROTATIONS
	void UpdateSpriteRotationHookesLaw(CSprite& objForRotation, float ANGLE, float SPRING);
private:
	// Rotation Variables
	float angle = 0.0f;             
	float velocity = 0.0f;          
};