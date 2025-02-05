// Fill out your copyright notice in the Description page of Project Settings.


#include "TinySwordGameMode.h"
#include "BaseCastle.h"
#include "Kismet/GameplayStatics.h"
#include "BaseBomb.h"
#include "Goblin.h"

void ATinySwordGameMode::BeginPlay()
{
    Super::BeginPlay();
    FindCastlesLocation();
    FindAllGoblins();
}


void ATinySwordGameMode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    struct HEAD *data = messageQueue.pop(); 
    if (data == nullptr) return;
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



// Packet Processing Function 

void ATinySwordGameMode::Moving(HEAD *data)
{
    
}