// Fill out your copyright notice in the Description page of Project Settings.


#include "TinySwordPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Goblin.h"
#include "BaseBomb.h"
#include "TinySwordGameMode.h"
#include "TinySwordGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"


void ATinySwordPlayerController::BeginPlay()
{
    Super::BeginPlay();

    GameMode = Cast<ATinySwordGameMode>(GetWorld()->GetAuthGameMode());
    GI = Cast<UTinySwordGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

    
    bShowMouseCursor = true;
  
    
    
    // onpossess


}

void ATinySwordPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime); 

    // SendTimer += GetWorld()->DeltaTimeSeconds; 
    // if (SendTimer >= 0.2f)
    // {
    //     bool bMoveUp = IsInputKeyDown(EKeys::W); 
    //     bool bMoveDown = IsInputKeyDown(EKeys::S); 
    //     bool bMoveRight = IsInputKeyDown(EKeys::D); 
    //     bool bMoveLeft = IsInputKeyDown(EKeys::A); 
    //     if (bMoveUp || bMoveDown || bMoveLeft || bMoveRight)
    //     {
    //         controlledChar->SendMoveResponseMsg(0, controlledChar->GetTagId(), bMoveUp, bMoveDown, bMoveRight, bMoveLeft);
    //     }
    //     SendTimer = 0.0f;
    // }

    bool bMoveUp = IsInputKeyDown(EKeys::W); 
    bool bMoveDown = IsInputKeyDown(EKeys::S); 
    bool bMoveRight = IsInputKeyDown(EKeys::D); 
    bool bMoveLeft = IsInputKeyDown(EKeys::A); 
    if (bMoveUp || bMoveDown || bMoveLeft || bMoveRight)
    {
        // controlledChar->SendMoveResponseMsg(0, controlledChar->GetTagId(), bMoveUp, bMoveDown, bMoveRight, bMoveLeft);
    }


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





/////////////////////////////////////////////////////////////////////

void ATinySwordPlayerController::SpawnBomb(FVector spawnLocation, int tagId)
{
    if (GameMode)
    {
        // TMap<int32, FVector>& CastleMap = GameMode->GetCastleMap();
        UObject* spawnActor = nullptr; 
        switch (tagId)
        {
            case 1: 
                spawnActor = StaticLoadObject(UObject::StaticClass(), NULL, TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Bombs/BP_Bomb.BP_Bomb'"));
                break;

            case 2: 
                spawnActor = StaticLoadObject(UObject::StaticClass(), NULL, TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Bombs/BP_Bomb_Purple.BP_Bomb_Purple'"));
                break;

            case 3: 
                spawnActor = StaticLoadObject(UObject::StaticClass(), NULL, TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Bombs/BP_Bomb_Red.BP_Bomb_Red'"));
                break;

            case 4: 
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

        // FVector* FoundLocation = CastleMap.Find(controlledChar->GetTagId()); //////////////////////////
       
        // if(FoundLocation)
        // {
            
            AActor* OwnerActor = controlledChar; 
            // AActor* SpawnedActor = World->SpawnActor<AActor>(GeneratedBP->GeneratedClass, GetBombSpawnPoint(*FoundLocation), OwnerActor->GetActorRotation(), SpawnParams);
            AActor* SpawnedActor = World->SpawnActor<AActor>(GeneratedBP->GeneratedClass, spawnLocation, OwnerActor->GetActorRotation(), SpawnParams);

            if (SpawnedActor)
            {
                SpawnedBomb = Cast<ABaseBomb>(SpawnedActor); 
                if (SpawnedBomb) {
                    SetBombIndex();
                    SpawnedBomb->SetOwnerTagId(tagId); /////////////////////////////
                    // UE_LOG(LogTemp, Warning, TEXT("Owner Tag Id (PC) : %d -> %d"), playerController->GetTagId(), SpawnedBomb->GetOwnerTagId());

                    // SendSpawnResponseMsg(); 
                    // SendSpawnNotiMsg(1, SpawnedBomb->GetTagId(), SpawnedBomb->GetActorLocation().X, SpawnedBomb->GetActorLocation().Y);
                }
            }
        // }
    }
}

void ATinySwordPlayerController::SetBombIndex()
{
    int32 id = 0; 
    if (!GameMode->ReuseBombId.IsEmpty()) GameMode->ReuseBombId.Dequeue(id);
    else id = GameMode->ActiveBombId.Num() + 1; 
    SpawnedBomb->SetTagId(id); 
    GameMode->ActiveBombId.Add(SpawnedBomb, id);
}



FVector ATinySwordPlayerController::GetBombSpawnPoint(FVector &FoundLocation)
{
    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
    if (!NavSys) return FVector::ZeroVector;

    SpawnLocation = FoundLocation; 
    SpawnLocation.X += 100.0f;
    SpawnLocation.Z = -275.647822f; 
    SpawnLocation.Y += 80.0f;

    FNavLocation DestinationLocation;
    FVector QueryExtent(100.0f, 100.0f, 10.0f);
    UE_LOG(LogTemp, Warning, TEXT("Entered in GetBombSpawnPoint..."));
    return SpawnLocation;
}

////////////////////////////////////////////////////////////////
// void ATinySwordPlayerController::SpawnGoblin(FVector spawnLocation, int tagId)
// {
//     if (GameMode)
//     {
//         TMap<int32, FVector>& CastleMap = GameMode->GetCastleMap();
//         UObject* spawnActor = nullptr; 

     
//         // if (GI)
//         // {
//             switch(tagId)
//             {
//                 case 0: 
//                     spawnActor = StaticLoadObject(UObject::StaticClass(), NULL, TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Goblins/Goblin_Blue.Goblin_Blue'")); 
//                     break; 

//                 case 1: 
//                     spawnActor = StaticLoadObject(UObject::StaticClass(), NULL, TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Goblins/Goblin_Purple.Goblin_Purple'")); 
//                     break; 

//                 case 2: 
//                     spawnActor = StaticLoadObject(UObject::StaticClass(), NULL, TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Goblins/Goblin_Red.Goblin_Red'")); 
//                     break; 

//                 case 3: 
//                     spawnActor = StaticLoadObject(UObject::StaticClass(), NULL, TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Goblins/Goblin_Yellow.Goblin_Yellow'")); 
//                     break; 

//                 default: 
//                     UE_LOG(LogTemp, Warning, TEXT("Spawn Actor is not valid"));
//                     break;

//             // }

//         }

        
//         UBlueprint* GeneratedBP = Cast<UBlueprint>(spawnActor); 
//         UWorld* World = GetWorld(); 

//         if (!spawnActor || !GeneratedBP || !GeneratedBP->GeneratedClass || !World) return; 
        
//         FActorSpawnParameters SpawnParams; 
//         SpawnParams.Owner = this; 
//         SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; 


//         FRotator SpawnRotation(0.0f, 0.0f, 0.0f);
//         AGoblin* SpawnedChar = World->SpawnActor<AGoblin>(GeneratedBP->GeneratedClass, spawnLocation, SpawnRotation, SpawnParams);

//         if (SpawnedChar) 
//         {
//             UE_LOG(LogTemp, Warning, TEXT("SpawnedChar : %s"), *SpawnedChar->GetName());
//             // PlayerController->
//             OnPossess(SpawnedChar);
//             // PlayerController->SetViewTargetWithBlend(SpawnedChar, 0.0f);

//             // SendSelectCharResponseMsg(tagId);
//             char myPlayerId[40] = "test";
//             // SendSelectCharNotiMsg(myPlayerId, tagId);

//             //SendSpawnResponseMsg(0, GI->GetTagId(), SpawnCharLocation); /////////////////////////////////////
//             // SendSpawnNotiMsg(0, tagId, spawnLocation.X, spawnLocation.Y);
//             GI->SetChar(SpawnedChar);
//             GameMode->GoblinMap.Add(SpawnedChar, tagId);
//         }
//     }
// }