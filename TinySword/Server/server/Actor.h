#pragma once
#include "IocpServer.h"
#include "Protocol.h"
//
//#include "CastleManager.h"
//#include "BombManager.h"
//#include "SheepManager.h"
//#include "GoldmineManager.h"
//
//
//
//extern CastleManager castleManager;
//extern BombManager bombManager;
//extern SheepManager sheepManager;
//extern GoldmineManager goldmineManager;

class Actor : public IBindObject
{
protected:
	int HP; 
	Vector Location;
	short TagId; 

public: 
	Actor(int hp, Vector location, short tagid) : HP(hp), Location(location), TagId(tagid) {}
	//virtual ~Actor(){}

	int DecreaseHP(int Damage) {
		return HP = max(0, HP - Damage);
	}

	bool IsDead() const {
		return HP <= 0;
	}

	short GetTagId() const {
		return TagId; 
	}

	Vector GetLocation() const {
		return Location;
	}

	int GetHP() const {
		return HP;
	}
};