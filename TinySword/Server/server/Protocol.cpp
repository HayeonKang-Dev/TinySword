#include "Protocol.h"
#include <iostream>

extern int CharacterSelectExecute(SOCKET socket, LPWSABUF lpWsaBuf);
extern int MoveExecute(SOCKET socket, LPWSABUF lpWsaBuf);
extern int AttackExecute(SOCKET socket, LPWSABUF lpWsaBuf);
extern int SpawnExecute(SOCKET socket, LPWSABUF lpWsaBuf); 
extern int GetItemExecute(SOCKET socket, LPWSABUF lpWsaBuf);

struct PROTOCOL_MAPPING PROTOCOL__MAPPING[]
{
	{ NONE, nullptr},
	{ CHARACTERSELECT_REQUEST, CharacterSelectExecute},
	{ MOVE_REQUEST, MoveExecute },
	{ ATTACK_REQUEST, AttackExecute },
	{ SPAWN_REQUEST, SpawnExecute },
	{ GETITEM_REQUEST, GetItemExecute }
};


//VOID* EncodeMessage(char* buffer, PROTOCOL_IDS id, int length)
//{
//	struct HEAD* head = (struct HEAD*)buffer;
//	head->Index = (short)id;
//	head->Length = (unsigned short)(sizeof(struct HEAD) + length);
//
//	std::cout << "Head->Length: " << head->Length << std::endl;
//
//	if ((sizeof(struct HEAD) + length) > sizeof(buffer)) {
//		std::cout << "MEMORY OVERFLOW - EncodedMessage" << std::endl;
//		std::cout << "\tHead + Length: " << (sizeof(struct HEAD) + length) << "\tbuffer Size: " << sizeof(buffer) << std::endl;
//		return nullptr;
//	}
//	return (void*)(head + 1);
//}

VOID* EncodeMessage(char* buffer, size_t buffer_size, PROTOCOL_IDS id, int length)
{
	struct HEAD* head = (struct HEAD*)buffer;
	head->Index = (short)id;
	head->Length = (unsigned short)(sizeof(struct HEAD) + length);



	if ((sizeof(struct HEAD) + length) > buffer_size) {
		std::cout << "MEMORY OVERFLOW - EncodedMessage" << std::endl;
		std::cout << "\tHead + Length: " << (sizeof(struct HEAD) + length) << "\tbuffer Size: "
			<< buffer_size <<"\tSize(HEAD) :" << sizeof(HEAD) << std::endl;
		return nullptr;
	}
	return (void*)(head + 1);
}
