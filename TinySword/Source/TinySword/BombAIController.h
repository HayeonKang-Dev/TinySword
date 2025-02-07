// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
// #include "BaseBomb.h"
#include "BombAIController.generated.h"

/**
 * 
 */

class ATinySwordGameMode;
class ABaseBomb;

UCLASS()
class TINYSWORD_API ABombAIController : public AAIController
{
	GENERATED_BODY()


public: 
	virtual void Tick(float DeltaTime) override;

	bool IsDead() const; 

	void AddToReuseId();

protected:
	virtual void BeginPlay() override; 

	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnUnPossess() override;




private:
	FVector EnemyCastleLocation; 

	ATinySwordGameMode* GameMode; 

	ABaseBomb* controlledBomb; 

	bool bIsReadyToExplode; 

	bool bHasArrived; 

	float ElapsedTime;

	FVector GetRandomCastleLocation(int TagId);	

	void MoveToCastle(const FVector &CastleLocation);

	int32 OwnerTagId; 

	FVector CurrentLocation; 

	void CheckOwnerTagId();
};
