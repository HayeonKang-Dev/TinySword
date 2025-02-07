// Fill out your copyright notice in the Description page of Project Settings.


#include "SelectCharacterWidget.h"
#include "TinySwordPlayerController.h"
#include "TinySwordGameMode.h"
#include "Goblin.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "protocol.h"
#include "MainWidget.h"

void USelectCharacterWidget::NativeConstruct()
{
    Super::NativeConstruct(); 

    GameMode = Cast<ATinySwordGameMode>(GetWorld()->GetAuthGameMode());

    RedButton = Cast<UButton>(GetWidgetFromName(TEXT("RedButton")));
    YellowButton = Cast<UButton>(GetWidgetFromName(TEXT("YellowButton")));
    BlueButton = Cast<UButton>(GetWidgetFromName(TEXT("BlueButton")));
    PurpleButton = Cast<UButton>(GetWidgetFromName(TEXT("PurpleButton")));
    QuitButton = Cast<UButton>(GetWidgetFromName(TEXT("QuitButton")));

    if (RedButton) RedButton->OnClicked.AddDynamic(this, &USelectCharacterWidget::OnRedButtonClicked);   
    if (YellowButton) YellowButton->OnClicked.AddDynamic(this, &USelectCharacterWidget::OnYellowButtonClicked);
    if (BlueButton) BlueButton->OnClicked.AddDynamic(this, &USelectCharacterWidget::OnBlueButtonClicked);
    if (PurpleButton) PurpleButton->OnClicked.AddDynamic(this, &USelectCharacterWidget::OnPurpleButtonClicked);
    if (QuitButton) QuitButton->OnClicked.AddDynamic(this, &USelectCharacterWidget::OnQuitButtonClicked);


}


void USelectCharacterWidget::NativeTick(const FGeometry &MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (ClickCnt == 1) // 4
    {
        UE_LOG(LogTemp, Warning, TEXT("Click Cnt == 4!!"));
        if (PlayingWidgetClass)
        {
            playingWidget = CreateWidget<UPlayingWidget>(this, PlayingWidgetClass);
            UWidgetLayoutLibrary::RemoveAllWidgets(this);
            PC->SetPlayingWidget(playingWidget);
            if (playingWidget) playingWidget->AddToViewport(); // 위젯 화면 추가

        }
    }

}
// Blue, Purple, Red, Yellow = 0, 1, 2, 3

void USelectCharacterWidget::OnRedButtonClicked()
{
    PC = Cast<ATinySwordPlayerController>(GetOwningPlayer()); 
    if (PC) 
    {
        PC->SetTagId(2);
        PC->playingWidget = playingWidget;

        ClickCnt++;
        UE_LOG(LogTemp, Warning, TEXT("RedButtonClicked: %d"), ClickCnt);
        RedButton->SetIsEnabled(false);
        // AGoblin** Goblin = GameMode->GoblinMap.Find(2);
        // if (Goblin) PC->OnPossess(*Goblin);

        SpawnGoblin(PC);
    }
    
}

void USelectCharacterWidget::OnYellowButtonClicked()
{
    PC = Cast<ATinySwordPlayerController>(GetOwningPlayer()); 
    if (PC) 
    {
        PC->SetTagId(3);
        PC->playingWidget = playingWidget;
    }

    ClickCnt++;
    UE_LOG(LogTemp, Warning, TEXT("YellowButtonClicked: %d"), ClickCnt);
    YellowButton->SetIsEnabled(false);
    // AGoblin** Goblin = GameMode->GoblinMap.Find(3);
    // if (Goblin) PC->OnPossess(*Goblin);
    SpawnGoblin(PC);
}

void USelectCharacterWidget::OnBlueButtonClicked()
{
    PC = Cast<ATinySwordPlayerController>(GetOwningPlayer()); 
    if (PC) 
    {
        PC->SetTagId(0);
        PC->playingWidget = playingWidget;
    }
    ClickCnt++;
    UE_LOG(LogTemp, Warning, TEXT("BlueButtonClicked: %d"), ClickCnt);
    BlueButton->SetIsEnabled(false);
    // AGoblin** Goblin = GameMode->GoblinMap.Find(0);
    // if (Goblin) PC->OnPossess(*Goblin);
    SpawnGoblin(PC);
}

void USelectCharacterWidget::OnPurpleButtonClicked()
{
    PC = Cast<ATinySwordPlayerController>(GetOwningPlayer()); 
    if (PC)
    {
        PC->SetTagId(1);
        PC->playingWidget = playingWidget;
    } 
    ClickCnt++;
    UE_LOG(LogTemp, Warning, TEXT("PurpleButtonClicked: %d"), ClickCnt);
    PurpleButton->SetIsEnabled(false);
    // AGoblin** Goblin = GameMode->GoblinMap.Find(1);
    // if (Goblin) PC->OnPossess(*Goblin);
    SpawnGoblin(PC);
}

void USelectCharacterWidget::OnQuitButtonClicked()
{
    if (MainWidgetClass)
    {
        mainWidget = CreateWidget<UMainWidget>(this, MainWidgetClass);
        UWidgetLayoutLibrary::RemoveAllWidgets(this);
        if (mainWidget) mainWidget->AddToViewport(); // 위젯 화면 추가

    }
}

void USelectCharacterWidget::SpawnGoblin(ATinySwordPlayerController* PlayerController)
{
    if (GameMode)
    {
        TMap<int32, FVector>& CastleMap = GameMode->GetCastleMap();
        UObject* spawnActor = nullptr; 

        switch(PlayerController->GetTagId())
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

        UBlueprint* GeneratedBP = Cast<UBlueprint>(spawnActor); 
        UWorld* World = GetWorld(); 

        if (!spawnActor || !GeneratedBP || !GeneratedBP->GeneratedClass || !World) return; 
        
        FActorSpawnParameters SpawnParams; 
        SpawnParams.Owner = PlayerController; 
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; 

        FVector SpawnLocation = GameMode->FindCastleLocationByTagId(PlayerController->GetTagId());  
        // SpawnLocation.X += 150.0f;
        SpawnLocation.Y += 100.0f;
        SpawnLocation.Z = -250.0f;
        FRotator SpawnRotation(0.0f, 0.0f, 0.0f);

        APaperZDCharacter* SpawnedChar = World->SpawnActor<APaperZDCharacter>(GeneratedBP->GeneratedClass, SpawnLocation, SpawnRotation, SpawnParams);

        if (SpawnedChar) 
        {
            PlayerController->OnPossess(SpawnedChar);
            // PlayerController->SetViewTargetWithBlend(SpawnedChar, 0.0f);

            SendSelectCharResponseMsg(PlayerController->GetTagId());
            char myPlayerId[40] = "test";
            SendSelectCharNotiMsg(myPlayerId, PlayerController->GetTagId());

            SendSpawnResponseMsg();
            SendSpawnNotiMsg(0, PlayerController->GetTagId(), SpawnLocation.X, SpawnLocation.Y);
        }
    }
}

void USelectCharacterWidget::SendSelectCharResponseMsg(int playerIndex)
{
    struct CharacterSelect::Response *response = new CharacterSelect::Response(); 
    response->H.Command = 0x01; 
    response->playerIndex = playerIndex; 
    GameMode->messageQueue.push((struct HEAD*)response);
}

void USelectCharacterWidget::SendSelectCharNotiMsg(const char playerId[40], int playerIndex)
{
    struct CharacterSelect::Notification *noti = new CharacterSelect::Notification(); 
    noti->H.Command = 0x02; 
    // strncpy(noti->playerId, playerId, sizeof(noti->playerId));
    strncpy_s(noti->playerId, sizeof(noti->playerId), playerId, _TRUNCATE);


    noti->playerId[sizeof(noti->playerId) - 1] = '\0';
    noti->playerIndex = playerIndex;
    GameMode->messageQueue.push((struct HEAD*)noti);
}

void USelectCharacterWidget::SendSpawnResponseMsg()
{
    struct Spawn::Response *response = new Spawn::Response(); 
    response->H.Command = 0x31; 
    response->successyn = 1; 
    GameMode->messageQueue.push((struct HEAD *)response);
    delete response;
}

void USelectCharacterWidget::SendSpawnNotiMsg(int spawnType, int spawnActorIndex, float X, float Y)
{
    struct Spawn::Notification *noti = new Spawn::Notification(); 
	noti->H.Command = 0x32; 
	noti->SpawnType = spawnType; 
	noti->SpawnActorIndex = spawnActorIndex; 
	noti->X = X; 
	noti->Y = Y; 
	GameMode->messageQueue.push((struct HEAD *)noti);
}
