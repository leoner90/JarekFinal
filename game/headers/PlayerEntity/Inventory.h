#pragma once

class Inventory
{
public:
	Inventory();

	//Main Functions
	void OnDraw(CGraphics* g);

	//Inventory Events
	void OpenInventory();
	void CloseInventory();
	int selectWeapon(Uint16 x, Uint16 y);

private:
	//Weapon Statuses
	enum weaponTypes { AXE, MISSILE, BOMB, BANANA, DYNAMIT, MINE, UZI, BOW, MAIL, SKIP };

	//Sprite For Bg
	CSprite InventoryBg;

	//Conditions
	bool isInventoryOpen;
};