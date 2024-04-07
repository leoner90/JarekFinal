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
	bool GameWonConditon;
	bool gameStarted;
	bool isGameWon;
	bool isPlayerDead;
	int currentPlayerTurnIndex;

	// how long to show dead / Win screen img  
	float deathScreenTimer; // saves current time + resetTimer
	float resetTimer; // actual waiting time

	//Map Class
	MapGen* mapGen;

	PlayerEntity* playerEntity;
	//PlayerInterface Class
	PlayerInterface* playerInterface;

	//Sprites todo NEW pointers
	void initSpritesHandler();
	CSprite* gameOver, *gameWon, *star, *LocationCompass;
	CSprite* startScreen;

	//Menu / Cutscenes / Char stats
	void menuHandler(CGraphics* g);
	int startScreenSelection;
	bool showControllImg;
	int currentMenuState;
	enum game_menu_state { MENU, CHARSTATS, INGAME };
	enum menuScreenSelection { CONTINUE, NEWGAME, CONTROLS, EXIT, BACK };

 
	//Spawn Enemies at certain point
	void enemyCreator(int enemyList[][7]);
	void GameLevel1Spawn();
	bool spawnLevel2, spawnLevel3;
	void OnStartLevel(Sint16 nLevel);



	//Sounds
	CSoundPlayer BgMusic;
	CSoundPlayer  menuMusic;
	CSoundPlayer  victorySound;


	//main GFC fun
	virtual void OnUpdate();
	virtual void OnDraw(CGraphics* g);
	virtual void OnInitialize();
	virtual void OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode);
	virtual void OnLButtonUp(Uint16 x, Uint16 y);
};
