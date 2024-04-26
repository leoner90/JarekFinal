#include "stdafx.h"
#include "../headers/MyGame.h"
#include "../headers/PlayerEntity/PlayerEntity.h"
#include "../headers/Map.h"
#include "../headers/GameInterface.h"
 

//Global for class
CMyGame::CMyGame()
{
	mapGen = new MapGen();
	gameInterface = new GameInterface();
	playerEntity = new PlayerEntity();
}

/******************************** INIT ********************************/
void CMyGame::OnInitialize()
{
	//menu Music
	menuMusic.Play("mainMenu.wav", -1);
	menuMusic.Volume(0.15);

	//Basic reset
	HideMouse();

	//init sprites for this class only
	initSpritesHandler();

	//INIT Main Menu Selection
	startScreenSelection = NEWGAME;

}

//ON NEW GAME START
void CMyGame::OnLeveLStart()
{
	//Create Players
	PlayerCreator();

	//RESETS
	PauseGame(false);
	ChangeMode(MODE_MENU); //Dosent stop the game time , have to use PAUSE
	isLoadingComplite = false;
	initAllPlayersAreOnGround = false;
	gameStarted = IsGameWon = false;
	isTurnFinished = true;
	CurentPlayerTeamIndex = 0;
	gameResetTimer = 0;
	teamOneTotalHealth = teamTwoTotalHealth = 0;

	//Main Inits
	gameInterface->init(GetWidth(), GetHeight());
	mapGen->init(GetHeight(), GetWidth());


}

//Checks is the a ground under player on start of the game as map is randomly generated!!
void CMyGame::arePlayersOnTheGround()
{
	int amountOfPlayersOnGround = 0;
	//returns true if player touches the platform
	for (auto player : PlayerEntitys)
		if (player->FindGroundForAllPlayersOnStart(GetTime(), *mapGen)) amountOfPlayersOnGround++;
	
	//if  all players  touches the platform sets initAllPlayersAreOnGround to true, false by default
	if (amountOfPlayersOnGround >= PlayerEntitys.size())
		initAllPlayersAreOnGround = true;
}

/******************************** UPDATE ********************************/
void CMyGame::OnUpdate()
{
	/*** 
		Move player by X axis if there is no ground under it's curent pos, as map is randomly generated
		# DISABLED by Deafault, just didn't have time to test it properly
		# TO ENABLE just put ! in front of initAllPlayersAreOnGround, so check false state
		# You can set init players X pos to 0 at PlayerCreator() to check it in work , as usualy there is no map there
	 ***/
	if (initAllPlayersAreOnGround)
	{
		arePlayersOnTheGround();
		return;
	}
 
	//################ IF MENU MODE
	if (IsMenuMode() || IsGameOver()  || !gameStarted) 
	{
		loadingLogo.Update(GetTime());
		if (loadingTimer < GetTime()) isLoadingComplite = true;
		return;  
	}
	//################ IF GAME OVER
	else if (IsGameWon)
	{
		GameOverCutscene();

	}
	//################ IF IN GAME MODE
	else 
	{
		//Change Turn and Teams
		TurnChangeLogic();

		//PlayersAndGameOverUpdateLogic
		PlayersAndGameOverUpdateLogic();

		// IN GAME MODE Map Update 
		mapGen->OnUpdate(GetTime(), GetHeight(), windStrengthXVel);

		// IN GAME MODE Interface Update 
		gameInterface->OnUpdate(GetTime(), teamOneTotalHealth, teamTwoTotalHealth, windStrengthXVel);
	}
}

/******************************** DRAW ********************************/
void CMyGame::OnDraw(CGraphics* g)
{
	//######### IF MENU MODE OR PAUSE
	if (IsMenuMode() || IsPaused()) 
	{
		if (currentMenuState == MENU)  menuHandler(g);
		if (currentMenuState == LOADING) LoadingScreen(g);
	}

	//######### IF IN GAME!
	else 
	{
		mapGen->OnDraw(g);
		if(!IsGameWon) gameInterface->OnDraw(g, GetWidth(), GetHeight());

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

		//#########  IF GAME OVER 
		if (IsGameWon)
		{
			winnerTeamBg.Draw(g);
			winConditionOverlay.Draw(g);

			//Winner Team Msg
			if(winnerTeamNumber !=0) *g << font(55) << color(CColor::DarkRed()) << xy(GetWidth() / 2 - 120, GetHeight() - 250) << "TEAM " << winnerTeamNumber << " WON!!!";
			else *g << font(55) << color(CColor::DarkRed()) << xy(GetWidth() / 2 - 50, GetHeight() - 250) << "DRAW";
		}
	}
}

//** LOADING SCREEN DRAW
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


//** GAME OVER
void CMyGame::GameFinished(int winnerTeam)
{
	IsGameWon = true;
	winnerTeamNumber = winnerTeam;
	gameResetTimer = GetTime() + 7000;
}


//** GAME OVER CUTSCENE
void CMyGame::GameOverCutscene()
{
	//PLAY WINING SOUND
	if (!victoryApplauds.IsPlaying()) victoryApplauds.Play("victoryApplauds.wav");

	//Player UPDATES FOR GAME OVER ANIMATION
	for (auto player : PlayerEntitys)
	{
		int victoryAnimation = 8;
		player->entitySprite->SetStatus(victoryAnimation);

		//player and map update
		player->GameWonPlayerUpdate(GetTime());

		//makes sure that all dead players are deleted
		if (player->isDead) {
			PlayerEntitys.erase(find(PlayerEntitys.begin(), PlayerEntitys.end(), player));
			delete player;
		}
	}

	//Map Update FOR GAME OVER Weather 
	mapGen->OnUpdate(GetTime(), GetHeight(), windStrengthXVel);

	//IF RESET TIMER < CURRENT TIME , RESET THE GAME AND SHOW MAIN MENU
	if (gameResetTimer < GetTime())
	{
		victoryApplauds.Stop();
		gameResetTimer = 0;
		gameStarted = false;
		currentMenuState = MENU;
		startScreenSelection = NEWGAME;
		ChangeMode(MODE_MENU);
		IsGameWon = false;
	}
}


//** PLAYER TURN CHANGE LOGIC
void CMyGame::TurnChangeLogic()
{
	//IF TURN FINISHED SWITCH TEAM TO OPOSITE
	if (isTurnFinished)
	{
		if (CurentPlayerTeamIndex == 0) CurentPlayerTeamIndex = 1;
		else CurentPlayerTeamIndex = 0;
	}

	//***** Change TURN LOGIC
	for (auto player : PlayerEntitys)
	{
		if (CurentPlayerTeamIndex == player->teamMemberNumber && isTurnFinished)
		{
			// check is turns left for current team
			bool havePlayerTurnsLeft = false;
			for (auto playerForReset : PlayerEntitys)
				if (CurentPlayerTeamIndex == playerForReset->teamMemberNumber && playerForReset->beenUsed == false) havePlayerTurnsLeft = true;


			//if no turn left -> reset all turns
			if (!havePlayerTurnsLeft)
			{
				for (auto playerForResetTurn : PlayerEntitys)
					if (CurentPlayerTeamIndex == playerForResetTurn->teamMemberNumber) playerForResetTurn->beenUsed = false;
			}


			//if this player NOT BEEN USED BEFORE -> set PlayerTurn = true and Generate new Wind Force
			if (player->beenUsed == false)
			{
				bool posOrnegative = rand() % 2;
				windStrengthXVel = rand() % 7; // radnom -7 to + 7
				windStrengthXVel *= posOrnegative > 0 ? 1 : -1;

				player->windStrength = { windStrengthXVel , 0 };
				player->isPlayerTurn = true;
				player->beenUsed = true;
				isTurnFinished = false;
			}
		}
	}
}

//ALL PLAYERS UPDATE , DELETE IF NEEDED && GAME OVER IF ONE THE TEAM HAVE 0 HP LEFT
void CMyGame::PlayersAndGameOverUpdateLogic()
{
	teamOneTotalHealth = teamTwoTotalHealth = 0; // health reset

	// just to make sure that all enemies deleted and animation finished before call GAME OVER
	bool teamOneMemberAlive = false; 
	bool teamTwoMemberAlive = false;
	for (auto player : PlayerEntitys)
	{
		//UPDATE player
		bool movement[5] = { IsKeyDown(SDLK_d), IsKeyDown(SDLK_a),  IsKeyDown(SDLK_w), IsKeyDown(SDLK_s) };
		player->OnUpdate(GetTime(), *mapGen, movement, isTurnFinished, PlayerEntitys);

		// CHECKS TOTAL TEAM HEALTH USED TO DETERMINE GAME OVER CONDITION
		if (player->teamMemberNumber) {
			teamOneMemberAlive = true;
			teamOneTotalHealth += player->CurrentEnemyHealth;
		}
		if (!player->teamMemberNumber)
		{
			teamTwoMemberAlive = true;
			teamTwoTotalHealth += player->CurrentEnemyHealth;
		}

		//IF DEAD => Delete
		if (player->isDead) { // && !player->isPlayerTurn
			PlayerEntitys.erase(find(PlayerEntitys.begin(), PlayerEntitys.end(), player));
			delete player;
		}
	}

	//IF ONE OF THE TEAM OR BOTH HAVE 0 HEALTHS => GAME OVER , RUN CUTSCENE
	if (!teamOneMemberAlive || !teamTwoMemberAlive)
	{
		if (teamOneTotalHealth <= 0 && teamTwoTotalHealth > 0) GameFinished(1);
		else if (teamTwoTotalHealth <= 0 && teamOneTotalHealth > 0) GameFinished(2);
		else GameFinished(0);
	}
}


//CREATE PLAYERS
void CMyGame::PlayerCreator()
{
	//Clear First For New Games Needed
	for (auto player : PlayerEntitys) delete player;
	PlayerEntitys.clear();

	// 0 - is team one , 1 - is team 2
	PlayerEntitys.push_back(new PlayerEntity());
	PlayerEntitys.back()->init(1200, 700, 0);

	PlayerEntitys.push_back(new PlayerEntity());
	PlayerEntitys.back()->init(900, 700, 0);

	PlayerEntitys.push_back(new PlayerEntity());
	PlayerEntitys.back()->init(700, 700, 0);

	PlayerEntitys.push_back(new PlayerEntity());
	PlayerEntitys.back()->init(420, 700, 1);

	PlayerEntitys.push_back(new PlayerEntity());
	PlayerEntitys.back()->init(550, 700, 1);

	PlayerEntitys.push_back(new PlayerEntity());
	PlayerEntitys.back()->init(1400, 700, 1);
}


//******* DRAW MENU & Control Page
void CMyGame::menuHandler(CGraphics* g)
{
	startScreen.Draw(g);
	//CONTROLS PAGE
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

	//MAIN MENU PAGE
	else
	{
		//IF GAME STARTED SWITCH NEW GAME AND CONTINUE POSITION
		if (gameStarted)
		{
			*g << font(52) << color(startScreenSelection == CONTINUE ? CColor::White() : CColor::LightGray()) << xy(885, 650) << "CONTINUE";
			if (startScreenSelection == CONTINUE) mainMenuSelectionLogo.SetPosition(800, 670);
			*g << font(52) << color(startScreenSelection == NEWGAME ? CColor::White() : CColor::LightGray()) << xy(885, 550) << "NEW GAME";
			if (startScreenSelection == NEWGAME) mainMenuSelectionLogo.SetPosition(800, 570);
		}
		else
		{
			if (startScreenSelection == NEWGAME) mainMenuSelectionLogo.SetPosition(800, 670);
			*g << font(52) << color(startScreenSelection == NEWGAME ? CColor::White() : CColor::LightGray()) << xy(885, 650) << "NEW GAME";
			*g << font(52) << color(CColor::DarkGray()) << xy(885, 550) << "CONTINUE";
		}

		*g << font(52) << color(startScreenSelection == CONTROLS ? CColor::White() : CColor::LightGray()) << xy(885, 450) << "CONTROLS";
		if (startScreenSelection == CONTROLS) mainMenuSelectionLogo.SetPosition(800, 470);
		*g << font(52) << color(startScreenSelection == EXIT ? CColor::White() : CColor::LightGray()) << xy(885, 350) << "EXIT";
		if (startScreenSelection == EXIT) mainMenuSelectionLogo.SetPosition(800, 370);

		mainMenuSelectionLogo.Draw(g);
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
	////*** CALL PLAYER ON KEY DOWN IF NOT IN MENU MODE OR PAUSE
	if (!IsMenuMode() && !IsPaused())
	{
		for (auto currentPlayer : PlayerEntitys)
		{
			if (currentPlayer->isPlayerTurn) currentPlayer->OnKeyDown(sym);
		}
	}

	//*** MENU NAVIGATION DOWN
	if ((IsMenuMode() || IsPaused()) && ((sym == SDLK_s) || (sym == SDLK_DOWN)) && !showControllImg)
	{
		startScreenSelection++;
		if (startScreenSelection > 3) startScreenSelection = gameStarted ? 0 : 1; //Change Sequence if game started
	}

	//*** MENU NAVIGATION UP
	if ((IsMenuMode() || IsPaused()) && ((sym == SDLK_w) || (sym == SDLK_UP)) && !showControllImg)
	{
		startScreenSelection--;
		int range = gameStarted ? 0 : 1;
		if (startScreenSelection < range) startScreenSelection = 3;
	}

	//*** MAIN MENU LOGIC
	if ((IsMenuMode() || IsPaused()) && (sym == 13) ) //enter
	{
		//### LOADING COMPLITED
		if (currentMenuState == LOADING && isLoadingComplite)
		{
			ChangeMode(MODE_GAME);
			currentMenuState = INGAME;
			gameStarted = true;
			menuMusic.Stop();
			BgMusic.Play("mainBg.wav", -1);
			BgMusic.Volume(0.15);
		}
		//IN IN LOADING STATE
		if (currentMenuState == LOADING) return;


		//###  NEW GAME
		if (startScreenSelection == NEWGAME)
		{
			OnLeveLStart();
			currentMenuState = LOADING;
			startScreenSelection = CONTINUE;
			loadingTimer = 3000; // as game time is reset after OnInitialize
		}

		//###  CONTINUE THE GAME IF IT'S STARTED
		if (startScreenSelection == CONTINUE && gameStarted ) {
			currentMenuState = INGAME;
			PauseGame(false);
		}

		//###  CONTROLS PAGE LOGIC
		if (startScreenSelection == CONTROLS) {
			showControllImg = true;
			startScreenSelection = BACK;
		}

		else if (startScreenSelection == BACK && showControllImg) 
		{
			showControllImg = false;
			startScreenSelection = CONTROLS; 
		}

		//### EXIT
		if (startScreenSelection == EXIT) StopGame();
	}

	//*** ESC MENU SHOW HIDE, WHEN ESC PRESSED
	if (sym == SDLK_ESCAPE && !IsGameOver())
	{ 
		if (gameStarted && currentMenuState == MENU && !showControllImg)
		{
			PauseGame(false);
			currentMenuState = INGAME;
		}
		else if(currentMenuState != LOADING)
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
				PauseGame(true);
				currentMenuState = MENU;
			}
		}
	}
}


//*************************************MOUSE EVENTS  *************************************
void CMyGame::OnLButtonDown(Uint16 x,Uint16 y)
{
	if (!IsMenuMode() && !IsPaused())
	{
		for (auto currentPlayer : PlayerEntitys)
			if (currentPlayer->isPlayerTurn) currentPlayer->OnLButtonDown(x,y);
	}
}

void CMyGame::OnMouseMove(Uint16 x, Uint16 y, Sint16 relx, Sint16 rely, bool bLeft, bool bRight, bool bMiddle)
{
	for (auto currentPlayer : PlayerEntitys)
		if (currentPlayer->isPlayerTurn) currentPlayer->OnMouseMove(x, y);
}


//************************************* INIT SPRITES *************************************
void CMyGame::initSpritesHandler()
{
	//Menu BG
	startScreen.LoadImage("menuBar.jpg");
	startScreen.SetImage("menuBar.jpg");
	startScreen.SetSize(GetWidth(), GetHeight());
	startScreen.SetPosition(GetWidth() / 2, GetHeight() / 2);

	//Main Menu Scroling Logo
	mainMenuSelectionLogo.LoadImage("star.png");
	mainMenuSelectionLogo.SetImage("star.png");
	mainMenuSelectionLogo.SetSize(110, 90);;

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

	//Game Over Screen Overlay
	winConditionOverlay.LoadImage("winConditionOverlay.png");
	winConditionOverlay.SetImage("winConditionOverlay.png");
	winConditionOverlay.SetSize(GetWidth(), GetHeight());
	winConditionOverlay.SetPosition(GetWidth() / 2, GetHeight() / 2);

	//BG to display Winner Team name
	winnerTeamBg.LoadImage("winnerTeamBg.png");
	winnerTeamBg.SetImage("winnerTeamBg.png");
	winnerTeamBg.SetSize(400, 300);
	winnerTeamBg.SetPosition(GetWidth() / 2, GetHeight() - 200);
}