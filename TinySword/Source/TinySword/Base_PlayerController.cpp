// Fill out your copyright notice in the Description page of Project Settings.


#include "Base_PlayerController.h"
#include "GameFramework/InputSettings.h"

void ABase_PlayerController::BeginPlay()
{
    Super::BeginPlay(); 

    bShowMouseCursor = true; 

    FInputModeGameAndUI InputMode; 
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    SetInputMode(InputMode);
}
