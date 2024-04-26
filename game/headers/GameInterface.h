#pragma once
 
class GameInterface
{
public:
	GameInterface();
	//Base Methods
	void init(int screenWidth, int screenHeight);
	void OnUpdate(float t, float& teamOneTotalHealth, float& teamTwoTotalHealth, float windStrength);
	void OnDraw(CGraphics* g, int screenWidth, int screenHeight);

	//WIND INTERFACE DRAW
	void windInterfaceDraw(CGraphics* g);
private:
	//SPRITE & POS INIT
	void SpriteInits();
	void InitPositions();

	//localVar just miroring to be able use in whole class
	float localScreenHeight, localScreenWidth;
	float localTeamOneHealth, localTeamTwoHealth;
	float localWindStrength;
	float localH, localW;

	//SPRITES
	CSprite timerBg, windInfoBg, windLogo;
	CSprite windLeftArrow, windRightArrow;
	CSpriteRect teamOneRectHp, TeamTwoRectHp;

	//SAVES TEAMS MAX HP FOR PROPER HP BAR DRAWING
	float teamOneMaxHp, teamTwoMaxHp;
};