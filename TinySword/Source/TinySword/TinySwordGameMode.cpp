// Fill out your copyright notice in the Description page of Project Settings.


#include "TinySwordGameMode.h"
#include "BaseCastle.h"
#include "Kismet/GameplayStatics.h"
#include "BaseBomb.h"
#include "Goblin.h"
#include "protocol.h"
#include "BaseGoldBag.h"
#include "BaseGoldMine.h"
#include "BaseAISheep.h"
#include "BaseMeat.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NavigationSystem.h"
#include "SheepAIController.h"
#include "Serialization/ArrayWriter.h"
#include "EngineUtils.h" 
#include "Sockets.h"
#include "SelectCharacterWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/KismetMathLibrary.h"


#include "AsyncNetworking.h"
#include "Misc/DateTime.h"

ATinySwordGameMode::ATinySwordGameMode()
{
    PlayerSpawnLocations.SetNum(5); // 0~4번까지

    PlayerSpawnLocations[1] = FVector(-6401.79f, -5111.76f, -247.27f);
    PlayerSpawnLocations[2] = FVector(-4080.1f, -5107.36f, -247.27f);
    PlayerSpawnLocations[3] = FVector(-6439.94f, -4075.64f, -247.27f);
    PlayerSpawnLocations[4] = FVector(-4038.44f, -4100.43f, -247.27f);
}
// void ATinySwordGameMode::SaveAllPathsToTextFile()
// {
//     FString FullText;

//     for (int32 i = 1; i <= 4; ++i)
//     {
//         for (int32 j = 1; j <= 4; ++j)
//         {
//             if (i == j) continue;

//             FVector Start = PlayerSpawnLocations[i];
//             FVector End = PlayerSpawnLocations[j];
//             TArray<FVector> PathPoints = GetAllPathPoints(Start, End);

//             UE_LOG(LogTemp, Warning, TEXT("Start: %s, End: %s => PathPoints: %d"), *Start.ToString(), *End.ToString(), PathPoints.Num());


//             FullText += FString::Printf(TEXT("Path from %d to %d:\n"), i, j);
//             for (const FVector& Point : PathPoints)
//             {
//                 FullText += Point.ToString() + TEXT("\n");
//             }
//             FullText += TEXT("\n");
//         }
//     }

//     FString FilePath = FPaths::ProjectDir() + TEXT("SavedPaths.txt");
//     FFileHelper::SaveStringToFile(FullText, *FilePath);
// }



void ATinySwordGameMode::StartPlay()
{
    Super::StartPlay(); 

    // UE_LOG(LogTemp, Warning, TEXT("GameMode::StartPlay is Running"));

    //TCPSocketClient_Async.Connect();
    ClickCnt = 0;
    
    GI = Cast<UTinySwordGameInstance>(GetWorld()->GetGameInstance());
    GI->GetInstance(GetWorld());
    // GI->Init();

    TCPClient = GI->GetTCPClient();
    // if (TCPClient.IsValid())
    // {
    //     TCPClient->Initialize(GetWorld());
    // }

    NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
    
    // TCPClient = GI->GetTCPClient();

    

}

void ATinySwordGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    TCPSocketClient_Async.Disconnect(); 
    Super::EndPlay(EndPlayReason);
}

void ATinySwordGameMode::PossessPlayer(ATinySwordPlayerController *PC, short TagId)
{

    // ATinySwordPlayerController* PC = Cast<ATinySwordPlayerController>(NewPlayer); 
    // if (PC)
    // {
    //     const short TagId = PC->GetTagId(); 
        AGoblin* myGoblin = FindGoblinById(GoblinMap, TagId); 
        if (myGoblin)
        {
            // UE_LOG(LogTemp, Warning, TEXT("Player Possess Goblin %d"), TagId);
            PC->Possess(myGoblin);
        }

        for (auto& Pair : GoblinMap)
        {
            if (Pair.Value != TagId)
            {
                AGoblin* otherGoblin = Pair.Key; 
                if (otherGoblin && otherGoblin->GetController() == nullptr)
                {
                    AAIController* AICon = GetWorld()->SpawnActor<AAIController>(
                        AAIController::StaticClass(), otherGoblin->GetActorLocation(), otherGoblin->GetActorRotation());
                    if (AICon)
                    {
                        AICon->Possess(otherGoblin); 
                        // UE_LOG(LogTemp, Warning, TEXT("AI Possess Goblin %d"), Pair.Value);
                    }
                    
                }
            }
        }
    // }
}







void ATinySwordGameMode::FindAllGoblins(UWorld *world)
{
    if (!world) return;
    for(TActorIterator<AGoblin> It(world); It; ++It)
    {
        AGoblin* goblin = *It; 
        if (goblin)
        {
            GoblinMap.Add(goblin, goblin->GetTagId());
        }
    }
}

void ATinySwordGameMode::BeginPlay()
{
    Super::BeginPlay();

    
    // UE_LOG(LogTemp, Warning, TEXT("Starting GameMode"));
    
    // GI = Cast<UTinySwordGameInstance>(GetWorld()->GetGameInstance());
    // GI->GetInstance(GetWorld());
    // GI->Init();

    // if (!bHasInitialized) {
    //     GI->Init();
    //     bHasInitialized = true;
    // // }
    // TCPClient = GI->GetTCPClient();
    // TCPClient->Initialize(GetWorld());

    
    // TCPClient = GI->GetTCPClient();

    
}


void ATinySwordGameMode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    // UE_LOG(LogTemp, Warning, TEXT("Game Mode is running..."));
    // struct HEAD *data = messageQueue.pop(); 
    // if (data == nullptr || data->Command > 22 || data->Command == 0) return;
    // if (PROTOCOLS[data->Command] != nullptr) (this->*PROTOCOLS[data->Command])(data);

    if (TCPClient.IsValid() && TCPClient->GetSocket().IsValid())// && bIsConnected) // GI->
    {
        // TCPClient->BeginRecvPhase();
        // 데이터 수신 시작
        //TCPClient->BeginRecvPhase(); /////////////////////////////////////////////////
        // UE_LOG(LogTemp, Warning, TEXT("Socket connection State: %d"), (int32)TCPClient->GetSocket().Get()->GetConnectionState());

        uint32 PendingDataSize = 0;
        if (TCPClient->GetSocket().Get()->HasPendingData(PendingDataSize) && PendingDataSize > 0)
        {
            // UE_LOG(LogTemp, Warning, TEXT("Is Packet Comming.!"));
            TCPClient->BeginRecvPhase();
        }
        

        // BeginRecvPhase에서 PROTOCOLS[data->Command]???? 



        // 응답 

    }

}


/// @brief 경유지 구하기 -> BOMB의 Spawn::Noti 도착 시, 계산해서 서버에 Request  
/// @param StartLocation 
/// @param EndLocation 
/// @return 2차원 경유지 배열 
TArray<FVector> ATinySwordGameMode::GetAllPathPoints(FVector &StartLocation, FVector &EndLocation)
{
   
    TArray<FVector> AllPathPoints; 

    UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (!NavSystem)
    {
        return AllPathPoints;
    }
    
    // 경로 찾기
    UNavigationPath* NavPath = NavSystem->FindPathToLocationSynchronously(GetWorld(), StartLocation, EndLocation);

    
    if (NavPath && NavPath->IsValid())
    {
       
        const TArray<FVector>& PathPoints = NavPath->PathPoints; 
        UE_LOG(LogTemp, Log, TEXT("경로 포인트 개수: %d"), PathPoints.Num());
    
        for (int i = 0; i < PathPoints.Num(); i++)
        {
            // FVector Point = PathPoints[i];
            // AllPathPoints[i] = Point;
            AllPathPoints.Add(PathPoints[i]);
            //UE_LOG(LogTemp, Log, TEXT("경유지 %d: X=%f, Y=%f, Z=%f"), i, Point.X, Point.Y, Point.Z);
        }
    }
    return AllPathPoints;
}

void ATinySwordGameMode::FindCastlesLocation()
{
    TArray<AActor*> AllCastles; 
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseCastle::StaticClass(), AllCastles); 
    for(AActor* Castle : AllCastles)
    {
        ABaseCastle* castle = Cast<ABaseCastle>(Castle); 
        CastleMap.Add(castle->GetTagId(), castle->GetActorLocation());
    }
    UE_LOG(LogTemp, Warning, TEXT("Find Castle Num : %d"), AllCastles.Num());
}

void ATinySwordGameMode::CollectAllCastles()
{
    TArray<AActor*> AllCastles; 
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseCastle::StaticClass(), AllCastles); 
    for(AActor* Castle : AllCastles)
    {
        ABaseCastle* castle = Cast<ABaseCastle>(Castle); 
        ActiveCastleMap.Add(castle, castle->GetTagId());
    }
}

void ATinySwordGameMode::CollectGoldMine()
{
    TArray<AActor*> AllGoldMine; 
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseGoldMine::StaticClass(), AllGoldMine); 
    for(AActor* goldmine : AllGoldMine)
    {
        ABaseGoldMine* GoldMine = Cast<ABaseGoldMine>(goldmine);
        ActiveGoldMineMap.Add(GoldMine, GoldMine->GetTagId());
    }
}

void ATinySwordGameMode::CollectSheep() 
{
    TArray<AActor*> AllSheep; 
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseAISheep::StaticClass(), AllSheep); 
    for (AActor* sheep : AllSheep)
    {
        ABaseAISheep* Sheep = Cast<ABaseAISheep>(sheep); 
        ActiveSheepId.Add(Sheep, Sheep->GetTagId());
    }
}


ATinySwordPlayerController *ATinySwordGameMode::FindControllerById(const TMap<ATinySwordPlayerController *, int32> &Map, int32 TargetValue)
{
    for (const auto& Pair : Map)
    {
        if (Pair.Value == TargetValue) return Pair.Key;
    }
    return nullptr;
}

ABaseGoldMine *ATinySwordGameMode::FindGoldMineById(const TMap<ABaseGoldMine *, int32> &Map, int32 TargetValue)
{
    for (const auto& Pair : Map)
    {
        if (Pair.Value == TargetValue) return Pair.Key;
    }
    return nullptr;
}

ABaseCastle *ATinySwordGameMode::FindCastleById(const TMap<ABaseCastle *, int32>&Map, int32 TargetValue)
{
    for (const auto& Pair : Map)
    {
        if (Pair.Value == TargetValue) return Pair.Key;
    }
    return nullptr;
}

AGoblin *ATinySwordGameMode::FindGoblinById(const TMap<AGoblin*, int32> &Map, int32 TargetValue)
{
    for (const auto& Pair : Map)
    {
        if (Pair.Value == TargetValue) return Pair.Key;
    }
    return nullptr;
}

FVector ATinySwordGameMode::FindCastleLocationByTagId(int32 TagId)
{
    if (CastleMap.Contains(TagId)) return *CastleMap.Find(TagId);
    else return FVector::ZeroVector;
}

ABaseBomb *ATinySwordGameMode::FindBombById(const TMap<ABaseBomb *, int32> &Map, int32 TargetValue)
{
    for (const auto& Pair : Map)
    {
        if (Pair.Value == TargetValue) return Pair.Key;
    }
    return nullptr;
}

ABaseGoldBag *ATinySwordGameMode::FindGoldBagById(const TMap<ABaseGoldBag *, int32> &Map, int32 TargetValue)
{
    for (const auto& Pair : Map)
    {
        if (Pair.Value == TargetValue) return Pair.Key; 
    }
    return nullptr;
}


ABaseAISheep *ATinySwordGameMode::FindSheepById(const TMap<ABaseAISheep *, int32> &Map, int32 TargetValue)
{
    for (const auto& Pair : Map)
    {
        if (Pair.Value == TargetValue) return Pair.Key; 
    }
    return nullptr;
}

ABaseMeat *ATinySwordGameMode::FindMeatById(const TMap<ABaseMeat *, int32> &Map, int32 TargetValue)
{
    for (const auto& Pair : Map)
    {
        if (Pair.Value == TargetValue) return Pair.Key; 
    }
    return nullptr;
}

// FVector ATinySwordGameMode::ValidateLocation(ASheepAIController *controller, const FVector &Destination)
FVector ATinySwordGameMode::ValidateLocation(AAIController *controller, const FVector &Destination)
{
    UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(controller->GetWorld());
    if (!controller || !NavSystem) return Destination; 

    FNavLocation ProjectedLocation; 
    if (NavSystem->ProjectPointToNavigation(
        Destination,
        ProjectedLocation,
        FVector(100.0f, 100.0f, 100.0f)))
        {
            return ProjectedLocation;
        }
    
    return Destination;
}

void ATinySwordGameMode::Interpolation(AActor *actor, FVector Destination, double seconds)
{
    if (!actor || seconds <= 0.0f)
    {
        if (actor)
        {
            actor->SetActorLocation(Destination, true);
        }
        return;
    }

    // 기존 보간 중이면 타이머 정지 및 제거
    if (FTimerHandle* ExistingHandle = ActiveInterpolations.Find(actor))
    {
        GetWorld()->GetTimerManager().ClearTimer(*ExistingHandle);
        ActiveInterpolations.Remove(actor);
    }


    FVector StartLocation = actor->GetActorLocation();
    float ElapsedTime = 0.f;

    FTimerHandle TimerHandle;

    // 새 타이머 등록
    ActiveInterpolations.Add(actor, TimerHandle);

    GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([=]() mutable {
        if (!IsValid(actor))
        {
            GetWorld()->GetTimerManager().ClearTimer(ActiveInterpolations[actor]);
            ActiveInterpolations.Remove(actor);
            return;
        }

        ElapsedTime += GetWorld()->GetDeltaSeconds();
        float Alpha = FMath::Clamp(ElapsedTime / seconds, 0.0f, 1.0f);

        FVector NewLocation = FMath::Lerp(StartLocation, Destination, Alpha);
        actor->SetActorLocation(NewLocation, true);

        if (Alpha >= 1.0f)
        {
            GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
            ActiveInterpolations.Remove(actor);
        }

    }), GetWorld()->GetDeltaSeconds(), true);
}


void ATinySwordGameMode::SpawnBomb(FVector spawnLocation, short OwnertagId, short BombTagId)
{
    // TMap<int32, FVector>& CastleMap = GameMode->GetCastleMap();
    UObject* spawnActor = nullptr; 
    switch (OwnertagId)
    {
        UE_LOG(LogTemp, Warning, TEXT("!>!>!>!>!>!>!>!>!>!>!>!> SPAWNACTOR BOMB IS NOT NULLPTR"));
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

    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; 
      
    // AActor* SpawnedActor = World->SpawnActor<AActor>(GeneratedBP->GeneratedClass, GetBombSpawnPoint(*FoundLocation), OwnerActor->GetActorRotation(), SpawnParams);
    AActor* SpawnedActor = World->SpawnActor<AActor>(GeneratedBP->GeneratedClass, spawnLocation, FRotator(0, 0, 0), SpawnParams);

    if (SpawnedActor)
    {
        ABaseBomb* SpawnedBomb = Cast<ABaseBomb>(SpawnedActor); 
        if (SpawnedBomb) {
            SpawnedBomb->SetTagId(BombTagId);
            // SpawnedBomb->SetOwnerTagId(tagId); 
            ActiveBombId.Add(SpawnedBomb, BombTagId); 
        }
    }
}


void ATinySwordGameMode::SpawnMeat(FVector Location, short tagId)
{
    UObject* SpawnActor = StaticLoadObject(UObject::StaticClass(), NULL, TEXT("/Game/Blueprints/BP_Meat.BP_Meat")); 
    UBlueprint* GeneratedBP = Cast<UBlueprint>(SpawnActor);
    UWorld* World = GetWorld(); 

    if (!SpawnActor || !GeneratedBP || !GeneratedBP->GeneratedClass || !World) return;

    FActorSpawnParameters SpawnParams; 
	// SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // Adjust Spawn Location (Lower Z)
    FVector SpawnLocation = Location;
    //SpawnLocation.Z = 45.f;

    // Spawn Meat & Set Meat TagId
    AActor* SpawnedActor; 
	SpawnedActor = World->SpawnActor<AActor>(GeneratedBP->GeneratedClass, SpawnLocation, FRotator(0, 0, 0), SpawnParams);
    ABaseMeat* SpawnedMeat = Cast<ABaseMeat>(SpawnedActor);

    if (SpawnedMeat)
    {
        SpawnedMeat->SetTagId(tagId); 
        ActiveMeatId.Add(SpawnedMeat, tagId);
        
    }

}

void ATinySwordGameMode::SpawnGoldBag(FVector spawnLocation, short tagId)
{

    // 시간 출력
    FDateTime Now = FDateTime::Now();
    FString TimeString = Now.ToString(TEXT("%H:%M:%S.%s"));
    UE_LOG(LogTemp, Warning, TEXT("[SpawnGoldBag] Time: %s | tagId: %d"), *TimeString, tagId);


    // drop goldbag logic
    UObject* SpawnActor = StaticLoadObject(UObject::StaticClass(), NULL, TEXT("/Script/Engine.Blueprint'/Game/Blueprints/BP_GoldBag.BP_GoldBag'"));
    UBlueprint* GeneratedBP = Cast<UBlueprint>(SpawnActor);
    UWorld* World = GetWorld();
    if (!SpawnActor || !GeneratedBP || !GeneratedBP->GeneratedClass || !World) return;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; 

    AActor* SpawnedActor; 
    SpawnedActor = World->SpawnActor<AActor>(GeneratedBP->GeneratedClass, spawnLocation, FRotator(0, 0, 0), SpawnParams);
    ABaseGoldBag* SpawnedGoldBag = Cast<ABaseGoldBag>(SpawnedActor);		
	
    if (SpawnedGoldBag)
    {
        SpawnedGoldBag->SetTagId(tagId);
        ActiveGoldBagId.Add(SpawnedGoldBag, tagId);
    }
}






///////////////////////////////////////////////////////////////////////////////////////////////////////


// Packet Processing Function 

void ATinySwordGameMode::OnCharacterSelectNotification(struct CharacterSelect::Notification *data)
// void ATinySwordGameMode::OnCharacterSelectNotification(HEAD *data)
{
    if (!data)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid HEAD Pointer"));
        return;
    }

    // ClickCnt++;

    // if (data==nullptr) 
    // {
    //     UE_LOG(LogTemp, Warning, TEXT("[OnMoveNotification] MOVE_NOTIFICATION NOT AVAILABLE"));
    //     return;
    // }
    // else if (data != nullptr) {
    //     UE_LOG(LogTemp, Warning, TEXT("[OnMoveNotification] MOVE_NOTIFICATION AVAILABLE : %d"), data->SelectedTagId);
    // }

    if (!GI)
    {
        UE_LOG(LogTemp, Error, TEXT("[NULL POINTER] GAME INSTANCE NULL"));
    }


    FindCharacterSelectWidget();

    if (CharacterSelectWidget == nullptr) 
    {
        UE_LOG(LogTemp, Fatal, TEXT("[NULL POINTER]: CHARACTER SELECT WIDGET"));
        return;
    }
    // CharacterSelect = Cast<USelectCharacterWidget>()
    if (CharacterSelectWidget) ///////////// 왜 NULL?????? 
    {
        UE_LOG(LogTemp, Warning, TEXT("[NOTI->SELECTEDTAGID] %d"), data->SelectedTagId);
        switch (data->SelectedTagId)
        {
        case 1:
            DisableCharacterButton("BlueButton");
            break;
        case 2:
            DisableCharacterButton("PurpleButton");
            break;
        case 3:
            DisableCharacterButton("RedButton");
            break;
        case 4:
            DisableCharacterButton("YellowButton");
            break;
        
        default:
            break;
        }
    }
    
    // SpawnGoblin(noti->SpawnLocation.ToFVector(), noti->SelectedTagId);
    // delete noti;
}

// void ATinySwordGameMode::OnMoveResponse(HEAD *data)
// {
//     if (data->MoveAc)
// }

void ATinySwordGameMode::OnMoveNotification(struct Move::Notification *data)
{
    // struct Move::Notification *noti = (struct Move::Notification *)data; 
    
    // GOBLIN
    if (data->MoveActorType == GOBLIN){
        
        // GOBLIN TagId로 찾아서 SimpleMove
        AGoblin* goblin = FindGoblinById(GoblinMap, data->MoveActorTagId);
        if (goblin) // NOT MY CHARACTER
        {
            //goblin->SetActorLocation(data->Location.ToFVector());
            AAIController* AICon = Cast<AAIController>(goblin->GetController());
            if (AICon)
            {
                FVector ValidLocation = ValidateLocation(AICon, data->Location.ToFVector());
                
                if (!goblin->bIsMovingToTarget || FVector::Dist(goblin->LastTargetLocation, ValidLocation) > 0.01f) // 10.0f
                {
                    goblin->LastTargetLocation = data->Location.ToFVector(); // ValidLocation; 
                    goblin->bIsMovingToTarget = true; 


                    FVector Direc = ValidLocation - goblin->GetActorLocation(); 

                    if (Direc.X > 0) goblin->FlipCharacter(1);
                    else if (Direc.X < 0) goblin->FlipCharacter(-1);

                    UAIBlueprintHelperLibrary::SimpleMoveToLocation(AICon, ValidLocation);
                }
            }
        }

    }

    // SHEEP 
        // noti->Location이 이동 불가 지역일 경우, 수정해서 Request 전송 
    if (data->MoveActorType == SHEEP) {
        
        ABaseAISheep* sheep = FindSheepById(ActiveSheepId, data->MoveActorTagId);
        if (sheep)
        {
            ASheepAIController* sheepController = Cast<ASheepAIController>(sheep->GetController());
            if (sheepController)
            {
                //UE_LOG(LogTemp, Warning, TEXT("================= RECV [%d] SHEEP MOVE NOTIFICATION : X=%f, Y=%f, Z=%f ======================"),
                //                                data->MoveActorTagId, data->Location.ToFVector().X, data->Location.ToFVector().Y, data->Location.ToFVector().Z);
                // FVector ValidLocation = ValidateLocation(sheepController, data->Location.ToFVector());
                FVector ValidLocation = ValidateLocation(Cast<AAIController>(sheepController), data->Location.ToFVector());

                if (data->Location.ToFVector() != ValidLocation)
                {
                    // request 전송해야 함 
                    Move::Request Request; 
                    
                    Request.Location = Vector(ValidLocation.X, ValidLocation.Y, ValidLocation.Z); 
                    Request.MoveActorType = SHEEP; 
                    Request.MoveActorTagId = data->MoveActorTagId;

                    // TArray<uint8> ByteArray = StructToByteArray(Request);

                    FArrayWriter WriterArray; 
                    WriterArray.Serialize((UTF8CHAR*)&Request, sizeof(Request));
                    TSharedPtr<FBufferArchive> Packet = FTCPSocketClient_Async::CreatePacket((short)MOVE_REQUEST, WriterArray.GetData(), WriterArray.Num());
                    
                    // Send PACKET 
                    TCPClient->BeginSendPhase(Packet);
                }

                UAIBlueprintHelperLibrary::SimpleMoveToLocation(sheepController, ValidLocation);
                // sheepController->MoveToLocation(ValidLocation);
            }
        }
        
    }

    // BOMB
    if (data->MoveActorType == BOMB) {
        UE_LOG(LogTemp, Warning, TEXT("[ON MOVE NOTIFICATION] BOMB MOVING... [X = %f, Y = %f, Z = %f]"), 
                                data->Location.ToFVector().X, data->Location.ToFVector().Y, data->Location.ToFVector().Z);
        ABaseBomb* bomb = FindBombById(ActiveBombId, data->MoveActorTagId);
        if (bomb)
        {
            // UE_LOG(LogTemp, Warning, TEXT(">!>!>!>>!>!>!>!>!> BOMB ACTOR IS AVAILABLE"));
            ABombAIController* bombController = Cast<ABombAIController>(bomb->GetController()); 
            if (bombController)
            {
                //FVector ValidLocation = ValidateLocation(Cast<AAIController>(bombController), data->Location.ToFVector());
                if (bombController->LastTargetLocation != data->Location.ToFVector())
                {
                    // UE_LOG(LogTemp, Warning, TEXT(">>>>>>>>>>> BOMB CONTROLLER IS AVAILABLE"));
                    bombController->bIsMoving = true; 
                    bombController->LastTargetLocation = data->Location.ToFVector();
                    UAIBlueprintHelperLibrary::SimpleMoveToLocation(bombController, data->Location.ToFVector());
                }
                
            }
        }
    }


    // delete noti; 
}

void ATinySwordGameMode::OnAttackNotification(struct Attack::Notification* data)
{
    // struct Attack::Notification *noti = (struct Attack::Notification *)(data+1); 
    
    ////////////////// ATTACKER //////////////////
    // GOBLIN
    if (data->AttackerActorType == GOBLIN)
    {
        // CASTING
        AGoblin* goblin = FindGoblinById(GoblinMap, data->AttackerTagId);
        if (!goblin) return;

        // 공격 위치 보간 
        // Interpolation(goblin, data->AttackLocation.ToFVector(), 0.1);

        // play animation 
        goblin->PlayAttackAnimation();
        
    }

    // BOMB
    if (data->AttackerActorType == BOMB)
    {
        ABaseBomb* bomb = FindBombById(ActiveBombId, data->AttackerTagId);
        if (!bomb) return; 

        // 공격 위치 보간
        // Interpolation(bomb, data->AttackLocation.ToFVector(), 0.1);

        // play Anim
        bomb->PlayExplodeAnim();
    }

    /////////////// TARGET ////////////////
    // 보간 -> 데미지 처리 
    switch (data->TargetActorType)
    {
    case GOBLIN:
    {
        UE_LOG(LogTemp, Warning, TEXT("********************** Target = GOBLIN"));
        AGoblin* goblin = FindGoblinById(GoblinMap, data->TargetTagId); 
        if (!goblin) {
            UE_LOG(LogTemp, Warning, TEXT(">>>>>>>>>>>>>>> GOBLIN IS NULLPTR"));
            return; 
        }

        // Interpolation(goblin, data->TargetLocation.ToFVector(), 0.1);
        goblin->DecreaseHealth(10);
        // ui 반영 

        break;
    }

    case BOMB:
    {
        ABaseBomb* bomb = FindBombById(ActiveBombId, data->TargetTagId);
        if (!bomb) return;

        // Interpolation(bomb, data->TargetLocation.ToFVector(), 0.1);
        bomb->DecreaseHealth(10);
        break;
    }

    case CASTLE:
    {
        ABaseCastle* castle = FindCastleById(ActiveCastleMap, data->TargetTagId);
        if (!castle) return; 

        // Interpolation(castle, data->TargetLocation.ToFVector(), 0.1);
        castle->DecreaseDurability(10);
        break;
    }

    case GOLDMINE:
    {
        UE_LOG(LogTemp, Warning, TEXT("********************** Target = GOLDMINE"));
        ABaseGoldMine* goldmine = FindGoldMineById(ActiveGoldMineMap, data->TargetTagId);
        if (!goldmine) return; 

        // Interpolation(goldmine, data->TargetLocation.ToFVector(), 0.1);
        goldmine->DecreaseDurability(10);
        break;
    }

    case SHEEP:
    {
        UE_LOG(LogTemp, Warning, TEXT("********************** Target = SHEEP"));
        ABaseAISheep* sheep = FindSheepById(ActiveSheepId, data->TargetTagId); 
        if (!sheep) return; 

        // Interpolation(sheep, data->TargetLocation.ToFVector(), 0.1);
        sheep->DecreaseHealth(10);


        break;
    }

    default:
        break;
    }


    //delete noti; 
}


void ATinySwordGameMode::OnSpawnNotification(struct Spawn::Notification *data)
{
    
    // struct Spawn::Notification *noti = (struct Spawn::Notification *)(data+1);
    UE_LOG(LogTemp, Warning, TEXT("Entered in OnSpawnNotification: %d"), static_cast<int>(data->SpawnActorType));

    // CHARACTERSELECT_NOTI처리에서 GOBLIN 스폰 처리 

    // BOMB
    if (data->SpawnActorType == BOMB)
    {
        UE_LOG(LogTemp, Warning, TEXT("!>!>!>!>!>!>!>!>!> BOMB SPAWNING.....: %d"), data->SpawnActorTagId);
        SpawnBomb(data->Location.ToFVector(), data->OwnerTagId, data->SpawnActorTagId);
    }

    // MEAT
    if (data->SpawnActorType == MEAT)
    {
        SpawnMeat(data->Location.ToFVector(), data->SpawnActorTagId);
    }

    // GOLDBAG
    if (data->SpawnActorType == GOLDBAG)
    {
        UE_LOG(LogTemp, Warning, TEXT("SPAWN ACTOR TYPE == GOLDBAG"));
        SpawnGoldBag(data->Location.ToFVector(), data->SpawnActorTagId);
    }

    //delete noti; 
}

void ATinySwordGameMode::OnGetItemNotification(struct GetItem::Notification *data)
{
    UE_LOG(LogTemp, Warning, TEXT("~*~*~*~**~*~*~*~*~*~**~ [GET ITEM] NOTIFICATION F"));
    // struct GetItem::Notification *noti = (struct GetItem::Notification *)data; 
    AGoblin* goblin = FindGoblinById(GoblinMap, data->PlayerTagId);
    if (!goblin) 
    {
        UE_LOG(LogTemp, Fatal, TEXT(">>>>>>>>>>>>> [ON GETITEM NOTI] CANNOT FIND GOBLIN"));
        return; 
    }

    // Coin/HP update
    // destroy actor (Remove Actor from Entry)
    if (data->ItemType == MEAT)
    {
        UE_LOG(LogTemp, Warning, TEXT(">>>>>>>>>>>>>>>>>>>>>>>>>>>>> ITEM TYPE == MEAT"));
        //goblin->IncreaseHealth(10);

        // remove
        ABaseMeat* meat = FindMeatById(ActiveMeatId, data->ItemTagId); 
        if (meat) 
        {
            meat->Destroy();
            ActiveMeatId.Remove(meat);
        }    
        else
        {
            UE_LOG(LogTemp, Error, TEXT("> > > >  > >  CANNOT FIND MEAT...: %d / (Size:%d)"), data->ItemTagId, ActiveMeatId.Num());
        }
        
    }

    if (data->ItemType == GOLDBAG)
    {
        //goblin->IncreaseMoney(10);
        ABaseGoldBag* coin = FindGoldBagById(ActiveGoldBagId, data->ItemTagId);
        if (coin)
        {
            coin->Destroy(); 
            ActiveGoldBagId.Remove(coin);    
        }
        
    }

    // delete noti; 
}

void ATinySwordGameMode::OnDeadNotification(struct Dead::Notification *data)
{
    UE_LOG(LogTemp, Warning, TEXT(">>>>>>>>>>>>>>> ENTERED IN ON DEAD NOTIFICATION"));
    // struct Dead::Notification *noti = (struct Dead::Notification *)data; 

    // GOBLIN Dead -> Goblin.HandleDeath

    // CASTLE Dead -> Change Sprite

    // BOMB Dead -> Bomb.HandleDeath : Play Dead Anim

    // SHEEP Dead -> Sheep.HandleDeath : Destroy Sheep + PlayDeadAnim(?)

    // GOLDMINE Dead -> Change Sprite

    switch (data->DeadActorType)
    {
    UE_LOG(LogTemp, Log, TEXT("Entered in DeadActorType Switch-case"));
    case GOBLIN:
    {
        AGoblin* goblin = FindGoblinById(GoblinMap, data->DeadActorTagId); 
        if (!goblin) 
        {
            UE_LOG(LogTemp, Warning, TEXT("[ON DEAD NOTI] CANNOT FIND GOBLIN!!!"));
            return; 
        }

        // Interpolation(goblin, data->Location.ToFVector(), 0.05); // 보간 
        // goblin->SetHealth(0);
        // goblin->DecreaseHealth(100);
        goblin->HandleDeath(); 

        break;
    }
    
    case CASTLE:
    {
        ABaseCastle* castle = FindCastleById(ActiveCastleMap, data->DeadActorTagId); 
        if (!castle) return; 
        castle->SetDurability(0);
        castle->OnCollapse();
        DeadCastleCnt++;
        break;
    }
        

    case BOMB:
    {
        ABaseBomb* bomb = FindBombById(ActiveBombId, data->DeadActorTagId); 
        if (!bomb) return; 
        bomb->SetHealth(0);
        bomb->HandleDeath();
        ActiveBombId.Remove(bomb);
        bomb->Destroy();

        break;
    }
        

    case SHEEP: 
    {
        // Spawn Meat는 Spawn::Noti 도착하면 수행함. 
        ABaseAISheep* sheep = FindSheepById(ActiveSheepId, data->DeadActorTagId);
        if (!sheep) return; 
        sheep->SetHealth(0);
        ActiveSheepId.Remove(sheep);
        sheep->Destroy(); 

        // MEAT SPAWN이 이루어질 것... 
        break;
    }
        

    case GOLDMINE: 
    {
        ABaseGoldMine* goldmine = FindGoldMineById(ActiveGoldMineMap, data->DeadActorTagId); 
        if (!goldmine) return; 
        goldmine->SetDurability(0);
        break;
    }

    


    default:
        break;
    }
}

void ATinySwordGameMode::OnBombExpNotification(struct BombExplode::Notification *data)
{
    // cf. 방사형 데미지는 서버에서 Attack으로 전송할 예정. 

    // struct BombExplode::Notification *noti = (struct BombExplode::Notification *)data; 

    // play brink->exp anim.
    ABaseBomb* bomb = FindBombById(ActiveBombId, data->TagId);
    if (!bomb) return; 

    bomb->PlayBrinkAnim(); 

    // 1.9초 후 동작하도록 타이머 설정 (람다 사용)
    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(
        TimerHandle,
        FTimerDelegate::CreateLambda([this, bomb]()
        {
            bomb->Destroy();
        }),
        1.9f,  // 딜레이 시간 (초)
        false  // 반복 실행 안함
    );
}

void ATinySwordGameMode::DisableCharacterButton(FName buttonName)
{
    UButton* button = Cast<UButton>(CharacterSelectWidget->GetWidgetFromName(buttonName));
    if (button) button->SetIsEnabled(false);
}


void ATinySwordGameMode::FindCharacterSelectWidget()
{
    TArray<UUserWidget*> FoundWidgets;
    UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundWidgets, USelectCharacterWidget::StaticClass(), true);

    if (FoundWidgets.Num() > 0)
    {
        CharacterSelectWidget = Cast<USelectCharacterWidget>(FoundWidgets[0]);
        if (CharacterSelectWidget)
        {
            // UE_LOG(LogTemp, Warning, TEXT("Found and set CharacterSelectWidget."));
        }
    }
    else
    {
        // UE_LOG(LogTemp, Error, TEXT("No CharacterSelectWidget found."));
    }
}

void ATinySwordGameMode::OpenPlayingLevel()
{
    UGameplayStatics::OpenLevel(GetWorld(), FName("PlayLevel"));
}