#include "stdafx.h"
#include "../headers/MyGame.h"
#include "../headers/PlayerEntity/PlayerEntity.h"
#include "../headers/Map.h"
#include "../headers/playerInterface.h"
 

//Global for class
CMyGame::CMyGame()
{
	mapGen = new MapGen();
	playerInterface = new PlayerInterface(GetWidth(), GetHeight());
	playerEntity = new PlayerEntity();

	//menu Music
	BgMusic.Play("mainBg.wav", -1);
	BgMusic.Volume(0.15);
}

/******************************** INIT ********************************/
void CMyGame::OnInitialize()
{
	//Basic reset
	HideMouse();
 
	gameStarted = isGameWon   = false;

	startScreenSelection = NEWGAME;
	gameResetTimer = 0;
	currentPlayerTeam = 0;

	
	
	playerInterface->init(GetWidth(), GetHeight());
	mapGen->init(GetHeight(), GetWidth());


	//init sprites for this class only
	initSpritesHandler();

	PlayerEntitys.push_back(new PlayerEntity());
	PlayerEntitys.back()->init(500, 700, 0);

	PlayerEntitys.push_back(new PlayerEntity());
	PlayerEntitys.back()->init(300, 700, 0);

	PlayerEntitys.push_back(new PlayerEntity());
	PlayerEntitys.back()->init(100, 700, 0);

	PlayerEntitys.push_back(new PlayerEntity());
	PlayerEntitys.back()->init(600, 700, 1);

	PlayerEntitys.push_back(new PlayerEntity());
	PlayerEntitys.back()->init(250, 700, 1);

	PlayerEntitys.push_back(new PlayerEntity());
	PlayerEntitys.back()->init(400, 700, 1);

	currentPlayerTurnIndex = -1;
	isLoadingComplite = false;
	isTurnFinished = true;



	teamOneTotalHealth = teamTwoTotalHealth = 0;
	CurentPlayerTeamIndex = 0;
}


void CMyGame::OnStartLevel(Sint16 nLevel)
{

}



/******************************** UPDATE ********************************/
void CMyGame::OnUpdate()
{

	if (IsMenuMode() || IsGameOver()  || !gameStarted) 
	{
	 
		loadingLogo.Update(GetTime());
		if (loadingTimer < GetTime()) isLoadingComplite = true;
		return;  
	}
	else if (IsGameWon)
	{
		if (!victoryApplauds.IsPlaying(), 0) victoryApplauds.Play("victoryApplauds.wav");
		for (auto player : PlayerEntitys)
		{
			player->enemySprite->SetStatus(8);
			player->GameWonPlayerUpdate(GetTime());
			mapGen->OnUpdate(GetTime(), GetHeight(), windStrengthXVel);
		}

		if (gameResetTimer < GetTime())
		{
			gameResetTimer = 0;
			gameStarted = false;
			currentMenuState = MENU;
			startScreenSelection = NEWGAME;
			ChangeMode(MODE_MENU);
			IsGameWon = false;
		}
	}
	else 
	{
		//*** PLAYERS UPDATES
		if (isTurnFinished) 
		{
			if (CurentPlayerTeamIndex == 0) CurentPlayerTeamIndex = 1;
			else CurentPlayerTeamIndex = 0;
		}
			

		//***** Change TURN
		for (auto player : PlayerEntitys)
		{
			if (CurentPlayerTeamIndex == player->isFriend && isTurnFinished)
			{

				// check is turns left for current team
				bool havePlayerTurnsLeft = false; 
				for (auto playerForReset : PlayerEntitys)
					if (CurentPlayerTeamIndex == playerForReset->isFriend && playerForReset->beenUsed == false) havePlayerTurnsLeft = true;
 

				//if no turn left -> reset all turns
				if (!havePlayerTurnsLeft)
				{
					for (auto playerForResetTurn : PlayerEntitys)
						if (CurentPlayerTeamIndex == playerForResetTurn->isFriend) playerForResetTurn->beenUsed = false;
				}


				//if this player didn't have turn
				if (player->beenUsed == false)
				{
					bool posOrnegative = rand() % 2;
					windStrengthXVel = rand() % 5; // radnom -5 to + 5
					windStrengthXVel *= posOrnegative > 0 ? 1 : -1;


					player->windStrength = { windStrengthXVel , 0 };
					player->isPlayerTurn = true;
					player->beenUsed = true;
					isTurnFinished = false;
				}
			}
		}


		//AIPLAYERS UPDATE
		teamOneTotalHealth = teamTwoTotalHealth = 0; // health reset for UI

		bool teamOneMemberAlive = false; // just to make sure that all enemies deleted and animation finished before call GAME OVER
		bool teamTwoMemberAlive = false;
		for (auto player : PlayerEntitys)
		{
			//UPDATE
			player->OnUpdate(GetTime(), *mapGen, IsKeyDown(SDLK_d), IsKeyDown(SDLK_a), IsKeyDown(SDLK_w), IsKeyDown(SDLK_s), IsKeyDown(SDLK_f), isTurnFinished, PlayerEntitys);

			//HEALTH
		
			if (player->isFriend) {
				teamOneMemberAlive = true;
				teamOneTotalHealth += player->CurrentEnemyHealth;
			}
			if (!player->isFriend)
			{
				teamTwoMemberAlive = true;
				teamTwoTotalHealth += player->CurrentEnemyHealth;
			}

			//IF DEAD
			if (player->isDead  ) { // && !player->isPlayerTurn
				PlayerEntitys.erase(find(PlayerEntitys.begin(), PlayerEntitys.end(), player));
				delete player;
			}
		}

		if (!teamOneMemberAlive || !teamTwoMemberAlive)
		{
			if (teamOneTotalHealth <= 0 && teamTwoTotalHealth > 0) GameFinished(1);
			else if (teamTwoTotalHealth <= 0 && teamOneTotalHealth > 0) GameFinished(2);
			else GameFinished(0);

		 
		}

		// Map Update 
		mapGen->OnUpdate(GetTime(), GetHeight(), windStrengthXVel);

		// Interface Update 
		playerInterface->OnUpdate(GetTime(), teamOneTotalHealth, teamTwoTotalHealth, windStrengthXVel);
	}
}

/******************************** DRAW ********************************/
void CMyGame::OnDraw(CGraphics* g)
{
 
 
	//*** IF MENU MODE OR CHARSTATS
	if (IsMenuMode()) 
	{
		if (currentMenuState == MENU)  menuHandler(g);
		if (currentMenuState == LOADING) LoadingScreen(g);
	}

	//*** IF IN GAME!
	else 
	{
		mapGen->OnDraw(g);
		if(!IsGameWon) playerInterface->OnDraw(g, GetWidth(), GetHeight());

		PlayerEntity* curentPlayerTurnPlayer = nullptr;
		for (auto player : PlayerEntitys)
		{
			if (player->isPlayerTurn) // for menu to be drawn last , as player inherest inventory , and each of them hhave individuall one
			{
				curentPlayerTurnPlayer = player;
				continue;
			}
			player->OnDraw(g);
		}
		if (curentPlayerTurnPlayer != nullptr) curentPlayerTurnPlayer->OnDraw(g); // for inventory , inventory is drawn last

		if (IsGameWon)
		{
			winnerTeamBg->Draw(g);
			winConditionOverlay->Draw(g);
			if(winnerTeamNumber !=0)
				*g << font(55) << color(CColor::DarkRed()) << xy(GetWidth() / 2 - 120, GetHeight() - 250) << "TEAM " << winnerTeamNumber << " WON!!!";
			else
				*g << font(55) << color(CColor::DarkRed()) << xy(GetWidth() / 2 - 50, GetHeight() - 250) << "DRAW";
		}
	}

}

void CMyGame::LoadingScreen(CGraphics* g)
{
	loadingScreen.Draw(g);
	if (!isLoadingComplite)
	{
		loadingLogo.Draw(g);
		*g << font(35) << color(CColor::LightGray()) << xy(loadingLogo.GetX() - 40, loadingLogo.GetY() - 80) << "LOADING...";
	}
	else *g << font(35) << color(CColor::LightGray()) << xy(loadingLogo.GetX() - 150, loadingLogo.GetY() - 20 ) << "PRESS ENTER  TO  CONTINUE";
	
}

void CMyGame::GameFinished(int winnerTeam)
{
	IsGameWon = true;
	winnerTeamNumber = winnerTeam;
	gameResetTimer = GetTime() + 8000;

}

//************************************* INIT SPRITES *************************************
void CMyGame::initSpritesHandler()
{
//gameOverScreen
	delete winConditionOverlay;
	winConditionOverlay = new CSprite();
	winConditionOverlay->LoadImage("winConditionOverlay.png");
	winConditionOverlay->SetImage("winConditionOverlay.png");
	winConditionOverlay->SetSize(GetWidth(), GetHeight());
	winConditionOverlay->SetPosition(GetWidth() / 2, GetHeight() / 2);


	delete winnerTeamBg;
	winnerTeamBg = new CSprite();
	winnerTeamBg->LoadImage("winnerTeamBg.png");
	winnerTeamBg->SetImage("winnerTeamBg.png");
	winnerTeamBg->SetSize(400, 300);
	winnerTeamBg->SetPosition(GetWidth() / 2, GetHeight() - 200);



	//menu
	delete startScreen;
	startScreen = new CSprite();
	startScreen->LoadImage("menuBar.jpg");
	startScreen->SetImage("menuBar.jpg");
	startScreen->SetSize(GetWidth(), GetHeight());
	startScreen->SetPosition(GetWidth() / 2, GetHeight() / 2);

	delete star;
	star = new CSprite();
	star->LoadImage("star.png");
	star->SetImage("star.png");
	star->SetSize(110, 90);;


	//Loading
 
	loadingLogo.AddImage("loadingLogo.png", "loadingLogo");
	loadingLogo.SetAnimation("loadingLogo", 1);
	loadingLogo.SetPosition(980, 290);
	loadingLogo.SetSize(70, 70);
	 
 
	loadingLogo.SetOmega(90);

	//loading
	loadingScreen.LoadImage("loadingScreen.jpg");
	loadingScreen.SetImage("loadingScreen.jpg");
	loadingScreen.SetSize(GetWidth(), GetHeight());
	loadingScreen.SetPosition(GetWidth() / 2, GetHeight() / 2);
}

//************************************* ALL PLAYERS *************************************



//******* DRAW THE MENU/ Control Page
void CMyGame::menuHandler(CGraphics* g)
{
	startScreen->Draw(g);
	if (showControllImg)
	{
 
		*g << font(32) << color(CColor::White()) << xy(800, 670) << "S - Move Left";
		*g << font(32) << color(CColor::White()) << xy(800, 630) << "D - Move Right";
		*g << font(32) << color(CColor::White()) << xy(800, 590) << "W - AIM UP";
		*g << font(32) << color(CColor::White()) << xy(800, 550) << "S - AIM DOWN";

		*g << font(32) << color(CColor::White()) << xy(800, 510) << "F - Jump";;
		*g << font(32) << color(CColor::White()) << xy(800, 470) << "TAB - Select Weapon";
		*g << font(32) << color(CColor::White()) << xy(800, 430) << "SPACE - START ATTACK";


		*g << font(52) << color(startScreenSelection == BACK ? CColor::White() : CColor::LightGray()) << xy(880, 350) << "BACK";
	}
	else
	{
		if (gameStarted)
		{
			*g << font(52) << color(startScreenSelection == CONTINUE ? CColor::White() : CColor::LightGray()) << xy(885, 650) << "CONTINUE";
			if (startScreenSelection == CONTINUE) star->SetPosition(800, 670);

			*g << font(52) << color(startScreenSelection == NEWGAME ? CColor::White() : CColor::LightGray()) << xy(885, 550) << "NEW GAME";
			if (startScreenSelection == NEWGAME) star->SetPosition(800, 570);
		}
		else
		{
			if (startScreenSelection == NEWGAME) star->SetPosition(800, 670);
			*g << font(52) << color(startScreenSelection == NEWGAME ? CColor::White() : CColor::LightGray()) << xy(885, 650) << "NEW GAME";
			*g << font(52) << color(CColor::DarkGray()) << xy(885, 550) << "CONTINUE";
		}

		*g << font(52) << color(startScreenSelection == CONTROLS ? CColor::White() : CColor::LightGray()) << xy(885, 450) << "CONTROLS";
		if (startScreenSelection == CONTROLS) star->SetPosition(800, 470);

		*g << font(52) << color(startScreenSelection == EXIT ? CColor::White() : CColor::LightGray()) << xy(885, 350) << "EXIT";
		if (startScreenSelection == EXIT) star->SetPosition(800, 370);

		star->Draw(g);
	}
}


//*************************************KEYBOARD EVENTS  *************************************
void CMyGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (!IsMenuMode())
	{
		for (auto currentPlayer : PlayerEntitys)
		{
			if (currentPlayer->isPlayerTurn) currentPlayer->OnKeyUp(sym);
		}
	}
}


void CMyGame::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
{

	if (!IsMenuMode())
	{
		for (auto currentPlayer : PlayerEntitys)
		{
			if (currentPlayer->isPlayerTurn) currentPlayer->OnKeyDown(sym);
		}
	}

	//*** MENU NAVIGATION
	if (IsMenuMode() && ((sym == SDLK_s) || (sym == SDLK_DOWN)) && !showControllImg)
	{
		startScreenSelection++;
		if (startScreenSelection > 3) startScreenSelection = gameStarted ? 0 : 1; //Change Sequence if game started
	}

	//F11 CHANGE TURN
	if (!IsMenuMode() && (sym == SDLK_F11))
	{
		isTurnFinished = true;
	}


	if (IsMenuMode() && ((sym == SDLK_w) || (sym == SDLK_UP)) && !showControllImg)
	{
		startScreenSelection--;
		int range = gameStarted ? 0 : 1;
		if (startScreenSelection < range) startScreenSelection = 3;
	}

	////*** MAIN MENU
	if (IsMenuMode() && (sym == 13) ) //enter
	{

		if (currentMenuState == LOADING && isLoadingComplite)
		{
			StartGame();
			ChangeMode(MODE_GAME);
			currentMenuState = INGAME;
			gameStarted = true;
		}

		if (currentMenuState == LOADING) return;

		//New Game
		if (startScreenSelection == NEWGAME    )
		{
			for (auto player : PlayerEntitys) { delete player;}
			PlayerEntitys.clear();

			OnInitialize();
			currentMenuState = LOADING;
			startScreenSelection = CONTINUE;
			loadingTimer = GetTime() + 3000;
		
		}

		//Continue
		if (startScreenSelection == CONTINUE && gameStarted ) {
			ChangeMode(MODE_GAME);
			currentMenuState = INGAME;
		}

		//Controls
		if (startScreenSelection == CONTROLS) {
			showControllImg = true;
			startScreenSelection = BACK;
		}

		else if (startScreenSelection == BACK && showControllImg) 
		{
			showControllImg = false;
			startScreenSelection = CONTROLS; 
		}

		//Exit
		if (startScreenSelection == EXIT) StopGame();
	}

	//ESC MENU
	if (sym == SDLK_ESCAPE && !IsGameOver())
	{ 
		if (gameStarted && IsMenuMode() && !showControllImg) 
		{
			ChangeMode(MODE_GAME);
			currentMenuState = INGAME;
		}
		else 
		{
			//stop footsteps ....
			for (auto currentPlayer : PlayerEntitys) if (currentPlayer->isPlayerTurn) currentPlayer->footsteps.Stop();
	 
			if (showControllImg)
			{
				showControllImg = false;
				startScreenSelection = CONTROLS;
			}
			else 
			{
				ChangeMode(MODE_MENU);
				currentMenuState = MENU;
			}
		}
	}
}



//*** MOUSE Update Player Stats
void CMyGame::OnLButtonDown(Uint16 x,Uint16 y)
{
	if (!IsMenuMode())
	{
		for (auto currentPlayer : PlayerEntitys)
		{
			if (currentPlayer->isPlayerTurn) currentPlayer->OnLButtonDown(x,y);
		}
	}
}

void CMyGame::OnMouseMove(Uint16 x, Uint16 y, Sint16 relx, Sint16 rely, bool bLeft, bool bRight, bool bMiddle)
{
	//mousePointer.SetPosition(x, y);
	for (auto currentPlayer : PlayerEntitys)
	{
		if (currentPlayer->isPlayerTurn) currentPlayer->OnMouseMove(x, y);
	}
}