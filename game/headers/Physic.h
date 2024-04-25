#pragma once

class Physic
{
public:
	bool hitTestHandler(CSprite& mainObj, CSprite& obstacle, float restitution, bool isReflectNeeded, float dt);
	void UpdateSpriteRotationHookesLaw(CSprite& objForRotation, float ANGLE_INCREMENT, float SPRING_CONSTANT);
private:
	// Rotation Variables
	float angle = 0.0f;             
	float velocity = 0.0f;          
};