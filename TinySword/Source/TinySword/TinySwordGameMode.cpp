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
#include "SheepAIController.h"


void ATinySwordGameMode::BeginPlay()
{
    Super::BeginPlay();

    // UGameplayStatics::OpenLevel(GetWorld(), FName("MainMenuLevel"));

    // FindCastlesLocation();
    //FindAllGoblins();
    UE_LOG(LogTemp, Warning, TEXT("Starting GameMode"));
}


void ATinySwordGameMode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    // UE_LOG(LogTemp, Warning, TEXT("Game Mode is running..."));
    struct HEAD *data = messageQueue.pop(); 
    if (data == nullptr || data->Command > 22 || data->Command == 0) return;
    if (PROTOCOLS[data->Command] != nullptr) (this->*PROTOCOLS[data->Command])(data);

}

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



///////////////////////////////////////////////////////////////////////////////////////////////////////


// Packet Processing Function 

void ATinySwordGameMode::OnCharacterSelectResponse(HEAD *data)
{
    struct CharacterSelect::Response *response = (struct CharacterSelect::Response *)data; 
    UE_LOG(LogTemp, Warning, TEXT("[CharacterSelect Response] player index: %d"), response->playerIndex);
    delete response; 
}

void ATinySwordGameMode::OnCharacterSelectNotification(HEAD *data)
{
    struct CharacterSelect::Notification *noti = (struct CharacterSelect::Notification *)data; 
    UE_LOG(LogTemp, Warning, TEXT("[CharacterSelect Notification] player index: %d"), noti->playerIndex);
    delete noti;
}

void ATinySwordGameMode::OnMoveResponse(HEAD *data)
{
    struct Move::Response *response = (struct Move::Response *)data;
    UE_LOG(LogTemp, Warning, TEXT("[Move Response]"));

    switch (response->ActorType)
    {
    case 0:
    {
        AGoblin* goblin = FindGoblinById(GoblinMap, response->ActorIndex); 
        if (goblin)
        {
            // FVector newLoc = goblin->GetActorLocation(); 
            FVector movement(0.0f, 0.0f, 0.0f); 

            float speed = response->Speed; 
            float deltaTime = 5.2f; //GetWorld()->DeltaTimeSeconds;
            // UE_LOG(LogTemp, Warning, TEXT("    DeltaTime: %f, Speed: %.2f | %d %d %d %d"), deltaTime, speed, response->bMoveUp, response->bMoveDown, response->bMoveRight, response->bMoveLeft);
            if (response->bMoveUp) movement.Y -= speed * deltaTime; 
            if (response->bMoveDown) movement.Y += speed  * deltaTime; 
            if (response->bMoveRight) movement.X += speed * deltaTime; 
            if (response->bMoveLeft) movement.X -= speed * deltaTime; 

            goblin->GetCharacterMovement()->AddInputVector(movement);
        }
        break;
    }

    case 1: 
    {
        ABaseAISheep* sheep = FindSheepById(ActiveSheepId, response->ActorIndex); 
        if (sheep)
        {
            UE_LOG(LogTemp, Warning, TEXT("Sheep Is Moving!!"));
            float speed = response->Speed; 
            ASheepAIController* sheepController = Cast<ASheepAIController>(sheep->GetController()); 
            if (sheepController)
            {
                UAIBlueprintHelperLibrary::SimpleMoveToLocation(sheepController, response->Destination);
            }
            
        }
    }

    case 2: 
    {
        ABaseBomb* bomb = FindBombById(ActiveBombId, response->ActorIndex); 
        if (bomb)
        {
            UE_LOG(LogTemp, Warning, TEXT("Bomb is Moving!!!"));
            float speed = response->Speed; 
            ABombAIController* bombController = Cast<ABombAIController>(bomb->GetController()); 
            if (bombController)
            {
                UAIBlueprintHelperLibrary::SimpleMoveToLocation(bombController, response->Destination);
            }
        }
    }
        
    
    default:
        break;
    }



    delete response;
}

void ATinySwordGameMode::OnMoveNotification(HEAD *data)
{
    struct Move::Notification *noti = (struct Move::Notification *)data; 
    UE_LOG(LogTemp, Warning, TEXT("[Move Notification] Actor Index: %d / X: %f Y: %f"), noti->ActorIndex, noti->X, noti->Y);
    delete noti; 
}

void ATinySwordGameMode::OnAttackResponse(HEAD *data)
{
    struct Attack::Response *response = (struct Attack::Response *)data; 
    UE_LOG(LogTemp, Warning, TEXT("[Attack Response] hit? : %d"), response->hityn); 
    // 공격 받은 액터에게 noti 전송하고, 액터가 데미지 받음 처리하기 

    switch (response->TargetType)
    {
    case 0: // goblin
    {
        AGoblin* goblin = FindGoblinById(GoblinMap, response->TargetIndex); 
        if (goblin)
        {
            goblin->SetHealth(goblin->GetHealth()-response->Damage);
            // noti 전송 
        }
        break;
    }

    case 1: // bomb
    {
        ABaseBomb* bomb = FindBombById(ActiveBombId, response->TargetIndex); 
        if (bomb)
        {
            if (response->AttackerType == 0)
            {
                AGoblin* attacker = Cast<AGoblin>(FindGoblinById(GoblinMap, response->AttackerIndex)); 
                if (attacker && attacker->GetTagId() != bomb->GetTagId())
                {
                    bomb->SetHealth(bomb->GetHealth()-response->Damage);
                }
            }
            
            // noti 
        }
        break;
    }

    case 2: // castle 
    {
        ABaseCastle* castle = FindCastleById(ActiveCastleMap, response->TargetIndex);
        if (castle)
        {
            if (response->AttackerType == 0)
            {
                AGoblin* attacker = Cast<AGoblin>(FindGoblinById(GoblinMap, response->AttackerIndex)); 
                if (attacker && attacker->GetTagId() != castle->GetTagId()) 
                {
                    castle->SetDurability(castle->GetDurability()-response->Damage);
                }
            }
            if (response->AttackerType == 1)
            {
                ABaseBomb* attacker = Cast<ABaseBomb>(FindBombById(ActiveBombId, response->AttackerIndex)); 
                if (attacker && attacker->GetTagId() != castle->GetTagId())
                {
                    castle->SetDurability(castle->GetDurability()-response->Damage);
                }
            }
            
            // noti
        }
        break;
    }
    case 3: // sheep
    {
        ABaseAISheep* sheep = FindSheepById(ActiveSheepId, response->TargetIndex); 
        if (sheep)
        {
            sheep->SetHealth(sheep->GetHealth()-response->Damage);
            // noti
        }
        break;
    }

    case 4: // goldmine
    {
        ABaseGoldMine* goldmine = FindGoldMineById(ActiveGoldMineMap, response->TargetIndex); 
        if (goldmine)
        {
            
            goldmine->SetDurability(goldmine->GetDurability()-response->Damage);
            UE_LOG(LogTemp, Warning, TEXT("Attacking GOLDMINE! : %d"), goldmine->GetDurability());
            // goldmine->DropGoldBag();
            // noti
        }
        break;
    }

    
    
    default:
        break;
    }

    delete response; 
}

void ATinySwordGameMode::OnAttackNotification(HEAD *data)
{
    struct Attack::Notification *noti = (struct Attack::Notification *)data; 
    UE_LOG(LogTemp, Warning, TEXT("[Attack Notification] Attacker Type: %d / Attacker Index: %d / target Type: %d / target Index: %d"), 
        noti->AttackerType, noti->AttackerIndex, noti->targetType, noti->targetIndex);
    delete noti; 
}

void ATinySwordGameMode::OnSpawnResponse(HEAD *data)
{
    struct Spawn::Response *response = (struct Spawn::Response *)data; 
    UE_LOG(LogTemp, Warning, TEXT("[Spawn Response] success? %d"), response->successyn);


    // goblin, goldbag, meat

    switch (response->SpawnType)
    {
    case 0: // goblin
    {

        ATinySwordPlayerController* controller = Cast<ATinySwordPlayerController>(FindControllerById(PlayerControllerMap, response->SpawnActorIndex)); 
        if (controller)
        {
            controller->SpawnGoblin(response->Location, response->SpawnActorIndex); 
        }
        break;
    
    }

    case 1: // bomb
    {
        AGoblin* goblin = Cast<AGoblin>(FindGoblinById(GoblinMap, response->SpawnActorIndex)); 
        if (goblin)
        {
            ATinySwordPlayerController* controller = Cast<ATinySwordPlayerController>(goblin->GetController()); 
            if (controller)
            {
                controller->SpawnBomb(response->Location, response->SpawnActorIndex);
                // SpawnBomb(GetBombSpawnPoint(*CastleMap.Find(controlledChar->GetTagId()))); ///////////////////////////////////////////////////
   
            }
        }
        break;
    }

    case 2: // meat
    {
        ABaseAISheep* sheep = Cast<ABaseAISheep>(FindSheepById(ActiveSheepId, response->SpawnActorIndex)); 
        if (sheep)
        {
            sheep->SpawnMeat();
        }
        break;
    }

    case 3: // goldbag
    {
        ABaseGoldMine* goldmine = Cast<ABaseGoldMine>(FindGoldMineById(ActiveGoldMineMap, response->SpawnActorIndex));
        if (goldmine) goldmine->DropGoldBag(response->Location);
        break;
    }
    default:
        break;
    }



    delete response; 
}

void ATinySwordGameMode::OnSpawnNotification(HEAD *data)
{
    struct Spawn::Notification *noti = (struct Spawn::Notification *)data; 
    UE_LOG(LogTemp, Warning, TEXT("[Spawn Notification] Spawn Actor Type : %d / Spawn Actor Index: %d"), noti->SpawnType, noti->SpawnActorIndex);
    delete noti; 
}

void ATinySwordGameMode::OnGetItemResponse(HEAD *data)
{
    struct GetItem::Response *response = (struct GetItem::Response *)data; 

    AGoblin* player = Cast<AGoblin>(FindGoblinById(GoblinMap, response->playerIndex)); 
    if (player)
    {
        if (response->ItemType == 0) // meat 
        {
            player->IncreaseHealth(10);
        }
        if (response->ItemType == 1)
        {
            player->IncreaseMoney(10);
            player->UpdateMoneyCount_(player->GetMoneyCount());
        }
    }

    delete response;
}

void ATinySwordGameMode::OnGetItemNotification(HEAD *data)
{
    struct GetItem::Notification *noti = (struct GetItem::Notification *)data; 
    UE_LOG(LogTemp, Warning, TEXT("[GetItem Notification] player Index: %d / HP: %d / COIN : %d / X: %f Y: %f"), 
        noti->playerIndex, noti->playerHp, noti->playerCoin, noti->X, noti->Y);
    delete noti; 
}

void ATinySwordGameMode::OnBombExplodeResponse(HEAD *data)
{
    struct BombExplode::Response *response = (struct BombExplode::Response *)data; 

    // play animation

    UE_LOG(LogTemp, Warning, TEXT("[BombExplode Response]"));
    delete response; 
}

void ATinySwordGameMode::OnBombExplodeNotification(HEAD *data)
{
    struct BombExplode::Notification *noti = (struct BombExplode::Notification *)data; 
    
    delete noti; 
}

void ATinySwordGameMode::OnPlayerDeadResponse(HEAD *data)
{
    struct PlayerDead::Response *response = (struct PlayerDead::Response *)data; 
    UE_LOG(LogTemp, Warning, TEXT("[PlayerDead Response] "));
    delete response;
}

void ATinySwordGameMode::OnPlayerDeadNotification(HEAD *data)
{
    struct PlayerDead::Notification *noti = (struct PlayerDead::Notification *)data; 
    UE_LOG(LogTemp, Warning, TEXT("[PlayerDead Notification] player index: %d / X: %f  Y: %f"), noti->playerIndex, noti->X, noti->Y);
    delete noti; 
}

void ATinySwordGameMode::OnDestroyResponse(HEAD *data)
{
    struct Destroy::Response *response = (struct Destroy::Response *)data; 
    UE_LOG(LogTemp, Warning, TEXT("[Destroy Response]"));

    // bomb, goldbag, sheep, meat = 0, 1, 2, 3
    switch (response->ActorType)    
    {
        case 0: // bomb
        {
            ABaseBomb* bomb = FindBombById(ActiveBombId, response->ActorIndex); 
            if (bomb)
            {
                UE_LOG(LogTemp, Warning, TEXT("Destroy Bomb"));
                bomb->AddToReuseId(response->ActorIndex);
                bomb->Destroy();
                ActiveBombId.Remove(bomb);
            }
            break;
        }

        case 1: // goldbag 
        {
            ABaseGoldBag* goldbag = FindGoldBagById(ActiveGoldBagId, response->ActorIndex); 
            if (goldbag)
            {
                UE_LOG(LogTemp, Warning, TEXT("Destroy GoldBag: %d"), response->ActorIndex);
                goldbag->Destroy(); 
                ActiveGoldBagId.Remove(goldbag); 
                ReuseGoldBagId.Enqueue(response->ActorIndex);
            } 
            break; 
        }

        case 2: // sheep 
        {
            ABaseAISheep* sheep = FindSheepById(ActiveSheepId, response->ActorIndex); 
            if (sheep)
            {
                UE_LOG(LogTemp, Warning, TEXT("Destroy Sheep"));
                sheep->Destroy(); 
                ActiveSheepId.Remove(sheep);
            }
            break;
        }

        case 3: // meat
        {
            ABaseMeat* meat = FindMeatById(ActiveMeatId, response->ActorIndex);
            if (meat)
            {
                UE_LOG(LogTemp, Warning, TEXT("Destroy Meat"));
                meat->Destroy();
                ActiveMeatId.Remove(meat);
            }
            break;
        }

        default:
            break;
    }
    delete response; 
}

void ATinySwordGameMode::OnDestroyNotification(HEAD *data)
{
    struct Destroy::Notification *noti = (struct Destroy::Notification *)data; 
    UE_LOG(LogTemp, Warning, TEXT("[Destroy Notification] Actor Type: %d / Actor Index: %d / X: %f Y: %f"), 
        noti->ActorType, noti->ActorIndex, noti->X, noti->Y);
    delete noti; 
}

void ATinySwordGameMode::OnEndGameResponse(HEAD *data)
{ 
    struct EndGame::Response *response = (struct EndGame::Response *)data; 
    UE_LOG(LogTemp, Warning, TEXT("[EndGame Response] "));
    delete response; 
}

void ATinySwordGameMode::OnEndGameNotification(HEAD *data)
{
    struct EndGame::Notification *noti = (struct EndGame::Notification *)data; 
    UE_LOG(LogTemp, Warning, TEXT("[EndGame Notification] "));
    delete noti;
}

void ATinySwordGameMode::OnStartGameResponse(HEAD *data)
{
    struct StartGame::Response *response = (struct StartGame::Response *)data; 
    UE_LOG(LogTemp, Warning, TEXT("[StartGame Response]"));
    delete response; 
}

void ATinySwordGameMode::OnStartGameNotification(HEAD *data)
{
    struct StartGame::Notification *noti = (struct StartGame::Notification *)data; 
    UE_LOG(LogTemp, Warning, TEXT("[StartGame Notification]"));
    delete noti; 
}

void ATinySwordGameMode::OnPauseGameResponse(HEAD *data)
{
    struct PauseGame::Response *response = (struct PauseGame::Response *)data; 
    UE_LOG(LogTemp, Warning, TEXT("[PauseGame Response]"));
    delete response;
}

void ATinySwordGameMode::OnPauseGameNotification(HEAD *data)
{
    struct PauseGame::Notification *noti = (struct PauseGame::Notification *)data; 
    UE_LOG(LogTemp, Warning, TEXT("[PauseGame Notification]"));
    delete noti; 
}
