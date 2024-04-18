#include "stdafx.h"
#include "../headers/PlayerEntity/Inventory.h"





void Inventory::init()
{
	InventoryBg.LoadImage("InventoryBg.png");
	InventoryBg.SetImage("InventoryBg.png");
	InventoryBg.SetSize(400, 700);
	InventoryBg.SetPosition(200, 350);

	isShopOpen = false;

}

void Inventory::OnUpdate(Uint32 t)
{
}

void Inventory::OnDraw(CGraphics* g)
{
	if(isShopOpen)InventoryBg.Draw(g);
}

void Inventory::OnKeyDown(SDLKey sym)
{

}

void Inventory::OpenShop()
{
	isShopOpen = true;
}

void Inventory::CloseInventory()
{
	isShopOpen = false;
}

int Inventory::selectWeapon(Uint16 x, Uint16 y)
{
 
	
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
	OpenShop();
	return -1;



}

void Inventory::OnLButtonDown(Uint16 x, Uint16 y)
{
}
