#include "stdafx.h"
#include "../headers/Physic.h"

////HIT TEST AND REFLECTION INCLUDING RORATED OBJECTS
bool Physic::hitTestHandler(CSprite& mainObj, CSprite& obstacle, float restitution, bool isReflectNeeded, float dt)
{
	float timeFrame = (float)dt / 1000.0f; // time between last frame in sec
	CVector v = mainObj.GetVelocity() * timeFrame;	//next frame velocity?

	//All possible Normals
	float alpha = DEG2RAD(obstacle.GetRotation()); //in our case it's allways will be 0

	//ALL NORMALS
	CVector normals[4] =
	{
		{  sin(alpha), cos(alpha) }, // top normal
		{ -sin(alpha), -cos(alpha) }, // bottom normal
		{ -cos(alpha), sin(alpha) }, //left normal
		{ cos(alpha), -sin(alpha) } // right normal
	};

	float R = mainObj.GetWidth() / 2 - 10; //  width , i know , i know it not a radius after all
	float X = obstacle.GetWidth() / 2; // half of the width of the box
	float Y = obstacle.GetHeight() / 2; //  half the height of the box
	CVector t = obstacle.GetPos() - mainObj.GetPos(); // displacment vector
	CVector n = CVector(sin(alpha), cos(alpha));

	//TOP & BOTTOM CHECK
	for (int i = 0; i < 2; i++)
	{
		n = normals[i];
		if (Dot(v, n) < 0)
		{
			float vy = Dot(v, n);
			CVector d = t + (Y + R) * n;
			float dy = Dot(d, n);
			float f1 = dy / vy;
			float vx = Cross(v, n);
			float tx = Cross(t, n);
			float f2 = (tx - vx * f1) / (X + R);

			if (f1 >= 0 && f1 <= 1 && f2 >= -1 && f2 <= 1)
			{
				if(isReflectNeeded) mainObj.SetVelocity(Reflect(mainObj.GetVelocity(), n) * restitution);
				return true;
			}
		}
	}

	//LEFT & RIGHT CHECK
	for (int i = 2; i < 4; i++)
	{
		n = normals[i];
		if (Dot(v, n) < 0)
		{
			float vy = Dot(v, n);
			CVector d = t + (X + R) * n;
			float dy = Dot(d, n);

			float vx = Cross(v, n);
			float tx = Cross(t, n);

			float f1 = dy / vy;
			float f2 = (tx - vx * f1) / (Y + R);

			if (f1 >= 0 && f1 <= 1 && f2 >= -1 && f2 <= 1)
			{
				if (isReflectNeeded) mainObj.SetVelocity(Reflect(mainObj.GetVelocity(), n) * restitution);
				return true;
			}
		}
	}
	return false;
}


// HOOKS ROTATIONS 
void Physic::UpdateSpriteRotationHookesLaw(CSprite& objForRotation, float ANGLE , float SPRING) 
{
	// Increment the angle
	angle += ANGLE;

	// Calculate the spring force
	float force = -SPRING * angle;

	// Update velocity using F = ma (mass is assumed to be 1)
	velocity += force;

	// Apply damping to avoid perpetual motion
	velocity *= 0.95f;

	// Apply rotation
	objForRotation.Rotate(angle + velocity);
}