// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayingWidget.h"
#include "MainWidget.h"
#include "TinySwordPlayerController.generated.h"

/**
 * 
 */
class AGoblin;
class UTinySwordGameInstance;
class ABaseBomb;

UCLASS()
class TINYSWORD_API ATinySwordPlayerController : public APlayerController
{
	GENERATED_BODY()

public: 
	virtual void Tick(float DeltaTime) override;

	AGoblin* controlledChar; 

	UPROPERTY()
	UMainWidget* MainWidget; 

	UPROPERTY()
	UPlayingWidget* playingWidget; 

	int32 GetTagId() const {return TagId;}

	void SetTagId(int32 newTagId);

	virtual void OnPossess(APawn* aPawn) override;
	virtual void OnUnPossess() override;
	
	void SetPlayingWidget(UPlayingWidget* playing);
	UPlayingWidget* GetPlayingWidget() const {return playingWidget;}

	/////////////////////////////////
	// void ShowLoseWidget(); 
	// void ShowWinWidget(); 

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
	// TSubclassOf<UUserWidget> LoseWidgetClass; 

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
	// TSubclassOf<UUserWidget> WinWidgetClass; 
	/////////////////////////////////

	void SpawnBomb(FVector spawnLocation, int tagId);

	void SetBombIndex();

	FVector GetBombSpawnPoint(FVector &FoundLocation);

	// void SpawnGoblin(FVector spawnLocation, int tagId);

	void FindPlayingWidget(); 
	
protected: 
	virtual void BeginPlay() override; 

	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<class UMainWidget> MainWidgetClass; 

	UPROPERTY(VisibleAnywhere)
	int32 TagId;

	
	
private:
	UTinySwordGameInstance* GI;

	ATinySwordGameMode* GameMode; 

	ABaseBomb* SpawnedBomb;

	float SendTimer = 0.0f;

	// void SendMoveRequestMsg(short ActorTagId, bool bMoveUp, bool bMoveDown, bool bMoveRight, bool bMoveLeft);

	

};
