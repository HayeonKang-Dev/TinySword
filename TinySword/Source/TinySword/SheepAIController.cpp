// Fill out your copyright notice in the Description page of Project Settings.


#include "SheepAIController.h"
#include "Kismet/GameplayStatics.h"
#include "TinySwordGameMode.h"
#include "BaseAISheep.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "protocol.h"


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
    ControlledCharacter = Cast<ABaseAISheep>(InPawn);

    if (ControlledCharacter)    GetWorldTimerManager().SetTimer(Timer, this, &ASheepAIController::MoveRandomPos, 3.f, true, 0.f);
    
    
}

void ASheepAIController::OnUnPossess()
{
    Super::OnUnPossess(); 
    GetWorldTimerManager().ClearTimer(Timer);
}
///////////////



bool ASheepAIController::IsDead() const
{
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

        SendMoveResponseMsg(1, ControlledCharacter->GetTagId(), RandomPos.Location, ControlledCharacter->GetVelocity().Size());
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
        {
            SendMoveResponseMsg(1, ControlledCharacter->GetTagId(), NavLocation.Location, ControlledCharacter->GetVelocity().Size());
        }
            

        
    }
}
////////////////////////////

void ASheepAIController::SendMoveResponseMsg(int ActorType, int ActorIndex, FVector destination, float Speed)
{
    struct Move::Response *response = new Move::Response(); 
    response->H.Command = 0x11; 
    response->ActorType = ActorType; 
    response->ActorIndex = ActorIndex; 
    response->Destination = destination;
    response->Speed = Speed; 
    GameMode->messageQueue.push((struct HEAD *)response);
}

void ASheepAIController::SendMoveNotiMsg(int actorType, int actorIndex, float X, float Y)
{
    struct Move::Notification *noti = new Move::Notification(); 
    noti->H.Command = 0x12;
    noti->ActorType = actorType; 
    noti->ActorIndex = actorIndex; 
    noti->X = X; 
    noti->Y = Y;
    GameMode->messageQueue.push((struct HEAD *)noti);
}
