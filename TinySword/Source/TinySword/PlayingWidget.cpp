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
// #include "TinySwordPlayerState.h"
#include "TinySwordGameInstance.h"

void UPlayingWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    GI = Cast<UTinySwordGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    playerController = Cast<ATinySwordPlayerController>(GetOwningPlayer()); 
    GameMode = Cast<ATinySwordGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    GameMode->FindCastlesLocation();
    
    FTimerHandle tmpTimer;
    GetWorld()->GetTimerManager().SetTimer(tmpTimer, FTimerDelegate::CreateUObject(this, &UPlayingWidget::SpawnGoblin, playerController), 0.2f, false); 
    
}

void UPlayingWidget::NativeConstruct()
{
    Super::NativeConstruct(); 

    if (playerController)
    {
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
    if (DecreasePlayerMoney())
    {
        UE_LOG(LogTemp, Warning, TEXT("Decrease Player's Money Success"));
        SpawnBomb();
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

bool UPlayingWidget::DecreasePlayerMoney()
{
    controlledChar = Cast<AGoblin>(playerController->GetPawn());
    if (playerController && controlledChar) 
    {
        return controlledChar->DecreaseMoney(10);
    }
    return false;
}

void UPlayingWidget::SpawnBomb()
{
    if (GameMode)
    {
        TMap<int32, FVector>& CastleMap = GameMode->GetCastleMap();
        UObject* spawnActor = nullptr; 
        switch (controlledChar->GetTagId())
        {
            case 0: 
                spawnActor = StaticLoadObject(UObject::StaticClass(), NULL, TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Bombs/BP_Bomb.BP_Bomb'"));
                break;

            case 1: 
                spawnActor = StaticLoadObject(UObject::StaticClass(), NULL, TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Bombs/BP_Bomb_Purple.BP_Bomb_Purple'"));
                break;

            case 2: 
                spawnActor = StaticLoadObject(UObject::StaticClass(), NULL, TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Bombs/BP_Bomb_Red.BP_Bomb_Red'"));
                break;

            case 3: 
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

        FVector* FoundLocation = CastleMap.Find(controlledChar->GetTagId()); 
       
        if(FoundLocation)
        {
            
            AActor* OwnerActor = controlledChar; 
            AActor* SpawnedActor = World->SpawnActor<AActor>(GeneratedBP->GeneratedClass, GetBombSpawnPoint(World, *FoundLocation), OwnerActor->GetActorRotation(), SpawnParams);
            
            
            if (SpawnedActor)
            {
                SpawnedBomb = Cast<ABaseBomb>(SpawnedActor); 
                if (SpawnedBomb) {
                    SetBombIndex();
                    SpawnedBomb->SetOwnerTagId(playerController->GetTagId()); /////////////////////////////
                    UE_LOG(LogTemp, Warning, TEXT("Owner Tag Id (PC) : %d -> %d"), playerController->GetTagId(), SpawnedBomb->GetOwnerTagId());

                    SendSpawnResponseMsg(); 
                    SendSpawnNotiMsg(1, SpawnedBomb->GetTagId(), SpawnedBomb->GetActorLocation().X, SpawnedBomb->GetActorLocation().Y);
                }
            }
        }
    }
}

void UPlayingWidget::SetBombIndex()
{
    int32 id = 0; 
    if (!GameMode->ReuseBombId.IsEmpty()) GameMode->ReuseBombId.Dequeue(id);
    else id = GameMode->ActiveBombId.Num() + 1; 
    SpawnedBomb->SetTagId(id); 
    GameMode->ActiveBombId.Add(SpawnedBomb, id);
}



FVector UPlayingWidget::GetBombSpawnPoint(UWorld *World, FVector &FoundLocation)
{
    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(World);
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


void UPlayingWidget::SendSpawnResponseMsg()
{
    struct Spawn::Response *response = new Spawn::Response(); 
    response->H.Command = 0x31; 
    response->successyn = 1; 
    GameMode->messageQueue.push((struct HEAD *)response);
    delete response;
}

void UPlayingWidget::SendSpawnNotiMsg(int spawnType, int spawnActorIndex, float X, float Y)
{
    struct Spawn::Notification *noti = new Spawn::Notification(); 
	noti->H.Command = 0x32; 
	noti->SpawnType = spawnType; 
	noti->SpawnActorIndex = spawnActorIndex; 
	noti->X = X; 
	noti->Y = Y; 
	GameMode->messageQueue.push((struct HEAD *)noti);
}



void UPlayingWidget::SendSelectCharResponseMsg(int playerIndex)
{
    struct CharacterSelect::Response *response = new CharacterSelect::Response(); 
    response->H.Command = 0x01; 
    response->playerIndex = playerIndex; 
    GameMode->messageQueue.push((struct HEAD*)response);
}

void UPlayingWidget::SendSelectCharNotiMsg(const char playerId[40], int playerIndex)
{
    struct CharacterSelect::Notification *noti = new CharacterSelect::Notification(); 
    noti->H.Command = 0x02; 
    // strncpy(noti->playerId, playerId, sizeof(noti->playerId));
    strncpy_s(noti->playerId, sizeof(noti->playerId), playerId, _TRUNCATE);


    noti->playerId[sizeof(noti->playerId) - 1] = '\0';
    noti->playerIndex = playerIndex;
    GameMode->messageQueue.push((struct HEAD*)noti);
}







///////////////////////////////////////////////////////////////

void UPlayingWidget::SpawnGoblin(ATinySwordPlayerController* PlayerController)
{
    if (GameMode)
    {
        TMap<int32, FVector>& CastleMap = GameMode->GetCastleMap();
        UObject* spawnActor = nullptr; 

     
        if (GI)
        {
            switch(GI->GetTagId())
            {
                case 0: 
                    spawnActor = StaticLoadObject(UObject::StaticClass(), NULL, TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Goblins/Goblin_Blue.Goblin_Blue'")); 
                    break; 

                case 1: 
                    spawnActor = StaticLoadObject(UObject::StaticClass(), NULL, TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Goblins/Goblin_Purple.Goblin_Purple'")); 
                    break; 

                case 2: 
                    spawnActor = StaticLoadObject(UObject::StaticClass(), NULL, TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Goblins/Goblin_Red.Goblin_Red'")); 
                    break; 

                case 3: 
                    spawnActor = StaticLoadObject(UObject::StaticClass(), NULL, TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Goblins/Goblin_Yellow.Goblin_Yellow'")); 
                    break; 

                default: 
                    UE_LOG(LogTemp, Warning, TEXT("Spawn Actor is not valid"));
                    break;

            }

        }

        
        UBlueprint* GeneratedBP = Cast<UBlueprint>(spawnActor); 
        UWorld* World = GetWorld(); 

        if (!spawnActor || !GeneratedBP || !GeneratedBP->GeneratedClass || !World) return; 
        
        FActorSpawnParameters SpawnParams; 
        SpawnParams.Owner = PlayerController; 
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; 

        FVector SpawnCharLocation = GameMode->FindCastleLocationByTagId(GI->GetTagId()); ////////////////////////////////// 
        // SpawnCharLocation.X += 150.0f;
        SpawnCharLocation.Y += 100.0f;
        SpawnCharLocation.Z = -250.0f;
        FRotator SpawnRotation(0.0f, 0.0f, 0.0f);

        AGoblin* SpawnedChar = World->SpawnActor<AGoblin>(GeneratedBP->GeneratedClass, SpawnCharLocation, SpawnRotation, SpawnParams);

        if (SpawnedChar) 
        {
            UE_LOG(LogTemp, Warning, TEXT("SpawnedChar : %s"), *SpawnedChar->GetName());
            PlayerController->OnPossess(SpawnedChar);
            // PlayerController->SetViewTargetWithBlend(SpawnedChar, 0.0f);

            SendSelectCharResponseMsg(PlayerController->GetTagId());
            char myPlayerId[40] = "test";
            SendSelectCharNotiMsg(myPlayerId, PlayerController->GetTagId());

            SendSpawnResponseMsg();
            SendSpawnNotiMsg(0, PlayerController->GetTagId(), SpawnCharLocation.X, SpawnCharLocation.Y);
            GI->SetChar(SpawnedChar);
        }
    }
}