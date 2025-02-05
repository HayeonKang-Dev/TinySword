// Fill out your copyright notice in the Description page of Project Settings.


#include "TinySwordPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "TinySwordGameMode.h"

void ATinySwordPlayerController::BeginPlay()
{
    Super::BeginPlay();

    GameMode = Cast<ATinySwordGameMode>(GetWorld()->GetAuthGameMode());

    bShowMouseCursor = true; 
    
    if (MainWidgetClass)
    {
        MainWidget = CreateWidget<UMainWidget>(this, MainWidgetClass);
        if (MainWidget) MainWidget->AddToViewport(); // 위젯 화면 추가
    }
}

void ATinySwordPlayerController::SetPlayingWidget(UPlayingWidget* playing)
{
    playingWidget = playing;
}

void ATinySwordPlayerController::SetTagId(int32 newTagId)
{
    TagId = newTagId;
}


void ATinySwordPlayerController::OnPossess(APawn *aPawn)
{
    Super::OnPossess(aPawn);
}

void ATinySwordPlayerController::OnUnPossess()
{
    Super::OnUnPossess();
}
