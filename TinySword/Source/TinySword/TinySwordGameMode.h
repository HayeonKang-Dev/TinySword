// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include <queue>
#include "protocol.h"
#include "TinySwordGameMode.generated.h"

/**
 * 
 */

class ABaseGoldBag; 
class ABaseAISheep; 
class ABaseMeat; 
class ABaseBomb;
class AGoblin;


class MessageQueue
{
public: 
	struct Node 
	{
		struct HEAD *data; 
		struct Node *next; 
	}; 

	void push(struct HEAD *value)
	{
		struct Node *node = new Node(); 

		node->data = value; 
		node->next = nullptr;
		if (head == nullptr) this->head = this->tail = node; 
		else 
		{
			this->tail->next = node; 
			this->tail = node; 
		}
	}

	struct HEAD* pop()
	{
		if (this->head == nullptr) return nullptr;

		struct HEAD *value = this->head->data; 
		
		delete this->head; 
		this->head = this->head->next;
		return value; 
	}

private:
	struct Node *head, *tail;
};




UCLASS()
class TINYSWORD_API ATinySwordGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	virtual void Tick(float DeltaTime) override;

	MessageQueue messageQueue; 

	TMap<int32, FVector>& GetCastleMap() { return CastleMap;}
	
	TMap<ABaseBomb*, int32> ActiveBombId;
    TQueue<int32> ReuseBombId;

	TMap<ABaseGoldBag*, int32> ActiveGoldBagId; 
	TQueue<int32> ReuseGoldBagId;
	ABaseGoldBag* FindGoldBagById(const TMap<ABaseGoldBag*, int32>&Map, int32 TargetId);

	TMap<int32, FVector> CastleMap;

	TMap<ABaseAISheep*, int32> ActiveSheepId;
    ABaseAISheep* FindSheepById(const TMap<ABaseAISheep*, int32>& Map, int32 TargetValue);

	TMap<ABaseMeat*, int32> ActiveMeatId; 
    ABaseMeat* FindMeatById(const TMap<ABaseMeat*, int32>&Map, int32 TargetValue);

	TMap<int32, AGoblin*> GoblinMap;

protected:
	virtual void BeginPlay() override; 

private:
	void FindCastlesLocation();

	void FindAllGoblins();

	void Moving(struct HEAD *data); 
	
};
