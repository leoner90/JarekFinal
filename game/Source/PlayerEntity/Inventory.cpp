#include "stdafx.h"
#include "../headers/PlayerEntity/Inventory.h"


Inventory::Inventory()
{
	InventoryBg.LoadImage("InventoryBg.png");
	InventoryBg.SetImage("InventoryBg.png");
	InventoryBg.SetSize(400, 700);
	InventoryBg.SetPosition(200, 350);
	isInventoryOpen = false;
}

//*** DRAW
void Inventory::OnDraw(CGraphics* g)
{
	if(isInventoryOpen)InventoryBg.Draw(g);
}

////*** OPEN INVENTORY
void Inventory::OpenInventory()
{
	isInventoryOpen = true;
}

////*** CLOSE INVENTORY
void Inventory::CloseInventory()
{
	isInventoryOpen = false;
}

////*** SELECT WEAPON
int Inventory::selectWeapon(Uint16 x, Uint16 y)
{
	// close first then return, if no return , reopen shop .... Best Logic!!!
	CloseInventory(); 
	if (x >= 70 && x < 180 && y >= 520 && y < 600)	return AXE;
	if (x >= 180 && x < 300 && y >= 520 && y < 600)	return MISSILE;

	if (x >= 70 && x < 180 && y >= 415 && y < 495)	return BOMB;
	if (x >= 180 && x < 300 && y >= 415 && y < 495)	return BANANA;

	if (x >= 70 && x < 180 && y >= 320 && y < 400)	return DYNAMIT;
	if (x >= 180 && x < 300 && y >= 320 && y < 400)	return MINE;

	if (x >= 70 && x < 180 && y >= 225 && y < 300)	return UZI;
	if (x >= 180 && x < 300 && y >= 225 && y < 300)	return BOW;

	if (x >= 70 && x < 180 && y >= 125 && y < 206)	return MAIL;
	if (x >= 180 && x < 300 && y >= 125 && y < 206)	return SKIP;
	 
	//if clicked somewhere else
	OpenInventory();
	return -1;
}