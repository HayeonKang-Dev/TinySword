// Fill out your copyright notice in the Description page of Project Settings.


#include "BombAIController.h"
#include "BaseCastle.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "BaseBomb.h"

void ABombAIController::BeginPlay()
{
    Super::BeginPlay(); 

    GameMode = (ATinySwordGameMode*)GetWorld()->GetAuthGameMode();  
}


void ABombAIController::OnPossess(APawn *InPawn)
{
    Super::OnPossess(InPawn); 
    UE_LOG(LogTemp, Warning, TEXT("Start Possess Bomb"));
    controlledBomb = Cast<ABaseBomb>(GetPawn()); 
    bHasArrived = false; 
    bIsReadyToExplode = false; 
    ElapsedTime = 0.0f;

    
    //controlledBomb = Cast<ABaseBomb>(InPawn); 

    if (controlledBomb)
    {
        GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ABombAIController::CheckOwnerTagId);
    }
    
}

void ABombAIController::OnUnPossess()
{
    Super::OnUnPossess();
}


void ABombAIController::CheckOwnerTagId()
{
    UE_LOG(LogTemp, Warning, TEXT("Controlled Bomb is not null"));
    CurrentLocation = controlledBomb->GetActorLocation(); 
    EnemyCastleLocation = GetRandomCastleLocation(controlledBomb->GetOwnerTagId()); 
    UE_LOG(LogTemp, Warning, TEXT("OwnerTagId: %d / Destination_Origin : %s"), controlledBomb->GetOwnerTagId(), *EnemyCastleLocation.ToString());

    FVector NewLocation(EnemyCastleLocation.X, EnemyCastleLocation.Y, CurrentLocation.Z);
    EnemyCastleLocation = NewLocation;
    UE_LOG(LogTemp, Warning, TEXT("Destination : %s"), *EnemyCastleLocation.ToString());
}


void ABombAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!IsDead())
    {
       
        MoveToCastle(EnemyCastleLocation);
        const float DistanceSquared = FVector::DistSquared(controlledBomb->GetActorLocation(), EnemyCastleLocation); 
        if (DistanceSquared <= FMath::Square(100.0f))
        {
            if (!bHasArrived) bHasArrived = true;
            else if (bHasArrived && !bIsReadyToExplode)
            {
                controlledBomb->PlayBrinkAnim();
                ElapsedTime += DeltaTime; 
                if (ElapsedTime >= 1.5f)
                {
                    bIsReadyToExplode = true; 
                    controlledBomb->PlayExplodeAnim();
                    ElapsedTime = 0.0f;
                }
            }
        }

        if (bIsReadyToExplode && controlledBomb)
        {
            ElapsedTime += DeltaTime; 
            if (ElapsedTime >= 0.25f)
            {
                controlledBomb->DealRadialDamage();
                AddToReuseId(); 
                controlledBomb->Destroy();
            }
        }
    }

    
}

bool ABombAIController::IsDead() const
{
    if (controlledBomb != nullptr) return controlledBomb->IsDead(); 
    return true;
}

void ABombAIController::AddToReuseId()
{
    // 폭발된 폭탄의 Id 재사용하기 위해 모아둠 
    GameMode->ReuseBombId.Enqueue(controlledBomb->GetTagId());

    // Bomb 생성 시 ActiveBombId 기록은 widget class에서 수행 
}

FVector ABombAIController::GetRandomCastleLocation(int TagId)
{
    TArray<FVector> ValidCastleLocations; 

    for (const TPair<int32, FVector>& CastlePair : GameMode->GetCastleMap())
    {
        if (CastlePair.Key != TagId) ValidCastleLocations.Add(CastlePair.Value);
    }

    if (ValidCastleLocations.Num() > 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Valid Castle numebr: %d"), ValidCastleLocations.Num());
        int32 RandomIndex = FMath::RandRange(0, ValidCastleLocations.Num() - 1); 
        UE_LOG(LogTemp, Warning, TEXT("TagId: %d / Selected Castle Num: %d"), TagId, RandomIndex);
        return ValidCastleLocations[RandomIndex];
    }
    else return FVector::ZeroVector;
}

void ABombAIController::MoveToCastle(const FVector &CastleLocation)
{
    FNavLocation ClosetPoint; 
    if(CastleLocation != FVector::ZeroVector)
    {
        UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()); 
        if (NavSystem && NavSystem->ProjectPointToNavigation(CastleLocation, ClosetPoint, FVector(150.0f, 150.0f, 70.0f)))
        {
            // UE_LOG(LogTemp, Warning, TEXT("Bomb AI Controller - Find Destination in NAV"));
            UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, ClosetPoint.Location);
            // MoveToLocation(ClosetPoint.Location); 
        }
    }
}
