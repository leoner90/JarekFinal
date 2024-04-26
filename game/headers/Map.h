#pragma once
 
class MapGen 
{
public:
	MapGen();

	//Base Functions
	void init(float screenHeight, int screenWidth);
	void OnUpdate(long t, float screenHeight, float windStrength);
	void OnDraw(CGraphics* g);
	
	//Adds Grave and Explosion when Entity dies
	void addGrave(float xGravePos, float yGravePos);

	//GETTERS & SETTER NEEDED IF WILL HAVE TIME
	CSpriteList mapList;
	CSpriteList lootList;

private:
	//localVar just miroring to be able use in whole class
	float localScreenHeight;
	float localScreenWidth;
	float localPlayerXpos;
	float localWindStrength;
	float localTime;

	//INIT
	void mapCreationInit();
	void WeatherInit();
	void SpritesInit();

	//UPDATE
	void LootUpdate();
	void WeatherUpdate();
	void GravesUpadte();

	//ADD LOOT (AirDrop Helath Boxes)
	void addLootOnTheMap();
	CSoundPlayer dropSound;
	CSprite* healthBoxPreFab;
	float lootTimer;

	//Main Game Bg Img
	CSprite gameBg;
	
	//Weather List
	CSpriteList weatherParticleList;

	//Graves
	CSpriteList graveList;
	CSprite* gravePrefab;

	//Explosion - probably more logical is to put it into player entity, but I delete obj before explodition , and its easier this way :)
	CSprite* deathExplosionSprite;
	CSpriteList deathexplosionAnimationList;
};