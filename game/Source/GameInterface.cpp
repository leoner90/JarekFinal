#include "stdafx.h"
#include "../headers/GameInterface.h"
 

//Constructor
GameInterface::GameInterface():
	//CREATE TWO RECTANGLES HP BARS
	teamOneRectHp(900, 35, 300, 20, CColor::Blue(), CColor::Black(), 1),
	TeamTwoRectHp(900, 15, 300, 20, CColor::Red(), CColor::Black(), 1)
{
	//INIT SPRITES
	SpriteInits();
}

//************************ INIT ************************
void GameInterface::init(int screenWidth, int screenHeight)
{
	//localVar just miroring to be able use in whole class
	localH = screenHeight;
	localW = screenWidth;
	
	//Resets
	InitPositions();
	float teamOneMaxHp = teamTwoMaxHp = 0;
}

//************************ UPDATE ************************
void GameInterface::OnUpdate(float t, float& teamOneTotalHealth, float& teamTwoTotalHealth, float windStrength)
{
	//localVar just miroring to be able use in whole class
	localTeamOneHealth = teamOneTotalHealth;
	localTeamTwoHealth = teamTwoTotalHealth;
	localWindStrength = windStrength;

	//IF end Of the Game Condition return
	if (localTeamOneHealth == 0 || localTeamTwoHealth == 0) return;

	//Saves TEAMS MAX Health on game start
	if (teamOneMaxHp == 0 || teamTwoMaxHp == 0)
	{
		teamOneMaxHp = localTeamOneHealth;
		teamTwoMaxHp = localTeamTwoHealth;
	}

	//Rotates Wind Logo Depending On Wind Strenght and Direction
	windLogo.SetOmega(45 * windStrength);
	windLogo.Update(t);

	//Set Proper Size And Position each Frame (in case of Damage)
	teamOneRectHp.SetSize(localTeamOneHealth, 20);
	teamOneRectHp.SetPosition(localW / 2 - ((teamOneMaxHp - localTeamOneHealth) / 2), 45);
	TeamTwoRectHp.SetSize(localTeamTwoHealth, 20);
	TeamTwoRectHp.SetPosition(localW / 2 - ((teamTwoMaxHp - localTeamTwoHealth) / 2), 15);
}

//************************ DRAW ************************
void GameInterface::OnDraw(CGraphics* g, int screenWidth, int screenHeight)
{
	if (localTeamOneHealth == 0 || localTeamTwoHealth == 0) return;
	
	timerBg.Draw(g);
	windInterfaceDraw(g);

	//TEAM HP BARS And Names DRAW
	teamOneRectHp.Draw(g);
	*g << font(26)  << color(CColor::DarkBlue()) << xy(teamOneRectHp.GetLeft() - 100, 35) << "BLUE TEAM: ";
	TeamTwoRectHp.Draw(g);
	*g << font(26) << color(CColor::DarkRed()) << xy(TeamTwoRectHp.GetLeft() - 100, 6) << "RED TEAM: ";

	//Draws Teams Health Numbers
	*g << font("AFontPTSerif.ttf") << font(18)  << color(CColor::White()) << xy(teamOneRectHp.GetCenter().GetX(), 37) << localTeamOneHealth;
	*g << font(18)   << color(CColor::White()) << xy(TeamTwoRectHp.GetCenter().GetX(), 8) << localTeamTwoHealth;
	*g << font("ARIAL.ttf");
}

//************************ WIND INTERFACE DRAW ************************
void GameInterface::windInterfaceDraw(CGraphics* g)
{
	windInfoBg.Draw(g);
	windLogo.Draw(g);
	//OffSet For text , if negative number - biggeroffset 
	float offset = localWindStrength >= 0 ? 205 : 225;

	//BEST CODE EVER V 1.22
	if (localWindStrength == -0) localWindStrength = 0;

	//Draws Wind Strenght Number
	*g << font(52) << color(CColor::Red()) << xy(localW - offset, 55) << localWindStrength;

	//Draws Wind Arrow Depending on Wind Strenght And Direction
	//Right
	if (localWindStrength > 0)
	{
		for (float i = 0; i < localWindStrength; i++)
		{
			windRightArrow.SetPosition(localW - 221 + i * 14, 20);
			windRightArrow.Draw(g);
		}
	}

	//Left
	else if (localWindStrength < 0)
	{
		for (float i = 0; i > localWindStrength; i--)
		{
			windLeftArrow.SetPosition(localW - 242 + i * 14, 20);
			windLeftArrow.Draw(g);
		}
	}
}

//****** INIT SPRITES
void GameInterface::SpriteInits()
{
	//timerBg
	timerBg.LoadImage("timerBg.png");
	timerBg.SetImage("timerBg.png");

	//windInfoBg
	windInfoBg.LoadImage("windInfoBg.png");
	windInfoBg.SetImage("windInfoBg.png");

	//windLogo
	windLogo.AddImage("windLogo.png", "windLogo");
	windLogo.SetAnimation("windLogo", 1);

	//windLeftArrow
	windLeftArrow.LoadImage("windLeftArrow.jpg");
	windLeftArrow.SetImage("windLeftArrow.jpg");

	//windRightArrow
	windRightArrow.LoadImage("windRightArrow.jpg");
	windRightArrow.SetImage("windRightArrow.jpg");
}


//****** INIT POSITIONS FOR SPRITES (dosen't work in constractor!!)
void GameInterface::InitPositions()
{
	timerBg.SetSize(120, 120);
	timerBg.SetPosition(localW / 2, localH - 60);

	windInfoBg.SetSize(470, 133);
	windInfoBg.SetPosition(localW - 235, 67);

	windLogo.SetSize(40, 40);
	windLogo.SetPosition(localW - 270, 80);

	//windLeftArrow & windRightArrow
	windLeftArrow.SetSize(14, 26);
	windRightArrow.SetSize(14, 26);
}