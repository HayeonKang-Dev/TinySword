// Fill out your copyright notice in the Description page of Project Settings.


#include "TinySwordGameMode.h"
#include "BaseCastle.h"
#include "Kismet/GameplayStatics.h"
#include "BaseBomb.h"
#include "Goblin.h"
#include "protocol.h"



void ATinySwordGameMode::BeginPlay()
{
    Super::BeginPlay();

    // UGameplayStatics::OpenLevel(GetWorld(), FName("MainMenuLevel"));

    // FindCastlesLocation();
    //FindAllGoblins();
}


void ATinySwordGameMode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    struct HEAD *data = messageQueue.pop(); 
    if (data == nullptr) return;

    // switch case 





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

void ATinySwordGameMode::FindAllGoblins()
{
    TArray<AActor*> AllGoblins; 
    UWorld* World = GetWorld(); 
    if (World)
    {
        UGameplayStatics::GetAllActorsOfClass(World, AGoblin::StaticClass(), AllGoblins);
        for (AActor* Goblin : AllGoblins)
        {
            AGoblin* goblin = Cast<AGoblin>(Goblin); 
            if (goblin) GoblinMap.Add(goblin->GetTagId(), goblin);
        }
    }
}



FVector ATinySwordGameMode::FindCastleLocationByTagId(int32 TagId)
{
    if (CastleMap.Contains(TagId)) return *CastleMap.Find(TagId);
    else return FVector::ZeroVector;
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

void ATinySwordGameMode::CharacterSelectResponse(HEAD *data)
{
    struct CharacterSelect::Response *response = (struct CharacterSelect::Response *)data; 
    UE_LOG(LogTemp, Warning, TEXT("[CharacterSelect Response] player index: %d"), response->playerIndex);
    delete response; 
}

void ATinySwordGameMode::CharacterSelectNotification(HEAD *data)
{
    struct CharacterSelect::Notification *noti = (struct CharacterSelect::Notification *)data; 
    UE_LOG(LogTemp, Warning, TEXT("[CharacterSelect Notification] player index: %d"), noti->playerIndex);
    delete noti;
}

void ATinySwordGameMode::MoveResponse(HEAD *data)
{
    struct Move::Response *response = (struct Move::Response *)data;
    UE_LOG(LogTemp, Warning, TEXT("[Move Response]"));
    delete response;
}

void ATinySwordGameMode::MoveNotification(HEAD *data)
{
    struct Move::Notification *noti = (struct Move::Notification *)data; 
    UE_LOG(LogTemp, Warning, TEXT("[Move Notification] Actor Index: %d / X: %f Y: %f"), noti->ActorIndex, noti->X, noti->Y);
    delete noti; 
}

void ATinySwordGameMode::AttackResponse(HEAD *data)
{
    struct Attack::Response *response = (struct Attack::Response *)data; 
    UE_LOG(LogTemp, Warning, TEXT("[Attack Response] hit? : %d"), response->hityn);
    delete response; 
}

void ATinySwordGameMode::AttackNotification(HEAD *data)
{
    struct Attack::Notification *noti = (struct Attack::Notification *)data; 
    UE_LOG(LogTemp, Warning, TEXT("[Attack Notification] Attacker Type: %d / Attacker Index: %d / target Type: %d / target Index: %d"), 
        noti->AttackerType, noti->AttackerIndex, noti->targetType, noti->targetIndex);
    delete noti; 
}

void ATinySwordGameMode::SpawnResponse(HEAD *data)
{
    struct Spawn::Response *response = (struct Spawn::Response *)data; 
    UE_LOG(LogTemp, Warning, TEXT("[Spawn Response] success? %d"), response->successyn);
    delete response; 
}

void ATinySwordGameMode::SpawnNotification(HEAD *data)
{
    struct Spawn::Notification *noti = (struct Spawn::Notification *)data; 
    UE_LOG(LogTemp, Warning, TEXT("[Spawn Notification] Spawn Actor Type : %d / Spawn Actor Index: %d"), noti->SpawnType, noti->SpawnActorIndex);
    delete noti; 
}

void ATinySwordGameMode::GetItemResponse(HEAD *data)
{
    struct GetItem::Response *response = (struct GetItem::Response *)data; 
    UE_LOG(LogTemp, Warning, TEXT("[GetItem Response] success? %d / player HP: %d / player Coin: %d"), response->successyn, response->playerHp, response->playerCoin);
    delete response;
}

void ATinySwordGameMode::GetItemNotification(HEAD *data)
{
    struct GetItem::Notification *noti = (struct GetItem::Notification *)data; 
    UE_LOG(LogTemp, Warning, TEXT("[GetItem Notification] player Index: %d / HP: %d / COIN : %d / X: %f Y: %f"), 
        noti->playerIndex, noti->playerHp, noti->playerCoin, noti->X, noti->Y);
    delete noti; 
}

void ATinySwordGameMode::BombExplodeResponse(HEAD *data)
{
    struct BombExplode::Response *response = (struct BombExplode::Response *)data; 
    UE_LOG(LogTemp, Warning, TEXT("[BombExplode Response]"));
    delete response; 
}

void ATinySwordGameMode::BombExplodeNotification(HEAD *data)
{
    struct BombExplode::Notification *noti = (struct BombExplode::Notification *)data; 
    UE_LOG(LogTemp, Warning, TEXT("[BombExplode Notification] Damaged Actor index: %d, target HP : %d / X: %f Y: %f"), 
        noti->DamagedActorIndex, noti->targetHp, noti->X, noti->Y);
    delete noti; 
}

void ATinySwordGameMode::PlayerDeadResponse(HEAD *data)
{
    struct PlayerDead::Response *response = (struct PlayerDead::Response *)data; 
    UE_LOG(LogTemp, Warning, TEXT("[PlayerDead Response] "));
    delete response;
}

void ATinySwordGameMode::PlayerDeadNotification(HEAD *data)
{
    struct PlayerDead::Notification *noti = (struct PlayerDead::Notification *)data; 
    UE_LOG(LogTemp, Warning, TEXT("[PlayerDead Notification] player index: %d / X: %f  Y: %f"), noti->playerIndex, noti->X, noti->Y);
    delete noti; 
}

void ATinySwordGameMode::DestroyResponse(HEAD *data)
{
    struct Destroy::Response *response = (struct Destroy::Response *)data; 
    UE_LOG(LogTemp, Warning, TEXT("[Destroy Response]"));
    delete response; 
}

void ATinySwordGameMode::DestroyNotification(HEAD *data)
{
    struct Destroy::Notification *noti = (struct Destroy::Notification *)data; 
    UE_LOG(LogTemp, Warning, TEXT("[Destroy Notification] Actor Type: %d / Actor Index: %d / X: %f Y: %f"), 
        noti->ActorType, noti->ActorIndex, noti->X, noti->Y);
    delete noti; 
}

void ATinySwordGameMode::EndGameResponse(HEAD *data)
{ 
    struct EndGame::Response *response = (struct EndGame::Response *)data; 
    UE_LOG(LogTemp, Warning, TEXT("[EndGame Response] "));
    delete response; 
}

void ATinySwordGameMode::EndGameNotification(HEAD *data)
{
    struct EndGame::Notification *noti = (struct EndGame::Notification *)data; 
    UE_LOG(LogTemp, Warning, TEXT("[EndGame Notification] "));
    delete noti;
}

void ATinySwordGameMode::StartGameResponse(HEAD *data)
{
    struct StartGame::Response *response = (struct StartGame::Response *)data; 
    UE_LOG(LogTemp, Warning, TEXT("[StartGame Response]"));
    delete response; 
}

void ATinySwordGameMode::StartGameNotification(HEAD *data)
{
    struct StartGame::Notification *noti = (struct StartGame::Notification *)data; 
    UE_LOG(LogTemp, Warning, TEXT("[StartGame Notification]"));
    delete noti; 
}

void ATinySwordGameMode::PauseGameResponse(HEAD *data)
{
    struct PauseGame::Response *response = (struct PauseGame::Response *)data; 
    UE_LOG(LogTemp, Warning, TEXT("[PauseGame Response]"));
    delete response;
}

void ATinySwordGameMode::PauseGameNotification(HEAD *data)
{
    struct PauseGame::Notification *noti = (struct PauseGame::Notification *)data; 
    UE_LOG(LogTemp, Warning, TEXT("[PauseGame Notification]"));
    delete noti; 
}
