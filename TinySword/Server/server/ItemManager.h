#pragma once
#ifndef ITEMMANAGER_H
#define ITEMMANAGER_H


#include "Protocol.h"
#include <map>
#include "Actor.h"

class ItemManager {
public:
	//ItemManager() {
	//	//InitializeCriticalSection(&item_cs);
	//}

	//~ItemManager() {
	//	//DeleteCriticalSection(&item_cs);
	//}

protected:
	//mutable CRITICAL_SECTION item_cs;



public:
	class MeatObject {
	public:
		MeatObject(Vector location, short tagid) : Location(location), TagId(tagid) {
			//InitializeCriticalSection(&meat_cs);
		}

		~MeatObject() {
			//DeleteCriticalSection(&meat_cs);
		}

		short GetTagId() const {
			return TagId;
		}

		Vector GetLocation() const {
			return Location;
		}

	protected:
		Vector Location;
		short TagId;

		//mutable CRITICAL_SECTION meat_cs;

	};

	class CoinObject {
	public:
		CoinObject(Vector location, short tagid) : Location(location), TagId(tagid) {}
		
		//~CoinObject() {}

		short GetTagId() const {
			return TagId;
		}
		Vector GetLocation() const {
			return Location;
		}

	protected:
		Vector Location;
		short TagId;

		//mutable CRITICAL_SECTION coin_cs; 
	};

protected:
	/*std::map<short, MeatObject*> MeatEntry;
	std::map<short, CoinObject*> CoinEntry;*/

public:
	MeatObject* FindMeatByTagId(short tagId) {
		return MeatEntry.find(tagId)->second;
	}
	CoinObject* FindCoinByTagId(short tagId) {
		return CoinEntry.find(tagId)->second;
	}

	MeatObject* CreateMeatObject(Vector location, short tagId) {
		
		MeatObject* meat = new MeatObject(location, tagId);
		MeatEntry[tagId] = meat;
		return meat;
	}
	CoinObject* CreateCoinObject(Vector location, short tagId) {
		
		CoinObject* coin = new CoinObject(location, tagId);
		CoinEntry[tagId] = coin;
		return coin;
	}

	short GetNewMeatTagId() {
		short idx = -1;
		if (!MeatEntry.empty()) {
			auto it = MeatEntry.rbegin();
			return it->first + 1;
		}
		return 1;
	}
	short GetNewCoinTagId() {
		short idx = -1;
		if (!CoinEntry.empty()) {
			auto it = CoinEntry.rbegin();
			return it->first + 1;
		}
		return 1;
	}

	void RemoveCoin(short tagId) {
		auto it = CoinEntry.find(tagId);
		if (it != CoinEntry.end())
		{
			delete it->second;
			CoinEntry.erase(it);
		}
	}

	void RemoveMeat(short tagId) {
		auto it = MeatEntry.find(tagId);
		if (it != MeatEntry.end()) {
			delete it->second;
			MeatEntry.erase(it);
		}
	}

private:
	ItemManager(){}
	std::map<short, MeatObject*> MeatEntry;
	std::map<short, CoinObject*> CoinEntry;
	static ItemManager* instance; 

public:
	static ItemManager* getInstance() {
		if (instance == nullptr) instance = new ItemManager(); 
		return instance;
	}
};


#endif // !ITEMMANAGER_H