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
class USettingWidget;

UCLASS()
class TINYSWORD_API UPlayingWidget : public UUserWidget
{
	GENERATED_BODY()

public: 
	virtual void NativeConstruct() override; 

	void UpdateHealthBar(float HealthPercent);
	void UpdateMoneyCount(float Money);

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HPBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* moneyCount;

	UPROPERTY()
	USettingWidget* SettingWidget; 

	void EnableSpawnButton(bool bEnable);

protected: 
	UPROPERTY(meta = (BindWidget))
	class UButton* SpawnButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* SettingButton; 

	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<class USettingWidget> SettingWidgetClass; 


	UFUNCTION()
	void OnSpawnButtonClicked(); 

	UFUNCTION()
	void OnSettingButtonClicked();

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

	FVector GetBombSpawnPoint(UWorld* World, FVector& FoundLocation);
};
