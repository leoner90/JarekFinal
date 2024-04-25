#include "stdafx.h"
#include "../headers/PlayerEntity/PlayerEntity.h"
#include "../headers/Map.h"
 
PlayerEntity::PlayerEntity()
{
	enemySprite = new CSprite();
}

PlayerEntity::~PlayerEntity()
{
	delete enemySprite;
	delete enemyHpBarRect2;
	//delete deathExplosionSprite;
}

void PlayerEntity::init(int xPos, int yPos, int typeofPlayer)
{
	Inventory::init();
	//Enemy Sprite
	enemySprite = new CSprite();
	isPlayerTurn = false;
	isInventoryOpen = false;
	weaponSelected = false;
	isOnStartOfTheTurn = true;
	turnStartTimer = 0;
	DeathTimer = DeathExploditionTimer = 0;
	weaponType = -1;
	aimAngle = 0;
	inDamageTimer = 0;
	showTurnArrowTimer = 0;
	infoMsgTimer = 0;
 

	if (typeofPlayer == 0) isFriend = false;
	if (typeofPlayer == 1) isFriend = true;
	initEnemyPos = { float(xPos), float(yPos) };

 

	//reset Basic
	isDead = false;
	
	//Enemy Stats
 
	CurrentEnemyHealth = maxEnemyHealth = 100 ;
 
	//Animations and images
	initAnimations();

	//default Statuses
	enemySprite->SetStatus(STANDRIGHT);
	enemySprite->SetPosition(initEnemyPos);
	enemySprite->SetAnimation("idleRight", 1);
	enemySprite->SetSpeed(0);
	EnemyDirection = 90;
	TurnTransitStatusTimer = 0;
	EntityWeapon::init();



	deathExplosionSprite = new CSprite();
	deathExplosionSprite->AddImage("explosion.bmp", "deathExplosion", 5, 5, 0, 0, 4, 4, CColor::Black());
	deathExplosionSprite->SetAnimation("deathExplosion", 10);



}

//**************************** UPDATE ****************************
void PlayerEntity::OnUpdate(Uint32 t, MapGen& mapGen, bool Dkey, bool Akey, bool Wkey, bool Skey, bool Fkey, bool& isTurnFinished, std::vector<PlayerEntity*> PlayerEntitys)
{
	CurrentTime = t;
	localMapVar = &mapGen;
	localAllPlayersEntitys = PlayerEntitys;
	localIsTurnFinished = &isTurnFinished;

	if (enemySprite->GetY() < 0)
	{
		if (isPlayerTurn)isTurnFinished = true;
		isDead = true;
		deadSound.Play("UnderWaterDeath.wav");
		deadSound.Volume(0.15);
		return;

	}

	if (((TurnTransitStatusTimer != 0 && TurnTransitStatusTimer < t) || turnEndTimer  <= 0) && turnStartTimer != 0 && isPlayerTurn && DeathTimer == 0)
	{
		if(IsInAttackChargeMode) EntityWeapon::OnKeyUp(SDLK_SPACE);
		IsInAttackChargeMode = false;
		enemySprite->SetSpeed(0);
		footsteps.Stop();
		isTurnFinished = true;
		TurnTransitStatusTimer = 0;
		turnStartTimer = 0;
		isInTurnTransitStatus = false;
		isPlayerTurn = false;
		weaponSelected = false;
		weaponType = -1;
		isOnStartOfTheTurn = true;
		showTurnArrowTimer = 0;
		cout << "TURN DONE";
		enemySprite->SetStatus(NONE); // save animation if on hit
	}

	// isOnStartOfTheTurn

	if (isOnStartOfTheTurn && isPlayerTurn)
	{
		isOnStartOfTheTurn = false;
		showTurnArrowTimer = CurrentTime + 4000;
		StartRoundSound.Play("StartRound.wav");
		//reset Turn Timer
		turnStartTimer = 20000 + t;
	}
	if (isPlayerTurn) turnEndTimer = (int)(turnStartTimer - t) / 1000;



	if (enemySprite->GetStatus() == ONHIT && inDamageTimer < t)
	{
		cout << "ONHIT DONE";
		enemySprite->SetStatus(ONHIT); //dosen't change animation ............. why for curent Turn player?
		if (EnemyDirection == 90) enemySprite->SetStatus(STANDRIGHT);
		else enemySprite->SetStatus(STANDLEFT);
		inDamageTimer = 0;
	}










	EntityWeapon::OnUpdate(CurrentTime, localAllPlayersEntitys, *enemySprite, weaponType, mapGen, EnemyDirection, aimAngle);
	EntityAnimation::EnemyAnimation(old_animation_status, enemySprite, weaponSelected, weaponType, EnemyDirection, aimAngle);
	old_animation_status = enemySprite->GetStatus();



	// ** COllisions
	EntityPhysic::playerCollision(CurrentTime, playerState, enemySprite,pos, &mapGen);

	// ** Enemy Controller
 
	EntityPhysic::playerControler(EntityWeapon::IsInAttackChargeMode, playerState, enemySprite, isPlayerTurn, isInTurnTransitStatus, isInventoryOpen, Dkey, Akey, Wkey, Skey, Fkey, EnemyDirection, aimAngle);



	//Enemy position
	pos = enemySprite->GetPos();
	enemySprite->Update(CurrentTime);

	if (weaponSelected && !isInventoryOpen && weaponType != AXE && weaponType != MAIL )
	{
		CVector side;
		if (EnemyDirection == 90)	side = { 60, aimAngle };
		else  side = { -60, aimAngle };
		aimPointer.SetPos(enemySprite->GetPos() + side);
	}


	playerPickUpItem();


	deathExplosionSprite->Update(CurrentTime);
	deathHandler();
 
	// ** Interface
	EnemyInterface();

}


//**************************** DRAW ****************************
void PlayerEntity::OnDraw(CGraphics* g)
{

	showInfoMsg(g);
	if(DeathExploditionTimer != 0) deathExplosionSprite->Draw(g);
 

	//HealthBar
	if(CurrentEnemyHealth > 0 && isFriend) enemyHpBarRect->Draw(g);
	else if (CurrentEnemyHealth > 0 && !isFriend)  enemyHpBarRect2->Draw(g);
	

	EntityWeapon::OnDraw(g);

	if(!isDead && DeathExploditionTimer == 0 ) enemySprite->Draw(g);

	//curent turn Arrow
	if (showTurnArrowTimer > CurrentTime)
	{
		showTurnArrow.SetPosition(enemySprite->GetX(), enemySprite->GetTop() + 35 + rand() % 10);
		showTurnArrow.Draw(g);
	}

	//Show Turn Timer if isPlayerTurn && not a final cutscene
	if (isPlayerTurn && !hideTimer)
	{
		Inventory::OnDraw(g);
		*g << font(52) << color(turnEndTimer < 6 ? CColor::Red() :CColor::White()) << xy(0, 985) << center <<  turnEndTimer ;
	}

	if (!isDead && isPlayerTurn && (weaponSelected || isInventoryOpen) && (weaponType != MAIL || isInventoryOpen)  && !isInTurnTransitStatus) aimPointer.Draw(g);

 
	


}
 
//***** Health Bar
void PlayerEntity::EnemyInterface()
{
	float baseHpBarWidth = 50;



	float hpBarSize = baseHpBarWidth * (CurrentEnemyHealth / maxEnemyHealth);
	if (hpBarSize < 0) hpBarSize = 0;

	enemyHpBarRect2->SetSize(hpBarSize, 15);
	enemyHpBarRect2->Update(CurrentTime);

	enemyHpBarRect->SetSize(hpBarSize, 15);
	enemyHpBarRect->Update(CurrentTime);

	enemyHpBarRect2->SetPosition(enemySprite->GetX(), enemySprite->GetTop() + 20);
	enemyHpBarRect->SetPosition(enemySprite->GetX(), enemySprite->GetTop() + 20);
}


void PlayerEntity::playerPickUpItem()
{
	for (auto loot : localMapVar->lootList)
	{
		if (enemySprite->HitTest(loot))
		{
			pickUp.Play("pickUp.wav");
			CurrentEnemyHealth += 50;
			infoMsg = 50;
			infoMsgTimer = CurrentTime + 2000;
			if (CurrentEnemyHealth > maxEnemyHealth) CurrentEnemyHealth = maxEnemyHealth;
			loot->Delete();
		}
	}
}

//**** INITIAL SPRITES DEPENDING ON TYPE OF ENEMY
void PlayerEntity::initAnimations()
{
	EntityAnimation::initAnimations(enemySprite);
	//aim
	aimPointer.LoadImage("mousePointer.png");
	aimPointer.SetImage("mousePointer.png");
	aimPointer.SetSize(30, 30);

	//showTurnArrow
	showTurnArrow.LoadImage("showTurnArrow.png");
	showTurnArrow.SetImage("showTurnArrow.png");
	showTurnArrow.SetSize(30, 30);

	//infoMsgBg
	infoMsgBg.LoadImage("infoMsgBg.png");
	infoMsgBg.SetImage("infoMsgBg.png");
	infoMsgBg.SetSize(60, 40);



}

void PlayerEntity::deathHandler()
{
	if (enemySprite->GetStatus() == DEAD && DeathTimer < CurrentTime && DeathTimer != 0)
	{
		DeathTimer = 0;
		DeathExploditionTimer = CurrentTime + 1000;
		deathExplosionSprite->SetPosition(enemySprite->GetX(), enemySprite->GetY());
	 
		deadSound.Play("Explosion.wav");
		deadSound.Volume(0.3);
		localMapVar->addGrave(enemySprite->GetX(), enemySprite->GetY());
		isDead = true;
		if (isPlayerTurn) *localIsTurnFinished = true;
	}
}

void PlayerEntity::EndOfTurn()
{

}


//**FINAL WHRN GAME WON SPRITES UPDATE
void PlayerEntity::GameWonPlayerUpdate(float t)
{
	hideTimer = true;
	enemySprite->SetStatus(WINERTEAM);
	EntityAnimation::EnemyAnimation(old_animation_status, enemySprite, weaponSelected, weaponType, EnemyDirection, aimAngle);
	old_animation_status = enemySprite->GetStatus();
	enemySprite->Update(t);
}


void PlayerEntity::showInfoMsg(CGraphics* g)
{
	if (infoMsgTimer > CurrentTime)
	{
		float floatingEffect = (infoMsgTimer - CurrentTime) / 50;


		infoMsgBg.SetPosition(enemySprite->GetCenter().GetX(), enemySprite->GetTop() + 65 - floatingEffect);
		infoMsgBg.SetX(enemySprite->GetCenter().GetX());
		infoMsgBg.Draw(g);

 
		*g << font("AFontPTSerif.ttf") << font(24) << color(infoMsg > 0 ? CColor::Green() : CColor::Red()) << xy(enemySprite->GetCenter().GetX() - 18, enemySprite->GetTop() + 55 - floatingEffect) << infoMsg;
		*g << font("ARIAL.ttf");
	}
}


//ENEMY RECEIVING DAMAGE
void PlayerEntity::EnemyGettingDamage(float damageAmount)
{
	if (enemySprite->GetStatus() == DEAD || CurrentEnemyHealth <= 0) return;
	CurrentEnemyHealth -= damageAmount;
 
	
	infoMsg = -damageAmount;
	infoMsgTimer = CurrentTime + 2000;

	// IF ENEMY DIES , Sets proper Loot and Animations
	if (CurrentEnemyHealth <= 0)
	{
		preDeath.Play("preDeath.wav");
		CurrentEnemyHealth = 0;
		enemySprite->SetStatus(DEAD);
		DeathTimer = CurrentTime + 2000;
		//turnStartTimer += 3000;
		inDamageTimer = 0;

	}
	else
	{  
		hitSound.Play("hitSound.wav");
		inDamageTimer = 2000 + CurrentTime;
		enemySprite->SetStatus(ONHIT);
		enemySprite->SetVelocity(0, 0);
		
	}
}

void PlayerEntity::OnLButtonDown(Uint16 x, Uint16 y)
{
	//Weapon Selection
	if (isInventoryOpen) 
	{
		weaponType = Inventory::selectWeapon(x, y);
		if (weaponType != -1)
		{
			isInventoryOpen = false;
			enemySprite->SetStatus(NONE); // animation reset
			weaponSelected = true;
			weaponSelectedSound.Play("weaponSelected.wav");
			weaponSelectedSound.Volume(75);
		}
	}
}

void PlayerEntity::OnKeyDown(SDLKey sym)
{
	if (!EntityWeapon::IsInAttackChargeMode && enemySprite->GetStatus() != INATTACK && TurnTransitStatusTimer == 0)
	{
		//SHOW HIDE INVENTORY
		if (sym == SDLK_TAB && isInventoryOpen)
		{
			isInventoryOpen = false;
			Inventory::CloseInventory();
		}
		else if (sym == SDLK_TAB)
		{
			//stop footsteps and reset animtaion
			footsteps.Stop();
			if (EnemyDirection == 90) enemySprite->SetStatus(STANDRIGHT);
			else enemySprite->SetStatus(STANDLEFT);
			enemySprite->SetSpeed(0);

			//open inventory
			isInventoryOpen = true;
			Inventory::OpenShop();
			openInventorySound.Play("openInventory.wav");
			openInventorySound.Volume(200);

		}

		//ON SPACE BAR PRESSED
		if (weaponSelected)
		{
			//stop footsteps and reset animtaion
			footsteps.Stop();
			enemySprite->SetSpeed(0);

			EntityWeapon::OnKeyDown(sym);
			if (weaponType != AXE)
			{
				if (EnemyDirection == 90) enemySprite->SetStatus(STANDRIGHT);
				else enemySprite->SetStatus(STANDLEFT);
			}
		}



		if (sym == SDLK_f && playerState != INAIR)
		{
			enemySprite->SetPosition(enemySprite->GetPosition() + CVector(0, 100)); // use set Y Velocity bettter???
			enemySprite->SetStatus(JUMP);
			jumpSound.Play("jump.wav");
			jumpSound.Volume(0.5);
		}
	}

	
}

void PlayerEntity::OnKeyUp(SDLKey sym)
{

	if (weaponSelected && sym == SDLK_SPACE && TurnTransitStatusTimer == 0)
	{
			
			EntityWeapon::OnKeyUp(sym);
			TurnTransitStatusTimer = CurrentTime + 2500;
			turnStartTimer = CurrentTime + 2500;
			isInTurnTransitStatus = true;

			//RESET AMMO IN HAND -> becouse it was thrown
			if (weaponType == BOMB || weaponType == BANANA || weaponType == MINE || weaponType == DYNAMIT)
			{
				weaponSelected = false;
				weaponType != -1;
				enemySprite->SetStatus(NONE);
			}
		
	}
}


void PlayerEntity::OnMouseMove(Uint16 x, Uint16 y)
{
	if(isInventoryOpen) aimPointer.SetPosition(x, y);

	if (weaponType == MAIL && !isInventoryOpen) EntityWeapon::OnMouseMove(x,y);  aimPointer.SetPosition(x, y);
}