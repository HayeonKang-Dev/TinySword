// GOBLIN :
	// ������ ����...
	// ü�� 0�̸� ��� 
// CASTLE :

// ������ ���� �÷��̾� or ������ ����ִ� ���� = ��!

//			GOBLIN      |			CASTLE
// ==============================================
//    ����� �÷��̾� 3 |                         => ������ ���� �÷��̾� ��
//                              �μ��� �� 3       => ������ ���� �� �� 

// ��� ���� �μ������� ĳ���� �� �� ���� �� ���� �ο�� 
// �÷��̾� �� ���� ����� ��� ����ߴٸ�, ���� �� ���� ���� 

// Attack::Request
	// ������ ó�� �������� ����� GOBLIN(Player), CASTLE�� ���� cnt �ؾ� �� 


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
//	// Ŭ���̾�Ʈ�� CHARACTERSELECT_NOTIFICATION ������ �ش� �ڸ��� Spawn (Spawn::noti�� ����? )
//
//	for (auto& pair : BindObjects)
//	{
//		dynamic_cast<PlayerObject*>(pair.second)->Emit(GAMEEND_NOTIFICATION, buffer_noti);
//	}
//
//}
