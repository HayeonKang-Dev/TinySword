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


#include "AsyncNetworking.h"



void ATinySwordGameMode::StartPlay()
{
    Super::StartPlay(); 

    UE_LOG(LogTemp, Warning, TEXT("GameMode::StartPlay is Running"));

    //TCPSocketClient_Async.Connect();
    ClickCnt = 0;
    
    GI = Cast<UTinySwordGameInstance>(GetWorld()->GetGameInstance());
    GI->GetInstance(GetWorld());
    // GI->Init();

    TCPClient = GI->GetTCPClient();
    // TCPClient->Initialize(GetWorld());



    
    // TCPClient = GI->GetTCPClient();



}

void ATinySwordGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    TCPSocketClient_Async.Disconnect(); 
    Super::EndPlay(EndPlayReason);
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

    
    UE_LOG(LogTemp, Warning, TEXT("Starting GameMode"));
    
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
            UE_LOG(LogTemp, Warning, TEXT("Is Packet Comming.!"));
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
        //UE_LOG(LogTemp, Log, TEXT("경로 포인트 개수: %d"), PathPoints.Num());
    
        for (int i = 0; i < PathPoints.Num(); i++)
        {
            FVector Point = PathPoints[i];
            AllPathPoints[i] = Point;
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

FVector ATinySwordGameMode::ValidateLocation(ASheepAIController *controller, const FVector &Destination)
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
    UPrimitiveComponent* root = Cast<UPrimitiveComponent>(actor->GetRootComponent()); 
        if (root) {
            UKismetSystemLibrary::MoveComponentTo(
                root,                       // 이동 대상: 루트 컴포넌트
                Destination,             // 목표 위치
                FRotator(0, 0, 0),           // 목표 회전
                true,                       // 부드러운 가속
                true,                       // 부드러운 감속
                seconds,                        // 이동 시간 (초)
                false,                      // 최단 회전 경로 사용 여부
                EMoveComponentAction::Move, // Move 동작
                FLatentActionInfo() // 비동기 작업 정보
            );
        }
}


void ATinySwordGameMode::SpawnBomb(FVector spawnLocation, int tagId)
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

    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; 
      
    // AActor* SpawnedActor = World->SpawnActor<AActor>(GeneratedBP->GeneratedClass, GetBombSpawnPoint(*FoundLocation), OwnerActor->GetActorRotation(), SpawnParams);
    AActor* SpawnedActor = World->SpawnActor<AActor>(GeneratedBP->GeneratedClass, spawnLocation, FRotator(0, 0, 0), SpawnParams);

    if (SpawnedActor)
    {
        ABaseBomb* SpawnedBomb = Cast<ABaseBomb>(SpawnedActor); 
        if (SpawnedBomb) {
            SpawnedBomb->SetTagId(tagId);
            // SpawnedBomb->SetOwnerTagId(tagId); 
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

void ATinySwordGameMode::OnCharacterSelectNotification(CharacterSelect::Notification *data)
// void ATinySwordGameMode::OnCharacterSelectNotification(HEAD *data)
{
    if (!data)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid HEAD Pointer"));
        return;
    }

    // CharacterSelect::Notification *noti = reinterpret_cast<CharacterSelect::Notification*>(
    //     reinterpret_cast<uint8*>(data) + sizeof(HEAD)
    // );

    // CharacterSelect::Notification* noti = reinterpret_cast<CharacterSelect::Notification*>(data+1); // +1

    // struct CharacterSelect::Notification *noti = (struct CharacterSelect::Notification *)data;
    // UI의 버튼 비활성화 시키기 

    // struct CharacterSelect::Notification *noti = (struct CharacterSelect::Notification *)data; 
    ClickCnt++;

    if (data==nullptr) 
    {
        UE_LOG(LogTemp, Warning, TEXT("[OnMoveNotification] MOVE_NOTIFICATION NOT AVAILABLE"));
        return;
    }
    else if (data != nullptr) {
        UE_LOG(LogTemp, Warning, TEXT("[OnMoveNotification] MOVE_NOTIFICATION AVAILABLE : %d"), data->SelectedTagId);
    }

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

void ATinySwordGameMode::OnMoveNotification(HEAD *data)
{
    struct Move::Notification *noti = (struct Move::Notification *)data; 

    // GOBLIN
    if (noti->MoveActorType == GOBLIN){
        UE_LOG(LogTemp, Warning, TEXT("[OnMoveNotification] MOVE_NOTIFICATION: GOBLIN AVAILABLE"));
        // GOBLIN TagId로 찾아서 SimpleMove
        AGoblin* goblin = FindGoblinById(GoblinMap, noti->MoveActorTagId);

        // [보간] 서버가 준 위치로 0.5초 간 이동 
        Interpolation(goblin, noti->Location.ToFVector(), 0.5);

    }

    // SHEEP 
        // noti->Location이 이동 불가 지역일 경우, 수정해서 Request 전송 
    if (noti->MoveActorType == SHEEP) {
        ABaseAISheep* sheep = FindSheepById(ActiveSheepId, noti->MoveActorTagId);
        if (sheep)
        {
            ASheepAIController* sheepController = Cast<ASheepAIController>(sheep->GetController());
            if (sheepController)
            {
                FVector ValidLocation = ValidateLocation(sheepController, noti->Location.ToFVector());

                if (noti->Location.ToFVector() != ValidLocation)
                {
                    // request 전송해야 함 
                    Move::Request Request; 
                    
                    Request.Location = Vector(ValidLocation.X, ValidLocation.Y, ValidLocation.Z); 
                    Request.MoveActorType = SHEEP; 
                    Request.MoveActorTagId = noti->MoveActorTagId;

                    // TArray<uint8> ByteArray = StructToByteArray(Request);

                    FArrayWriter WriterArray; 
                    WriterArray.Serialize((UTF8CHAR*)&Request, sizeof(Request));
                    TSharedPtr<FBufferArchive> Packet = FTCPSocketClient_Async::CreatePacket((short)MOVE_REQUEST, WriterArray.GetData(), WriterArray.Num());
                    
                    // Send PACKET 
                    TCPSocketClient_Async.BeginSendPhase(Packet);
                }

                UAIBlueprintHelperLibrary::SimpleMoveToLocation(sheepController, ValidLocation);
            }
        }
        
    }

    // BOMB
    if (noti->MoveActorType == BOMB) {
        ABaseBomb* bomb = FindBombById(ActiveBombId, noti->MoveActorTagId);
        if (bomb)
        {
            ABombAIController* bombController = Cast<ABombAIController>(bomb->GetController()); 
            if (bombController)
            {
                UAIBlueprintHelperLibrary::SimpleMoveToLocation(bombController, noti->Location.ToFVector());
            }
        }
    }


    // delete noti; 
}

void ATinySwordGameMode::OnAttackNotification(HEAD *data)
{
    struct Attack::Notification *noti = (struct Attack::Notification *)(data+1); 
    
    ////////////////// ATTACKER //////////////////
    // GOBLIN
    if (noti->AttackerActorType == GOBLIN)
    {
        // CASTING
        AGoblin* goblin = FindGoblinById(GoblinMap, noti->AttackerTagId);
        if (!goblin) return;

        // 공격 위치 보간 
        Interpolation(goblin, noti->AttackLocation.ToFVector(), 0.1);

        // play animation 
        goblin->PlayAttackAnimation();
    }

    // BOMB
    if (noti->AttackerActorType == BOMB)
    {
        ABaseBomb* bomb = FindBombById(ActiveBombId, noti->AttackerTagId);
        if (!bomb) return; 

        // 공격 위치 보간
        Interpolation(bomb, noti->AttackLocation.ToFVector(), 0.1);

        // play Anim
        bomb->PlayExplodeAnim();
    }

    /////////////// TARGET ////////////////
    // 보간 -> 데미지 처리 
    switch (noti->TargetActorType)
    {
    case GOBLIN:
    {
        AGoblin* goblin = FindGoblinById(GoblinMap, noti->TargetTagId); 
        if (!goblin) return; 

        Interpolation(goblin, noti->TargetLocation.ToFVector(), 0.1);
        goblin->DecreaseHealth(10);
        break;
    }

    case BOMB:
    {
        ABaseBomb* bomb = FindBombById(ActiveBombId, noti->TargetTagId);
        if (!bomb) return;

        Interpolation(bomb, noti->TargetLocation.ToFVector(), 0.1);
        bomb->DecreaseHealth(10);
        break;
    }

    case CASTLE:
    {
        ABaseCastle* castle = FindCastleById(ActiveCastleMap, noti->TargetTagId);
        if (!castle) return; 

        Interpolation(castle, noti->TargetLocation.ToFVector(), 0.1);
        castle->DecreaseDurability(10);
        break;
    }

    case GOLDMINE:
    {
        ABaseGoldMine* goldmine = FindGoldMineById(ActiveGoldMineMap, noti->TargetTagId);
        if (!goldmine) return; 

        Interpolation(goldmine, noti->TargetLocation.ToFVector(), 0.1);
        goldmine->DecreaseDurability(10);
        break;
    }

    case SHEEP:
    {
        ABaseAISheep* sheep = FindSheepById(ActiveSheepId, noti->TargetTagId); 
        if (!sheep) return; 

        Interpolation(sheep, noti->TargetLocation.ToFVector(), 0.1);
        sheep->DecreaseHealth(10);
        break;
    }

    default:
        break;
    }


    //delete noti; 
}


void ATinySwordGameMode::OnSpawnNotification(HEAD *data)
{
    
    struct Spawn::Notification *noti = (struct Spawn::Notification *)(data+1);
    UE_LOG(LogTemp, Warning, TEXT("Entered in OnSpawnNotification: %d"), static_cast<int>(noti->SpawnActorType));

    // CHARACTERSELECT_NOTI처리에서 GOBLIN 스폰 처리 

    // BOMB
    if (noti->SpawnActorType == BOMB)
    {
        SpawnBomb(noti->Location.ToFVector(), noti->SpawnActorTagId);
    }

    // MEAT
    if (noti->SpawnActorType == MEAT)
    {
        SpawnMeat(noti->Location.ToFVector(), noti->SpawnActorTagId);
    }

    // GOLDBAG
    if (noti->SpawnActorType == GOLDBAG)
    {
        UE_LOG(LogTemp, Warning, TEXT("SPAWN ACTOR TYPE == GOLDBAG"));
        SpawnGoldBag(noti->Location.ToFVector(), noti->SpawnActorTagId);
    }

    //delete noti; 
}

void ATinySwordGameMode::OnGetItemNotification(HEAD *data)
{
    struct GetItem::Notification *noti = (struct GetItem::Notification *)data; 


    // Coin/HP update
    // destroy actor (Remove Actor from Entry)
    if (noti->ItemType == MEAT)
    {
        AGoblin* goblin = FindGoblinById(GoblinMap, noti->PlayerTagId);
        if (!goblin) return; 

        goblin->IncreaseHealth(10);

        // remove
        ABaseMeat* meat = FindMeatById(ActiveMeatId, noti->ItemTagId); 
        if (meat) 
        {
            meat->Destroy();
            ActiveMeatId.Remove(meat);
        }    
        
    }

    if (noti->ItemType == GOLDBAG)
    {
        ABaseGoldBag* coin = FindGoldBagById(ActiveGoldBagId, noti->ItemTagId);
        if (coin)
        {
            coin->Destroy(); 
            ActiveGoldBagId.Remove(coin);    
        }
        
    }

    // delete noti; 
}

void ATinySwordGameMode::OnDeadNotification(HEAD *data)
{
    struct Dead::Notification *noti = (struct Dead::Notification *)data; 

    // GOBLIN Dead -> Goblin.HandleDeath

    // CASTLE Dead -> Change Sprite

    // BOMB Dead -> Bomb.HandleDeath : Play Dead Anim

    // SHEEP Dead -> Sheep.HandleDeath : Destroy Sheep + PlayDeadAnim(?)

    // GOLDMINE Dead -> Change Sprite

    switch (noti->DeadActorType)
    {
    case GOBLIN:
    {
        AGoblin* goblin = FindGoblinById(GoblinMap, noti->DeadActorTagId); 
        if (!goblin) return; 

        Interpolation(goblin, noti->Location.ToFVector(), 0.05); // 보간 
        goblin->SetHealth(0);
        goblin->HandleDeath(); 

        break;
    }
    
    case CASTLE:
    {
        ABaseCastle* castle = FindCastleById(ActiveCastleMap, noti->DeadActorTagId); 
        if (!castle) return; 
        castle->SetDurability(0);
        castle->OnCollapse();
        break;
    }
        

    case BOMB:
    {
        ABaseBomb* bomb = FindBombById(ActiveBombId, noti->DeadActorTagId); 
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
        ABaseAISheep* sheep = FindSheepById(ActiveSheepId, noti->DeadActorTagId);
        if (!sheep) return; 
        sheep->SetHealth(0);
        ActiveSheepId.Remove(sheep);
        sheep->Destroy(); 

        // MEAT SPAWN이 이루어질 것... 
        break;
    }
        

    case GOLDMINE: 
    {
        ABaseGoldMine* goldmine = FindGoldMineById(ActiveGoldMineMap, noti->DeadActorTagId); 
        if (!goldmine) return; 
        goldmine->SetDurability(0);
        break;
    }

    


    default:
        break;
    }
}

void ATinySwordGameMode::OnBombExpNotification(HEAD *data)
{
    // cf. 방사형 데미지는 서버에서 Attack으로 전송할 예정. 

    struct BombExplode::Notification *noti = (struct BombExplode::Notification *)data; 

    // play brink->exp anim.
    ABaseBomb* bomb = FindBombById(ActiveBombId, noti->TagId);
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
            UE_LOG(LogTemp, Warning, TEXT("Found and set CharacterSelectWidget."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("No CharacterSelectWidget found."));
    }
}