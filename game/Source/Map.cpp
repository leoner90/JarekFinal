#include "stdafx.h"
#include "../headers/Map.h"

void MapGen::init(float screenHeight, int screenWidth)
{
	//local variables
	localScreenHeight = screenHeight;
	localScreenWidth = screenWidth;

	// dismantle the old castle
	for each (CSprite * pSprite in mapList) delete pSprite;
	for each (CSprite * particle in weatherParticleList) delete particle;
	for each (CSprite * grave in graveList) delete grave;
	for each (CSprite * loot in lootList) delete loot;

	mapList.clear();
	weatherParticleList.clear();
	graveList.clear();
	lootList.clear();

	// build a new castle...
	const int W = 2000 / 32;	
	const int numberOfBrigs = 600;	
	int a[W];		 // array of integers	size of width

	memset(a, 0, sizeof(a)); // fill with 0
	lootTimer = 0; 

	for (int i = 0; i < numberOfBrigs; i++)
	{
		int random = rand() % (W / 4 + 1) + rand() % (W / 4 + 1) + rand() % (W / 4 + 1) + rand() % (W / 4);
		a[random]++; // puts + 1 in random array cell where width is max of 4
		if (a[random] > 15) a[random] = 15;	// maximum height limit
		//mapList.push_back(new CSprite(910.f + (random - W / 2) * 32 + 16, 180.f + a[random] * 32, 32, 32, "brick2.jpg", 0));
		mapList.push_back(new CSpriteRect(910.f + (random - W / 2) * 32 + 16, 180.f + a[random] * 32, 32, 32, CColor::DarkYellow(), 0));

	}

 
	gameBg.LoadImage("gameBg.jpg");
	gameBg.SetImage("gameBg.jpg");
	gameBg.SetSize(screenWidth , screenHeight );
	gameBg.SetPosition(screenWidth / 2, screenHeight / 2);

	turnChanged = false;

 


	for (int i = 0; i < 65; i++)
	{
		CSprite* weatherParticle = new CSprite();
		weatherParticle->AddImage("particles.png", "particles" );
		weatherParticle->SetAnimation("particles", 1);
		weatherParticle->SetYVelocity(-(rand() % 250 + 200));
		weatherParticle->SetPos(rand() % 1900, rand() % 700 + 300);
	 
		weatherParticleList.push_back(weatherParticle);
	}


	healthBox = new CSprite();
	healthBox->AddImage("ammo2.png", "UZI", 6, 1, 3, 0, 3, 0, CColor::Black());
	healthBox->SetAnimation("UZI", 1);


	gravePrefab = new CSprite();
	gravePrefab->AddImage("gravePrefab.png", CColor::Black());
	gravePrefab->SetAnimation("gravePrefab.png");




}


void MapGen::OnUpdate(long t, float screenHeight, float windStrength)
{
	localTime = t;
	localWindStrength = windStrength;

	for (auto brig : mapList) brig->Update(t);
	mapList.delete_if(deleted);

	if (lootTimer == 0) lootTimer = 5000 + t;

	if (lootTimer != 0 && lootTimer < t)
	{
		lootTimer = 0;
		addLootOnTheMap();
	}


	for (auto loot : lootList)
	{
	 
		if (loot->GetYVelocity() != 0)
		{
			for (auto brig : mapList)
			{
				if (loot->HitTest(brig, 8)) loot->SetYVelocity(0);
			}
		}
		
		if (loot->GetY() < 0) loot->Delete();

		loot->Update(localTime);
	}

	lootList.delete_if(deleted);


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


	for (auto grave : graveList) grave->Update(localTime);
}

void MapGen::OnDraw(CGraphics* g)
{

	gameBg.Draw(g);

	g->SetScrollPos(offset, 0);
	for (auto particle : weatherParticleList) particle->Draw(g);
	for (auto brig : mapList) brig->Draw(g);
	
	for(auto loot : lootList) loot->Draw(g);

	for (auto grave : graveList) grave->Draw(g);
	
}

void MapGen::addGrave(float xGravePos, float yGravePos)
{
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
	CSprite* AddHealthBox = new CSprite();
	AddHealthBox = healthBox->Clone();
	AddHealthBox->ResetTime(localTime);
	AddHealthBox->SetPosition(rand() % 1800, 1000);
	AddHealthBox->SetYVelocity(-350);
	lootList.push_back(AddHealthBox);

	dropSound.Play("dropSound.wav");
	dropSound.Volume(0.35);
}
