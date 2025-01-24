// Fill out your copyright notice in the Description page of Project Settings.


#include "TinySwordPlayerController.h"
#include "Blueprint/UserWidget.h"


void ATinySwordPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (PlayingWidgetClass)
    {
        PlayingWidgetInstance = CreateWidget<UPlayingWidget>(this, PlayingWidgetClass);
        if (PlayingWidgetInstance) PlayingWidgetInstance->AddToViewport(); // 위젯 화면 추가
    }
}
