// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "PlayingWidget.generated.h"


/**
 * 
 */
class ATinySwordGameMode;
class ATinySwordPlayerController; 
class AGoblin;
class ABaseBomb;

UCLASS()
class TINYSWORD_API UPlayingWidget : public UUserWidget
{
	GENERATED_BODY()

public: 
	virtual void NativeConstruct() override; 

protected: 
	UPROPERTY(meta = (BindWidget))
	class UButton* SpawnButton;

	void OnSpawnButtonClicked(); 

	bool DecreasePlayerMoney();

	void SpawnBomb(); 

	void SetBombIndex(); 

private:
	ATinySwordGameMode* GameMode; 

	ATinySwordPlayerController* playerController; 

	AGoblin* controlledChar;

	FVector SpawnLocation;

	ABaseBomb* SpawnedBomb;
};
