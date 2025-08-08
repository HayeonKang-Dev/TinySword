#pragma once

#ifndef CASTLEMANAGER_H
#define CASTLEMANAGER_H


#include "Actor.h"
#include "Protocol.h"
#include <array>
#include "IocpServer.h"
#include <iostream>



class CastleManager {
protected:
	// CRITICAL_SECTION g_CriticalSection;

public:
	//CastleManager() {
	//	//InitializeCriticalSection(&g_CriticalSection);
	//}
	//~CastleManager() {
	//	//DeleteCriticalSection(&g_CriticalSection);
	//}



	class CastleObject : public Actor {
	public:
		CastleObject(int hp, Vector location, short tagid) : Actor(hp, location, tagid) {}
		virtual ~CastleObject() {}
	};

private:
	std::map<short, CastleObject*> CastleEntry;
	static CastleManager* instance; 

	CastleManager() {}
	//~CastleManager() {}


public:

	static CastleManager* getInstance() {
		if (instance == nullptr) instance = new CastleManager();
		return instance;
	}




	CastleObject* FindCastleByTagId(short tagId) {
		//LockGuard lock(g_CriticalSection);

		auto it = CastleEntry.find(tagId); 
		if (it == CastleEntry.end()) return nullptr; 
		return it->second;
	}

	Vector GetCastleLocationByTagId(short tagId) {
		//LockGuard lock(g_CriticalSection);
		return CastleLocations[tagId];
	}
	std::map<short, CastleObject*> &GetCastleEntry() {
		//LockGuard lock(g_CriticalSection);
		return CastleEntry;
	}

	CastleObject* CreateCastle(short tagId) { // CastleObject*
		//LockGuard lock(g_CriticalSection);
		CastleObject* castle = new CastleObject(100, CastleLocations[tagId], tagId);
		CastleEntry.insert({ tagId, castle });
		std::cout << "CastleEntry SIZE: " << CastleEntry.size() << std::endl;
		return castle;

		//delete castle;
	}

private:
	std::array<Vector, 5> CastleLocations = {
		Vector(),
		Vector(-6401.79, -5211.76, -317.6),
		Vector(-4080.1, -5207.36, -321.78),
		Vector(-6439.94, -4175.64, -319.88),
		Vector(-4038.44, -4200.43, -323.36)
	};
};


#endif // !CASTLEMANAGER_H