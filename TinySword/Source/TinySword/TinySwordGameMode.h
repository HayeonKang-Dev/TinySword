// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include <queue>
#include "TinySwordGameMode.generated.h"

/**
 * 
 */

class ABaseGoldBag; 
class ABaseAISheep; 
class ABaseMeat; 
class ABaseBomb;

UCLASS()
class TINYSWORD_API ATinySwordGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	virtual void Tick(float DeltaTime) override;

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

protected:
	virtual void BeginPlay() override; 

private:
	void FindCastlesLocation();
	
};
