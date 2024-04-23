#include "stdafx.h"
#include "../headers/playerInterface.h"
 


PlayerInterface::PlayerInterface(float screenWidth, float screenHeight):
	teamOneRectHp(900, 35, 300, 20, CColor::Blue(), CColor::Black(), 1),
	TeamTwoRectHp(900, 15, 300, 20, CColor::Red(), CColor::Black(), 1)
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

//************************ INIT ************************
void PlayerInterface::init(int screenWidth, int screenHeight)
{
	localH = screenHeight;
	localW = screenWidth;
	timerBg.SetSize(120, 120);
	timerBg.SetPosition(screenWidth / 2 , screenHeight - 60);

	windInfoBg.SetSize(470, 133);
	windInfoBg.SetPosition(screenWidth - 235, 67);

	windLogo.SetSize(40, 40);
	windLogo.SetPosition(screenWidth - 270, 80);


	//windLeftArrow & windRightArrow
	windRightArrow.SetSize(14, 26);
	windRightArrow.SetSize(14, 26);
	float teamOneMaxHp = teamTwoMaxHp = 0;
}

//************************ UPDATE ************************
void PlayerInterface::OnUpdate(float t, float& teamOneTotalHealth, float& teamTwoTotalHealth, float windStrength)
{
	if (teamOneTotalHealth == 0 || teamTwoTotalHealth == 0) return;
	if (teamOneMaxHp == 0 || teamTwoMaxHp == 0)
	{
		teamOneMaxHp = teamOneTotalHealth;
		teamTwoMaxHp = teamTwoTotalHealth;
	}

	localTeamOneHealth = teamOneTotalHealth;
	localTeamTwoHealth = teamTwoTotalHealth;
	localWindStrength = windStrength;
	windLogo.SetOmega(45 * windStrength);
	windLogo.Update(t);


	teamOneRectHp.SetSize(localTeamOneHealth, 20);
	teamOneRectHp.SetPosition(localW / 2 - ((teamOneMaxHp - localTeamOneHealth) / 2), 45);
	TeamTwoRectHp.SetSize(localTeamTwoHealth, 20);
	TeamTwoRectHp.SetPosition(localW / 2 - ((teamTwoMaxHp - localTeamTwoHealth) / 2), 15);
}

//************************ DRAW ************************
void PlayerInterface::OnDraw(CGraphics* g, int screenWidth, int screenHeight)
{
	if (localTeamOneHealth == 0 || localTeamTwoHealth == 0) return;
	
	timerBg.Draw(g);
	windDraw(g);


	//TEAM HP BARS DRAW
	teamOneRectHp.Draw(g);
	*g << font(26)  << color(CColor::DarkBlue()) << xy(teamOneRectHp.GetLeft() - 100, 35) << "BLUE TEAM: ";
	TeamTwoRectHp.Draw(g);
	*g << font(26) << color(CColor::DarkRed()) << xy(TeamTwoRectHp.GetLeft() - 100, 6) << "RED TEAM: ";

	*g << font("AFontPTSerif.ttf") << font(18)  << color(CColor::White()) << xy(teamOneRectHp.GetCenter().GetX(), 37) << localTeamOneHealth;
	*g << font(18)   << color(CColor::White()) << xy(TeamTwoRectHp.GetCenter().GetX(), 8) << localTeamTwoHealth;
	*g << font("ARIAL.ttf");
}

void PlayerInterface::windDraw(CGraphics* g)
{
	windInfoBg.Draw(g);
	windLogo.Draw(g);
	float offset = localWindStrength >= 0 ? 205 : 225;
	if (localWindStrength == -0) localWindStrength = 0; // :)))))))))))))
	*g << font(52) << color(CColor::Red()) << xy(localW - offset, 55) << localWindStrength;


	if (localWindStrength > 0)
	{
		for (float i = 0; i < localWindStrength; i++)
		{
			windRightArrow.SetPosition(localW - 221 + i * 14, 20);
			windRightArrow.Draw(g);
		}
	}

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
void PlayerInterface::SpriteInits()
{
	//Control Panel
	//controllPanel->LoadImage("skillBarTest.png", "skillBar");
	//controllPanel->SetImage("skillBar", false);
	//controllPanel->SetSize(localScreenWidth, 120);
}