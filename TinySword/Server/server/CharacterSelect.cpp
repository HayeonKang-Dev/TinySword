#include "PlayerObject.h"
#include <iostream>

extern short PlayerCnt;
//extern void GameReadyExecute();
extern void GameStart(); 

int CharacterSelectExecute(SOCKET socket, LPWSABUF lpWsaBuf)
{
	// 모든 플레이어 가져오기 
	std::map<SOCKET, IBindObject*> BindObjects;
	CtxtBindFroms(BindObjects);

	

	std::cout << "CHARACTERSELECT EXECUTE is running...: " << PlayerCnt<<std::endl;
	
	struct CharacterSelect::Request* request = ((struct CharacterSelect::Request*)(((struct HEAD*)lpWsaBuf->buf) + 1));


	//PlayerObject* newPlayer = new PlayerObject(socket, 100, PlayerSpawnLocations[request->TagId], request->TagId, 0);

	PlayerObject* newPlayer = static_cast<PlayerObject*>(CtxtBindFrom(socket)); 
	if (newPlayer)
	{
		newPlayer->SetLocation(PlayerSpawnLocations[request->TagId]);
		newPlayer->SetTagId(request->TagId);
	}

	std::cout << "Half in CharacterSelectExecute.." << std::endl;
	//CtxtBindTo(socket, newPlayer); 

	{
		char buffer_rsp[sizeof(struct HEAD) + sizeof(struct CharacterSelect::Response)];
		void* encoded = EncodeMessage(buffer_rsp, sizeof(buffer_rsp), CHARACTERSELECT_RESPONSE, sizeof(CharacterSelect::Response));
		if (!encoded) {
			std::cerr << "Encoding failed for CHARACTERSELECT_RESPONSE" << std::endl;
			return 0;
		}
		///////////////////////////////////
		/*std::cout << "Encoded Packet Data: ";
		for (size_t i = 0; i < sizeof(buffer_rsp); ++i) {
			printf("%02X ", (unsigned char)buffer_rsp[i]);
		}
		std::cout << std::endl;*/
		/////////////////////////////////


		CharacterSelect::Response* rsp = (CharacterSelect::Response*)encoded;
		rsp->bSuccess = 1;

		std::cout << "Success encoded message" << std::endl;

		newPlayer->Emit(CHARACTERSELECT_RESPONSE, buffer_rsp); //////////////////////////
		memset(buffer_rsp, 0, sizeof(buffer_rsp));
	}

	std::cout << "Success Send CHARACTERSELECT_RESPOSNE" << std::endl;

	

	{
		char buffer_noti[sizeof(struct HEAD) + sizeof(struct CharacterSelect::Notification)]{};
		void* encoded = EncodeMessage(buffer_noti, sizeof(buffer_noti), CHARACTERSELECT_NOTIFICATION, sizeof(CharacterSelect::Notification));
		CharacterSelect::Notification* noti = (CharacterSelect::Notification*)encoded;
		noti->SelectedTagId = request->TagId;
		noti->SpawnLocation = PlayerSpawnLocations[request->TagId];
		// 클라이언트는 CHARACTERSELECT_NOTIFICATION 받으면 해당 자리에 Spawn (Spawn::noti와 유사? )

		for (auto& pair : BindObjects)
		{
			if (pair.first == socket) continue;
			if (!pair.second)
			{
				std::cout << "\t\t\t pair.second is nullptr" << std::endl;
			}
			dynamic_cast<PlayerObject*>(pair.second)->Emit(CHARACTERSELECT_NOTIFICATION, buffer_noti);
		}
	}
	std::cout << "Success Send CHARACTERSELECT_NOTIFICATION" << std::endl;

	PlayerCnt++;
	std::cout << "PlayerCnt: " << PlayerCnt << std::endl;
	if (PlayerCnt == 4) {
		std::cout << "Entered in Player Cnt 4" << std::endl;

		{
			GameStart();
			//char buffer_noti[sizeof(struct HEAD) + sizeof(struct GameStart::Notification)]{};
			//void* encoded = EncodeMessage(buffer_noti, sizeof(buffer_noti), GAMESTART_NOTIFICATION, sizeof(GameStart::Notification));
			//GameStart::Notification* noti = (GameStart::Notification*)encoded;
			//noti->bStart = 1;
			//for (auto& pair : BindObjects)
			//{
			//	//if (pair.first == socket) continue;
			//	if (!pair.second)
			//	{
			//		std::cout << "\t\t\t pair.second is nullptr" << std::endl;
			//	}
			//	dynamic_cast<PlayerObject*>(pair.second)->Emit(GAMESTART_NOTIFICATION, buffer_noti);
			//}
		}
		PlayerCnt = 0;
	}


	return 0;
}