// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "TinySwordPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class TINYSWORD_API ATinySwordPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	int32 GetTagId() const {return TagId; }
	void SetTagId(int32 newTagId) {TagId = newTagId;}

private:
	int32 TagId;
	
};
