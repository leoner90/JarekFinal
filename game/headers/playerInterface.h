#pragma once
 

class PlayerInterface
{
public:
	//Base Methods
	void init(int screenWidth, int screenHeight);
	void OnUpdate();
	void OnDraw(CGraphics* g, int screenWidth, int screenHeight);

private:
	void SpriteInits();

	//variable miroring
	float localScreenHeight, localScreenWidth;

};