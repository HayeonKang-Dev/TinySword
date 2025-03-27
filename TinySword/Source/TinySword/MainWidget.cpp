// Fill out your copyright notice in the Description page of Project Settings.


#include "MainWidget.h"
#include "Kismet/GameplayStatics.h"

void UMainWidget::NativeConstruct()
{
    Super::NativeConstruct(); 

    GameStartButton = Cast<UButton>(GetWidgetFromName(TEXT("GameStartButton")));
    if (GameStartButton) GameStartButton->OnClicked.AddDynamic(this, &UMainWidget::OnStartGameButtonClicked); 
}

void UMainWidget::OnStartGameButtonClicked()
{
    UGameplayStatics::OpenLevel(GetWorld(), FName("SelectCharLevel"));
    // if (UWorld* World = GetWorld())
    // {
    //     World->ServerTravel(TEXT("/Game/SelectCharLevel"), true);
    // }
}
