#pragma once
#include "headers/Player.h"
class Player;

class playerStats
{
public:
	//Base functions
	void init(float screenHeight, float screenWidth);
	void OnUpdate(Player& player);
	void OnDraw(CGraphics* g, float screenHeight);
	void OnLButtonUp(Player& player, int& Skillpoint, CVector mouseCoordinats);
	
	//bg for character Stats
	CSprite* charStatsScreen;

private:
	//var miroring
	Player* LocalPlayer;
	//Lvl Up Btn
	CSprite* levelUpBtn;
	void LevelUpBtnLogic(int& stat, float btnY, int& Skillpoint, CVector mouseCoordinats);
	void updatePlayerStats();

	//stats coordinats
	double strCoor, dexCoor, conCoor, intCoor;
	void StatsPosition(CGraphics* g, int& stat, float textY, float btnY, int& Skillpoint, char* statName);
	void drawStatInformation(CGraphics* g);
};