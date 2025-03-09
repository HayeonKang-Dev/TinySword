#pragma once
#include "CoreMinimal.h"

struct HEAD {
	short Command;
};

struct CharacterSelect {
	struct Request {
		struct HEAD H;
		char playerId[40];
		int selectCharIndex; 
	};

	struct Response {
		struct HEAD H;
		int playerIndex;
		int bSuccess;
	};

	struct Notification {
		struct HEAD H;
		char playerId[40];
		int playerIndex;
	};
};

struct Move {
	// 0x10, 0x11, 0x12
	// key => 'W' : 0x01, 'A' : 0x02, 'S' : 0x04, 'D': 0x08
	//   0x01 + 0x-8 = 0x09
	// key = index 
	struct Request {
		struct HEAD H;
		int ActorType; // player, sheep, bomb = 0, 1, 2
		int ActorIndex; //  playerIndex
		char key; //  
		bool bMoveUp, bMoveDown, bMoveRight, bMoveLeft; 
		FVector Pos;
		float X; 
		float Y;
	};

	struct Response {
		struct HEAD H;
		int ActorType; 
		int ActorIndex; 
		bool bMoveUp, bMoveDown, bMoveRight, bMoveLeft; 
		float Speed; 
		float X, Y;
		FVector Destination;
	};

	struct Notification {
		struct HEAD H;
		int ActorType;
		int ActorIndex;
		char key;
		FVector Location;
		float X; 
		float Y; 
	};
};

struct Attack { // 0x2
	struct Request {
		struct HEAD H;
		int targetIndex;
		int targetType;
		int AttackerType;
		int AttackerIndex;
		int Damage; 
		float X, Y;
	};

	struct Response {
		struct HEAD H;
		int AttackerType; 
		int AttackerIndex; 
		int TargetType; // goblin, bomb, castle, sheep, goldmine = 0, 1, 2, 3, 4
		int TargetIndex; 
		int Damage; 
		int hityn;
		FVector Location;
	};

	struct Notification {
		struct HEAD H;
		int AttackerType; // player, bomb = 0, 1
		int AttackerIndex;
		int targetType; // player, bomb, castle, sheep, goldmine = 0, 1, 2, 3, 4
		int targetIndex;
		//FVector Pos;
		int Damage;
		int targetHp;
		float X, Y;
	};
};

struct Spawn { // 0x3
	struct Request {
		struct HEAD H;
		int SpawnActorIndex; 
		int SpawnType; // player, bomb, meat, goldbag = 0, 1, 2, 3
		//FVector Pos;
		int playerIndex;
		float X, Y;
	};

	struct Response {
		struct HEAD H;
		int SpawnType; // goblin, bomb, meat, goldbag = 0, 1, 2, 3
		int SpawnActorIndex; 
		FVector Location; 
		int successyn;
	};

	struct Notification {
		struct HEAD H;
		//FVector Pos;
		int SpawnType;
		int SpawnActorIndex;
		FVector Location;
		float X, Y;
	};
};

struct GetItem { // 0x4
	struct Request {
		struct HEAD H;
		int playerIndex;
		int ItemType; // Meat, Coin = 0, 1
		int itemIndex;
		int playerHp; 
		int playerCoin; 
		float X, Y;
	};

	struct Response {
		struct HEAD H;
		int successyn;
		int playerIndex; 
		int ItemType; // Meat, Coin = 0, 1

	};

	struct Notification {
		struct HEAD H;
		int playerIndex;
		//FVector Pos;
		int playerHp;
		int playerCoin;
		int ItemType;
		int ItemIndex;
		float X, Y;
	};
};

struct BombExplode { // 0x5
	struct Request {
		struct HEAD H;
		int BombIndex;
		int Damage; 
		int targetType; // player, castle, bomb, sheep, goldmine = 0, 1, 2, 3, 4 
		//int targetIndex; 
		int DamagedActorIndex;
		float X, Y;
	};

	struct Response {
		struct HEAD H;
	};

	struct Notification {
		struct HEAD H;
		//FVector Pos;
		//int DamagedActorIndex;
		//int Damage;
		// int targetType;
		// int targetHp; // target last hp
		float X, Y;
	};
};

struct PlayerDead { // 0x6
	struct Request {
		struct HEAD H;
		int playerIndex;
		float X, Y;
	};

	struct Response {
		struct HEAD H;
	};

	struct Notification {
		struct HEAD H;
		//FVector Pos;
		int playerIndex;
		float X, Y;
	};
};

struct Destroy { // 0x7
	struct Request {
		struct HEAD H;
		int ActorType;
		int ActorIndex;
		float X, Y;
	};

	struct Response {
		struct HEAD H;
		int ActorType; 
		int ActorIndex; 
		float X, Y;
	};

	struct Notification {
		struct HEAD H;
		int ActorType; // bomb, goldbag, sheep, meat = 0, 1, 2, 3
		int ActorIndex;
		//FVector Pos;
		float X, Y;
	};
};

struct EndGame {
	struct Request {
		struct HEAD H;

	};

	struct Response {
		struct HEAD H;
	};

	struct Notification {
		struct HEAD H;
	};
};

struct StartGame {
	struct Request {
		struct HEAD H;
	};

	struct Response {
		struct HEAD H;
	};

	struct Notification {
		struct HEAD H;
	};
};

struct PauseGame {
	struct Request {
		struct HEAD H;
	};

	struct Response {
		struct HEAD H;
	};

	struct Notification {
		struct HEAD H;
	};
};