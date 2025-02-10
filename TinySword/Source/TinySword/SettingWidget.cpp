// Fill out your copyright notice in the Description page of Project Settings.


#include "SettingWidget.h"
#include "PlayingWidget.h"
#include "MainWidget.h"
#include "Kismet/GameplayStatics.h"
#include "TinySwordPlayerController.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void USettingWidget::NativeConstruct()
{
    Super::NativeConstruct(); 

    playerController = Cast<ATinySwordPlayerController>(GetOwningPlayer()); 

    ContinueButton = Cast<UButton>(GetWidgetFromName(TEXT("ContinueButton")));
    MainMenuButton = Cast<UButton>(GetWidgetFromName(TEXT("MainMenuButton"))); 

    if (ContinueButton) ContinueButton->OnClicked.AddDynamic(this, &USettingWidget::OnContinueButtonClicked);   
    if (MainMenuButton) MainMenuButton->OnClicked.AddDynamic(this, &USettingWidget::OnMainMenuButtonClicked);
}

void USettingWidget::OnContinueButtonClicked()
{
    UWorld* World = GetWorld();
    UGameplayStatics::SetGamePaused(World, false);
    RemoveFromParent(); 

    playerController->playingWidget->EnableSpawnButton(true);

}

void USettingWidget::OnMainMenuButtonClicked()
{
    if (MainWidgetClass)
    {
        UWorld* World = GetWorld(); 
        if (World) UGameplayStatics::SetGamePaused(World, true);

        APawn* pawn = playerController->GetPawn(); 
        playerController->OnUnPossess();
        pawn->Destroy();

        UWidgetLayoutLibrary::RemoveAllWidgets(this);

        UGameplayStatics::OpenLevel(GetWorld(), FName("MainMenuLevel"));
    }
}
