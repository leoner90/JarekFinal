#pragma once
 
class MapGen 
{
public:
	//Map Scrolling
	int scrollOffset;
	float leftScreenLimit, rightScreenLimit;

	//Base Functions
	void init(float screenHeight, int screenWidth);
	void OnUpdate(long t, float screenHeight, float windStrength);
	void OnDraw(CGraphics* g);
	void addGrave(float xGravePos, float yGravePos);
	void mapCreationInit();
	void WeatherInit();
	void SpritesInit();

	void LootUpdate();
	void WeatherUpdate();
	void GravesUpadte();
	CSpriteList mapList;
	void setCameraToCurrentPlayer(float curentPlayerPos);
	float offset;
	void addLootOnTheMap();
	CSpriteList lootList;
private:
	//localVar 
	float localScreenHeight;
	float localScreenWidth;
	float localPlayerXpos;
	float localWindStrength;

	CSprite gameBg;
	bool turnChanged;

	float lootTimer;
	
	CSprite* healthBox;
	float localTime;

	CSpriteList weatherParticleList;

	CSpriteList graveList;
	CSprite* gravePrefab;

	CSoundPlayer dropSound;
};
