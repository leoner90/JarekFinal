#include "stdafx.h"
#include "../headers/Map.h"

void MapGen::init(float screenHeight, int screenWidth)
{
	//local variables
	localScreenHeight = screenHeight;
	localScreenWidth = screenWidth;

	// resets
	for each (CSprite * pSprite in mapList) delete pSprite;
	for each (CSprite * particle in weatherParticleList) delete particle;
	for each (CSprite * grave in graveList) delete grave;
	for each (CSprite * loot in lootList) delete loot;

	mapList.clear();
	weatherParticleList.clear();
	graveList.clear();
	lootList.clear();
	lootTimer = 0;

	// MAP, WEATHER, Sprites INITS
	mapCreationInit();  
	WeatherInit();	 
	SpritesInit(); 
}


void MapGen::OnUpdate(long t, float screenHeight, float windStrength)
{
	cout << mapList.size() << endl;

	//local Variables
	localTime = t;
	localWindStrength = windStrength;

	//Map Update
	for (auto brig : mapList) brig->Update(t);
	mapList.delete_if(deleted);

	// Loot, WEATHER, Graves UPDATES
	LootUpdate();
	WeatherUpdate();
	GravesUpadte();


	for(auto explosion : deathexplosionAnimationList) 	explosion->Update(localTime);
	deathexplosionAnimationList.delete_if(deleted);
 
}

void MapGen::OnDraw(CGraphics* g)
{

	gameBg.Draw(g);

	for (auto particle : weatherParticleList) particle->Draw(g);
	for (auto brig : mapList) brig->Draw(g);
	for (auto loot : lootList) loot->Draw(g);
	for (auto grave : graveList) grave->Draw(g);
	for (auto explosion : deathexplosionAnimationList) 	explosion->Draw(g);	
}

void MapGen::addGrave(float xGravePos, float yGravePos)
{
 
	//probably more logical is to put it into player entity, but I delete obj before explodition , and its easier this way :)
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



void MapGen::setCameraToCurrentPlayer(float curentPlayerPos)
{
	offset = curentPlayerPos;
}

void MapGen::addLootOnTheMap()
{
	if (lootList.size() > 3) return;
	CSprite* AddHealthBox = new CSprite();
	AddHealthBox = healthBox->Clone();
	AddHealthBox->ResetTime(localTime);
	AddHealthBox->SetPosition(rand() % 1800, 1000);
	AddHealthBox->SetYVelocity(-350);
	lootList.push_back(AddHealthBox);

	dropSound.Play("dropSound.wav");
	dropSound.Volume(0.35);
}


void MapGen::mapCreationInit()
{

	//MA
	const int W = 1920 / 32;
	const int numberOfBrigs = 400; // creates less as if max height achived it returns
	int a[W];		 // array of integers	size of width

	memset(a, 0, sizeof(a)); // fill with 0


	for (int i = 0; i < numberOfBrigs; i++)
	{
		int random = rand() % (W / 4 + 1) + rand() % (W / 4 + 1) + rand() % (W / 4 + 1) + rand() % (W / 4);
		a[random]++; // puts + 1 in random array cell where width is max of 60

		// maximum height limit, return if maximum achived
		if (a[random] > 15)
		{
			a[random] = 15;
			continue;
		}

		mapList.push_back(new CSpriteRect(932.f + (random - W / 2) * 32 + 16, 180.f + a[random] * 32, 32, 32, CColor::DarkYellow(), 0));

	}
}

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

void MapGen::SpritesInit()
{
	gameBg.LoadImage("gameBg.jpg");
	gameBg.SetImage("gameBg.jpg");
	gameBg.SetSize(localScreenWidth, localScreenHeight);
	gameBg.SetPosition(localScreenWidth / 2, localScreenHeight / 2);

	//HEALT BOX SPRITe PREFAB
	healthBox = new CSprite();
	healthBox->AddImage("ammo2.png", "UZI", 6, 1, 3, 0, 3, 0, CColor::Black());
	healthBox->SetAnimation("UZI", 1);

	//Grave SPRITe PREFAB
	gravePrefab = new CSprite();
	gravePrefab->AddImage("gravePrefab.png", CColor::Black());
	gravePrefab->SetAnimation("gravePrefab.png");


	deathExplosionSprite = new CSprite();
	deathExplosionSprite->AddImage("explosion.bmp", "deathExplosion", 5, 5, 0, 0, 4, 4, CColor::Black());
	deathExplosionSprite->SetAnimation("deathExplosion", 20);
}

void MapGen::LootUpdate()
{

	if (lootTimer == 0) lootTimer = 15000 + localTime;
	if (lootTimer != 0 && lootTimer < localTime)
	{
		lootTimer = 0;
		addLootOnTheMap();
	}


	for (auto loot : lootList)
	{

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

	lootList.delete_if(deleted);
}

void MapGen::WeatherUpdate()
{
	for (auto particle : weatherParticleList)
	{
		//X velocity depending on wind
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

void MapGen::GravesUpadte()
{

	for (auto grave : graveList)
	{
		for (auto brig : mapList)
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
