// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayingWidget.h"
#include "Blueprint/UserWidget.h"
#include "Goblin.h"
#include "Kismet/GameplayStatics.h"
#include "TinySwordPlayerController.h"
#include "TinySwordGameMode.h"
#include "BaseBomb.h"

void UPlayingWidget::NativeConstruct()
{
    Super::NativeConstruct(); 

    playerController = Cast<ATinySwordPlayerController>(GetOwningPlayer()); 
    GameMode = Cast<ATinySwordGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    controlledChar = Cast<AGoblin>(playerController->GetPawn());
    SpawnButton = Cast<UButton>(GetWidgetFromName(TEXT("SpawnButton")));
    HPBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("HPBar"))); 
    moneyCount = Cast<UTextBlock>(GetWidgetFromName(TEXT("MoneyCount")));

    if (SpawnButton) SpawnButton->OnClicked.AddDynamic(this, &UPlayingWidget::OnSpawnButtonClicked);   

}


void UPlayingWidget::OnSpawnButtonClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("SpawnButton Clicked!"));
    if (DecreasePlayerMoney())
    {
        UE_LOG(LogTemp, Warning, TEXT("Decrease Player's Money Success"));
        SpawnBomb();
    }
}

bool UPlayingWidget::DecreasePlayerMoney()
{
    if (playerController && controlledChar) 
    {
        return controlledChar->DecreaseMoney(10);
    }
    return false;
}

void UPlayingWidget::SpawnBomb()
{
    if (GameMode)
    {
        TMap<int32, FVector>& CastleMap = GameMode->GetCastleMap();
        UObject* spawnActor = nullptr; 
        switch (controlledChar->GetTagId())
        {
            case 0: 
                spawnActor = StaticLoadObject(UObject::StaticClass(), NULL, TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Bombs/BP_Bomb.BP_Bomb'"));
                break;

            case 1: 
                spawnActor = StaticLoadObject(UObject::StaticClass(), NULL, TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Bombs/BP_Bomb_Purple.BP_Bomb_Purple'"));
                break;

            case 2: 
                spawnActor = StaticLoadObject(UObject::StaticClass(), NULL, TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Bombs/BP_Bomb_Red.BP_Bomb_Red'"));
                break;

            case 3: 
                spawnActor = StaticLoadObject(UObject::StaticClass(), NULL, TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Bombs/BP_Bomb_Yellow.BP_Bomb_Yellow'"));
                break;

            default: 
                UE_LOG(LogTemp, Warning, TEXT("Case Number is ..."));
                break;
        }

        UBlueprint* GeneratedBP = Cast<UBlueprint>(spawnActor); 
        UWorld* World = GetWorld(); 

        if (!spawnActor || !GeneratedBP || !GeneratedBP->GeneratedClass || !World) {
            return; 
        }

        FActorSpawnParameters SpawnParams; 
        SpawnParams.Owner = controlledChar; 
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; 

        FVector* FoundLocation = CastleMap.Find(controlledChar->GetTagId()); 
       
        if(FoundLocation)
        {
            SpawnLocation = *FoundLocation; 
            SpawnLocation.X += 40.0f;
            SpawnLocation.Z = 73.0f; 
            SpawnLocation.Y += 50.0f;
            
            AActor* OwnerActor = controlledChar; 
            AActor* SpawnedActor = World->SpawnActor<AActor>(GeneratedBP->GeneratedClass, SpawnLocation, OwnerActor->GetActorRotation(), SpawnParams);
            
            
            if (SpawnedActor)
            {
                SpawnedBomb = Cast<ABaseBomb>(SpawnedActor); 
                if (SpawnedBomb) SetBombIndex();
            }
        }
    }
}

void UPlayingWidget::SetBombIndex()
{
    int32 id = 0; 
    if (!GameMode->ReuseBombId.IsEmpty()) GameMode->ReuseBombId.Dequeue(id);
    else id = GameMode->ActiveBombId.Num() + 1; 
    SpawnedBomb->SetTagId(id); 
    GameMode->ActiveBombId.Add(SpawnedBomb, id);
}

float UPlayingWidget::GetHpBarPercent()
{
    UE_LOG(LogTemp, Warning, TEXT("Health(%f) / MaxHealth(%f) = %f"), controlledChar->GetHealth(), 100.0f, controlledChar->GetHealth()/100.0f);
    return controlledChar->GetHealthPercent();
}

void UPlayingWidget::UpdateHealthBar()
{
    if (HPBar) HPBar-> SetPercent(GetHpBarPercent());
}

