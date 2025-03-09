// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include <queue>
#include "protocol.h"
#include "NavigationPath.h"
#include "NavigationData.h"
#include "TinySwordGameMode.generated.h"

/**
 * 
 */

class ABaseGoldBag; 
class ABaseAISheep; 
class ABaseMeat; 
class ABaseBomb;
class AGoblin;
class ABaseGoldMine;
class ABaseCastle;
class ATinySwordPlayerController; 

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

	typedef void(ATinySwordGameMode::*FN_fnCallback)(struct HEAD* data);
	FN_fnCallback PROTOCOLS[25];

	ATinySwordGameMode()
	{
		PROTOCOLS[1] = &ATinySwordGameMode::OnCharacterSelectResponse; 
		PROTOCOLS[2] = &ATinySwordGameMode::OnCharacterSelectNotification;
		PROTOCOLS[3] = &ATinySwordGameMode::OnMoveResponse;
		PROTOCOLS[4] = &ATinySwordGameMode::OnMoveNotification;
		PROTOCOLS[5] = &ATinySwordGameMode::OnAttackResponse;
		PROTOCOLS[6] = &ATinySwordGameMode::OnAttackNotification;
		PROTOCOLS[7] = &ATinySwordGameMode::OnSpawnResponse;
		PROTOCOLS[8] = &ATinySwordGameMode::OnSpawnNotification;
		PROTOCOLS[9] = &ATinySwordGameMode::OnGetItemResponse;
		PROTOCOLS[10] = &ATinySwordGameMode::OnGetItemNotification;
		PROTOCOLS[11] = &ATinySwordGameMode::OnBombExplodeResponse;
		PROTOCOLS[12] = &ATinySwordGameMode::OnBombExplodeNotification;
		PROTOCOLS[13] = &ATinySwordGameMode::OnPlayerDeadResponse;
		PROTOCOLS[14] = &ATinySwordGameMode::OnPlayerDeadNotification;
		PROTOCOLS[15] = &ATinySwordGameMode::OnDestroyResponse;
		PROTOCOLS[16] = &ATinySwordGameMode::OnDestroyNotification;
		PROTOCOLS[17] = &ATinySwordGameMode::OnEndGameResponse;
		PROTOCOLS[18] = &ATinySwordGameMode::OnEndGameNotification;
		PROTOCOLS[19] = &ATinySwordGameMode::OnStartGameResponse;
		PROTOCOLS[20] = &ATinySwordGameMode::OnStartGameNotification;
		PROTOCOLS[21] = &ATinySwordGameMode::OnPauseGameResponse;
		PROTOCOLS[22] = &ATinySwordGameMode::OnPauseGameNotification;

	}


	TArray<FVector> GetAllPathPoints(FVector &StartLocation, FVector &EndLocation);

	TMap<ATinySwordPlayerController*, int32> PlayerControllerMap; 
	ATinySwordPlayerController* FindControllerById(const TMap<ATinySwordPlayerController*, int32>&Map, int32 TargetValue);

	TMap<int32, FVector>& GetCastleMap() { return CastleMap;}
	
	TMap<ABaseBomb*, int32> ActiveBombId;
    TQueue<int32> ReuseBombId;
	ABaseBomb* FindBombById(const TMap<ABaseBomb*, int32>& Map, int32 TargetValue);

	TMap<ABaseGoldBag*, int32> ActiveGoldBagId; 
	TQueue<int32> ReuseGoldBagId;
	ABaseGoldBag* FindGoldBagById(const TMap<ABaseGoldBag*, int32>&Map, int32 TargetValue);

	TMap<ABaseGoldMine*, int32> ActiveGoldMineMap; 
	void CollectGoldMine();
	ABaseGoldMine* FindGoldMineById(const TMap<ABaseGoldMine*, int32>&Map, int32 TargetValue);
	

	TMap<ABaseAISheep*, int32> ActiveSheepId;
    ABaseAISheep* FindSheepById(const TMap<ABaseAISheep*, int32>& Map, int32 TargetValue);

	TMap<ABaseMeat*, int32> ActiveMeatId; 
    ABaseMeat* FindMeatById(const TMap<ABaseMeat*, int32>&Map, int32 TargetValue);

	TMap<AGoblin*, int32> GoblinMap;
	AGoblin* FindGoblinById(const TMap<AGoblin*, int32>& Map, int32 TargetValue);

	TMap<int32, FVector> CastleMap;
	FVector FindCastleLocationByTagId(int32 TagId);
	
	void FindCastlesLocation();

	TMap<ABaseCastle*, int32> ActiveCastleMap; 
	void CollectAllCastles(); 
	ABaseCastle* FindCastleById(const TMap<ABaseCastle*, int32>&Map, int32 TargetValue);
	
protected:
	virtual void BeginPlay() override; 

private:
	

	// void FindAllGoblins();

	// packet processing functions
	void OnCharacterSelectResponse(struct HEAD* data);
	void OnCharacterSelectNotification(struct HEAD* data);

	void OnMoveResponse(struct HEAD *data); 
	void OnMoveNotification(struct HEAD* data); 

	void OnAttackResponse(struct HEAD *data); 
	void OnAttackNotification(struct HEAD* data);

	void OnSpawnResponse(struct HEAD* data);
	void OnSpawnNotification(struct HEAD* data);

	void OnGetItemResponse(struct HEAD* data);
	void OnGetItemNotification(struct HEAD* data);

	void OnBombExplodeResponse(struct HEAD* data);
	void OnBombExplodeNotification(struct HEAD* data);

	void OnPlayerDeadResponse(struct HEAD* data);
	void OnPlayerDeadNotification(struct HEAD* data); 

	void OnDestroyResponse(struct HEAD* data); 
	void OnDestroyNotification(struct HEAD* data);

	void OnEndGameResponse(struct HEAD* data);
	void OnEndGameNotification(struct HEAD* data); 

	void OnStartGameResponse(struct HEAD* data); 
	void OnStartGameNotification(struct HEAD* data);

	void OnPauseGameResponse(struct HEAD* data); 
	void OnPauseGameNotification(struct HEAD* data); 
	
};
