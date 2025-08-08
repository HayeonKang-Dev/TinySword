#pragma once


#ifndef SHEEPMANAGER_H
#define SHEEPMANAGER_H



#include <map>
#include "Actor.h"
#include "Protocol.h"
#include <thread>
#include <queue>
#include <array>
#include "PlayerObject.h"

#include "IocpServer.h"


//extern HANDLE g_hIOCP;


struct SheepPQ {
	int mapKey;
	long long nextTime;

	SheepPQ(short mapKey, long long time) : mapKey(mapKey), nextTime(time) {}

	BOOL operator <(const SheepPQ& s) const { return this->nextTime > s.nextTime; }
};

//extern std::array<Vector, 5> SheepInitialLocations;




class SheepManager {
public:

	/*SheepManager() {
	}*/
	/*~SheepManager() {
	}*/


public: 

	class SheepObject : public Actor { // IBindObject

	public:
		SheepObject(int hp, Vector location, short tagid) : Actor(hp, location, tagid) {}
		//~SheepObject() {}

		Vector SetLocation(Vector newLocation) {
			return Location = newLocation;
		}
	};

protected:
	/*std::map<short, SheepObject*> SheepEntry;*/
	std::priority_queue<SheepPQ> SheepsPQ;

	//mutable CRITICAL_SECTION manager_cs;


public:
	SheepObject* FindSheepByTagId(short tagId) {
		//LockGuard lock(g_CriticalSection);
		auto it = SheepEntry.find(tagId); 
		if (it == SheepEntry.end()) return nullptr; 
		return it->second;
	}

	SheepObject* CreateSheep(short tagId) {
		if (tagId > 4) return nullptr;
		SheepObject* sheep = new SheepObject(50, GetSpawnLocation(tagId), tagId);
		SheepEntry[tagId] = sheep;
		//delete sheep; 
		
		return sheep;
	}

	Vector GetSpawnLocation(short tagId) {
		//LockGuard lock(g_CriticalSection);
		return SheepInitialLocations[tagId];
	}

	std::map<short, SheepObject*> &GetSheepEntry() { return SheepEntry; }



	// Move::Noti로 위치를 받은 클라에서는 해당 위치가 Nav Mesh 안인지, 밖인지 판단 후 위치 조정
	// 양이 이동하다가 서버에서 받은 위치까지 이동이 불가해 주변 위치로 이동조정했다면, 서버에 requeset 전송
	// Move::Request를 받은 서버는 클라 모두에 노티 
	// cf. 만약 여러 클라에서 Move::Request(보정좌표) 올라오면, 그 중 하나를 골라 전송 

	// Get Random Location (0.5초 간 이동 가능한 주변 위치 구하기)
	Vector GetRandomLocation(const Vector& currentLocation, float speed, float time, float maxRadius)
	{
		float distance = speed * time;

		float randomX = (rand() / (float)RAND_MAX) * 2 - 1;
		float randomY = (rand() / (float)RAND_MAX) * 2 - 1;

		float length = sqrt(randomX * randomX + randomY * randomY);
		randomX /= length;
		randomY /= length;

		Vector newLocation;
		newLocation.X = currentLocation.X + randomX * distance;
		newLocation.Y = currentLocation.Y + randomY * distance;
		newLocation.Z = currentLocation.Z; 

		return newLocation;
	}






	// Timer Thread
	static void TimerThread(void* This) {
		using namespace std::chrono;
		auto lastCallTime = steady_clock::now();
		const int intervalSeconds = 1;

		SheepManager* _this = (SheepManager*)This;

		// get all player
		std::map<SOCKET, IBindObject*> BindObjects;
		CtxtBindFroms(BindObjects);

		long long currentTime = GetTickCount64();
		short i = 1;
		while (_this->SheepsPQ.size() < 5) {
			// 양 생성 & entry에 추가
			_this->CreateSheep(i);
			_this->SheepsPQ.push(SheepPQ(i++, currentTime + 800));
			///////// client에서 양이 똑같은 시기에 움직이는 것 같으면 이 800 수정해야 함. 
		}



		while (TRUE)
		{
			// wait for the timer interval 
			//Sleep(1000);

			long long currentTime = GetTickCount64();

			while (!_this->SheepsPQ.empty()) {
				SheepPQ r = _this->SheepsPQ.top();

				if (r.nextTime > currentTime) { break; }

				_this->SheepsPQ.pop();

				// 0.8초 마다 Move::Noti 
				char buffer[sizeof(struct HEAD) + sizeof(struct Move::Notification)]{};
				void* encoded = EncodeMessage(buffer, sizeof(buffer), MOVE_NOTIFICATION, sizeof(Move::Notification));
				Move::Notification* noti = (Move::Notification*)encoded;

				SheepObject* sheep = _this->FindSheepByTagId(r.mapKey);

				if (sheep == nullptr) continue;
				if (sheep->IsDead())  continue; 

				//std::cout << "========================= SEND SHEEP MOVE NOTIFICATION ========================= " << std::endl;

				Vector newLocation = _this->GetRandomLocation(sheep->GetLocation(), 80.0f, 0.8f, 200.0f);
				sheep->SetLocation(newLocation);
				noti->Location = newLocation;
				noti->MoveActorType = SHEEP;
				noti->MoveActorTagId = sheep->GetTagId();
				
				for (auto& pair : BindObjects) { dynamic_cast<PlayerObject*>(pair.second)->Emit(MOVE_NOTIFICATION, buffer); }

				// 다시 삽입 
				r.nextTime = currentTime + 1100; // 800(moving) + 300(idle) 
				_this->SheepsPQ.push(r);

			}
		}
	}
private:

	SheepManager() {}
	//~SheepManager() {}
	std::map<short, SheepObject*> SheepEntry;
	static SheepManager* instance; 



	std::array<Vector, 5> SheepInitialLocations = {
	Vector(),
	Vector(-5976.53, -4759.65,-280.68),
	Vector(-5087.76, -4739.65, -280),
	Vector(-4638.37, -4130.43, -280),
	Vector(-5976.53, -4279.65,-280)
	};

public: 
	static SheepManager* getInstance() {
		if (instance == nullptr) instance = new SheepManager(); 
		return instance; 
	}

};

extern SheepManager* sheepManager; 

#endif // !SHEEPMANAGER_H