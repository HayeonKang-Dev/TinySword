// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayingWidget.h"
#include "Blueprint/UserWidget.h"
#include "Goblin.h"
#include "Kismet/GameplayStatics.h"
#include "TinySwordPlayerController.h"
#include "TinySwordGameMode.h"
#include "NavigationSystem.h"
#include "SettingWidget.h"
#include "BaseBomb.h"
#include "protocol.h"
#include "AsyncNetworking.h"
#include "TinySwordGameInstance.h"

void UPlayingWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    GI = Cast<UTinySwordGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    GameMode = Cast<ATinySwordGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    playerController = Cast<ATinySwordPlayerController>(GetOwningPlayer()); 
    playerController->SetTagId(GI->GetTagId());
    UE_LOG(LogTemp, Warning, TEXT("GI TagId: %d / PC TagId: %d"), GI->GetTagId(), playerController->GetTagId());
    GameMode->PlayerControllerMap.Add(playerController, playerController->GetTagId());
    GameMode->FindCastlesLocation();
    GameMode->CollectAllCastles();
    GameMode->CollectGoldMine();
    GameMode->CollectSheep();
    
    // Send Goblin Spawn Packet
    //SendSpawnResponseMsg(GetSpawnGoblinLocation(GI->GetTagId()), 0, GI->GetTagId());

    GameMode->FindAllGoblins(GetWorld());
    

    // goblin onpossess
    playerController->OnPossess(GameMode->FindGoblinById(GameMode->GetGoblinMap(), GI->GetTagId()));

    // possess
    if (playerController && GameMode && playerController->HasAuthority())
    {
        GameMode->PossessPlayer(playerController, GI->GetTagId());
    }

    GI->GetInstance(GetWorld());
    TCPClient = GI->GetTCPClient();
    // TCPClient->Initialize(GetWorld()); ////////////////////////////////////////////
    
}

void UPlayingWidget::NativeConstruct()
{
    Super::NativeConstruct(); 

    if (playerController)
    {
        UE_LOG(LogTemp, Warning, TEXT("[SETTING WIDGET] PLAYING WIDGET"));
        playerController->SetPlayingWidget(this); 
    }
    else if (!playerController)
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayerController is not valid"));
    }
 

    SpawnButton = Cast<UButton>(GetWidgetFromName(TEXT("SpawnButton")));
    HPBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("HPBar"))); 
    moneyCount = Cast<UTextBlock>(GetWidgetFromName(TEXT("MoneyCount")));
    SettingButton = Cast<UButton>(GetWidgetFromName(TEXT("SettingButton")));

    if (SpawnButton) SpawnButton->OnClicked.AddDynamic(this, &UPlayingWidget::OnSpawnButtonClicked);   
    if (SettingButton) SettingButton->OnClicked.AddDynamic(this, &UPlayingWidget::OnSettingButtonClicked);

    // 초기화
    HPBar->SetPercent(100.0f);
    moneyCount->SetText(FText::AsNumber(0));
    controlledChar = GI->GetChar();

    
}


void UPlayingWidget::OnSpawnButtonClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("SpawnButton Clicked!"));
    controlledChar = Cast<AGoblin>(playerController->GetPawn());
    
    if (GameMode->DeadCastleCnt >=3 ) return;

    if (DecreasePlayerMoney(controlledChar))
    {
        UE_LOG(LogTemp, Warning, TEXT("Decrease Player's Money Success: %d"), controlledChar->GetTagId());
        
        // SEND Spawn::Request 
        Spawn::Request Request; 
        Request.Cost = 10; 
        Request.TagId = controlledChar->GetTagId(); 

        FArrayWriter WriterArray; 
        WriterArray.Serialize((UTF8CHAR*)&Request, sizeof(Request));
        TSharedPtr<FBufferArchive> Packet = FTCPSocketClient_Async::CreatePacket((short)SPAWN_REQUEST, WriterArray.GetData(), WriterArray.Num());
        // GameMode->GetTCPSocketClient().BeginSendPhase(Packet);
        GI->GetTCPClient()->BeginSendPhase(Packet);
    }
}

void UPlayingWidget::OnSettingButtonClicked()
{
    // Setting Button add to viewport
    if (SettingWidgetClass)
    {
        UWorld* World = GetWorld(); 
        if (World) UGameplayStatics::SetGamePaused(World, true);
        EnableSpawnButton(false);

        SettingWidget = CreateWidget<USettingWidget>(this, SettingWidgetClass);
        if (SettingWidget) SettingWidget->AddToViewport(); // 위젯 화면 추가

    }
}

bool UPlayingWidget::DecreasePlayerMoney(AGoblin* goblin)
{
    if (playerController && controlledChar) 
    {
        return controlledChar->DecreaseMoney(10);
    }
    return false;
}


FVector UPlayingWidget::GetBombSpawnPoint(FVector &FoundLocation)
{
    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetOwningPlayer()->GetWorld());
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

FVector UPlayingWidget::GetSpawnGoblinLocation(int tagId)
{
    FVector SpawnCharLocation = GameMode->FindCastleLocationByTagId(tagId); 
    // SpawnCharLocation.X += 150.0f;
    SpawnCharLocation.Y += 100.0f;
    SpawnCharLocation.Z = -250.0f;
    return SpawnCharLocation;
}

void UPlayingWidget::UpdateHealthBar(float HealthPercent)
{
    if (HPBar) HPBar-> SetPercent(HealthPercent);
}


void UPlayingWidget::UpdateMoneyCount(float Money)
{
    moneyCount->SetText(FText::AsNumber(Money));
}

void UPlayingWidget::EnableSpawnButton(bool bEnable)
{
    if (SpawnButton)
    {
        SpawnButton->SetIsEnabled(bEnable);
    }
}

//
//void UPlayingWidget::SendSpawnResponseMsg(FVector spawnLocation, int SpawnType, int SpawnActorIndex)
//{
//    struct Spawn::Response *response = new Spawn::Response(); 
//    response->H.Command = 7;//0x31; 
//    response->Location = spawnLocation;
//    response->SpawnType = SpawnType;
//    response->SpawnActorIndex = SpawnActorIndex; 
//    response->successyn = 1; 
//    GameMode->messageQueue.push((struct HEAD *)response);
//}
//
//void UPlayingWidget::SendSpawnNotiMsg(int spawnType, int spawnActorIndex, float X, float Y)
//{
//    struct Spawn::Notification *noti = new Spawn::Notification(); 
//	noti->H.Command = 8;//0x32; 
//	noti->SpawnType = spawnType; 
//	noti->SpawnActorIndex = spawnActorIndex; 
//	noti->X = X; 
//	noti->Y = Y; 
//	GameMode->messageQueue.push((struct HEAD *)noti);
//}
//
//
//
//void UPlayingWidget::SendSelectCharResponseMsg(int playerIndex)
//{
//    struct CharacterSelect::Response *response = new CharacterSelect::Response(); 
//    response->H.Command = 1;//0x01; 
//    response->playerIndex = playerIndex; 
//    GameMode->messageQueue.push((struct HEAD*)response);
//}
//
//void UPlayingWidget::SendSelectCharNotiMsg(const char playerId[40], int playerIndex)
//{
//    struct CharacterSelect::Notification *noti = new CharacterSelect::Notification(); 
//    noti->H.Command = 2;//0x02; 
//    // strncpy(noti->playerId, playerId, sizeof(noti->playerId));
//    strncpy_s(noti->playerId, sizeof(noti->playerId), playerId, _TRUNCATE);
//
//
//    noti->playerId[sizeof(noti->playerId) - 1] = '\0';
//    noti->playerIndex = playerIndex;
//    GameMode->messageQueue.push((struct HEAD*)noti);
//}



