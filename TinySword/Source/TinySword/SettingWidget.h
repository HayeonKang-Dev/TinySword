// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SettingWidget.generated.h"

/**
 * 
 */

class ATinySwordPlayerController; 
class UMainWidget; 

UCLASS()
class TINYSWORD_API USettingWidget : public UUserWidget
{
	GENERATED_BODY()

public: 
	virtual void NativeConstruct() override; 

	UPROPERTY()
	UMainWidget* MainWidget; 

protected: 
	UPROPERTY(meta = (BindWidget))
	class UButton* ContinueButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* MainMenuButton; 

	UFUNCTION()
	void OnContinueButtonClicked(); 

	UFUNCTION()
	void OnMainMenuButtonClicked();

	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<class UMainWidget> MainWidgetClass; 

private:
	ATinySwordPlayerController* playerController; 
	
};
