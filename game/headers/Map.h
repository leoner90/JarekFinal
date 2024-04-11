#pragma once
 
class MapGen 
{
public:
	//Map Scrolling
	int scrollOffset;
	float leftScreenLimit, rightScreenLimit;

	//Base Functions
	void init(float screenHeight, int screenWidth);
	void OnUpdate(long t, float screenHeight);
	void OnDraw(CGraphics* g);
	CSpriteList mapList;
	void setCameraToCurrentPlayer(float curentPlayerPos);
	float offset;

private:
	//localVar 
	float localScreenHeight;
	float localPlayerXpos;
	CSprite gameBg;
	bool turnChanged;

 
};
