#include "PlayerObject.h"

#include "Manager.h"

#include "BombManager.h"

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <iomanip>



void SendDeadNoti(Actor* actor, Attack::Request* request, std::map<SOCKET, IBindObject*> BindObjects);
void SendSpawnMeatNoti(Actor* actor, Attack::Request* request, std::map<SOCKET, IBindObject*> BindObjects, Vector DeadLocation);
void SendSpawnCoinNoti(Actor* actor, Attack::Request* request, std::map<SOCKET, IBindObject*> BindObjects);
void SendLoseNoti(short TagId, SOCKET loserSocket, PlayerObject* loser);
void SendWinNoti(short TagId, SOCKET winnerSocket, PlayerObject* winner);
PlayerObject* FindPlayerByTagId(std::map<SOCKET, IBindObject*>& BindObjects, short TargetTagId);
PlayerObject* FindLastPlayer(std::map<SOCKET, IBindObject*>& BindObjects);

PlayerObject* FindPlayerByTagId(std::map<SOCKET, IBindObject*>& BindObjects, short TargetTagId);

int deadPlayerCnt = 0;
int deadCastleCnt = 0;
int deadPlayerSet = 0;

void SendAttackResponse(SOCKET socket);
void SendAttackNoti(Attack::Request* request, std::map<SOCKET, IBindObject*> BindObjects, SOCKET socket, int hp);

float RandomFloatInRange(float min, float max) {
	float range = max - min;
	return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX) / range);
}

int AttackExecute(SOCKET socket, LPWSABUF lpWsaBuf)
{
	std::cout << "[ACCEPT] ATTACK_REQUEST" << std::endl;
	IBindObject* bindObject = CtxtBindFrom(socket);

	if (bindObject == NULL) 
	{
		std::cout << "bindObject IS NULL (IN ATTACK)" << std::endl;
		return 0;
	}
		

	struct Attack::Request* request = ((struct Attack::Request*)(((struct HEAD*)lpWsaBuf->buf) + 1));

	// 모든 플레이어 찾기(map<socket, player*> (noti 전송 위해 socket 필요함)

	std::map<SOCKET, IBindObject*> BindObjects;
	CtxtBindFroms(BindObjects);

	// 데미지 처리 -> ActorType 별로 TagId로 찾아서 데미지 처리 
	Actor* actor = dynamic_cast<Actor*>(bindObject);

	std::cout << "\t\tTarget Actor Type: " << request->TargetActorType << std::endl;
	//ItemManager itemManager;


	SendAttackResponse(socket);



	int hp = 0; 
	//// TARGET ACTOR
	switch (request->TargetActorType)
	{
	case 0: // NONEACTOR
	{
		SendAttackNoti(request, BindObjects, socket, 0);
		break;
	}

	case 1://GOBLIN:
	{
		
		std::cout << "\t\t[Attack] GOBLIN" << std::endl;
		//PlayerObject* player = dynamic_cast<PlayerObject*>(bindObject);
		PlayerObject* player = FindPlayerByTagId(BindObjects, request->TargetTagId);
		if (player)
		{
			player->DecreaseHP(request->Damage);
			hp = player->GetHP();
		}
		

		// Attack::Noti
		SendAttackNoti(request, BindObjects, socket, hp);

		if (player->IsDead()) {
			SendDeadNoti(player, request, BindObjects);

			deadPlayerCnt++;
			std::cout << "DEAD PLAYER CNT: " << deadPlayerCnt << std::endl;
			// deadPlayerCnt == 3 -> request->Attacker is winner
			//if (deadPlayerCnt == 3) {
			//	//GameEnd(request->AttackerTagId);
			//}
			if (castleManager->FindCastleByTagId(request->TargetTagId)->GetHP() == 0)
			{
				deadPlayerSet++;
				SendLoseNoti(request->TargetTagId, player->GetSocket(), player);

				if (deadPlayerSet == 3)
				{
					PlayerObject* lastPlayer = FindLastPlayer(BindObjects);
					SendWinNoti(lastPlayer->GetTagId(), lastPlayer->GetSocket(), lastPlayer);
				}
			}
		}
		break;
	}
	case 4://SHEEP:
	{
		//SheepManager sheepManager;
		std::cout << "\t\t[Attack] SHEEP" << std::endl;
		SheepManager::SheepObject* sheep = sheepManager->FindSheepByTagId(request->TargetTagId);
		sheep->DecreaseHP(request->Damage);

		hp = sheep->GetHP();

		// Attack::Noti
		SendAttackNoti(request, BindObjects, socket, hp);


		if (sheep->IsDead())
		{
			SendSpawnMeatNoti(sheep, request, BindObjects, sheep->GetLocation());
			SendDeadNoti(sheep, request, BindObjects);
			
		}
		break;
	}
	case 2://CASTLE:
	{
		//CastleManager castleManager;
		std::cout << "\t\t[Attack] CASTLE" << std::endl;
		CastleManager::CastleObject* castle = castleManager->FindCastleByTagId(request->TargetTagId); ////////////// NULL 

		if (castle != nullptr) std::cout << ">>>> CASTLE IS NOT NULL <<<<" << std::endl;
		else {
			std::cout << ">>>> CASTLE IS NULL <<<<<<<" << std::endl;
			return 0;
		}
		castle->DecreaseHP(request->Damage);

		hp = castle->GetHP();


		// Attack::Noti
		SendAttackNoti(request, BindObjects, socket, hp);

		if (castle->IsDead())
		{
			deadCastleCnt++;
			SendDeadNoti(castle, request, BindObjects);

			PlayerObject* player = FindPlayerByTagId(BindObjects, request->TargetTagId);
			if (player->GetHP() == 0)
			{
				deadPlayerSet++;
				SendLoseNoti(request->TargetTagId, player->GetSocket(), player);

				if (deadPlayerSet == 3)
				{
					PlayerObject* lastPlayer = FindLastPlayer(BindObjects);
					SendWinNoti(lastPlayer->GetTagId(), lastPlayer->GetSocket(), lastPlayer);
				}
			}
		}
		break;
	}
	case 3://BOMB:
	{
		 
		std::cout << "\t\t[Attack] BOMB" << std::endl;
		BombManager::BombObject* bomb = bombManager->FindBombByTagId(request->TargetTagId);
		bomb->DecreaseHP(request->Damage);


		// Attack::Noti
		SendAttackNoti(request, BindObjects, socket, hp);



		hp = bomb->GetHP();
		if (bomb->IsDead())
		{
			bombManager->RemoveBombFromEntry(actor->GetTagId());
			SendDeadNoti(bomb, request, BindObjects);
		}

		break;
	}
	case 5: //GOLDMINE:
	{
		//GoldmineManager goldmineManager; 
		std::cout << "\t\t[Attack] GOLDMINE: " << request->TargetTagId<< std::endl;
		std::cout << "\t\tGoldmine is null? : " << request->TargetTagId << "\t/ Is in Entry? : " 
			<< goldmineManager->GetGoldmineEntry().count(request->TargetTagId) << std::endl;
		GoldmineManager::GoldmineObject* goldmine = goldmineManager->FindGoldmineByTagId(request->TargetTagId);


		// Attack::Noti
		SendAttackNoti(request, BindObjects, socket, hp);



		if (goldmine != nullptr)
		{
			/*if (std::rand() % 2 == 0 && goldmine->GetHP() > 0)
			{
				SendSpawnCoinNoti(goldmine, request);
			}*/
			SendSpawnCoinNoti(goldmine, request, BindObjects);

			std::cout << "GOLDMINE is not null" << std::endl;
			
			goldmine->DecreaseHP(request->Damage);
			hp = goldmine->GetHP();

			if (goldmine->IsDead())
			{
				SendDeadNoti(goldmine, request, BindObjects);
			}
		}


		break;
	}
	/*default:
		break;*/
	}

	std::cout << "RUNNING ATTACK EXECUTE FUNCTION" << std::endl;

	//{
	//	std::cout << "Attack::Response SENDING..." << std::endl;
	//	// attack response
	//	//char buffer[sizeof(struct Attack::Response)];
	//	char buffer[sizeof(struct HEAD) + sizeof(struct Attack::Response)];
	//	void* encoded = EncodeMessage(buffer, sizeof(buffer), ATTACK_RESPONSE, sizeof(Attack::Response));
	//	Attack::Response* rsp = (Attack::Response*)encoded;
	//	rsp->bSuccess = 1;
	//	dynamic_cast<PlayerObject*>(CtxtBindFrom(socket))->Emit(ATTACK_RESPONSE, buffer);
	//	//memset(buffer, 0, sizeof(buffer));

	//	std::cout << "End Attack::Response SENDING!" << std::endl;
	//}


	// Attack::Noti 전송 
	//{
	//	char buffer[sizeof(struct HEAD) + sizeof(struct Attack::Notification)]{};
	//	void* encoded = EncodeMessage(buffer, sizeof(buffer), ATTACK_NOTIFICATION, sizeof(Attack::Notification));
	//	Attack::Notification* noti = (Attack::Notification*)encoded;
	//	noti->AttackerActorType = request->AttackerActorType;
	//	noti->AttackerTagId = request->AttackerTagId;
	//	noti->AttackLocation = request->AttackLocation;
	//	noti->TargetActorType = request->TargetActorType;
	//	noti->TargetTagId = request->TargetTagId;
	//	noti->TargetLocation = request->TargetLocation;
	//	noti->HP = hp;

	//	for (auto& pair : BindObjects)
	//	{
	//		if (pair.first == socket) continue;
	//		dynamic_cast<PlayerObject*>(pair.second)->Emit(ATTACK_NOTIFICATION, buffer);
	//	}
	//	//memset(buffer, 0, sizeof(buffer));
	//}



	return 0;

}

void SendAttackResponse(SOCKET socket)
{
	std::cout << "Attack::Response SENDING..." << std::endl;
	// attack response
	//char buffer[sizeof(struct Attack::Response)];
	char buffer[sizeof(struct HEAD) + sizeof(struct Attack::Response)]{};
	void* encoded = EncodeMessage(buffer, sizeof(buffer), ATTACK_RESPONSE, sizeof(Attack::Response));
	Attack::Response* rsp = (Attack::Response*)encoded;
	rsp->bSuccess = 1;
	dynamic_cast<PlayerObject*>(CtxtBindFrom(socket))->Emit(ATTACK_RESPONSE, buffer);
	//memset(buffer, 0, sizeof(buffer));

	std::cout << "End Attack::Response SENDING!" << std::endl;
}

void SendAttackNoti(Attack::Request* request, std::map<SOCKET, IBindObject*> BindObjects, SOCKET socket, int hp)
{

	char buffer[sizeof(struct HEAD) + sizeof(struct Attack::Notification)]{};
	void* encoded = EncodeMessage(buffer, sizeof(buffer), ATTACK_NOTIFICATION, sizeof(Attack::Notification));
	Attack::Notification* noti = (Attack::Notification*)encoded;
	noti->AttackerActorType = request->AttackerActorType;
	noti->AttackerTagId = request->AttackerTagId;
	noti->AttackLocation = request->AttackLocation;
	noti->TargetActorType = request->TargetActorType;
	noti->TargetTagId = request->TargetTagId;
	noti->TargetLocation = request->TargetLocation;
	noti->HP = hp;

	for (auto& pair : BindObjects)
	{
		//if (pair.first == socket) continue;
		dynamic_cast<PlayerObject*>(pair.second)->Emit(ATTACK_NOTIFICATION, buffer);
	}

}

void SendDeadNoti(Actor* actor, Attack::Request* request, std::map<SOCKET, IBindObject*> BindObjects) {
	char buffer[sizeof(struct HEAD) + sizeof(struct Dead::Notification)];
	void* encoded = EncodeMessage(buffer, sizeof(buffer), DEAD_NOTIFICATION, sizeof(Dead::Notification));
	Dead::Notification* noti = (Dead::Notification*)encoded;
	noti->DeadActorType = request->TargetActorType;
	noti->DeadActorTagId = request->TargetTagId;
	noti->Location = request->TargetLocation;

	for (auto& pair : BindObjects) {
		dynamic_cast<PlayerObject*>(pair.second)->Emit(DEAD_NOTIFICATION, buffer);
	}
	//memset(buffer, 0, sizeof(buffer));
}

void SendSpawnMeatNoti(Actor* actor, Attack::Request* request, std::map<SOCKET, IBindObject*> BindObjects, Vector DeadLocation) {
	//ItemManager itemManager;
	//SheepManager sheepManager;
	SheepManager::SheepObject* sheep = sheepManager->FindSheepByTagId(request->TargetTagId);


	// create MEAT
	itemManager->CreateMeatObject(sheep->GetLocation(), sheep->GetTagId());

	// spawn noti 
	char buffer[sizeof(struct HEAD) + sizeof(struct Spawn::Notification)]{};
	void* encoded = EncodeMessage(buffer, sizeof(buffer), SPAWN_NOTIFICATION, sizeof(Spawn::Notification));
	Spawn::Notification* noti = (Spawn::Notification*)encoded;
	noti->Location = DeadLocation; // sheep->GetLocation();
	noti->SpawnActorType = MEAT;
	noti->SpawnActorTagId = sheep->GetTagId();
	for (auto& pair : BindObjects) { dynamic_cast<PlayerObject*>(pair.second)->Emit(SPAWN_NOTIFICATION, buffer); }
	//memset(buffer, 0, sizeof(buffer));
}

void SendSpawnCoinNoti(Actor* actor, Attack::Request* request, std::map<SOCKET, IBindObject*> BindObjects)
{
	//ItemManager itemManager;
	// coin 생성&엔트리 추가 -> spawn::noti 
	Vector SpawnLocation = actor->GetLocation();
	SpawnLocation.Y += 60.0;
	float RandomOffsetX = RandomFloatInRange(-30.0f, 30.0f); 
	float RandomOffsetY = RandomFloatInRange(-10.0f, 10.0f);
	SpawnLocation.X += RandomOffsetX;
	SpawnLocation.Y += RandomOffsetY;
	ItemManager::CoinObject* coin = itemManager->CreateCoinObject(SpawnLocation, itemManager->GetNewCoinTagId()); // 위치 조정 예정 
	if (!coin) {
		std::cout << "Coin is NULL!" << std::endl;
	}
	
	std::cout << "Coin is AVAILABLE!!" << std::endl;
	if (coin)
	{
		char buffer[sizeof(struct HEAD) + sizeof(struct Spawn::Notification)]{};
		void* encoded = EncodeMessage(buffer, sizeof(buffer), SPAWN_NOTIFICATION, sizeof(Spawn::Notification));
		Spawn::Notification* noti = (Spawn::Notification*)encoded;
		noti->SpawnActorType = GOLDBAG;
		noti->SpawnActorTagId = coin->GetTagId();
		noti->Location = coin->GetLocation();

		for (auto& pair : BindObjects) { 
			// 시간 출력용
			auto now = std::chrono::system_clock::now();
			std::time_t now_c = std::chrono::system_clock::to_time_t(now);
			std::tm now_tm;
			localtime_s(&now_tm, &now_c);

			std::cout << "[SEND] SPAWN GOLDBAG!"
				<< std::put_time(&now_tm, "%H:%M:%S")
				<< std::endl;

			dynamic_cast<PlayerObject*>(pair.second)->Emit(SPAWN_NOTIFICATION, buffer); 	
		}
	}
}

void SendLoseNoti(short TagId, SOCKET loserSocket, PlayerObject* loser) {
	char buffer[sizeof(struct HEAD) + sizeof(struct Lose::Notification)]{};
	void* encoded = EncodeMessage(buffer, sizeof(buffer), LOSE_NOTIFICATION, sizeof(Lose::Notification));
	Lose::Notification* noti = (Lose::Notification*)encoded;
	noti->LoserTagId = TagId; 

	loser->Emit(LOSE_NOTIFICATION, buffer);
}

void SendWinNoti(short TagId, SOCKET winnerSocket, PlayerObject* winner) {
	char buffer[sizeof(struct HEAD) + sizeof(struct Win::Notification)]{};
	void* encoded = EncodeMessage(buffer, sizeof(buffer), WIN_NOTIFICATION, sizeof(Win::Notification));
	Win::Notification* noti = (Win::Notification*)encoded;
	noti->winnerTagId = TagId;

	winner->Emit(LOSE_NOTIFICATION, buffer);
}


PlayerObject* FindPlayerByTagId(std::map<SOCKET, IBindObject*>& BindObjects, short TargetTagId) {
	for (auto& pair : BindObjects)
	{
		IBindObject* object = pair.second;
		PlayerObject* player = dynamic_cast<PlayerObject*>(object);
		if (player && player->GetTagId() == TargetTagId)
		{
			return player;
		}
	}
	return nullptr;
}

PlayerObject* FindLastPlayer(std::map<SOCKET, IBindObject*>& BindObjects) {
	// 사망한 캐릭터 3일 때 호출할 함수. (한 플레이어만 살아남았음을 가정.)
	for (auto& pair : BindObjects) {
		PlayerObject* player = dynamic_cast<PlayerObject*>(pair.second); 
		if (player->GetHP() > 0) {
			return player; 
		}
	}
	return nullptr;
}