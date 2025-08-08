#pragma once


#ifndef BOMBMANAGER_H
#define BOMBMANAGER_H

#include <thread>   // std::this_thread 사용
#include <chrono>   // std::chrono::milliseconds 사용
#include <mutex>


#include "Actor.h"
#include <map>
#include <queue>
#include "Protocol.h"
#include "IocpServer.h"

#include "PlayerObject.h"
#include "Manager.h"

struct BombPQ {
	int mapKey;
	long long nextTime;

	BombPQ() : mapKey(0), nextTime(0) {}
	BombPQ(short mapKey, long long time) : mapKey(mapKey), nextTime(time) {}

	BOOL operator <(const BombPQ& b) const { return this->nextTime > b.nextTime; }
};

// Waypoints array?


extern std::array<Vector, 5> PlayerSpawnLocations;


class BombManager {

protected:
	//CRITICAL_SECTION g_CriticalSection;
	std::mutex PQMutex;

public:
	//BombManager() {} //InitializeCriticalSection(&g_CriticalSection); 
	//~BombManager() {} //DeleteCriticalSection(&g_CriticalSection);
	 


	class BombObject : public Actor {
	public:
		BombObject(int hp, Vector location, short tagid, short colorTagId) : Actor(hp, location, tagid), ColorTagId(colorTagId) {}
		//virtual ~BombObject() {}

		void SetDestination(Vector newDestination) {
			Destination = newDestination;
		}
		Vector GetDestination() const {
			return Destination;
		}

		void SetDestinationTagId(short newDestinationTagId) {
			DestinationTagId = newDestinationTagId;
		}
		short GetDestinationTagId() const {
			return DestinationTagId;
		}

		short GetColorTagId() const {
			return ColorTagId;
		}
		short GetWayPointNum() const {
			return WayPointNum;
		}

		void PlusWayPointNum() {
			WayPointNum++;
		}

		void SetLocation(Vector newLocation) {
			Location = newLocation;
		}

	private:
		Vector Destination;
		short DestinationTagId;
		short ColorTagId;
		int WayPointNum = 0; // 0;
	};

protected:
	std::map<short, BombObject*> BombEntry;
	std::queue<short> ReuseTagId;

	std::priority_queue<BombPQ> BombsPQ;

	std::map < std::pair<short, short>, std::vector <Vector>> WayPoints; // 경유지 배열 

	


private:

	BombManager() {}
	static BombManager* instance; 

public:
	static BombManager* getInstance() {
		if (instance == nullptr) instance = new BombManager(); 
		return instance;
	}

	void InitializeWayPoints() {
		WayPoints[{1, 2}] = {
			Vector(-6401.790f, -5111.760f, -280.642f),
			Vector(-6316.781f, -5071.058f, -280.642f),
			Vector(-6292.292f, -5056.364f, -280.642f),
			Vector(-6204.128f, -5002.486f, -280.642f),
			Vector(-6184.536f, -4992.690f, -280.642f),
			Vector(-5817.186f, -4849.016f, -280.642f),
			Vector(-4667.790f, -4849.016f, -280.642f),
			Vector(-4292.277f, -4992.690f, -280.642f),
			Vector(-4267.787f, -5007.384f, -280.642f),
			Vector(-4181.255f, -5061.262f, -280.642f),
			Vector(-4161.663f, -5071.058f, -280.642f),
			Vector(-4080.100f, -5107.360f, -280.642f)
		};

		WayPoints[{1, 3}] = {
			Vector(-6401.790f, -5111.760f, -280.642f),
			Vector(-6316.781f, -5071.058f, -280.642f),
			Vector(-6292.292f, -5056.364f, -280.642f),
			Vector(-6282.496f, -5036.772f, -280.642f),
			Vector(-6119.229f, -4703.708f, -280.642f),
			Vector(-6091.474f, -4646.565f, -280.642f),
			Vector(-6091.474f, -4564.932f, -280.642f),
			Vector(-6101.270f, -4545.340f, -280.642f),
			Vector(-6341.271f, -4115.949f, -280.642f),
			Vector(-6365.761f, -4101.255f, -280.642f),
			Vector(-6439.940f, -4075.640f, -280.642f)
		};

		WayPoints[{1, 4}] = {
			Vector(-6401.790, -5111.760 ,-280.642),
			Vector(-6316.781 ,-5071.058 ,-280.642), 
			Vector(-6292.292, -5056.364 ,-280.642),
			Vector(-6204.128, -5002.486, -280.642),
			Vector(-6184.536, -4992.690, -280.642),
			Vector(-5817.186, -4849.016, -280.642),
			Vector(-4659.626, -4635.136, -280.642),
			Vector(-4640.034, -4625.340, -280.642),
			Vector(-4132.276, -4137.174, -280.642),
			Vector(-4112.684, -4127.378, -280.642),
			Vector(-4038.440, -4100.430, -280.642)
		};

		WayPoints[{2, 1}] = {
			Vector(-4080.100, -5107.360, -280.642),
			Vector(-4161.663, -5071.058, -280.642),
			Vector(-4181.255, -5061.262, -280.642),
			Vector(-4267.787, -5007.384, -280.642),
			Vector(-4292.277, -4992.690, -280.642),
			Vector(-4667.790, -4849.016, -280.642),
			Vector(-5817.186, -4849.016, -280.642),
			Vector(-6184.536, -4992.690, -280.642),
			Vector(-6204.128, -5002.486, -280.642),
			Vector(-6292.292, -5056.364, -280.642),
			Vector(-6316.781, -5071.058, -280.642),
			Vector(-6401.790, -5111.760, -280.642)
		};

		WayPoints[{2, 3}] = {
			Vector(-4080.100, -5107.360, -280.642),
			Vector(-4161.663, -5071.058, -280.642),
			Vector(-4181.255, -5061.262, -280.642),
			Vector(-4267.787, -5007.384, -280.642),
			Vector(-4292.277, -4992.690, -280.642),
			Vector(-5384.530, -4489.829, -280.642),
			Vector(-5404.122, -4480.033, -280.642),
			Vector(-5649.021, -4282.481, -280.642),
			Vector(-5668.613, -4272.685, -280.642),
			Vector(-6184.536, -4202.480, -280.642),
			Vector(-6204.128, -4192.684, -280.642),
			Vector(-6365.761, -4101.255, -280.642),
			Vector(-6439.940, -4075.640, -280.642)
		};

		WayPoints[{2, 4}] = {
			Vector(-4080.100, -5107.360, -280.642),
			Vector(-4161.663, -5071.058, -280.642),
			Vector(-4181.255, -5061.262, -280.642),
			Vector(-4195.949, -5036.772, -280.642),
			Vector(-4337.991, -4631.871, -280.642),
			Vector(-4337.991, -4550.238, -280.642),
			Vector(-4146.969, -4161.663, -280.642),
			Vector(-4132.276, -4137.174, -280.642),
			Vector(-4112.684, -4127.378, -280.642),
			Vector(-4038.440, -4100.430, -280.642)
		};

		WayPoints[{3, 1}] = {
			Vector(-6439.940, -4075.640, -280.642),
			Vector(-6365.761, -4101.255, -280.642),
			Vector(-6341.271, -4115.949, -280.642),
			Vector(-6101.270, -4545.340, -280.642),
			Vector(-6091.474, -4564.932, -280.642),
			Vector(-6091.474, -4646.565, -280.642),
			Vector(-6119.229, -4703.708, -280.642),
			Vector(-6282.496, -5036.772, -280.642),
			Vector(-6292.292, -5056.364, -280.642),
			Vector(-6316.781, -5071.058, -280.642),
			Vector(-6401.790, -5111.760, -280.642)
		};

		WayPoints[{3, 2}] = {
			Vector(-6439.940, -4075.640, -280.642),
			Vector(-6365.761, -4101.255, -280.642),
			Vector(-6341.271, -4115.949, -280.642),
			Vector(-5836.778, -4625.340, -280.642),
			Vector(-5817.186, -4635.136, -280.642),
			Vector(-4667.790, -4849.016, -280.642),
			Vector(-4292.277, -4992.690, -280.642),
			Vector(-4267.787, -5007.384, -280.642),
			Vector(-4181.255, -5061.262, -280.642),
			Vector(-4161.663, -5071.058, -280.642),
			Vector(-4080.100, -5107.360, -280.642)
		};

		WayPoints[{3, 4}] = {
			Vector(-6439.940, -4075.640, -280.642),
			Vector(-6365.761, -4101.255, -280.642),
			Vector(-6204.128, -4192.684, -280.642),
			Vector(-6184.536, -4202.480, -280.642),
			Vector(-4293.909, -4202.480, -280.642),
			Vector(-4112.684, -4127.378, -280.642),
			Vector(-4038.440, -4100.430, -280.642)
		};

		WayPoints[{4, 1}] = {
			Vector(-4038.440, -4100.430, -280.642),
			Vector(-4112.684, -4127.378, -280.642),
			Vector(-4132.276, -4137.174, -280.642),
			Vector(-4640.034, -4625.340, -280.642),
			Vector(-4659.626, -4635.136, -280.642),
			Vector(-5817.186, -4849.016, -280.642),
			Vector(-6184.536, -4992.690, -280.642),
			Vector(-6204.128, -5002.486, -280.642),
			Vector(-6292.292, -5056.364, -280.642),
			Vector(-6316.781, -5071.058, -280.642),
			Vector(-6401.790, -5111.760, -280.642)
		};

		WayPoints[{4, 2}] = {
			Vector(-4038.440, -4100.430, -280.642),
			Vector(-4112.684, -4127.378, -280.642),
			Vector(-4132.276, -4137.174, -280.642),
			Vector(-4146.969, -4161.663, -280.642),
			Vector(-4337.991, -4550.238, -280.642),
			Vector(-4337.991, -4631.871, -280.642),
			Vector(-4195.949, -5036.772, -280.642),
			Vector(-4181.255, -5061.262, -280.642),
			Vector(-4161.663, -5071.058, -280.642),
			Vector(-4080.100, -5107.360, -280.642)
		};

		WayPoints[{4, 3}] = {
			Vector(-4038.440, -4100.430, -280.642),
			Vector(-4112.684, -4127.378, -280.642),
			Vector(-4293.909, -4202.480, -280.642),
			Vector(-6184.536, -4202.480, -280.642),
			Vector(-6204.128, -4192.684, -280.642),
			Vector(-6365.761, -4101.255, -280.642),
			Vector(-6439.940, -4075.640, -280.642)
		};

	}


	BombObject* FindBombByTagId(short tagId) {
		auto it = BombEntry.find(tagId); 
		if (it == BombEntry.end()) return nullptr; 
		return it->second; 
	}

	static void TimerThread(void* This) {
		using namespace std::chrono;
		/*auto lastCallTime = steady_clock::now();
		const int intervalSeconds = 1;*/

		BombManager* _this = (BombManager*)This;

		// get all players 
		std::map<SOCKET, IBindObject*> BindObjects;
		CtxtBindFroms(BindObjects);

		while (TRUE)
		{
			try {

				std::this_thread::sleep_for(std::chrono::milliseconds(50)); ////////////////////

				long long currentTime = GetTickCount64();
				BombPQ r;

				/*while (!_this->BombsPQ.empty())*/
				//{


				{
					std::lock_guard<std::mutex> lock(_this->PQMutex);

					if (_this->BombsPQ.empty()) continue;

					r = _this->BombsPQ.top();

					if (r.nextTime > currentTime) continue;

					_this->BombsPQ.pop();
				}


				BombObject* bomb = _this->FindBombByTagId(r.mapKey);
				if (!bomb)
				{
					std::cout << "BOMB IS NOT AVAILABLE.........." << std::endl;
					continue;
				}

				// 도착 확인 -> 폭발 & 데미지 적용 
				//if (_this->IsArrive(bomb->GetLocation(), bomb->GetDestination()))
				if (bomb->GetWayPointNum() == _this->WayPoints[{bomb->GetColorTagId(), bomb->GetDestinationTagId()}].size())
				{
					// BOMBEXPLODE_NOTIFICATION 
					_this->HandleBombArrival(bomb);

					std::this_thread::sleep_for(std::chrono::milliseconds(1900)); ///////////////////////

					// 반경 액터 확인: player, sheep, castle
					std::multimap<ActorType, Actor*> DamagedActors;

					for (auto& pair : BindObjects) {
						PlayerObject* player = dynamic_cast<PlayerObject*>(pair.second);
						// player가 폭발 반경 내
						if (bomb->GetLocation().IsWithinRadius(player->GetLocation())) {
							// 폭탄 스폰한 플레이어는 데미지 입지 않음
							//if (bomb->GetTagId() == player->GetTagId()) continue;
							if (bomb->GetColorTagId() == player->GetTagId()) continue; 
							DamagedActors.insert({ GOBLIN, player });
						}
					}

					//SheepManager sheepManager;
					for (auto& pair : sheepManager->GetSheepEntry()) {
						if (bomb->GetLocation().IsWithinRadius(pair.second->GetLocation())) {
							DamagedActors.insert({ SHEEP, pair.second });
						}
					}

					//CastleManager castleManager;
					for (auto& pair : castleManager->GetCastleEntry()) {
						if (bomb->GetLocation().IsWithinRadius(pair.second->GetLocation())) {
							DamagedActors.insert({ CASTLE, pair.second });
						}
					}

					// ATTACK_NOTIFICATION
					std::cout << "BEFORE ATTACK NOTIFICATION <- BOMB" << std::endl;
					for (auto& pair : DamagedActors) {
						char buffer[sizeof(struct HEAD) + sizeof(struct Attack::Notification)]{};
						void* encoded = EncodeMessage(buffer, sizeof(buffer), ATTACK_NOTIFICATION, sizeof(Attack::Notification));
						Attack::Notification* noti = (Attack::Notification*)encoded;
						noti->AttackerActorType = BOMB;
						noti->AttackerTagId = bomb->GetTagId();
						noti->AttackLocation = bomb->GetLocation();
						noti->TargetActorType = pair.first;
						noti->TargetTagId = pair.second->GetTagId();
						noti->TargetLocation = pair.second->GetLocation();
						for (auto& PlayerPair : BindObjects) {
							dynamic_cast<PlayerObject*>(PlayerPair.second)->Emit(ATTACK_NOTIFICATION, buffer);
						}

						// delete bomb : 폭발이후 삭제 
					}
					_this->RemoveBombFromEntry(bomb->GetTagId());
					//std::cout << "AFTER ATTACK NOTIFICATION <- BOMB" << std::endl;
					//if (_this->BombsPQ.empty()) continue; // break;
					continue;
				} // Arrive

				////////
				//if (_this->BombsPQ.empty()) break;

				// 이동 위치 갱신 & MOVE_NOTIFICATION 전송 
				_this->UpdateBombLocation(bomb, r);

				//if (bomb == nullptr) break; // continue


				// 다시 PQ에 입력
				r.nextTime = currentTime + 500;  // 0.5초 뒤에 PQ에서 pop 해서 목적지 업데이트 할 예정.
				{
					std::lock_guard<std::mutex> lock(_this->PQMutex);
					_this->BombsPQ.push(r);
				}

				//}

				/////
			}
			catch (const std::exception& e) {
				std::cerr << "[TimerThread] Exception occurred: " << e.what() << std::endl;
			}
			catch (...) {
				std::cerr << "[TimerThread] Unknown exception occurred" << std::endl;
			}

		}
	}

	/// Bomb 생성 후 Entry에 추가 
	BombObject* CreateBomb(int hp, Vector location, short tagId, short colorTagId) {
		BombObject* bomb;
		{
			bomb = new BombObject(hp, location, tagId, colorTagId);
			BombEntry[tagId] = bomb;
		}


		long long currentTime = GetTickCount64();

		// 목적지 할당 
		bomb->SetDestination(GetDestination(bomb));
		BombsPQ.push(BombPQ(tagId, currentTime + 100)); // 0.1초 뒤 BombsPQ에서 pop 해서 목적지 업데이트 해주세요

		return bomb;
	}

	short GetNewBombTagId() {
		short tagId = -1;
		if (!ReuseTagId.empty()) {
			tagId = ReuseTagId.front();
			ReuseTagId.pop();
		}
		else
		{
			if (!BombEntry.empty()) tagId = BombEntry.rbegin()->first + 1;
			else tagId = 1;
		}

		return tagId;
	}

	// Bomb 찾아 삭제 -> TagId는 ReuseTagId로 push 
	void RemoveBombFromEntry(short tagId) {
		auto it = BombEntry.find(tagId);
		if (it != BombEntry.end())
		{
			ReuseTagId.push(it->second->GetTagId());
			delete it->second;
			BombEntry.erase(it);

		}
	}

	Vector GetDestination(BombObject* bomb) {

		short myTagId = bomb->GetColorTagId(); //GetTagId();
		std::vector<short> candidates;

		// 나와 다른 TagId, 아직 무너지지 않은 castle을 후보군에 추가 
		for (auto& pair : castleManager->GetCastleEntry()) {
			if (pair.first != myTagId && pair.second->GetHP() > 0) {
				candidates.push_back(pair.first);
			}
		}
		// cf. 내 성을 제외한 모든 성이 무너졌을 땐 언리얼에서 SPAWN_REQUEST 전송하지 않음 

		short chosen = candidates[std::rand() % candidates.size()];
		bomb->SetDestinationTagId(chosen);
		std::cout << ">>>>>>>>>>>" << bomb->GetColorTagId() << " -> " << chosen << std::endl;
		return PlayerSpawnLocations[chosen];
	}

	bool IsArrive(Vector currentLocation, Vector Destination) {
		return currentLocation == Destination;
	}

	Vector GetFutureLocation(Vector currentLocation, Vector Waypoint, float speed = 80.0f, float time = 0.5f) {
		Vector direction = Waypoint - currentLocation;
		double distance = speed * time;
		double length = direction.Length();

		if (length < 1e-6) return currentLocation;

		Vector moveVector = direction.Normalize() * distance;

		//double dotProduct = (Waypoint - currentLocation).Dot(moveVector);

		// FutureLocation이 경유지 위치를 지날 땐, 경유지 위치를 반환
		/*if (dotProduct < 0) {
			return Waypoint;
		}*/
		if (distance >= length)
		{
			return Waypoint; 
		}
		return currentLocation + moveVector;
	}



	void HandleBombArrival(BombObject* bomb) {

		std::map<SOCKET, IBindObject*> BindObjects;
		CtxtBindFroms(BindObjects);


		// BOMBEXPLODE_NOTIFICATION 
		char buffer[sizeof(struct HEAD) + sizeof(struct BombExplode::Notification)]{};
		void* encoded = EncodeMessage(buffer, sizeof(buffer), BOMBEXPLODE_NOTIFICATION, sizeof(BombExplode::Notification));
		BombExplode::Notification* noti = (BombExplode::Notification*)encoded;
		noti->Location = bomb->GetLocation();
		noti->TagId = bomb->GetTagId();
		for (auto& pair : BindObjects) { dynamic_cast<PlayerObject*>(pair.second)->Emit(BOMBEXPLODE_NOTIFICATION, buffer); }

	}

	// 이동 위치 갱신 & MOVE_NOTIFICATION 전송 
	void UpdateBombLocation(BombObject* bomb, BombPQ& r) {

		std::cout << "UPDATE BOMB LOCATION IS RUNNING..." << std::endl;
		std::map<SOCKET, IBindObject*> BindObjects;
		CtxtBindFroms(BindObjects);

		// noti->Location = // WayPoints 받아서 할당해야 함.-> client가 서버에 알려야 함. // Bomb SetLocation으로 위치 고쳐줘야 함. 
				// 경유지 배열 가져오기
		std::vector<Vector> wayPoints = WayPoints[{bomb->GetColorTagId(), bomb->GetDestinationTagId()}];



		////////////
		Vector LastLocation = bomb->GetLocation(); 

		// 경유지 위치 
		int wp = bomb->GetWayPointNum(); 
		if (wp < 0 || wp >= wayPoints.size())
		{
			std::cout << "WAYPOINT NUM IS OUT OF BOUND: " << wp <<std::endl;
			return;
		}
		Vector wayPoint = wayPoints[bomb->GetWayPointNum()];

		// 이동 위치 업데이트 
		// 현위치에서 경유지 방향으로 80의 속도로 0.5초 간 움직일 때 위치
		Vector futureLocation = GetFutureLocation(bomb->GetLocation(), wayPoint); ////////////////

		

		bomb->SetLocation(futureLocation);

		if (futureLocation != LastLocation)
		{
			//std::cout << "[MOVE NOTIFICATION - BOMB] LOCATION (X = " << bomb->GetLocation().X << ", Y = " << bomb->GetLocation().Y << ", Z = " << bomb->GetLocation().Z << std::endl;
			char buffer[sizeof(struct HEAD) + sizeof(struct Move::Notification)]{};
			void* encoded = EncodeMessage(buffer, sizeof(buffer), MOVE_NOTIFICATION, sizeof(Move::Notification));
			Move::Notification* noti = (Move::Notification*)encoded;
			noti->Location = futureLocation; //  bomb->GetLocation();
			noti->MoveActorType = BOMB;
			noti->MoveActorTagId = r.mapKey;
			for (auto& pair : BindObjects) { dynamic_cast<PlayerObject*>(pair.second)->Emit(MOVE_NOTIFICATION, buffer); }
			std::cout << "Sending Move Notification: ActorType=" << noti->MoveActorType
				<< ", TagId=" << noti->MoveActorTagId
				<< ", Location: X =" << noti->Location.X<< ", Y = "<<noti->Location.Y<< std::endl;
		}

		// 미래 위치와 경유지가 같을 떄 
		if (futureLocation == wayPoint) {
			bomb->PlusWayPointNum();
			//std::cout << "BOMB WAY POINT NUMBER: " << bomb->GetWayPointNum() << std::endl;
		}

		
	}
};

#endif // !BOMBMANAGER_H