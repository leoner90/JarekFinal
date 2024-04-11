#include "stdafx.h"
#include "headers/MyGame.h"
#include "headers/PlayerEntity.h"
#include "headers/Map.h"
#include "headers/playerInterface.h"
 

//Global for class
CMyGame::CMyGame()
{
	mapGen = new MapGen();
	playerInterface = new PlayerInterface();
	playerEntity = new PlayerEntity();
}

/******************************** INIT ********************************/
void CMyGame::OnInitialize()
{
	//Basic reset
	HideMouse();
 
	gameStarted = isGameWon = GameWonConditon = isPlayerDead = false;

	startScreenSelection = NEWGAME;
	deathScreenTimer = 0;
	currentPlayerTeam = 0;

	//menu Music
	menuMusic.Play("menu.wav",44);
	menuMusic.Volume(0.5);
	
	playerInterface->init(GetWidth(), GetHeight());
	mapGen->init(GetHeight(), GetWidth());


	//init sprites for this class only
	initSpritesHandler();

	PlayerEntitys.push_back(new PlayerEntity());
	PlayerEntitys.back()->init(100, 700, 1);

	PlayerEntitys.push_back(new PlayerEntity());
	PlayerEntitys.back()->init(400, 700, 0);

	currentPlayerTurnIndex = 0;
	isLoadingComplite = false;
	isTurnFinished = false;
}


void CMyGame::OnStartLevel(Sint16 nLevel)
{

}



/******************************** UPDATE ********************************/
void CMyGame::OnUpdate()
{	
	if (IsMenuMode() || IsGameOver() || GameWonConditon || !gameStarted) 
	{
	 
		loadingLogo.Update(GetTime());
		if (loadingTimer < GetTime()) isLoadingComplite = true;
		return; //prevent update while in menu and cutscenes
	}
	else 
	{
		// Map Update 
		mapGen->OnUpdate(GetTime(),GetHeight());

		// Interface Update 
		playerInterface->OnUpdate();

		//All player update
		if (isTurnFinished) currentPlayerTurnIndex++;
		if (currentPlayerTurnIndex > 1) currentPlayerTurnIndex = 0;
		int i = 0;
		for (auto player : PlayerEntitys)
		{
			player->isPlayerTurn = false;
			if (currentPlayerTurnIndex == i && !player->isPlayerTurn)
			{
				player->isPlayerTurn = true;
				isTurnFinished = false;
				//mapGen->setCameraToCurrentPlayer(player->enemySprite->GetX());
			}
			player->OnUpdate(GetTime(), *mapGen, IsKeyDown(SDLK_d), IsKeyDown(SDLK_a), IsKeyDown(SDLK_w), IsKeyDown(SDLK_s), isTurnFinished);
			i++;
		}
		
		
		
		//if (isPlayerDead) GameOver();
	}
}

/******************************** DRAW ********************************/
void CMyGame::OnDraw(CGraphics* g)
{
	//*** IF GAME WON OR LOSE
	if (IsGameOver() || GameWonConditon) {
		BgMusic.Stop(); 
		gameStarted = false;
		float resetTimer = GameWonConditon ? 20500 : 2000;

		if (GameWonConditon) gameWon->Draw(g);
		else gameOver->Draw(g);
	}

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
		
		//reset world Scroll to zero
		//g->SetScrollPos(0, 0);

		playerInterface->OnDraw(g, GetWidth(), GetHeight());

		for (auto player : PlayerEntitys) player->OnDraw(g);

		
	}

	//mousePointer.Draw(g);
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

//************************************* INIT SPRITES *************************************
void CMyGame::initSpritesHandler()
{
//gameOverScreen
	delete gameOver;
	gameOver = new CSprite();
	gameOver->LoadImage("gameOver.jpg");
	gameOver->SetImage("gameOver.jpg");
	gameOver->SetSize(GetWidth(), GetHeight());
	gameOver->SetPosition(GetWidth() / 2, GetHeight() / 2);

	//gameWon
	delete gameWon;
	gameWon = new CSprite();
	gameWon->LoadImage("cutscene3.jpg");
	gameWon->SetImage("cutscene3.jpg");
	gameWon->SetSize(GetWidth(), GetHeight());
	gameWon->SetPosition(GetWidth() / 2, GetHeight() / 2);

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

	if (!IsMenuMode() && (sym == SDLK_F11))
	{
		currentPlayerTurnIndex++;
		if (currentPlayerTurnIndex > 1) currentPlayerTurnIndex = 0;
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

	//*** Char STATS (C Btn)
	if (gameStarted &&  currentMenuState != MENU && sym == SDLK_c && !IsGameOver() )
	{
		if (gameStarted && IsMenuMode()) 
		{
			ChangeMode(MODE_GAME);
			HideMouse();
		}
		else 
		{
 
			ShowMouse();
			ChangeMode(MODE_MENU);
 
			currentMenuState = CHARSTATS;
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