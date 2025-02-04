// Fill out your copyright notice in the Description page of Project Settings.


#include "SelectCharacterWidget.h"
#include "TinySwordPlayerController.h"
#include "TinySwordGameMode.h"
#include "Goblin.h"
#include "Blueprint/WidgetLayoutLibrary.h"

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
        AGoblin** Goblin = GameMode->GoblinMap.Find(2);
        if (Goblin) PC->OnPossess(*Goblin);
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
    AGoblin** Goblin = GameMode->GoblinMap.Find(3);
    if (Goblin) PC->OnPossess(*Goblin);
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
    AGoblin** Goblin = GameMode->GoblinMap.Find(0);
    if (Goblin) PC->OnPossess(*Goblin);
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
    AGoblin** Goblin = GameMode->GoblinMap.Find(1);
    if (Goblin) PC->OnPossess(*Goblin);
}

void USelectCharacterWidget::OnQuitButtonClicked()
{
}
