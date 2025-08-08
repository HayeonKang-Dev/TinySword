#pragma once


#ifndef GOLDMINEMANAGER_H

#define GOLDMINEMANAGER_H

#include "Actor.h"
#include "Protocol.h"
#include <array>
#include "IocpServer.h"
#include <iostream>

//extern std::array<Vector, 3> GoldmineInitialize;

class GoldmineManager {
public:
	//GoldmineManager() {
	//	//InitializeCriticalSection(&gm_cs);
	//}
	//~GoldmineManager() {
	//	//DeleteCriticalSection(&gm_cs);
	//}

	class GoldmineObject : public Actor {
	public:
		GoldmineObject(int hp, Vector location, short tagid) : Actor(hp, location, tagid) {}
		//virtual ~GoldmineObject() {}
	};

protected:
	/*std::map<short, GoldmineObject*> GoldmineEntry;*/

	//CRITICAL_SECTION gm_cs;

public:

	static GoldmineManager* getInstance() {
		if (instance == nullptr) instance = new GoldmineManager(); 
		return instance; 
	}

	GoldmineObject* FindGoldmineByTagId(short tagid) {
		//LockGuard lock(g_CriticalSection);
		//return GoldmineEntry.find(tagid)->second;
		std::cout << "----------------------goldmine cnt : " << GoldmineEntry.size() << std::endl;
		auto it = GoldmineEntry.find(tagid); 
		if (it == GoldmineEntry.end())
		{
			return nullptr; 
		}
		return it->second;

	}


	GoldmineObject* CreateGoldmine(short tagid) {
		//LockGuard lock(g_CriticalSection);
		GoldmineObject* goldmine = new GoldmineObject(100, GoldmineInitialize[tagid], tagid);
		GoldmineEntry.insert({ tagid, goldmine });

		//delete goldmine;
		return goldmine;
	}

	std::map<short, GoldmineObject*> &GetGoldmineEntry() {
		return GoldmineEntry;
	}

private:

	std::map<short, GoldmineObject*> GoldmineEntry;
	static GoldmineManager* instance; 

	GoldmineManager() {}
	//~GoldmineManager() {}



	std::array<Vector, 3> GoldmineInitialize = {
			Vector(),
			Vector(-6236.986725,-4611.879908,-311.833572),
			Vector(-4182.762700, -4597.790993, -311.833572)
	};
};
extern GoldmineManager* goldmineManager; 

#endif // !GOLDMINEMANAGER_H