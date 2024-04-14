#include "stdafx.h"
#include "../headers/playerInterface.h"
 

//Global energy bars for player (todo -> move to .h)
CSpriteRect hpBarRect(0, 500, 100, 100, CColor::Red(), CColor::Red(), 1);


//************************ INIT ************************
void PlayerInterface::init(int screenWidth, int screenHeight)
{

}

//************************ UPDATE ************************
void PlayerInterface::OnUpdate()
{

}

//************************ DRAW ************************
void PlayerInterface::OnDraw(CGraphics* g, int screenWidth, int screenHeight)
{

}

 

//****** INIT SPRITES
void PlayerInterface::SpriteInits()
{
	//Control Panel
	//controllPanel->LoadImage("skillBarTest.png", "skillBar");
	//controllPanel->SetImage("skillBar", false);
	//controllPanel->SetSize(localScreenWidth, 120);
}