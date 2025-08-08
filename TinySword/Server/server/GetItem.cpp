#include "PlayerObject.h"
#include "Manager.h"

int GetItemExecute(SOCKET socket, LPWSABUF lpWsaBuf) {

	std::cout << "Entered in GetItemExecute..." << std::endl;

	IBindObject* bindObject = CtxtBindFrom(socket);


	if (bindObject == NULL)
	{
		std::cout << "bindObject is NULL!!!" << std::endl;
		return 0;
	}
		

	struct GetItem::Request* request = ((struct GetItem::Request*)(((struct HEAD*)lpWsaBuf->buf) + 1));
	PlayerObject* player = dynamic_cast<PlayerObject*>(bindObject);

	// get all player
	std::map<SOCKET, IBindObject*> BindObjects;
	CtxtBindFroms(BindObjects);

	//ItemManager itemManager;

	// coin 먹으면 increasecoin
	if (request->ItemType == GOLDBAG)
	{

		player->IncreaseCoin(request->IncreaseVal);

		//// getitem response 
		//{
		//	char buffer[sizeof(struct HEAD) + sizeof(struct GetItem::Response)]{};
		//	void* encoded = EncodeMessage(buffer, sizeof(buffer), GETITEM_RESPONSE, sizeof(GetItem::Response));
		//	GetItem::Response* rsp = (GetItem::Response*)encoded;
		//	rsp->bSuccess = 1;
		//	rsp->Coin = player->GetCoin();
		//	dynamic_cast<PlayerObject*>(bindObject)->Emit(GETITEM_RESPONSE, buffer);
		//}

		// getitem noti 
		{
			std::cout << ">>>>>>>>>>>>>> IN GETITEM NOTI ..." << std::endl;
			char buffer[sizeof(struct HEAD) + sizeof(struct GetItem::Notification)]{};
			void* encoded = EncodeMessage(buffer, sizeof(buffer), GETITEM_NOTIFICATION, sizeof(GetItem::Notification));
			GetItem::Notification* noti = (GetItem::Notification*)encoded;
			noti->PlayerTagId = request->PlayerTagId;
			noti->ItemType = GOLDBAG;
			noti->ItemTagId = request->ItemTagId;
			noti->Coin = player->GetCoin();
			noti->HP = player->GetHP();
			noti->Location = request->Location;

			 
			for (auto& pair : BindObjects) {
				//if (pair.first == socket) continue;
				dynamic_cast<PlayerObject*>(pair.second)->Emit(GETITEM_NOTIFICATION, buffer);
			}
		}
		// 클라이언트에서 geitem noti 받으면 액터 destroy

		// getitem response 
		{
			char buffer[sizeof(struct HEAD) + sizeof(struct GetItem::Response)]{};
			void* encoded = EncodeMessage(buffer, sizeof(buffer), GETITEM_RESPONSE, sizeof(GetItem::Response));
			GetItem::Response* rsp = (GetItem::Response*)encoded;
			rsp->bSuccess = 1;
			rsp->Coin = player->GetCoin();
			dynamic_cast<PlayerObject*>(bindObject)->Emit(GETITEM_RESPONSE, buffer);
		}


		// Remove Coin
		itemManager->RemoveCoin(request->ItemTagId);
	}


	// meat 먹으면 increasehp
	if (request->ItemType == MEAT)
	{
		std::cout << "> > > > > > > > > > > ITEM TYPE == MEAT" << std::endl;
		player->IncreaseHP(request->IncreaseVal);

		// getitem resposne
		{
			char buffer[sizeof(struct HEAD) + sizeof(struct GetItem::Response)]{};
			void* encoded = EncodeMessage(buffer, sizeof(buffer), GETITEM_RESPONSE, sizeof(GetItem::Response));
			GetItem::Response* rsp = (GetItem::Response*)encoded;
			rsp->bSuccess = 1;
			rsp->Coin = player->GetHP();
			dynamic_cast<PlayerObject*>(bindObject)->Emit(GETITEM_RESPONSE, buffer);
		}

		// getitem noti 
		{
			char buffer[sizeof(struct HEAD) + sizeof(struct GetItem::Notification)]{};
			void* encoded = EncodeMessage(buffer, sizeof(buffer), GETITEM_NOTIFICATION, sizeof(GetItem::Notification));
			GetItem::Notification* noti = (GetItem::Notification*)encoded;
			noti->ItemType = MEAT;
			noti->ItemTagId = request->ItemTagId;
			noti->PlayerTagId = player->GetTagId();
			noti->Coin = player->GetCoin();
			noti->Location = request->Location;
			noti->HP = player->GetHP(); 
			


			// 
			for (auto& pair : BindObjects) {
				//if (pair.first == socket) continue;
				dynamic_cast<PlayerObject*>(pair.second)->Emit(GETITEM_NOTIFICATION, buffer);
			}
		}
		// Remove Meat 
		itemManager->RemoveMeat(request->ItemTagId);
	}

	return 0;
}