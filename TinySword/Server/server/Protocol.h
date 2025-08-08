#pragma once
#include <cmath>
#include "IocpServer.h"
//#include "GoldmineManager.h"
//#include "SheepManager.h"
//#include "BombManager.h"
//#include "CastleManager.h"

#define DECLARE_PROTOCOL(ID)	ID##_REQUEST, ID##_RESPONSE, ID##_NOTIFICATION

typedef enum {
	NONE,
	DECLARE_PROTOCOL(LOGIN),
	DECLARE_PROTOCOL(CHARACTERSELECT), 
	DECLARE_PROTOCOL(MOVE),
	DECLARE_PROTOCOL(ATTACK),
	DECLARE_PROTOCOL(GETITEM),
	DECLARE_PROTOCOL(SPAWN),
	DEAD_NOTIFICATION,
	BOMBEXPLODE_NOTIFICATION,
	GAMESTART_NOTIFICATION,
	WIN_NOTIFICATION, 
	LOSE_NOTIFICATION
} PROTOCOL_IDS;

typedef enum {
	NONEACTOR, 
	GOBLIN,
	CASTLE, 
	BOMB, 
	SHEEP, 
	GOLDMINE,
	MEAT,
	GOLDBAG
} ActorType;

#pragma pack(push, 1)
#ifndef VECTOR_H
#define VECTOR_H

struct Vector {
	double X, Y, Z; 
	Vector() : X(0), Y(0), Z(0) {}
	Vector(double x, double y, double z) : X(x), Y(y), Z(z) {}

	Vector operator+(const Vector& other) const {
		return { X + other.X, Y + other.Y, Z + other.Z };
	}

	Vector operator*(double scalar) const {
		return { X * scalar, Y * scalar, Z * scalar };
	}

	Vector operator-(const Vector& other) const {
		return { X - other.X, Y - other.Y, Z - other.Z };
	}

	bool operator==(const Vector& other) const {
		const double epsilon = 1e-6f; 
		return std::fabs(X - other.X) < epsilon && std::fabs(Y - other.Y) < epsilon;
		//return std::fabs(X - other.X) < epsilon && std::fabs(Y - other.Y) < epsilon && std::fabs(Z - other.Z) < epsilon;
	}

	bool operator!=(const Vector& other) const {
		return !(*this == other);
	}

	double Length() const { return std::sqrt(X * X + Y * Y + Z * Z); }

	Vector Normalize() const {
		double length = Length(); 
		if (length == 0) return Vector(); 
		return { X / length, Y / length, Z / length };
	}

	double Dot(const Vector& other) const {
		return X * other.X + Y * other.Y + Z * other.Z;
	}

	double DistanceTo(const Vector& other) const { 
		return std::sqrt(std::pow(X - other.X, 2) + std::pow(Y - other.Y, 2)); 
	}

	bool IsWithinRadius(const Vector& target, float radius = 200.0f) const {
		return DistanceTo(target) <= radius;
	}
};
#endif //VECTOR_H
#define MAX_NICKNAME_LEN 20


//#pragma pack(push, 1)
struct HEAD
{
	unsigned short Length;
	short Index; 
};
//#pragma pack(pop)


struct Login
{
	struct Request
	{
		char useId[32]; 
		char userPassword[32]; 
	};

	struct Response 
	{
		bool bSuccess; 
	};

	struct Notification
	{

	};
};

struct CharacterSelect
{
	struct Request
	{
		short TagId;
	};

	struct Response
	{
		bool bSuccess;
	};

	struct Notification
	{
		short SelectedTagId;
		Vector SpawnLocation;
	};
};

struct Move
{
	struct Request
	{
		// Vector Location : sheep에게 전송한 이동 위치가 이동 불가할 시, 
		// client에서 보내올 좌표 (sheep 이동에만 사용)
		Vector Location;
		ActorType MoveActorType;
		short MoveActorTagId;
		bool FacingX; // 0: Right, 1: Left

		bool bMoveUp, bMoveDown, bMoveRight, bMoveLeft;
	};

	struct Response
	{
		bool bIsSuccess;
	};

	struct Notification
	{
		Vector Location;
		ActorType MoveActorType;
		short MoveActorTagId;
		bool FacingX; 

		bool bMoveUp, bMoveDown, bMoveRight, bMoveLeft;
	};
};


struct Attack
{
	struct Request
	{
		ActorType TargetActorType, AttackerActorType;
		short TargetTagId, AttackerTagId;
		Vector TargetLocation, AttackLocation;
		int Damage;

	};

	struct Response
	{
		bool bSuccess;
	};

	struct Notification
	{
		ActorType AttackerActorType, TargetActorType;
		short AttackerTagId, TargetTagId;
		Vector TargetLocation, AttackLocation;
		int HP;
	};
};


struct Spawn
{
	struct Request
	{
		int Cost;
		short TagId;
	};

	struct Response
	{
		bool bSuccess;
		int CurrentCoin;
	};

	struct Notification
	{
		ActorType SpawnActorType;
		short SpawnActorTagId, OwnerTagId;
		Vector Location;
	};
};


struct Dead {
	struct Notification {
		ActorType DeadActorType;
		short DeadActorTagId;
		Vector Location;
	};
};


struct GetItem {
	struct Request {
		ActorType ItemType;
		short ItemTagId, PlayerTagId;
		Vector Location;
		int IncreaseVal;
	};
	struct Response {
		bool bSuccess;
		int Coin, HP;
	};
	struct Notification {
		ActorType ItemType;
		short ItemTagId, PlayerTagId;
		Vector Location;
		int Coin, HP;
	};
};

struct BombExplode {
	struct Notification {
		Vector Location;
		short TagId;
	};
};

struct GameStart {
	struct Notification {
		bool bStart;
	};
};

//struct GameEnd {
//	struct Notification {
//		short winnerId;
//	};
//};

struct Win {
	struct Notification {
		short winnerTagId;
	};
};

struct Lose {
	struct Notification {
		short LoserTagId;
	};
};



#pragma pack(pop)

// 프로토콜, 처리 함수 매핑 
typedef int (*PROTOCOL_EXECUTE)(SOCKET Socket, LPWSABUF lpWsaBuf);

struct PROTOCOL_MAPPING
{
	PROTOCOL_IDS ID; 
	PROTOCOL_EXECUTE EXECUTE;
};

VOID* EncodeMessage(char* buffer, size_t buffer_size, PROTOCOL_IDS id, int length);

//extern CastleManager castleManager;
//extern GoldmineManager goldmineManager;
//extern SheepManager sheepManager;
//extern BombManager bombManager;