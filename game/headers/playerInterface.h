#pragma once
#include "headers/Player.h"

class PlayerInterface
{
public:
	//Base Methods
	void init(int screenWidth, int screenHeight);
	void OnUpdate(Player& player);
	void OnDraw(Player& player, CGraphics* g, int screenWidth, int screenHeight);

private:
	//variable miroring
	float localScreenHeight, localScreenWidth;

	//Interface Sprites
	CVector fullHpPos, fullMpPos, fullEnergyPos, kunaiPos;
	CSprite* controllPanel;
	CSprite* fullHpPoution, *fullMpPoution, *fullEnergyPoution, *emptyHpPoution, *emptyMpPoution, *emptyEnergyPoution, *kunai;
	CSprite* energyBar;
	void SpriteInits();

	//Rectangle Update
	void playerEnergyBarUpdate(CSprite& ddEnergyBarRect, float& CurrentEnergy, float maxEnergy, float InitRectSize, float CustomeOffset, float RegenRate, float initWidth);
};