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
class ATinySwordPlayerState;
class AGoblin;
class ABaseBomb;
class USettingWidget;
class UTinySwordGameInstance;

UCLASS()
class TINYSWORD_API UPlayingWidget : public UUserWidget
{
	GENERATED_BODY()

public: 
	virtual void NativeOnInitialized() override;

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



private:
	ATinySwordGameMode* GameMode; 

	UTinySwordGameInstance* GI;

	ATinySwordPlayerController* playerController; 

	AGoblin* controlledChar;

	FVector SpawnLocation;

	ABaseBomb* SpawnedBomb;

	FVector GetBombSpawnPoint(UWorld* World, FVector& FoundLocation);

	void SpawnGoblin(ATinySwordPlayerController* PlayerController);

	void SendSpawnResponseMsg(); 
	void SendSpawnNotiMsg(int spawnType, int spawnActorIndex, float X, float Y);

	void SendSelectCharResponseMsg(int playerIndex); 
	void SendSelectCharNotiMsg(const char playerId[40], int playerIndex);

};
