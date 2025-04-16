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
#include "Blueprint/WidgetBlueprintLibrary.h"


void USelectCharacterWidget::NativeConstruct()
{
    Super::NativeConstruct(); 

    GameMode = Cast<ATinySwordGameMode>(GetWorld()->GetAuthGameMode());
    // GameMode->SetCharacterSelectWidget(this);

    ////////////////////////////////////
    TArray<UUserWidget*> FoundWidgets;
    UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundWidgets, USelectCharacterWidget::StaticClass(), false);

    if (FoundWidgets.Num() > 0)
    {
        USelectCharacterWidget* ActiveWidget = Cast<USelectCharacterWidget>(FoundWidgets[0]); // 첫 번째 위젯 선택
        if (ActiveWidget)
        {
            GameMode->SetCharacterSelectWidget(ActiveWidget);
            UE_LOG(LogTemp, Warning, TEXT("Successfully set the active SelectCharacterWidget!"));
        }
    }
    /////////////////////////////////////

    UE_LOG(LogTemp, Warning, TEXT("[SETTING] GAMEMODE: %d"), GameMode ? true:false);
    GI = Cast<UTinySwordGameInstance>(GetWorld()->GetGameInstance());

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

    if (ClickCnt == 4) // 4
    {
        UE_LOG(LogTemp, Warning, TEXT("Click Cnt == 4!!"));

        // UGameplayStatics::OpenLevel(GetWorld(), FName("PlayLevel"));
    }

}
// Blue, Purple, Red, Yellow = 1, 2, 3, 4

void USelectCharacterWidget::OnRedButtonClicked()
{
    SendCharacterSelectRequestMsg(3);

    // UGameplayStatics::OpenLevel(GetWorld(), FName("PlayLevel"));
    PC = Cast<ATinySwordPlayerController>(GetOwningPlayer()); 
    if (PC) 
    {
        PC->SetTagId(3);
        PC->playingWidget = playingWidget;

        // UTinySwordGameInstance* GI = Cast<UTinySwordGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
        if (GI)
        {
            GI->SetTagId(3);
        }

        // ClickCnt++;
        UE_LOG(LogTemp, Warning, TEXT("RedButtonClicked: %d"), ClickCnt);
        RedButton->SetIsEnabled(false);
    } 
}

void USelectCharacterWidget::OnYellowButtonClicked()
{
    SendCharacterSelectRequestMsg(4);
    PC = Cast<ATinySwordPlayerController>(GetOwningPlayer()); 
    if (PC) 
    {
        PC->SetTagId(4);
        PC->playingWidget = playingWidget;

        // UTinySwordGameInstance* GI = Cast<UTinySwordGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
        if (GI)
        {
            GI->SetTagId(4);
        }
    }

    // ClickCnt++;
    UE_LOG(LogTemp, Warning, TEXT("YellowButtonClicked: %d"), ClickCnt);
    YellowButton->SetIsEnabled(false);
}

void USelectCharacterWidget::OnBlueButtonClicked()
{
    SendCharacterSelectRequestMsg(1);
    PC = Cast<ATinySwordPlayerController>(GetOwningPlayer()); 
    if (PC) 
    {
        PC->SetTagId(1);
        PC->playingWidget = playingWidget;

        // UTinySwordGameInstance* GI = Cast<UTinySwordGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
        if (GI)
        {
            GI->SetTagId(1);
        }
    }
    // ClickCnt++;
    UE_LOG(LogTemp, Warning, TEXT("BlueButtonClicked: %d"), ClickCnt);
    BlueButton->SetIsEnabled(false);

}

void USelectCharacterWidget::OnPurpleButtonClicked()
{
    SendCharacterSelectRequestMsg(2);

    PC = Cast<ATinySwordPlayerController>(GetOwningPlayer()); 
    if (PC)
    {
        PC->SetTagId(2);
        PC->playingWidget = playingWidget;

        // UTinySwordGameInstance* GI = Cast<UTinySwordGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
        if (GI)
        {
            GI->SetTagId(2);
        }
    } 
    
    // ClickCnt++;
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

void USelectCharacterWidget::SendCharacterSelectRequestMsg(short tagId)
{
    UE_LOG(LogTemp, Warning, TEXT("Entered in Send CharacterSelect Request MSG"));
    CharacterSelect::Request Request; 
    Request.TagId = tagId; 

    FArrayWriter WriterArray; 
    WriterArray.Serialize((UTF8CHAR*)&Request, sizeof(Request));
    TSharedPtr<FBufferArchive> Packet = FTCPSocketClient_Async::CreatePacket((short)CHARACTERSELECT_REQUEST, WriterArray.GetData(), WriterArray.Num());
    
    // Send PACKET 
    // GameMode->GetTCPSocketClient().BeginSendPhase(Packet);

    GI->GetTCPClient()->BeginSendPhase(Packet);
}


//
//void USelectCharacterWidget::SendSelectCharResponseMsg(int playerIndex)
//{
//    struct CharacterSelect::Response *response = new CharacterSelect::Response(); 
//    response->H.Command = 1;//0x01; 
//    response->playerIndex = playerIndex; 
//    GameMode->messageQueue.push((struct HEAD*)response);
//}
//
//void USelectCharacterWidget::SendSelectCharNotiMsg(const char playerId[40], int playerIndex)
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
//
//void USelectCharacterWidget::SendSpawnResponseMsg()
//{
//    struct Spawn::Response *response = new Spawn::Response(); 
//    response->H.Command = 7;//0x31; 
//    response->successyn = 1; 
//    GameMode->messageQueue.push((struct HEAD *)response);
//
//}
//
//void USelectCharacterWidget::SendSpawnNotiMsg(int spawnType, int spawnActorIndex, float X, float Y)
//{
//    struct Spawn::Notification *noti = new Spawn::Notification(); 
//	noti->H.Command = 8;//0x32; 
//	noti->SpawnType = spawnType; 
//	noti->SpawnActorIndex = spawnActorIndex; 
//	noti->X = X; 
//	noti->Y = Y; 
//	GameMode->messageQueue.push((struct HEAD *)noti);
//}
