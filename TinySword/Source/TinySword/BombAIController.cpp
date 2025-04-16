// Fill out your copyright notice in the Description page of Project Settings.


#include "BombAIController.h"
#include "BaseCastle.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "BaseBomb.h"
#include "protocol.h"




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

    // if (controlledBomb)
    // {
    //     GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ABombAIController::CheckOwnerTagId);
    // }
    
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
        if (LastTargetLocation == controlledBomb->GetActorLocation())
        {
            bIsMoving = false; 
        }
        // MoveToCastle(EnemyCastleLocation);
        // const float DistanceSquared = FVector::DistSquared(controlledBomb->GetActorLocation(), EnemyCastleLocation); 
        // if (DistanceSquared <= FMath::Square(100.0f))
        // {
        //     if (!bHasArrived) bHasArrived = true;
        //     else if (bHasArrived && !bIsReadyToExplode)
        //     {
        //         controlledBomb->PlayBrinkAnim();
        //         ElapsedTime += DeltaTime; 
        //         if (ElapsedTime >= 1.5f)
        //         {
        //             bIsReadyToExplode = true; 
        //             controlledBomb->PlayExplodeAnim();
        //             /*SendBombExpResponseMsg(); 
        //             SendBombExpNotiMsg(controlledBomb->GetActorLocation().X, controlledBomb->GetActorLocation().Y);*/

        //             ElapsedTime = 0.0f;
        //         }
        //     }
        // }

        // if (bIsReadyToExplode && controlledBomb)
        // {
        //     ElapsedTime += DeltaTime; 
        //     if (ElapsedTime >= 0.25f)
        //     {
        //         controlledBomb->DealRadialDamage();
        //         // AddToReuseId(); 

        //         /*SendDestroyResponseMsg(0, controlledBomb->GetTagId(), controlledBomb->GetActorLocation().X, controlledBomb->GetActorLocation().Y); 
        //         SendDestroyNotiMsg(0, controlledBomb->GetTagId(), controlledBomb->GetActorLocation().X, controlledBomb->GetActorLocation().Y);*/

        //         // controlledBomb->Destroy();
        //     }
        // }
    }

    
}

bool ABombAIController::IsDead() const
{
    if (controlledBomb != nullptr) return controlledBomb->IsDead(); 
    return true;
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
            // SendMoveResponseMsg(2, controlledBomb->GetTagId(), ClosetPoint.Location, controlledBomb->GetSpeed()); 
        }
    }

}

////////////////////////////////////////////////////
//void ABombAIController::SendBombExpResponseMsg()
//{
//    struct BombExplode::Response *response = new BombExplode::Response(); 
//    response->H.Command = 11;//0x51; 
//    GameMode->messageQueue.push((struct HEAD *)response);
//}
//
//void ABombAIController::SendBombExpNotiMsg(float X, float Y)
//{
//    struct BombExplode::Notification *noti = new BombExplode::Notification(); 
//    noti->H.Command = 12;//0x52; 
//    noti->X = X; 
//    noti->Y = Y; 
//    GameMode->messageQueue.push((struct HEAD *)noti);
//}
//
//void ABombAIController::SendDestroyResponseMsg(int actorType, int actorIndex, float X, float Y)
//{
//    struct Destroy::Response *response = new Destroy::Response(); 
//    response->H.Command = 15;//0x71; 
//    response->ActorType = actorType; 
//    response->ActorIndex = actorIndex; 
//    response->X = X; 
//    response->Y = Y; 
//    GameMode->messageQueue.push((struct HEAD *)response);
//}
//
//void ABombAIController::SendDestroyNotiMsg(int actorType, int actorIndex, float X, float Y)
//{
//    struct Destroy::Notification *noti = new Destroy::Notification(); 
//    noti->H.Command = 16;//0x72;
//    noti->ActorType = actorType; 
//    noti->ActorIndex = actorIndex; 
//    noti->X = X; 
//    noti->Y = Y; 
//    GameMode->messageQueue.push((struct HEAD *)noti);
//}
//
//void ABombAIController::SendMoveResponseMsg(int ActorType, int ActorIndex, FVector Destination, float speed)
//{
//    struct Move::Response *response = new Move::Response();      
//    response->H.Command = 3;//0x11; 
//    response->ActorType = ActorType; 
//    response->ActorIndex = ActorIndex; 
//    response->Destination = Destination; 
//    response->Speed = speed; 
//    GameMode->messageQueue.push((struct HEAD *)response);
//}
//
//void ABombAIController::SendMoveNotiMsg(int actorType, int actorIndex, FVector location)
//{
//    struct Move::Notification *noti = new Move::Notification(); 
//    noti->H.Command = 4;//0x12; 
//    noti->ActorType = actorType; 
//    noti->ActorIndex = actorIndex; 
//    noti->Location = location;
//    GameMode->messageQueue.push((struct HEAD *)noti);
//}


/////////////////////////////////////////////
// TArray<FVector> ABombAIController::GetAllPathPoints(FVector &StartLocation, FVector &EndLocation)
// {
   
//     TArray<FVector> AllPathPoints; 

//     UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
//     if (!NavSystem)
//     {
//         return AllPathPoints;
//     }
    
//     // 경로 찾기
//     UNavigationPath* NavPath = NavSystem->FindPathToLocationSynchronously(GetWorld(), StartLocation, EndLocation);

    
//     if (NavPath && NavPath->IsValid())
//     {
       
//         const TArray<FVector>& PathPoints = NavPath->PathPoints; 
//         //UE_LOG(LogTemp, Log, TEXT("경로 포인트 개수: %d"), PathPoints.Num());
    
//         for (int i = 0; i < PathPoints.Num(); i++)
//         {
//             FVector Point = PathPoints[i];
//             AllPathPoints[i] = Point;
//             //UE_LOG(LogTemp, Log, TEXT("경유지 %d: X=%f, Y=%f, Z=%f"), i, Point.X, Point.Y, Point.Z);
//         }
//     }
//     return AllPathPoints;
// }