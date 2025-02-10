// Fill out your copyright notice in the Description page of Project Settings.


#include "TinySwordPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Goblin.h"
#include "TinySwordGameMode.h"
#include "TinySwordGameInstance.h"
#include "Kismet/GameplayStatics.h"

void ATinySwordPlayerController::BeginPlay()
{
    Super::BeginPlay();

    GameMode = Cast<ATinySwordGameMode>(GetWorld()->GetAuthGameMode());
    GI = Cast<UTinySwordGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

    bShowMouseCursor = true; 
    

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
    controlledChar = Cast<AGoblin>(aPawn);
    if (controlledChar && GI)
    {
        controlledChar->SetTagId(GI->GetTagId());
        controlledChar->SetPlayerController(this);
        TagId = GI->GetTagId(); 
        UE_LOG(LogTemp, Warning, TEXT("Possessed Char: %s"), *aPawn->GetName());
    }
    
}

void ATinySwordPlayerController::OnUnPossess()
{
    Super::OnUnPossess();
}
