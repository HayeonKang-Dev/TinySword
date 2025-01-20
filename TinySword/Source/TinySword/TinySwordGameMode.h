// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TinySwordGameMode.generated.h"

/**
 * 
 */
UCLASS()
class TINYSWORD_API ATinySwordGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override; 

private:
	
};
