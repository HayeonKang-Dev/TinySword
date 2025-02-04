// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "Blueprint/UserWidget.h"
#include "SelectCharacterWidget.h"
#include "MainWidget.generated.h"

/**
 * 
 */
UCLASS()
class TINYSWORD_API UMainWidget : public UUserWidget
{
	GENERATED_BODY()

public: 
	virtual void NativeConstruct() override; 

	UPROPERTY()
	USelectCharacterWidget* selectWidget; 
	
protected:
	UPROPERTY(meta = (BindWidget))
	class UButton* GameStartButton;

	UFUNCTION()
	void OnStartGameButtonClicked(); 

	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<class USelectCharacterWidget> selectWidgetClass; 
};
