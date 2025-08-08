#include "PlayerObject.h"
#include "SheepManager.h"
#include "BombManager.h"


static bool isSheepThreadRunning = false;


void GameStart() {
	// 모든 플레이어에게 noti 


	std::map<SOCKET, IBindObject*> BindObjects;
	CtxtBindFroms(BindObjects);

	char buffer_noti[sizeof(struct HEAD) + sizeof(struct GameStart::Notification)]{};
	void* encoded = EncodeMessage(buffer_noti, sizeof(buffer_noti), GAMESTART_NOTIFICATION, sizeof(GameStart::Notification));
	GameStart::Notification* noti = (GameStart::Notification*)encoded;
	noti->bStart = 1;
	// 클라이언트는 CHARACTERSELECT_NOTIFICATION 받으면 해당 자리에 Spawn (Spawn::noti와 유사? )

	for (auto& pair : BindObjects)
	{
		dynamic_cast<PlayerObject*>(pair.second)->Emit(GAMESTART_NOTIFICATION, buffer_noti);
	}

	std::thread sheepThread(SheepManager::TimerThread, SheepManager::getInstance());				

	sheepThread.detach();

	bombManager->InitializeWayPoints();

	// Bomb의 TimerThread 실행 -> 이동/폭발 처리 
	std::thread bombThread(BombManager::TimerThread, BombManager::getInstance());
	bombThread.detach();

	
}