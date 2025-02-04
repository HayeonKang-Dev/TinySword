// Fill out your copyright notice in the Description page of Project Settings.


#include "TinySwordPlayerController.h"
#include "Blueprint/UserWidget.h"


void ATinySwordPlayerController::BeginPlay()
{
    Super::BeginPlay();

    bShowMouseCursor = true; 
    
    if (MainWidgetClass)
    {
        MainWidget = CreateWidget<UMainWidget>(this, MainWidgetClass);
        if (MainWidget) MainWidget->AddToViewport(); // 위젯 화면 추가
    }
}

void ATinySwordPlayerController::SetTagId(int32 newTagId)
{
    TagId = newTagId;
}
