// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "PlayingWidget.h"
#include "SelectCharacterWidget.generated.h"

/**
 * 
 */
class ATinySwordGameMode;
class UMainWidget;
UCLASS()
class TINYSWORD_API USelectCharacterWidget : public UUserWidget
{
	GENERATED_BODY()

public: 
	virtual void NativeConstruct() override; 

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY()
	UPlayingWidget* playingWidget; 

	UPROPERTY()
	UMainWidget* mainWidget; 

	
protected:
	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<class UPlayingWidget> PlayingWidgetClass; 

	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<class UMainWidget> MainWidgetClass; 

	UPROPERTY(meta = (BindWidget))
	class UButton* RedButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* YellowButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* BlueButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* PurpleButton;


	UPROPERTY(meta = (BindWidget))
	class UButton* QuitButton;

	UFUNCTION()
	void OnRedButtonClicked();

	UFUNCTION()
	void OnYellowButtonClicked();

	UFUNCTION()
	void OnBlueButtonClicked();

	UFUNCTION()
	void OnPurpleButtonClicked();

	UFUNCTION()
	void OnQuitButtonClicked();

private:
	int32 ClickCnt = 0;

	ATinySwordPlayerController* PC; 

	ATinySwordGameMode* GameMode;

	void SpawnGoblin(ATinySwordPlayerController* PlayerController);

	void SendSelectCharResponseMsg(int playerIndex); 
	void SendSelectCharNotiMsg(const char playerId[40], int playerIndex);

	void SendSpawnResponseMsg(); 
	void SendSpawnNotiMsg(int spawnType, int spawnActorIndex, float X, float Y);
};
