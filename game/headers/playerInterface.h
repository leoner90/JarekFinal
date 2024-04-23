#pragma once
 

class PlayerInterface
{
public:
	PlayerInterface(float screenWidth, float screenHeight);
	//Base Methods
	void init(int screenWidth, int screenHeight);
	void OnUpdate(float t, float& teamOneTotalHealth, float& teamTwoTotalHealth, float windStrength);
	void OnDraw(CGraphics* g, int screenWidth, int screenHeight);
	void windDraw(CGraphics* g);
private:
	void SpriteInits();

	//variable miroring
	float localScreenHeight, localScreenWidth;
	float localTeamOneHealth, localTeamTwoHealth;
	float localWindStrength;
	float localH, localW;

	CSprite timerBg, windInfoBg, windLogo;
	CSprite windLeftArrow, windRightArrow;
	CSpriteRect teamOneRectHp, TeamTwoRectHp;
	float teamOneMaxHp, teamTwoMaxHp;
};