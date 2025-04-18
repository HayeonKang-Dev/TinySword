// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include <queue>
#include "protocol.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "NavigationData.h"
#include "SelectCharacterWidget.h"
#include "AsyncNetworking.h"
#include "TinySwordGameInstance.h"
#include "AIController.h"
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
class ASheepAIController;
class USelectCharacterWidget;

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
	ATinySwordGameMode(); 
	void SaveAllPathsToTextFile();

	virtual void Tick(float DeltaTime) override;

	virtual void StartPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

	void PossessPlayer(ATinySwordPlayerController *PC, short TagId);

	////////////////
	// UFUNCTION(Server, Reliable)
	// void Server_RequestPossessGoblin(ATinySwordPlayerController* RequestingPC, int32 TagId);
	
	///////////////

	UNavigationSystemV1* NavSys;

	// Network 
	// TSharedPtr<FTCPSocketClient_Async> TCPClient;
	// FTCPSocketClient_Async TCPClient;

	MessageQueue messageQueue; 

	typedef void(ATinySwordGameMode::*FN_fnCallback)(struct HEAD* data);
	FN_fnCallback PROTOCOLS[25];

	// ATinySwordGameMode()
	// {
	// 	PROTOCOLS[3] = &ATinySwordGameMode::OnCharacterSelectNotification;
	// 	PROTOCOLS[6] = &ATinySwordGameMode::OnMoveNotification; 
	// 	PROTOCOLS[9] = &ATinySwordGameMode::OnAttackNotification;
	// 	PROTOCOLS[12] = &ATinySwordGameMode::OnGetItemNotification;
	// 	PROTOCOLS[15] = &ATinySwordGameMode::OnSpawnNotification;
	// 	PROTOCOLS[16] = &ATinySwordGameMode::OnDeadNotification;

	// 	PROTOCOLS[1] = &ATinySwordGameMode::OnCharacterSelectResponse; 
	// 	PROTOCOLS[2] = &ATinySwordGameMode::OnCharacterSelectNotification;
	// 	PROTOCOLS[3] = &ATinySwordGameMode::OnMoveResponse;
	// 	PROTOCOLS[4] = &ATinySwordGameMode::OnMoveNotification;
	// 	PROTOCOLS[5] = &ATinySwordGameMode::OnAttackResponse;
	// 	PROTOCOLS[6] = &ATinySwordGameMode::OnAttackNotification;
	// 	PROTOCOLS[7] = &ATinySwordGameMode::OnSpawnResponse;
	// 	PROTOCOLS[8] = &ATinySwordGameMode::OnSpawnNotification;
	// 	PROTOCOLS[9] = &ATinySwordGameMode::OnGetItemResponse;
	// 	PROTOCOLS[10] = &ATinySwordGameMode::OnGetItemNotification;
	// 	PROTOCOLS[11] = &ATinySwordGameMode::OnBombExplodeResponse;
	// 	PROTOCOLS[12] = &ATinySwordGameMode::OnBombExplodeNotification;
	// 	PROTOCOLS[13] = &ATinySwordGameMode::OnPlayerDeadResponse;
	// 	PROTOCOLS[14] = &ATinySwordGameMode::OnPlayerDeadNotification;
	// 	PROTOCOLS[15] = &ATinySwordGameMode::OnDestroyResponse;
	// 	PROTOCOLS[16] = &ATinySwordGameMode::OnDestroyNotification;
	// 	PROTOCOLS[17] = &ATinySwordGameMode::OnEndGameResponse;
	// 	PROTOCOLS[18] = &ATinySwordGameMode::OnEndGameNotification;
	// 	PROTOCOLS[19] = &ATinySwordGameMode::OnStartGameResponse;
	// 	PROTOCOLS[20] = &ATinySwordGameMode::OnStartGameNotification;
	// 	PROTOCOLS[21] = &ATinySwordGameMode::OnPauseGameResponse;
	// 	PROTOCOLS[22] = &ATinySwordGameMode::OnPauseGameNotification;

	// }


	TArray<FVector> GetAllPathPoints(FVector &StartLocation, FVector &EndLocation);

	TMap<ATinySwordPlayerController*, int32> PlayerControllerMap; 
	ATinySwordPlayerController* FindControllerById(const TMap<ATinySwordPlayerController*, int32>&Map, int32 TargetValue);

	TMap<int32, FVector>& GetCastleMap() { return CastleMap;}
	
	ABaseBomb* FindBombById(const TMap<ABaseBomb*, int32>& Map, int32 TargetValue);
	
	ABaseGoldBag* FindGoldBagById(const TMap<ABaseGoldBag*, int32>&Map, int32 TargetValue);

	void CollectGoldMine();
	ABaseGoldMine* FindGoldMineById(const TMap<ABaseGoldMine*, int32>&Map, int32 TargetValue);
	
	void CollectSheep();
    ABaseAISheep* FindSheepById(const TMap<ABaseAISheep*, int32>& Map, int32 TargetValue);
	
    ABaseMeat* FindMeatById(const TMap<ABaseMeat*, int32>&Map, int32 TargetValue);
	
	AGoblin* FindGoblinById(const TMap<AGoblin*, int32>& Map, int32 TargetValue);
	TMap<AGoblin*, int32> GetGoblinMap() { return GoblinMap;}

	FVector FindCastleLocationByTagId(int32 TagId);
	
	void FindCastlesLocation();

	
	void CollectAllCastles(); 
	ABaseCastle* FindCastleById(const TMap<ABaseCastle*, int32>&Map, int32 TargetValue);

	FTCPSocketClient_Async& GetTCPSocketClient() { return TCPSocketClient_Async; }
	
	void SetCharacterSelectWidget(USelectCharacterWidget* widget) { 
		UE_LOG(LogTemp, Warning, TEXT("[SETTING] SELECT CHARACTER WIDGET"));
		CharacterSelectWidget = widget;}


	void FindAllGoblins(UWorld* world);

	TMap<ABaseBomb*, int32> ActiveBombId;
	TQueue<int32> ReuseBombId;
	TMap<ABaseGoldBag*, int32> ActiveGoldBagId; 
	TQueue<int32> ReuseGoldBagId;
	TMap<ABaseAISheep*, int32> ActiveSheepId;
	TMap<ABaseMeat*, int32> ActiveMeatId; 

	int DeadCastleCnt = 0;
	int DeadPlayerSet = 0;

private:
	
	TMap<ABaseGoldMine*, int32> ActiveGoldMineMap;
	
	TMap<AGoblin*, int32> GoblinMap;
	TMap<ABaseCastle*, int32> ActiveCastleMap; 
	TMap<int32, FVector> CastleMap;


	UTinySwordGameInstance* GI;

	UPROPERTY()
	TArray<FVector> PlayerSpawnLocations;


protected:
	virtual void BeginPlay() override; 


private:

	TSharedPtr<FTCPSocketClient_Async> TCPClient;	

	bool bIsConnected;
	bool bHasInitialized = false; 
	
	FTCPSocketClient_Async TCPSocketClient_Async;

	FSocket* RawSocket; 

	// void FindAllGoblins();

public:
	// packet processing functions
	void OnCharacterSelectResponse(struct HEAD* data);
	// void OnCharacterSelectNotification(struct HEAD* data);
	void OnCharacterSelectNotification(struct CharacterSelect::Notification* data);

	void OnMoveResponse(struct HEAD *data); 
	void OnMoveNotification(struct Move::Notification* data); 

	void OnAttackResponse(struct HEAD *data); 
	void OnAttackNotification(struct Attack::Notification* data);

	void OnSpawnResponse(struct HEAD* data);
	void OnSpawnNotification(struct Spawn::Notification* data);

	void OnGetItemResponse(struct HEAD* data);
	void OnGetItemNotification(struct GetItem::Notification* data);

	void OnDeadNotification(struct Dead::Notification* data);

	void OnBombExpNotification(struct BombExplode::Notification* data);

	void OnWinNotification(struct Win::Notification* data); 
	void OnLoseNotification(struct Lose::Notification* data);

	void OpenPlayingLevel();

	AGoblin* FindAliveGoblin();

private:
	FVector ValidateLocation(AAIController* controller, const FVector& Destination);
	
	void Interpolation(AActor* actor, FVector Destination, double seconds);
	void SpawnBomb(FVector spawnLocation, short OwnertagId, short BombTagId);
	void SpawnMeat(FVector Location, short tagId);
	void SpawnGoldBag(FVector spawnLocation, short tagId);
	// void SpawnGoblin(FVector spawnLocation, int tagId);

	USelectCharacterWidget* CharacterSelectWidget;
	void DisableCharacterButton(FName buttonName);

	void FindCharacterSelectWidget();

	UPROPERTY()
	TMap<TWeakObjectPtr<AActor>, FTimerHandle> ActiveInterpolations;
};
