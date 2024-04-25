#pragma once
class PlayerEntity;
class MapGen;
class PlayerInterface;

class CMyGame : public CGame
{
public:
	CMyGame();
private:
	 
	//Enemies
	std::vector<PlayerEntity*> PlayerEntitys;

	//Game Conditions
	bool gameStarted;
	bool isGameWon;
 
	int currentPlayerTurnIndex;
	bool currentPlayerTeam;
	bool isTurnFinished;

	// how long to show dead / Win screen img  

	float gameResetTimer; // actual waiting time

	//Map Class
	MapGen* mapGen;

	PlayerEntity* playerEntity;
	//PlayerInterface Class
	PlayerInterface* playerInterface;

	//Sprites todo NEW pointers
	void initSpritesHandler();
	CSprite* winConditionOverlay, *winnerTeamBg, *star, *LocationCompass;
	CSprite* startScreen,loadingScreen;
	CSprite loadingLogo;
	CSprite mousePointer;

	//Menu / Cutscenes / Char stats
	void menuHandler(CGraphics* g);
	void LoadingScreen(CGraphics* g);
	void GameFinished(int winnerTeam);
	
	int startScreenSelection;
	bool showControllImg;
	int currentMenuState;

	//loading
	bool isLoadingComplite;
	float loadingTimer;

	enum game_menu_state { MENU,LOADING, CHARSTATS, INGAME };
	enum menuScreenSelection { CONTINUE, NEWGAME, CONTROLS, EXIT, BACK };

 
	//Spawn Enemies at certain point
	//void enemyCreator(int enemyList[][7]);
	//void GameLevel1Spawn();
	bool spawnLevel2, spawnLevel3;
	void OnStartLevel(Sint16 nLevel);



	//Sounds
	CSoundPlayer BgMusic;
	CSoundPlayer  menuMusic;
	CSoundPlayer  victoryApplauds;


	//main GFC fun
	virtual void OnUpdate();
	virtual void OnDraw(CGraphics* g);
	virtual void OnInitialize();
	virtual void OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode);
	virtual void OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode);
	virtual void OnLButtonDown(Uint16 x, Uint16 y);
	virtual void OnMouseMove(Uint16 x, Uint16 y, Sint16 relx, Sint16 rely, bool bLeft, bool bRight, bool bMiddle);

	float windStrengthXVel;


	void playersPosTester(CSprite& player);
	float teamOneTotalHealth, teamTwoTotalHealth;
	int CurentPlayerTeamIndex;

	bool changeTurn;
	bool IsGameWon;
	int winnerTeamNumber;

	
 
};
