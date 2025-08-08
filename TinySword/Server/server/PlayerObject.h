#pragma once
#include <array>
#include "Protocol.h"
#include "Actor.h"
#include <iostream>


extern std::array<Vector, 5> PlayerSpawnLocations; 

class PlayerObject : public Actor
{
protected:
	SOCKET m_Socket;
	int Coin;
	double speed = 100; // 80;

public:
	PlayerObject(SOCKET socket, int hp, const Vector& location = Vector(0, 0, 0), short tagid = 0, int coin = 0) // Vector
		: Actor(hp, location, tagid), m_Socket(socket), Coin(coin) {}

	//virtual ~PlayerObject() {}

	void Emit(PROTOCOL_IDS id, char* buffer) {
		//std::cout << ">> SENDING : " << id << std::endl;
		
		HEAD* header = reinterpret_cast<HEAD*>(const_cast<char*>(buffer));
		int size = static_cast<int>(header->Length);
		int totalSent = 0; 
		while (totalSent < size)
		{
			if (!m_Socket)
			{
				std::cout << "\t\tm_Socket is nullptr" << std::endl;
			}



			int sentBytes = send(m_Socket, buffer + totalSent, size - totalSent, 0); 

			if (sentBytes == SOCKET_ERROR)
			{
				std::cout << "Emit Failed: SOCKET_ERROR" << std::endl;
				return;
			}
			else
			{
				//std::cout << "send byte? : " << sentBytes << std::endl;
			}
			totalSent += sentBytes;
		}
	}

	Vector GetSpawnLocation(short TagId) { return PlayerSpawnLocations[TagId]; }
	void SetLocation(Vector newLocation) { Location = newLocation;  }
	void SetTagId(short newTagId) { TagId = newTagId; }

	int DecreaseCoin(int coin) { return Coin = max(0, Coin - coin); }
	int IncreaseCoin(int coin) { return Coin += coin; }
	int GetCoin() const { return Coin; }

	int IncreaseHP(int hp) { return HP = min(100, HP + hp); }
	
	double GetSpeed() const { return speed; }

	SOCKET GetSocket() { return m_Socket; }


	/*PlayerObject* FindPlayerByTagId(std::map<SOCKET, IBindObject*>& BindObjects, short TargetTagId) {
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
	}*/


};