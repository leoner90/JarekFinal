#pragma once

class MapGen;

class EntityWeapon
{
public:
	void performShot(CSprite* enemySprite, int weaponType, bool& IsInAttackChargeMode, float& chargeAttackTimer, float CurrentTime, float& maxChargeAttackTime, CSprite& chargedAttackBar,
		float EnemyDirection, float aimAngle, CSpriteList* EnemyShotList, bool* finishTurn);
	void shotsHandler(CSpriteList& EnemyShotList, MapGen& localMapVar, CSpriteList& explosionList, float CurrentTime);
private:
	enum weaponTypes { AXE, MISSILE, BOMB, BANANA, DYNAMIT, MINE, UZI, BOW, MAIL, SKIP };
	enum animation_state { NONE, STANDLEFT, STANDRIGHT  };
	float ProjectileSpeed;

};