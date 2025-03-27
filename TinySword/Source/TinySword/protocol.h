#pragma once
#include "CoreMinimal.h"



#define DECLARE_PROTOCOL(ID)			ID##_REQUEST, ID##_RESPONSE, ID##_NOTIFICATION

typedef enum {
	NONE,
	DECLARE_PROTOCOL(CHARACTERSELECT), 			// 1, 2, 3
	DECLARE_PROTOCOL(MOVE), 					// 4, 5, 6
	DECLARE_PROTOCOL(ATTACK), 					// 7, 8, 9
	DECLARE_PROTOCOL(GETITEM), 					// 10, 11, 12
	DECLARE_PROTOCOL(SPAWN), 					// 13, 14, 15
	DEAD_NOTIFICATION, 							// 16
	BOMBEXPLODE_NOTIFICATION, 					// 17
 	GAMESTART_NOTIFICATION, 					// 18
	GAMEEND_NOTIFICATION 						// 19
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

	Vector(const FVector& FVectorValue) : X(FVectorValue.X), Y(FVectorValue.Y), Z(FVectorValue.Z) {}

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
		const double epsilon = 1e-6f; // 허용 오차
		return std::fabs(X - other.X) < epsilon &&
			std::fabs(Y - other.Y) < epsilon;
	}

	double Length() const { return std::sqrt(X * X + Y * Y + Z * Z); }

	Vector Normalize() const {
		double length = Length(); 
		if (length == 0) return  Vector(); 
		return { X / length, Y / length, Z / length };
	}

	double Dot(const Vector& other) const {
		return X * other.X + Y * other.Y + Z * other.Z;
	}

	// 두 Vector 간 거리 계산 
	double DistanceTo(const Vector& other) const { return std::sqrt(std::pow(X - other.X, 2) + std::pow(Y - other.Y, 2)); }

	// 반경 200.0f 이내에 존재하는지 판단
	bool IsWithinRadius(const Vector& target, float radius = 200.0f) const { return DistanceTo(target) <= radius; }

	FVector ToFVector() const { return FVector(X, Y, Z);}
};
#endif // !VECTOR_H

// #pragma pack(push, 1)
struct HEAD 
{
	unsigned short Length;
	short Index;
};
// #pragma pack(pop)



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
		Vector Location; 
		ActorType MoveActorType;
		short MoveActorTagId;
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
		short SpawnActorTagId; 
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

struct Dead {
	struct Notification {
		ActorType DeadActorType; 
		short DeadActorTagId; 
		Vector Location;
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

struct GameEnd {
	struct Notification {
		short winnerId;
	};
};
#pragma pack(pop)
//  