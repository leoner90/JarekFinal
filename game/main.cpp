#include "stdafx.h"
#include "headers/MyGame.h"

CMyGame game;
CGameApp app;

int main(int argc, char* argv[])
{
	//app.DebugMemoryLeaks();
	app.OpenConsole();
	app.OpenWindow(1920, 1080, "WORMS");
	//app.OpenFullScreen(1920, 1080, 32);

	app.SetClearColor(CColor::Black());
	app.Run(&game);
	return(0);
}