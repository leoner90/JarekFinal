#pragma once

class Inventory
{
public:
 
	void init();
	void OnUpdate(Uint32 t);
	void OnDraw(CGraphics* g);
 
	void OnKeyDown(SDLKey sym);
	void OpenShop();
	void CloseInventory();
	int selectWeapon(Uint16 x, Uint16 y);
	void OnLButtonDown(Uint16 x, Uint16 y);
	enum weaponTypes {AXE, MISSILE, BOMB, BANANA, DYNAMIT, MINE, UZI, BOW, MAIL, SKIP};
	//ammo amount
	int mailBomb;
private:
	CSprite InventoryBg;
	bool isShopOpen;
};

 