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

UCLASS()
class TINYSWORD_API ATinySwordGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	virtual void Tick(float DeltaTime) override;

	TMap<ABaseGoldBag*, int32> ActiveGoldBagId; 
	TQueue<int32> ReuseGoldBagId;
	ABaseGoldBag* FindGoldBagById(const TMap<ABaseGoldBag*, int32>&Map, int32 TargetId);

	TMap<int32, FVector> CastleMap;

protected:
	virtual void BeginPlay() override; 

private:
	
};
