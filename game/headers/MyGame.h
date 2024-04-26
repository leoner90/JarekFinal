#pragma once

//Forward Class Declaration
class PlayerEntity;
class MapGen;
class GameInterface;

class CMyGame : public CGame
{
public:
	CMyGame();
private:
	//main GFC fun
	virtual void OnUpdate();
	virtual void OnDraw(CGraphics* g);
	virtual void OnInitialize();
	void OnLeveLStart();

	//Keyboard And Mouse Events Handler
	virtual void OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode);
	virtual void OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode);
	virtual void OnLButtonDown(Uint16 x, Uint16 y);
	virtual void OnMouseMove(Uint16 x, Uint16 y, Sint16 relx, Sint16 rely, bool bLeft, bool bRight, bool bMiddle);

	//Sprites INIT
	void initSpritesHandler();

	//Menu & Loading Screen Draw
	void menuHandler(CGraphics* g);
	void LoadingScreen(CGraphics* g);

	//GAME OVER Functions
	void GameFinished(int winnerTeam);
	void GameOverCutscene();

	//TurnChangeLogic
	void TurnChangeLogic();

	//Players Update Logic
	void PlayersAndGameOverUpdateLogic();

	//Create Players
	void PlayerCreator();

	//Move player by X axis if there is no ground under it's curent pos
	void arePlayersOnTheGround();

	//ENTITIES VECTOR - holds All Players
	std::vector<PlayerEntity*> PlayerEntitys;

	//Main Objects
	MapGen* mapGen;
	PlayerEntity* playerEntity;
	GameInterface* gameInterface;

	//Game Conditions
	bool gameStarted;
	bool IsGameWon;

	//Teams Health Used for Game Over Condition
	float teamOneTotalHealth, teamTwoTotalHealth;

	//GAME OVER VARIALBES
	int winnerTeamNumber;

	//loading
	bool isLoadingComplite;
	float loadingTimer;
 
	//Curent Turn Conditions
	bool isTurnFinished;
	int CurentPlayerTeamIndex;

	// how long to show Win & Draw screen img  
	float gameResetTimer;

	//Winds Strenght
	float windStrengthXVel;

	// MENU & CONTROL STATES and Variables
	enum game_menu_state { MENU, LOADING, CHARSTATS, INGAME };
	enum menuScreenSelection { CONTINUE, NEWGAME, CONTROLS, EXIT, BACK };
	int currentMenuState;
	bool showControllImg;
	int startScreenSelection;

	//SPRITES
	CSprite  winConditionOverlay, winnerTeamBg, mainMenuSelectionLogo, LocationCompass;
	CSprite startScreen,loadingScreen;
	CSprite loadingLogo;
	CSprite mousePointer;

	//Sounds
	CSoundPlayer BgMusic;
	CSoundPlayer  menuMusic;
	CSoundPlayer  victoryApplauds;

	// is true when all player are on the ground
	bool initAllPlayersAreOnGround;
};