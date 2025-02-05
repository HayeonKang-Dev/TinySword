// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayingWidget.h"
#include "Blueprint/UserWidget.h"
#include "Goblin.h"
#include "Kismet/GameplayStatics.h"
#include "TinySwordPlayerController.h"
#include "TinySwordGameMode.h"
#include "NavigationSystem.h"
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

    // 초기화
    HPBar->SetPercent(100.0f);

    FString moneyStr = FString::Printf(TEXT("%d"), 0);
    moneyCount->SetText(FText::FromString(moneyStr));

    //playerController->playingWidget = this;
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
            
            AActor* OwnerActor = controlledChar; 
            AActor* SpawnedActor = World->SpawnActor<AActor>(GeneratedBP->GeneratedClass, GetBombSpawnPoint(World, *FoundLocation), OwnerActor->GetActorRotation(), SpawnParams);
            
            
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

FVector UPlayingWidget::GetBombSpawnPoint(UWorld *World, FVector &FoundLocation)
{
    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(World);
    if (!NavSys) return FVector::ZeroVector;

    SpawnLocation = FoundLocation; 
    SpawnLocation.X += 100.0f;
    SpawnLocation.Z = -275.647822f; 
    SpawnLocation.Y += 80.0f;

    FNavLocation DestinationLocation;
    FVector QueryExtent(100.0f, 100.0f, 10.0f);
    UE_LOG(LogTemp, Warning, TEXT("Entered in GetBombSpawnPoint..."));
    // if (NavSys->ProjectPointToNavigation(SpawnLocation, DestinationLocation, QueryExtent)) return DestinationLocation.Location;
    // {
    //     UE_LOG(LogTemp, Warning, TEXT("Spawn Location Update to Nav : %s"), *DestinationLocation.Location.ToString());
    //     return DestinationLocation.Location;
    // }
    // if (NavSys->GetRandomPointInNavigableRadius(SpawnLocation, 300.0f, DestinationLocation)) return DestinationLocation.Location;
    return SpawnLocation;
}

void UPlayingWidget::UpdateHealthBar()
{
    if (HPBar) HPBar-> SetPercent(GetHpBarPercent());
}

