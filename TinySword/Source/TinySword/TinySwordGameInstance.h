// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Goblin.h"
#include "TinySwordGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class TINYSWORD_API UTinySwordGameInstance : public UGameInstance
{
	GENERATED_BODY()

public: 
	int32 GetTagId() const {return TagId; }
	void SetTagId(int32 newTagId) {TagId = newTagId;}

	AGoblin* GetChar() const {return controlledChar;}
	void SetChar(AGoblin* newChar) {controlledChar = newChar;}

private:
	int32 TagId; 

	AGoblin* controlledChar; 
};
