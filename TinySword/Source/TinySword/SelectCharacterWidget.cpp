// Fill out your copyright notice in the Description page of Project Settings.


#include "SelectCharacterWidget.h"
#include "TinySwordPlayerController.h"
#include "TinySwordGameMode.h"
#include "Goblin.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "protocol.h"
#include "MainWidget.h"
#include "Kismet/GameplayStatics.h"
#include "TinySwordGameInstance.h"



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

        UGameplayStatics::OpenLevel(GetWorld(), FName("PlayLevel"));
    }

}
// Blue, Purple, Red, Yellow = 0, 1, 2, 3

void USelectCharacterWidget::OnRedButtonClicked()
{
    // UGameplayStatics::OpenLevel(GetWorld(), FName("PlayLevel"));
    PC = Cast<ATinySwordPlayerController>(GetOwningPlayer()); 
    if (PC) 
    {
        PC->SetTagId(2);
        PC->playingWidget = playingWidget;

        UTinySwordGameInstance* GI = Cast<UTinySwordGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
        if (GI)
        {
            GI->SetTagId(2);
        }

        ClickCnt++;
        UE_LOG(LogTemp, Warning, TEXT("RedButtonClicked: %d"), ClickCnt);
        RedButton->SetIsEnabled(false);
    } 
}

void USelectCharacterWidget::OnYellowButtonClicked()
{
    PC = Cast<ATinySwordPlayerController>(GetOwningPlayer()); 
    if (PC) 
    {
        PC->SetTagId(3);
        PC->playingWidget = playingWidget;

        UTinySwordGameInstance* GI = Cast<UTinySwordGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
        if (GI)
        {
            GI->SetTagId(3);
        }
    }

    ClickCnt++;
    UE_LOG(LogTemp, Warning, TEXT("YellowButtonClicked: %d"), ClickCnt);
    YellowButton->SetIsEnabled(false);
}

void USelectCharacterWidget::OnBlueButtonClicked()
{
    PC = Cast<ATinySwordPlayerController>(GetOwningPlayer()); 
    if (PC) 
    {
        PC->SetTagId(0);
        PC->playingWidget = playingWidget;

        UTinySwordGameInstance* GI = Cast<UTinySwordGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
        if (GI)
        {
            GI->SetTagId(0);
        }
    }
    ClickCnt++;
    UE_LOG(LogTemp, Warning, TEXT("BlueButtonClicked: %d"), ClickCnt);
    BlueButton->SetIsEnabled(false);

}

void USelectCharacterWidget::OnPurpleButtonClicked()
{
    PC = Cast<ATinySwordPlayerController>(GetOwningPlayer()); 
    if (PC)
    {
        PC->SetTagId(1);
        PC->playingWidget = playingWidget;

        UTinySwordGameInstance* GI = Cast<UTinySwordGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
        if (GI)
        {
            GI->SetTagId(1);
        }
    } 
    
    ClickCnt++;
    UE_LOG(LogTemp, Warning, TEXT("PurpleButtonClicked: %d"), ClickCnt);
    PurpleButton->SetIsEnabled(false);

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


void USelectCharacterWidget::SendSelectCharResponseMsg(int playerIndex)
{
    struct CharacterSelect::Response *response = new CharacterSelect::Response(); 
    response->H.Command = 1;//0x01; 
    response->playerIndex = playerIndex; 
    GameMode->messageQueue.push((struct HEAD*)response);
}

void USelectCharacterWidget::SendSelectCharNotiMsg(const char playerId[40], int playerIndex)
{
    struct CharacterSelect::Notification *noti = new CharacterSelect::Notification(); 
    noti->H.Command = 2;//0x02; 
    // strncpy(noti->playerId, playerId, sizeof(noti->playerId));
    strncpy_s(noti->playerId, sizeof(noti->playerId), playerId, _TRUNCATE);


    noti->playerId[sizeof(noti->playerId) - 1] = '\0';
    noti->playerIndex = playerIndex;
    GameMode->messageQueue.push((struct HEAD*)noti);
}

void USelectCharacterWidget::SendSpawnResponseMsg()
{
    struct Spawn::Response *response = new Spawn::Response(); 
    response->H.Command = 7;//0x31; 
    response->successyn = 1; 
    GameMode->messageQueue.push((struct HEAD *)response);

}

void USelectCharacterWidget::SendSpawnNotiMsg(int spawnType, int spawnActorIndex, float X, float Y)
{
    struct Spawn::Notification *noti = new Spawn::Notification(); 
	noti->H.Command = 8;//0x32; 
	noti->SpawnType = spawnType; 
	noti->SpawnActorIndex = spawnActorIndex; 
	noti->X = X; 
	noti->Y = Y; 
	GameMode->messageQueue.push((struct HEAD *)noti);
}
