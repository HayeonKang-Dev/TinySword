// Fill out your copyright notice in the Description page of Project Settings.


#include "TinySwordPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Goblin.h"
#include "BaseBomb.h"
#include "TinySwordGameMode.h"
#include "TinySwordGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "PlayingWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "GameFramework/HUD.h"



void ATinySwordPlayerController::BeginPlay()
{
    Super::BeginPlay();

    GameMode = Cast<ATinySwordGameMode>(GetWorld()->GetAuthGameMode());
    GI = Cast<UTinySwordGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

    
    bShowMouseCursor = true;
  
    FindPlayingWidget();
    
    // onpossess


}

void ATinySwordPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime); 


    // float Right = IsInputKeyDown(EKeys::D) - IsInputKeyDown(EKeys::A); 
    // float Up = IsInputKeyDown(EKeys::W) - IsInputKeyDown(EKeys::S); 
    // if (Right != 0.0f || Up != 0.0f)
    // {
    //     if (controlledChar)
    //     {
    //         SendMoveRequestMsg(controlledChar->GetTagId(), Up < 0.0f, Up > 0.0f, Right > 0.0f, Right < 0.0f); 
    //     }
    // }
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

void ATinySwordPlayerController::FindPlayingWidget()
{
    TArray<UUserWidget*> FoundWidgets;
    UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundWidgets, UPlayingWidget::StaticClass(), true);

    if (FoundWidgets.Num() > 0)
    {
        playingWidget = Cast<UPlayingWidget>(FoundWidgets[0]);
        if (playingWidget)
        {
            // UE_LOG(LogTemp, Warning, TEXT("Found and set CharacterSelectWidget."));
        }
    }
}


// void ATinySwordPlayerController::SendMoveRequestMsg(short ActorTagId, bool bMoveUp, bool bMoveDown, bool bMoveRight, bool bMoveLeft)
// {

//     Move::Request Request; 
//     Request.MoveActorType = GOBLIN;
//     Request.MoveActorTagId = ActorTagId; 
//     Request.bMoveUp = bMoveUp; 
//     Request.bMoveDown = bMoveDown; 
//     Request.bMoveRight = bMoveRight; 
//     Request.bMoveLeft = bMoveLeft;
//     Request.Location = controlledChar->GetActorLocation(); /////////////

//     FArrayWriter WriterArray; 
//     WriterArray.Serialize((UTF8CHAR*)&Request, sizeof(Request));
//     TSharedPtr<FBufferArchive> Packet = FTCPSocketClient_Async::CreatePacket((short)MOVE_REQUEST, WriterArray.GetData(), WriterArray.Num());

//     // GameMode->GetTCPSocketClient().BeginSendPhase(Packet);
//     GI->GetTCPClient()->BeginSendPhase(Packet);
// }
