#include "PlayerObject.h"
#include "BombManager.h"


int SpawnExecute(SOCKET socket, LPWSABUF lpWsaBuf)
{
	std::cout << "!>!>!> Entered in SpawnExecute..." << std::endl;

	



	IBindObject* bindObject = CtxtBindFrom(socket);

	if (bindObject == NULL)
		return 0;

	std::map<SOCKET, IBindObject*> BindObjects;
	CtxtBindFroms(BindObjects);
	std::cout << "BindObjects SIZE: " << BindObjects.size() << std::endl;

	// player의 bomb spawn 요청 
	// 클라이언트에서 player의 coin이 10 이하면 요청 안보내게 함. 

	// request
	struct Spawn::Request* request = ((struct Spawn::Request*)(((struct HEAD*)lpWsaBuf->buf) + 1));


	PlayerObject* player = dynamic_cast<PlayerObject*>(bindObject);
	BombManager::BombObject* bomb = nullptr;

	if (player)
	{
		// 1. coin 감소 
		// 2. response 전송 
		char buffer[sizeof(struct HEAD) + sizeof(struct Spawn::Response)];
		void* encoded = EncodeMessage(buffer, sizeof(buffer), SPAWN_RESPONSE, sizeof(Spawn::Response));
		Spawn::Response* rsp = (Spawn::Response*)encoded;
		rsp->bSuccess = 1;
		rsp->CurrentCoin = player->DecreaseCoin(request->Cost); // #1
		player->Emit(CHARACTERSELECT_RESPONSE, buffer); // #2
		//memset(buffer, 0, sizeof(buffer));

		// 3. Bomb 생성 & 엔트리에 넣기 
		bomb = bombManager->CreateBomb(80, player->GetSpawnLocation(request->TagId), bombManager->GetNewBombTagId(), player->GetTagId());
		std::cout << ":>>>>>>>>>>>>>>> BOMB TAGID: " << bomb->GetTagId() << std::endl;
	}
	


	// 4, Spawn::Noti 전송
	{
		char buffer[sizeof(struct HEAD) + sizeof(struct Spawn::Notification)]{};
		void* encoded = EncodeMessage(buffer, sizeof(buffer), SPAWN_NOTIFICATION, sizeof(Spawn::Notification));
		Spawn::Notification* noti = (Spawn::Notification*)encoded;
		noti->OwnerTagId = request->TagId;
		noti->SpawnActorType = BOMB;
		noti->SpawnActorTagId = bomb->GetTagId();
		noti->Location = bomb->GetLocation();

		for (auto& pair : BindObjects)
		{
			dynamic_cast<PlayerObject*>(pair.second)->Emit(SPAWN_NOTIFICATION, buffer);
		}
	}

	return 0;

}