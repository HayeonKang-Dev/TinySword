// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayingWidget.h"
#include "TinySwordPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TINYSWORD_API ATinySwordPlayerController : public APlayerController
{
	GENERATED_BODY()

public: 

protected: 
	virtual void BeginPlay() override; 

	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<class UPlayingWidget> PlayingWidgetClass; 

	UPROPERTY()
	class UPlayingWidget* PlayingWidgetInstance; 
private:

};
