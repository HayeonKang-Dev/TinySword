// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"  

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

	void UpdateHealthBar();

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HPBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* moneyCount;


protected: 
	UPROPERTY(meta = (BindWidget))
	class UButton* SpawnButton;



	UFUNCTION()
	void OnSpawnButtonClicked(); 

	bool DecreasePlayerMoney();

	void SpawnBomb(); 

	void SetBombIndex(); 

	UFUNCTION()
	float GetHpBarPercent();
private:
	ATinySwordGameMode* GameMode; 

	ATinySwordPlayerController* playerController; 

	AGoblin* controlledChar;

	FVector SpawnLocation;

	ABaseBomb* SpawnedBomb;
};
