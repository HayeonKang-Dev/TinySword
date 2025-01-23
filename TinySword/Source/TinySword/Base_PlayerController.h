// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Base_PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TINYSWORD_API ABase_PlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
private:
	bool bShowMouseCursor;
};
