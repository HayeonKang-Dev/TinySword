// GOBLIN :
	// 리스폰 없애...
	// 체력 0이면 사망 
// CASTLE :

// 마지막 남은 플레이어 or 마지막 살아있는 진지 = 승!

//			GOBLIN      |			CASTLE
// ==============================================
//    사망한 플레이어 3 |                         => 마지막 남은 플레이어 승
//                              부서진 성 3       => 마지막 남은 성 승 

// 모든 성이 부서졌으면 캐릭터 한 명만 남을 때 까지 싸우기 
// 플레이어 한 명을 남기고 모두 사망했다면, 남은 한 명이 승자 

// Attack::Request
	// 데미지 처리 과정에서 사망한 GOBLIN(Player), CASTLE의 수를 cnt 해야 함 


#include "PlayerObject.h"


//
//void GameEnd(short winnerId) {
//
//	std::map<SOCKET, IBindObject*> BindObjects;
//	CtxtBindFroms(BindObjects);
//
//
//	char buffer_noti[sizeof(struct HEAD) + sizeof(struct GameEnd::Notification)]{};
//	void* encoded = EncodeMessage(buffer_noti, sizeof(buffer_noti), GAMEEND_NOTIFICATION, sizeof(GameEnd::Notification));
//	GameEnd::Notification* noti = (GameEnd::Notification*)encoded;
//	noti->winnerId = winnerId;
//	// 클라이언트는 CHARACTERSELECT_NOTIFICATION 받으면 해당 자리에 Spawn (Spawn::noti와 유사? )
//
//	for (auto& pair : BindObjects)
//	{
//		dynamic_cast<PlayerObject*>(pair.second)->Emit(GAMEEND_NOTIFICATION, buffer_noti);
//	}
//
//}
