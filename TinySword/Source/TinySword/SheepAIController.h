// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "SheepAIController.generated.h"

/**
 * 
 */
class ATinySwordGameMode; 
class ABaseAISheep;

UCLASS()
class TINYSWORD_API ASheepAIController : public AAIController
{
	GENERATED_BODY()

public: 
	bool IsDead() const; 

	void FleeFrom(const FVector& AttackLocation); 

	UNavigationSystemV1* NaviSystem; 

protected: 
	virtual void BeginPlay() override; 

	virtual void OnPossess(APawn* InPawn) override; 

	virtual void OnUnPossess() override; 

	ABaseAISheep* ControlledCharacter;

private: 	
	void MoveRandomPos(); 

	FTimerHandle Timer; 

	ATinySwordGameMode* GameMode; 

	void SendMoveResponseMsg(int ActorType, int ActorIndex, FVector destination, float Speed); 
	void SendMoveNotiMsg(int actorType, int actorIndex, float X, float Y);

	
};
