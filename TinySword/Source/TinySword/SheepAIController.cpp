// Fill out your copyright notice in the Description page of Project Settings.


#include "SheepAIController.h"
#include "Kismet/GameplayStatics.h"
#include "TinySwordGameMode.h"
#include "BaseAISheep.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"


void ASheepAIController::BeginPlay()
{
    Super::BeginPlay();

    GameMode = (ATinySwordGameMode *)GetWorld()->GetAuthGameMode(); 
    NaviSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
}


//////////////
void ASheepAIController::OnPossess(APawn *InPawn)
{
    Super::OnPossess(InPawn); 
    GetWorldTimerManager().SetTimer(Timer, this, &ASheepAIController::MoveRandomPos, 3.f, true, 0.f);
}

void ASheepAIController::OnUnPossess()
{
    Super::OnUnPossess(); 
    GetWorldTimerManager().ClearTimer(Timer);
}
///////////////



bool ASheepAIController::IsDead() const
{
    ABaseAISheep* ControlledCharacter = Cast<ABaseAISheep>(GetPawn());

    if (ControlledCharacter != nullptr) return ControlledCharacter->IsDead(); 

    return true;
}



////////////////
void ASheepAIController::MoveRandomPos()
{
    if (!IsValid(NaviSystem)) return; 

    FNavLocation RandomPos;
    if (NaviSystem->GetRandomPointInNavigableRadius(GetPawn()->GetActorLocation(), 200.f, RandomPos))
    {
        FVector Velocity = GetPawn()->GetVelocity(); 
        UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, RandomPos); 
    }
}

void ASheepAIController::FleeFrom(const FVector &AttackLocation)
{
    FVector SheepLocation = GetPawn()->GetActorLocation(); 

    // Get Opposition Vector from Attacker
    FVector fleeDirection = (SheepLocation - AttackLocation).GetSafeNormal(); 
    float fleeDistance = 1000.f; 
    FVector fleeLocation = SheepLocation + fleeDirection * fleeDistance; 

    // IsValid fleeLocation ?
    if (NaviSystem)
    {
        FNavLocation NavLocation; 
        if (NaviSystem->GetRandomPointInNavigableRadius(fleeLocation, 50.0f, NavLocation))
            UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, NavLocation.Location);
    }
}
////////////////////////////