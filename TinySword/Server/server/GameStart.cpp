#include "PlayerObject.h"
#include "SheepManager.h"
#include "BombManager.h"


static bool isSheepThreadRunning = false;


void GameStart() {
	// ��� �÷��̾�� noti 


	std::map<SOCKET, IBindObject*> BindObjects;
	CtxtBindFroms(BindObjects);

	char buffer_noti[sizeof(struct HEAD) + sizeof(struct GameStart::Notification)]{};
	void* encoded = EncodeMessage(buffer_noti, sizeof(buffer_noti), GAMESTART_NOTIFICATION, sizeof(GameStart::Notification));
	GameStart::Notification* noti = (GameStart::Notification*)encoded;
	noti->bStart = 1;
	// Ŭ���̾�Ʈ�� CHARACTERSELECT_NOTIFICATION ������ �ش� �ڸ��� Spawn (Spawn::noti�� ����? )

	for (auto& pair : BindObjects)
	{
		dynamic_cast<PlayerObject*>(pair.second)->Emit(GAMESTART_NOTIFICATION, buffer_noti);
	}

	std::thread sheepThread(SheepManager::TimerThread, SheepManager::getInstance());				

	sheepThread.detach();

	bombManager->InitializeWayPoints();

	// Bomb�� TimerThread ���� -> �̵�/���� ó�� 
	std::thread bombThread(BombManager::TimerThread, BombManager::getInstance());
	bombThread.detach();

	
}