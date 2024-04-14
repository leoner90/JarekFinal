#include "stdafx.h"
#include "../headers/Map.h"

void MapGen::init(float screenHeight, int screenWidth)
{
	//local variables
	localScreenHeight = screenHeight;


	// dismantle the old castle
	for each (CSprite * pSprite in mapList)
		delete pSprite;
	mapList.clear();

	// build a new castle...
	const int W = 2000 / 32;	
	const int numberOfBrigs = 600;	
	int a[W];		 // array of integers	size of width

	memset(a, 0, sizeof(a)); // fill with 0


	for (int i = 0; i < numberOfBrigs; i++)
	{
		int random = rand() % (W / 4 + 1) + rand() % (W / 4 + 1) + rand() % (W / 4 + 1) + rand() % (W / 4);
		a[random]++; // puts + 1 in random array cell where width is max of 4
		if (a[random] > 15) a[random] = 15;	// maximum height limit
		mapList.push_back(new CSprite(910.f + (random - W / 2) * 32 + 16, 180.f + a[random] * 32, 32, 32, "brick2.jpg", 0));
		//if (random % 2 == 0) m_castle.back()->Rotate(90);
	}

 
	gameBg.LoadImage("gameBg.jpg");
	gameBg.SetImage("gameBg.jpg");
	gameBg.SetSize(screenWidth , screenHeight );
	gameBg.SetPosition(screenWidth / 2, screenHeight / 2);

	turnChanged = false;
}


void MapGen::OnUpdate(long t, float screenHeight  )
{
	for (auto brig : mapList) brig->Update(t);
	mapList.delete_if(deleted);

}

void MapGen::OnDraw(CGraphics* g)
{

	gameBg.Draw(g);

	g->SetScrollPos(offset, 0);
	for (auto brig : mapList) brig->Draw(g);
		
}

void MapGen::setCameraToCurrentPlayer(float curentPlayerPos)
{
	offset = curentPlayerPos;
}
