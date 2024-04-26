#include "stdafx.h"
#include "../headers/Map.h"

MapGen::MapGen()
{
	SpritesInit();
	WeatherInit();
}

//*** INIT
void MapGen::init(float screenHeight, int screenWidth)
{
	//Local variables
	localScreenHeight = screenHeight;
	localScreenWidth = screenWidth;

	//RESETS
	for each (CSprite * pSprite in mapList) delete pSprite;
	for each (CSprite * grave in graveList) delete grave;
	for each (CSprite * loot in lootList) delete loot;

	//RESETS
	mapList.clear();
	graveList.clear();
	lootList.clear();
	lootTimer = 0;

	//MAP, WEATHER, Sprites INITS
	mapCreationInit();  
}


//*** UPDATE
void MapGen::OnUpdate(long t, float screenHeight, float windStrength)
{
	//Local variables
	localTime = t;
	localWindStrength = windStrength;

	//Map Update
	for (auto brig : mapList) brig->Update(t);
	mapList.delete_if(deleted);

	// Loot, Weather, Graves UPDATES
	LootUpdate();
	WeatherUpdate();
	GravesUpadte();

	//Explosion Update And Deletes
	for(auto explosion : deathexplosionAnimationList) explosion->Update(localTime);
	deathexplosionAnimationList.delete_if(deleted);
}


//*** DRAW
void MapGen::OnDraw(CGraphics* g)
{
	gameBg.Draw(g);
	for (auto particle : weatherParticleList) particle->Draw(g);
	for (auto brig : mapList) brig->Draw(g);
	for (auto loot : lootList) loot->Draw(g);
	for (auto grave : graveList) grave->Draw(g);
	for (auto explosion : deathexplosionAnimationList) 	explosion->Draw(g);	
}


//*** MAP CREATION
void MapGen::mapCreationInit()
{
	//MAP Variables
	const int widthOfMap = 1920 / 32; // full hd divided by brig size
	const int numberOfBrigs = 400; // actualy creates less brigs as if max height achived it returns
	int mapCreationList[widthOfMap]; // array of integers size of width, each cell is map hight
	memset(mapCreationList, 0, sizeof(mapCreationList)); // fill array with 0

	//Create A Map
	for (int i = 0; i < numberOfBrigs; i++)
	{
		int random = rand() % (widthOfMap / 4 + 1) + rand() % (widthOfMap / 4 + 1) + rand() % (widthOfMap / 4 + 1) + rand() % (widthOfMap / 4);
		mapCreationList[random]++; // puts + 1 in random array cell where width is max of 60

		// maximum height limit, return if maximum achived
		if (mapCreationList[random] > 15)
		{
			mapCreationList[random] = 15;
			continue;
		}
		//ADD Rect To List
		mapList.push_back(new CSpriteRect(932.f + (random - widthOfMap / 2) * 32 + 16, 180.f + mapCreationList[random] * 32, 32, 32, CColor::DarkYellow(), 0));
	}
}


//*** WEATHER INITS
void MapGen::WeatherInit()
{
	for (int i = 0; i < 65; i++)
	{
		CSprite* weatherParticle = new CSprite();
		weatherParticle->AddImage("particles.png", "particles");
		weatherParticle->SetAnimation("particles", 1);
		weatherParticle->SetYVelocity(-(rand() % 250 + 200));
		weatherParticle->SetPos(rand() % 1900, rand() % 700 + 300);
		weatherParticleList.push_back(weatherParticle);
	}
}


//*** SPRITE INITS
void MapGen::SpritesInit()
{
	gameBg.LoadImage("gameBg.jpg");
	gameBg.SetImage("gameBg.jpg");
	gameBg.SetSize(1920, 1080);
	gameBg.SetPosition(1920 / 2, 1080 / 2);

	//HEALT BOX SPRITe PREFAB
	healthBoxPreFab = new CSprite();
	healthBoxPreFab->AddImage("ammo2.png", "UZI", 6, 1, 3, 0, 3, 0, CColor::Black());
	healthBoxPreFab->SetAnimation("UZI", 1);

	//Grave SPRITe PREFAB
	gravePrefab = new CSprite();
	gravePrefab->AddImage("gravePrefab.png", CColor::Black());
	gravePrefab->SetAnimation("gravePrefab.png");

	//Explosion
	deathExplosionSprite = new CSprite();
	deathExplosionSprite->AddImage("explosion.bmp", "deathExplosion", 5, 5, 0, 0, 4, 4, CColor::Black());
	deathExplosionSprite->SetAnimation("deathExplosion", 20);
}


//*** LOOT UPDATE
void MapGen::LootUpdate()
{
	//ADD loot if timer < curent time
	if (lootTimer == 0) lootTimer = 15000 + localTime;
	if (lootTimer != 0 && lootTimer < localTime)
	{
		lootTimer = 0;
		addLootOnTheMap();
	}

	//Check for colision
	for (auto loot : lootList)
	{
		// HIT TEST WITH A MAP IN CASE IF MAP IS DESTROYED
		for (auto brig : mapList)
		{
			if (loot->HitTest(brig, 16)) {
				loot->SetYVelocity(0);
				break;
			}
			else loot->SetYVelocity(-250);
		}

		if (loot->GetY() < 0) loot->Delete();

		loot->Update(localTime);
	}

	//Delete If Deleted
	lootList.delete_if(deleted);
}


//*** WEATHER UPDATE
void MapGen::WeatherUpdate()
{
	for (auto particle : weatherParticleList)
	{
		//X velocity dependes on the wind strenght
		particle->SetXVelocity(localWindStrength * 50);
		particle->SetRotation(particle->GetDirection());
		if (particle->GetY() < 300 || particle->GetX() < 0 || particle->GetX() > localScreenWidth)
		{
			particle->SetX(rand() % 2000);
			particle->SetY(1050);
		}
		particle->Update(localTime);
	}
}


//*** GRAVES UPDATE
void MapGen::GravesUpadte()
{
	for (auto grave : graveList)
	{
		for (auto brig : mapList) // HIT TEST WITH A MAP IN CASE IF MAP IS DESTROYED
		{
			if (grave->HitTest(brig, 12))
			{
				grave->SetYVelocity(0);
				break;
			}
			else grave->SetYVelocity(-150);
		}

		if (grave->GetY() < 0) grave->Delete();
		grave->Update(localTime);
	}
}


//*** ADD GRAVE & Explosion When Enemy Dies
void MapGen::addGrave(float xGravePos, float yGravePos)
{
	//Explosion - probably more logical is to put it into player entity, but I delete obj before explodition , and its easier this way :)
	CSprite* newExplodition = new CSprite();
	newExplodition = deathExplosionSprite->Clone();
	newExplodition->ResetTime(localTime);
	newExplodition->SetPosition(xGravePos, yGravePos);
	newExplodition->Die(1000);
	deathexplosionAnimationList.push_back(newExplodition);

	//Grave
	CSprite* newGrave = new CSprite();
	newGrave = gravePrefab->Clone();
	newGrave->ResetTime(localTime);
	newGrave->SetPosition(xGravePos, yGravePos);
	graveList.push_back(newGrave);
}


//*** ADD LOOT
void MapGen::addLootOnTheMap()
{
	if (lootList.size() > 3) return;
	CSprite* AddHealthBox = new CSprite();
	AddHealthBox = healthBoxPreFab->Clone();
	AddHealthBox->ResetTime(localTime);
	AddHealthBox->SetPosition(rand() % 1800, 1000);
	AddHealthBox->SetYVelocity(-350);
	lootList.push_back(AddHealthBox);

	dropSound.Play("dropSound.wav");
	dropSound.Volume(0.35);
}