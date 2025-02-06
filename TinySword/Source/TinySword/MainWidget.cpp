// Fill out your copyright notice in the Description page of Project Settings.


#include "MainWidget.h"


void UMainWidget::NativeConstruct()
{
    Super::NativeConstruct(); 

    GameStartButton = Cast<UButton>(GetWidgetFromName(TEXT("GameStartButton")));
    if (GameStartButton) GameStartButton->OnClicked.AddDynamic(this, &UMainWidget::OnStartGameButtonClicked); 
}

void UMainWidget::OnStartGameButtonClicked()
{
    if (selectWidgetClass)
    {
        selectWidget = CreateWidget<USelectCharacterWidget>(GetWorld(), selectWidgetClass); 

        if (selectWidget)
        {
            selectWidget->AddToViewport();
            
        }
    }
}
