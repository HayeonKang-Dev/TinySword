// Fill out your copyright notice in the Description page of Project Settings.


#include "TinySwordGameInstance.h"
#include "UObject/UObjectGlobals.h"
#include "Kismet/GameplayStatics.h"


// void UTinySwordGameInstance::Init()
// {
    
//     Super::Init();
//     cnt++;
//     UE_LOG(LogTemp, Warning, TEXT("Game Instance - INIT : %d"), cnt);
    

//     if (!TCPClient.IsValid())
//     {
//         UE_LOG(LogTemp, Warning, TEXT("MakeShared TCPClient"));

//         TCPClient = MakeShared<FTCPSocketClient_Async>();
        
//         if (TCPClient.IsValid() && !bIsConnect)
//         {
//             TCPClient->Connect();
//             UE_LOG(LogTemp, Warning, TEXT("TCPClient->Connect() running."));
//             bIsConnect = true; 

//             // bHasInitialized = true;
//         }
//         else
//         {
//             UE_LOG(LogTemp, Warning, TEXT("TCPClient is not VALID"));
//         }
        
//     }
//     else
//     {
//         UE_LOG(LogTemp, Warning, TEXT("TCPClient is VALID "));
//         TCPClient->Connect();
//     }
	
// }

void UTinySwordGameInstance::Init()
{
    Super::Init();

    if (bIsConnect)
    {
        UE_LOG(LogTemp, Warning, TEXT("Game Instance already initialized. Skipping Init()..."));
        return;
    }

    cnt++;
    UE_LOG(LogTemp, Warning, TEXT("Game Instance - INIT : %d"), cnt);

    if (!TCPClient.IsValid())
    {
        TCPClient = MakeShared<FTCPSocketClient_Async>();
        UE_LOG(LogTemp, Warning, TEXT("Created new TCPClient."));
    }

    if (TCPClient.IsValid())
    {
        UWorld* World = GetWorld();
        if (World) TCPClient->Initialize(World);

        // FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UTinySwordGameInstance::HandlePostLoadMap);
        UE_LOG(LogTemp, Warning, TEXT("TCPClient->Connect() running."));
        //bIsConnect = true;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("TCPClient is not valid."));
    }
}

UTinySwordGameInstance *UTinySwordGameInstance::GetInstance(UWorld *World)
{
    if (World) return World->GetGameInstance<UTinySwordGameInstance>();
    return nullptr;
}

