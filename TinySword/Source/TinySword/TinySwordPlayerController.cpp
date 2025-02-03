// Fill out your copyright notice in the Description page of Project Settings.


#include "TinySwordPlayerController.h"
#include "Blueprint/UserWidget.h"


void ATinySwordPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (PlayingWidgetClass)
    {
        playingWidget = CreateWidget<UPlayingWidget>(this, PlayingWidgetClass);
        if (playingWidget) playingWidget->AddToViewport(); // 위젯 화면 추가
    }
}
