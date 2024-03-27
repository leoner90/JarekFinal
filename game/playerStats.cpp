#include "stdafx.h"
#include "headers/playerStats.h"
#include "headers/Player.h"

//************************ INIT ************************
void playerStats::init(float screenHeight, float screenWidth)
{
	//for restart
	delete charStatsScreen;
	delete levelUpBtn;
	charStatsScreen = new CSprite();
	levelUpBtn = new CSprite();

	//char stats menu BG/Sprite
	charStatsScreen->LoadImage("CharStats.jpg");
	charStatsScreen->SetImage("CharStats.jpg");
	charStatsScreen->SetSize(screenWidth, screenHeight);
	charStatsScreen->SetPosition(screenWidth / 2, screenHeight / 2);

	//lvl Up btn Sprite
	levelUpBtn->LoadImage("addSkillBtn.png");
	levelUpBtn->SetImage("addSkillBtn.png");
	levelUpBtn->SetPosition(screenWidth / 3, screenHeight / 2);
}

//************************ UPDATE ************************
void playerStats::OnUpdate(Player& player)
{
	LocalPlayer = &player;
}

//************************ DRAW ************************
void playerStats::OnDraw(CGraphics* g, float screenHeight)
{
	//Main stats Bg
	charStatsScreen->Draw(g);

	if (charStatsScreen->GetY() > screenHeight / 2 + 40) charStatsScreen->SetY(charStatsScreen->GetY() - 50);
	else
	{
		//coorditnats for stats on the screen
		strCoor = screenHeight - (screenHeight * 0.36);
		dexCoor = screenHeight - (screenHeight * 0.45);
		conCoor = screenHeight - (screenHeight * 0.54);
		intCoor = screenHeight - (screenHeight * 0.63);
		
		//sets stats text and lvlUp btn possition on the screen
		StatsPosition(g, LocalPlayer->str, strCoor - 10, strCoor, LocalPlayer->Skillpoint,"STR");
		StatsPosition(g, LocalPlayer->dex, dexCoor - 10, dexCoor, LocalPlayer->Skillpoint, "DEX");
		StatsPosition(g, LocalPlayer->con, conCoor - 15, conCoor, LocalPlayer->Skillpoint, "CON");
		StatsPosition(g, LocalPlayer->intellect, intCoor - 15, intCoor, LocalPlayer->Skillpoint, "INT");
		drawStatInformation(g);
	}
}

//position Of The Stats
void playerStats::StatsPosition(CGraphics* g, int& stat, float textY, float btnY,int & Skillpoint, char* statName)
{
	*g << font(42) << color(CColor::White()) << xy(levelUpBtn->GetX() - 180, textY) << statName;
	*g << font(33) << color(CColor::White()) << xy(levelUpBtn->GetX() - 100, textY) << stat;

	if (Skillpoint > 0) {
		levelUpBtn->SetPosition(levelUpBtn->GetX(), btnY);
		levelUpBtn->Draw(g);
	}
}

//Draw Damage & Leveling Statistic
void playerStats::drawStatInformation(CGraphics* g)
{
	//str
	*g << font("AFontPTSerif.ttf", 16) << color(CColor::White()) << xy(levelUpBtn->GetX() + 450, strCoor) << "MELEE DAMAGE:  " << LocalPlayer->meleeDamage;

	//dex
	*g << font("AFontPTSerif.ttf", 16) << color(CColor::White()) << xy(levelUpBtn->GetX() + 450, dexCoor + 30) << "KUNAI DAMAGE:  " << LocalPlayer->kunaiDamage;
	*g << font("AFontPTSerif.ttf", 16) << color(CColor::White()) << xy(levelUpBtn->GetX() + 450, dexCoor + 10) << "MAX. STAMINA  " << LocalPlayer->maxPlayerEnergy;
	*g << font("AFontPTSerif.ttf", 16) << color(CColor::White()) << xy(levelUpBtn->GetX() + 450, dexCoor - 10) << "CURRENT STAMINA  " << int(LocalPlayer->CurrentPlayerEnergy);
	*g << font("AFontPTSerif.ttf", 16) << color(CColor::White()) << xy(levelUpBtn->GetX() + 450, dexCoor - 30) << "STAMINA REGEN PER SEC  " << 0.01f * LocalPlayer->dex;

	//int
	*g << font("AFontPTSerif.ttf", 16) << color(CColor::White()) << xy(levelUpBtn->GetX() + 450, intCoor) << "FIREBOLT DAMAGE:  " << LocalPlayer->fireboltDamage;
	*g << font("AFontPTSerif.ttf", 16) << color(CColor::White()) << xy(levelUpBtn->GetX() + 450, intCoor - 20) << "MAX. MANA POINTS:  " << LocalPlayer->maxPlayerMp;
	*g << font("AFontPTSerif.ttf", 16) << color(CColor::White()) << xy(levelUpBtn->GetX() + 450, intCoor - 40) << "CURRENT MANA POINTS:  " << LocalPlayer->CurrentPlayerMp;

	//con
	*g << font("AFontPTSerif.ttf", 16) << color(CColor::White()) << xy(levelUpBtn->GetX() + 450, conCoor - 10) << "MAX. HEALTH:  " << LocalPlayer->maxPlayerHealth;
	*g << font("AFontPTSerif.ttf", 16) << color(CColor::White()) << xy(levelUpBtn->GetX() + 450, conCoor - 30) << "CURRENT HEALTH:  " << LocalPlayer->CurrentPlayerHealth;

	//player LVL
	*g << font("AFontPTSerif.ttf", 22) << color(CColor::White()) << xy(levelUpBtn->GetX() - 150, 160) << "LEVEL " << LocalPlayer->playerLVL;
	*g << font("AFontPTSerif.ttf", 22) << color(CColor::White()) << xy(levelUpBtn->GetX() + 350, 160) << "CURRENT EXP " << LocalPlayer->currentExp;

	if (LocalPlayer->maxLvl)
		*g << font("AFontPTSerif.ttf", 22) << color(CColor::White()) << xy(levelUpBtn->GetX() + 350, 120) << " MAX LVL REACHED ";
	else
		*g << font("AFontPTSerif.ttf", 22) << color(CColor::White()) << xy(levelUpBtn->GetX() + 350, 120) << "NEXT LVL EXP " << LocalPlayer->tillNextLvlExp;

	*g << font("AFontPTSerif.ttf", 22) << color(CColor::White()) << xy(levelUpBtn->GetX() - 150, 120) << "SKILLPOINTS " << LocalPlayer->Skillpoint;
}
 

//Leveling Up On Btn Pressed
void playerStats::LevelUpBtnLogic(int& stat, float btnY, int& Skillpoint, CVector mouseCoordinats )
{
	if (Skillpoint > 0 &&
		mouseCoordinats.GetX() <= levelUpBtn->GetX() + 20 &&
		mouseCoordinats.GetX() >= levelUpBtn->GetX() - 20 &&
		mouseCoordinats.GetY() <= btnY + 20 &&
		mouseCoordinats.GetY() >= btnY - 20
		)
	{
		stat += 1;
		Skillpoint -= 1;
		updatePlayerStats();
	}
}

//update Player Stats On Leveing up
void playerStats::updatePlayerStats()
{
	LocalPlayer->maxPlayerHealth = 100 * LocalPlayer->con;
	LocalPlayer->maxPlayerMp = 10 * LocalPlayer->intellect;
	LocalPlayer->maxPlayerEnergy = 10.0f * LocalPlayer->dex;
	LocalPlayer->meleeDamage = LocalPlayer->str * 10;
	LocalPlayer->kunaiDamage = LocalPlayer->dex * 20;
	LocalPlayer->fireboltDamage = LocalPlayer->intellect * 15;
}

//Check Did player hit the lvl Up btn
void playerStats::OnLButtonUp(Player& player,int& Skillpoint, CVector mouseCoordinats)
{
	LevelUpBtnLogic(player.str, strCoor, Skillpoint, mouseCoordinats);
	LevelUpBtnLogic(player.dex, dexCoor, Skillpoint, mouseCoordinats);
	LevelUpBtnLogic(player.con, conCoor, Skillpoint, mouseCoordinats);
	LevelUpBtnLogic(player.intellect, intCoor, Skillpoint, mouseCoordinats);
}