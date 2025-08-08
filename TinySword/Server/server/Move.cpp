#include "PlayerObject.h"
#include "SheepManager.h"



void SendMoveNoti(Move::Request* request, PlayerObject* player, std::map<SOCKET, IBindObject*> BindObjects)
{
	char buffer[sizeof(struct HEAD) + sizeof(struct Move::Notification)]{};
	void* encoded = EncodeMessage(buffer, sizeof(buffer), MOVE_NOTIFICATION, sizeof(struct Move::Notification));
	Move::Notification* noti = (Move::Notification*)encoded;
	noti->MoveActorType = GOBLIN;
	noti->MoveActorTagId = request->MoveActorTagId;
	noti->Location = player->GetLocation();
	noti->bMoveRight = request->bMoveRight;
	noti->bMoveLeft = request->bMoveLeft;
	noti->bMoveDown = request->bMoveDown;
	noti->bMoveUp = request->bMoveUp;
	noti->FacingX = request->FacingX;

	std::cout << "====================> Location: " << player->GetLocation().X << player->GetLocation().Y << std::endl;
	// noti 
	for (auto& pair : BindObjects)
	{
		// send 하기 -> emit? 
		//if (pair.first == socket) continue;
		dynamic_cast<PlayerObject*>(pair.second)->Emit(MOVE_NOTIFICATION, buffer);
	}
}



int MoveExecute(SOCKET socket, LPWSABUF lpWsaBuf)
{
	//std::cout << "ENTERED IN MoveExecute" << std::endl;
	// location 세팅하고, 모든 플레이어에게 노티 
	IBindObject* bindObject = CtxtBindFrom(socket);

	if (bindObject == nullptr)
		return 0;

	struct Move::Request* request = ((struct Move::Request*)(((struct HEAD*)lpWsaBuf->buf) + 1));
	std::map<SOCKET, IBindObject*> BindObjects;
	CtxtBindFroms(BindObjects);

	
	if (request->MoveActorType == GOBLIN)
	{
		//std::cout << "[ACCEPT] GOBLIN MOVE_REQUEST" << std::endl;
		PlayerObject* player = dynamic_cast<PlayerObject*>(bindObject);
		if (!player) {
			std::cout << "Player is NULL! (MOVEEXECUTE) " << std::endl;
			return 0;
		}
		//player->SetLocation()

		Vector prevLocation = player->GetLocation();

		player->SetLocation(request->Location);
		//if ((player->GetLocation() - prevLocation).Length() < 0.0000001f) return 0;

		// Send move::noti 
		SendMoveNoti(request, player, BindObjects);


		{
			char buffer[sizeof(struct HEAD) + sizeof(struct Move::Response)]{};
			void* encoded = EncodeMessage(buffer, sizeof(buffer), MOVE_RESPONSE, sizeof(struct Move::Response));
			Move::Response* rsp = (Move::Response*)encoded;
			rsp->bIsSuccess = 1;
			player->Emit(MOVE_RESPONSE, buffer);
		}

		//if ((player->GetLocation() - prevLocation).Length() < 0.0001f) return 0; //////////////////

		
		return 1;
	}


	//SheepManager sheepManager;
	if (request->MoveActorType == SHEEP) {
		// 서버가 클라에 보낸 목적지가 이동 불가 좌표라 조정했다는 알림 
		// sheep 위치 설정
		SheepManager::SheepObject* sheep = sheepManager->FindSheepByTagId(request->MoveActorTagId);
		sheep->SetLocation(request->Location);

		// move::noti 
		{
			//std::cout << "=========== SEND SHEEP MOVE NOTIFICATION ==============" << std::endl;
			char buffer[sizeof(struct HEAD) + sizeof(struct Move::Notification)]{};
			void* encoded = EncodeMessage(buffer, sizeof(buffer), MOVE_NOTIFICATION, sizeof(struct Move::Notification));
			Move::Notification* noti = (Move::Notification*)encoded;
			noti->MoveActorType = SHEEP;
			noti->MoveActorTagId = request->MoveActorTagId;
			noti->Location = request->Location;
			for (auto& pair : BindObjects) { dynamic_cast<PlayerObject*>(pair.second)->Emit(MOVE_NOTIFICATION, buffer); }
		}

		// 여러 보정 좌표가 올라왔을 때 하나만 선택하는 방법??

		return 1;
	}

	return 0;
}