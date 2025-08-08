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

	// player�� bomb spawn ��û 
	// Ŭ���̾�Ʈ���� player�� coin�� 10 ���ϸ� ��û �Ⱥ����� ��. 

	// request
	struct Spawn::Request* request = ((struct Spawn::Request*)(((struct HEAD*)lpWsaBuf->buf) + 1));


	PlayerObject* player = dynamic_cast<PlayerObject*>(bindObject);
	BombManager::BombObject* bomb = nullptr;

	if (player)
	{
		// 1. coin ���� 
		// 2. response ���� 
		char buffer[sizeof(struct HEAD) + sizeof(struct Spawn::Response)];
		void* encoded = EncodeMessage(buffer, sizeof(buffer), SPAWN_RESPONSE, sizeof(Spawn::Response));
		Spawn::Response* rsp = (Spawn::Response*)encoded;
		rsp->bSuccess = 1;
		rsp->CurrentCoin = player->DecreaseCoin(request->Cost); // #1
		player->Emit(CHARACTERSELECT_RESPONSE, buffer); // #2
		//memset(buffer, 0, sizeof(buffer));

		// 3. Bomb ���� & ��Ʈ���� �ֱ� 
		bomb = bombManager->CreateBomb(80, player->GetSpawnLocation(request->TagId), bombManager->GetNewBombTagId(), player->GetTagId());
		std::cout << ":>>>>>>>>>>>>>>> BOMB TAGID: " << bomb->GetTagId() << std::endl;
	}
	


	// 4, Spawn::Noti ����
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