// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"

#include "TinySwordGameMode.h"

#include "AsyncNetworking.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#undef SetPort

#include "protocol.h"
#include "Sockets.h"

#include "Common/TcpSocketBuilder.h"
#include "Serialization/ArrayWriter.h"

#include "SocketSubsystem.h"

#include "TCPSocketClientUtils.h"
#include "Networking.h"
#include "SocketSubsystemModule.h"

#include "Templates/SharedPointer.h"

// FTCPSocketClient_Async::FTCPSocketClient_Async(UMyGameInstance* InGameInstance)
//     : GameInstance(InGameInstance), Socket(nullptr){}

bool IsSocketConnected(TSharedPtr<FSocket> Socket)
{
    if (!Socket.IsValid())
    {
        return false;
    }

    uint8 Test;
    int32 BytesRead = 0;
    return Socket->Recv(&Test, 1, BytesRead, ESocketReceiveFlags::Peek);
}

// bool RecvFullPacket(TSharedPtr<FSocket> Socket, uint8* Buffer, int32 BufferSize)
// {
//     int32 TotalReceived = 0;
//     int32 NumRead = 0;

//     while (TotalReceived < BufferSize)
//     {
//         if (!Socket->Recv(Buffer + TotalReceived, BufferSize - TotalReceived, NumRead, ESocketReceiveFlags::None))
//         {
//             UE_LOG(LogTemp, Error, TEXT("Recv Failed: Connection closed or error occurred."));
//             return false;
//         }

//         if (NumRead == 0)
//         {
//             UE_LOG(LogTemp, Warning, TEXT("Recv returned 0 bytes. Connection might be closing."));
//             return false;
//         }

//         TotalReceived += NumRead;
//     }

//     return true;
// }

void FTCPSocketClient_Async::Initialize(UWorld *world)
{
    FString IPAddress = TEXT("127.0.0.1");
    uint16 PortNumber = 25000;

    GI = UTinySwordGameInstance::GetInstance(world);
    GameMode = Cast<ATinySwordGameMode>(world->GetAuthGameMode());
    World = world;

    if (!GameMode)
    {
        UE_LOG(LogTemp, Error, TEXT("[NULLPTR] GAMEMODE IS NULL"));
    }

    if (GI)
    {
        TCPClient = GI->GetTCPClient();
        TSharedPtr<FSocket> socket = TSharedPtr<FSocket>(FTcpSocketBuilder(TEXT("ClientSocket")).Build());
        if (!socket.IsValid())
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to Create the SOCKET"));
            return;
        }

        socket->SetNoDelay(true);
        if (TCPClient)
        {
            TCPClient->SetSocket(socket);
            TCPClient->GetSocket()->SetNonBlocking(true); ////////////////////////////
        }
    }

    if (TCPClient.IsValid() && TCPClient->GetSocket().IsValid())
    {
        if (FTCPSocketClientUtils::Connect(TCPClient->GetSocket().Get(), IPAddress, PortNumber))
        {
            UE_LOG(LogTemp, Warning, TEXT("Socket Connected"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Socket NOT CONNECTED"));
            if (TCPClient.IsValid() && TCPClient->GetSocket().IsValid())
            {
                FTCPSocketClientUtils::DestroySocket(TCPClient->GetSocket().Get());
                TCPClient->GetSocket().Reset();
            }
        }
    }
}

// 헤더와 payload를 결합한 패킷 생성 함수
TSharedPtr<FBufferArchive> FTCPSocketClient_Async::CreatePacket(short MessageType, const void *PayloadData, int32 PayloadSize)
{
    // UE_LOG(LogTemp, Warning, TEXT("Entered in CreatePacket..."));
    // 새로운 패킷 버퍼를 생성합니다.
    TSharedPtr<FBufferArchive> Packet = MakeShareable(new FBufferArchive());

    // 헤더 생성:
    // MessageType는 전달받은 메시지 타입,
    // Size는 payload의 크기 (unsigned short로 캐스팅; 헤더 크기 4바이트를 포함해서 계산)
    FMessageHeader Header(MessageType, static_cast<unsigned short>(PayloadSize));

    // 헤더 직렬화 (operator<<를 통해 HEAD 형식으로 기록됨)
    *Packet << Header;

    // UE_LOG(LogTemp, Warning, TEXT("Before Serialize"));
    // 이어서 payload를 버퍼에 기록합니다.
    Packet->Serialize(const_cast<void *>(PayloadData), PayloadSize);

    return Packet;
}

// void FTCPSocketClient_Async::Connect()
// {
//     // TSharedPtr<FSocket> socket = TSharedPtr<FSocket>(FTcpSocketBuilder(TEXT("ClientSocket")).Build());

//     // Socet = FTcpSocketBuilder(TEXT("ClientSocket"));

//     // if (!socket.IsValid())
//     // {
//     //     UE_LOG(LogTemp, Error, TEXT("Failed to Create the SOCKET"));
//     //     return;
//     // }

//     // FString IPAddress = TEXT("127.0.0.1");
//     // uint16 PortNumber = 25000;

//     // UWorld* World = GEngine->GetWorldContexts()[0].World();
//     if (GI && TCPClient)
//     {
//         UE_LOG(LogTemp, Warning, TEXT("Entered in Casting instance"));/////////////////////////////

//         // TCPClient = GI->GetTCPClient();
//         if (!TCPClient.IsValid())
//         {
//             UE_LOG(LogTemp, Error, TEXT("GameInstance is NULL"));
//         }
//         // TSharedPtr<FSocket> SharedSocket = socket;
//         // TCPClient->SetSocket(socket);

//         // if (TCPClient.IsValid() && TCPClient->GetSocket().IsValid())
//         // {
//         //     if (FTCPSocketClientUtils::Connect(TCPClient->GetSocket().Get(), IPAddress, PortNumber))
//         //     {
//         //         UE_LOG(LogTemp, Warning, TEXT("Socket Connected"));
//         //         TCPClient->GetSocket()->SetNonBlocking(true); // false

//         //     }
//         //     else
//         //     {
//         //         UE_LOG(LogTemp, Error, TEXT("Socket NOT CONNECTED"));
//         //         if (TCPClient.IsValid() && TCPClient->GetSocket().IsValid())
//         //         {
//         //             FTCPSocketClientUtils::DestroySocket(TCPClient->GetSocket().Get());
//         //             TCPClient->GetSocket().Reset();
//         //         }

//         //     }
//         // }
//     }

// }

void FTCPSocketClient_Async::Disconnect()
{
    if (!TCPClient.IsValid() || !TCPClient->GetSocket().IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Disconnect Called. "));
        return;
    }
    FTCPSocketClientUtils::DestroySocket(TCPClient->GetSocket().Get()); // Socket
    TCPClient->SetSocket(nullptr);
}

TSharedPtr<FBufferArchive> FTCPSocketClient_Async::CreatePacket(uint32 Type, const FString &Text)
{
    SCOPE_CYCLE_COUNTER(STAT_Send);

    // FString 텍스트를 UTF8로 변환
    FTCHARToUTF8 Convert(*Text);
    int32 PayloadSize = Convert.Length(); // UTF8 바이트 수

    // 새 CreatePacket 함수의 첫번째 인자는 short 타입이어야 하므로 변환
    // TSharedPtr<FBufferArchive> Packet = CreatePacket(static_cast<short>(Type), Convert.Get(), PayloadSize);
    TSharedPtr<FBufferArchive> Packet = CreatePacket(static_cast<short>(Type), reinterpret_cast<const void *>(Convert.Get()), PayloadSize);

    return Packet;
}

void FTCPSocketClient_Async::BeginSendPhase(TSharedPtr<FBufferArchive> Packet)
{

    // UE_LOG(LogTemp, Log, TEXT("BeginSendPhase"));

    if (!TCPClient.IsValid()) // || !TCPClient->GetSocket().IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("TCPClient is NULL"));
        // if (World) Initialize(World);
        // if (!World) UE_LOG(LogTemp, Fatal, TEXT("WORLD IS NULL (Async.cpp 246)"));
        return;
    }
    // else if (TCPClient.IsValid())
    // {
    //     // UE_LOG(LogTemp, Warning, TEXT("TCPClient is not null!"));
    // }

    if (!TCPClient->GetSocket().IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("Socket is NULL"));
        return;
    }

    // UE_LOG(LogTemp, Warning, TEXT("Socket connection State: %d"), (int32)TCPClient->GetSocket().Get()->GetConnectionState());

    //////////////////////////////////////
    AsyncTask(ENamedThreads::AnyThread, [this, Packet, TCPClient = TCPClient]()
              {
			// if (TCPClient->GetSocket().Get() == nullptr)
            if (!TCPClient.IsValid() || !TCPClient->GetSocket().IsValid())
			{
                UE_LOG(LogTemp, Error, TEXT("Socket is NULL"));
				return;
			}
            if (TCPClient == nullptr) return; // this

            if (!IsSocketConnected(TCPClient->GetSocket()))
            {
                UE_LOG(LogTemp, Warning, TEXT("Socket connection State: %d"), (int32)TCPClient->GetSocket().Get()->GetConnectionState());
                UE_LOG(LogTemp, Error, TEXT("Socket is not CONNECTED..."));
                return;
            }

			// send all things in queue
			int32 NumSend;
			bool bSuccess = TCPClient->GetSocket().Get()->Send(Packet->GetData(), Packet->Num(), NumSend);
            
            // if (bSuccess)
            // {
            //     UE_LOG(LogTemp, Warning, TEXT("Sent %d Bytes out of %d"), NumSend, Packet->Num());
            // }

            // this
			AsyncTask(ENamedThreads::GameThread, [this, bSuccess, TCPClient = TCPClient]() 
				{
					if (TCPClient->GetSocket().Get() == nullptr|| TCPClient == nullptr)
					{
                        UE_LOG(LogTemp, Error, TEXT("Socket is NULL"));
						return;
					}

					if (bSuccess)
					{
						OnSendCompleted();
						EndSendPhase();
                        BeginRecvPhase(); 
					}
					else
					{
						// send failed
						OnSendFailed();
						EndSendPhase();
					}
				}); });
    // UE_LOG(LogTemp, Warning, TEXT("Socket connection State: %d"), (int32)TCPClient->GetSocket().Get()->GetConnectionState());
}

void FTCPSocketClient_Async::EndSendPhase()
{
    // UE_LOG(LogTemp, Log, TEXT("EndSendPhase"));
    // BeginRecvPhase();
}

void FTCPSocketClient_Async::BeginRecvPhase()
{
    // UE_LOG(LogTemp, Log, TEXT("BeginRecvPhase"));

    if (!GI || !GI->GetTCPClient().IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("GI or TCPClient is INVALID"));
        return;
    }

    TCPClient = GI->GetTCPClient();
    TSharedPtr<FTCPSocketClient_Async> LocalTCPClient = TCPClient;

    AsyncTask(ENamedThreads::AnyThread, [this, LocalTCPClient]()
              {

        if (!LocalTCPClient->GetSocket().IsValid())
        {
            UE_LOG(LogTemp, Error, TEXT("Socket is not valid..."));
            return;
        }

        if (!IsSocketConnected(LocalTCPClient->GetSocket())) 
        {
            UE_LOG(LogTemp, Error, TEXT("Socket is NOT CONNECTED"));
            return;
        }

        //////////////////////////////////////////////////////
        TArray<uint8> HeaderBuffer;
        int32 HeaderSize = FMessageHeader::RequestLength;
        HeaderBuffer.AddZeroed(HeaderSize);

        // RECV **header**
        bool bSuccessRecvHeader = false; 
        int32 NumRead = 0; 

        // 여기에서도 game mode null 
        bSuccessRecvHeader = LocalTCPClient->GetSocket().Get()->Recv( // Socket
                HeaderBuffer.GetData(), HeaderBuffer.Num(), NumRead, ESocketReceiveFlags::Type::None); // WaitAll
        
        // UE_LOG(LogTemp, Warning, TEXT("******************** bSuccessRecvHeader: %s"), bSuccessRecvHeader ? TEXT("true") : TEXT("false"));
        
        int32 ErrorCode = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLastErrorCode();
        if (ErrorCode == SE_EWOULDBLOCK) return; 

        
        if (bSuccessRecvHeader)
        {
            // RECV **payload**
            HEAD Header;
            FMemory::Memcpy(&Header, HeaderBuffer.GetData(), sizeof(Header));
            // 이 시점에 game mode null 


            int32 PayloadSize = Header.Length - 4; // sizeof(Size) + sizeof(Type) = 4 

            // UE_LOG(LogTemp, Warning, TEXT("[Payload SIZE: %d]"), PayloadSize);
            if (PayloadSize < 0)
            {
                //UE_LOG(LogTemp, Error, TEXT("PayloadSize < 0"));

                OnRecvFailed(); 
                EndRecvPhase(); 
                return;
            }


            TArray<uint8> Payload;
            Payload.SetNumZeroed(PayloadSize);

            bool bSuccessRecvPayload = false; 
            bSuccessRecvPayload = LocalTCPClient->GetSocket().Get()->Recv( // Socket
                Payload.GetData(), Payload.Num(), NumRead, ESocketReceiveFlags::Type::None);
            
            
            

            AsyncTask(ENamedThreads::GameThread, [this, bSuccessRecvPayload, Payload, Header, LocalTCPClient]()
            {
                // Socket
                // if (LocalTCPClient->GetSocket().Get() == nullptr || this == nullptr) return;
                if (!LocalTCPClient->GetSocket().IsValid()) return;
                if (bSuccessRecvPayload)
                {
                    // UE_LOG(LogTemp, Warning, TEXT("Header.Index: %d"), Header.Index);
                    OnRecvCompleted(Header.Index, Payload);
                    // OnRecvCompleted(Header.Index, const_cast<HEAD*>(&Header)); //////////
                    EndRecvPhase();
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("Recv Payload Failed"));
                    OnRecvFailed();
                    EndRecvPhase();
                }
            });
        }
        else
        {
            AsyncTask(ENamedThreads::GameThread, [this, LocalTCPClient]()
            {
                // Socket
                if (!IsSocketConnected(LocalTCPClient->GetSocket())) 
                {
                    UE_LOG(LogTemp, Error, TEXT("Socket or this NULL"));
                    return;
                }

                int32 ErrorCode = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLastErrorCode();
                if (ErrorCode == SE_EWOULDBLOCK) return; 

                UE_LOG(LogTemp, Warning, TEXT("Socket connection State: %d"), (int32)TCPClient->GetSocket().Get()->GetConnectionState());
                UE_LOG(LogTemp, Error, TEXT("Recv Header Failed.")); //////////////////////////////////
                
                OnRecvFailed();
                EndRecvPhase();
            });
        } });
}

void FTCPSocketClient_Async::EndRecvPhase()
{
    // UE_LOG(LogTemp, Log, TEXT("EndRecvPhase"));
    // BeginSendPhase(); // Q. -> 뭘 보내? Recv 끝나고나서?
}

void FTCPSocketClient_Async::OnSendCompleted()
{
    // UE_LOG(LogTemp, Log, TEXT("OnSendCompleted"));
}

void FTCPSocketClient_Async::OnSendFailed()
{
    FString OutText;
    FTCPSocketClientUtils::PrintSocketError(OutText);
    UE_LOG(LogTemp, Log, TEXT("OnSendFailed Error : %s"), *OutText);
}
/////////////////////////////////////////////////////////////////////////////

// void* FTCPSocketClient_Async::GetDataPtr(const uint8* Data) {
//     if (!Data)
//     {
//         UE_LOG(LogTemp, Error, TEXT("Invalid Data Pointer"));
//         return nullptr;
//     }
//     HEAD* Header = reinterpret_cast<HEAD*>(const_cast<uint8*>(Data));
//     // 바이트 순서 변환 (필요한 경우)
//     uint16 PacketLength = ntohs(Header->Length);

//     // HEAD* Header = reinterpret_cast<HEAD*>(const_cast<uint8*>(Data));
//     // void* DataPtr;

//     UE_LOG(LogTemp, Warning, TEXT("Header->Length: %d"), Header->Length); //////ntohs

//     if (Header->Length <= sizeof(HEAD)) // 패킷 길이 확인
//     {
//         UE_LOG(LogTemp, Error, TEXT("Invalid packet length"));
//         return nullptr;
//     }

//     //void*
//     // return DataPtr = reinterpret_cast<void*>(const_cast<uint8*>(Data) + sizeof(HEAD)); // 헤더 이후 데이터
//     return reinterpret_cast<void*>(const_cast<uint8*>(Data) + sizeof(HEAD));
// }
// void* FTCPSocketClient_Async::GetDataPtr(const uint8* Data) {
//     if (!Data) {
//         UE_LOG(LogTemp, Error, TEXT("Invalid Data Pointer"));
//         return nullptr;
//     }
//     HEAD* Header = reinterpret_cast<HEAD*>(const_cast<uint8*>(Data)); ////////

//     uint16 PacketLength = Header->Length; //ntohs
//     UE_LOG(LogTemp, Warning, TEXT("Header->Length: %d"), PacketLength);
//     UE_LOG(LogTemp, Warning, TEXT("sizeof(HEAD): %d"), sizeof(HEAD));

//     // if (PacketLength <= sizeof(HEAD)) { // <=
//     //     UE_LOG(LogTemp, Error, TEXT("Invalid packet length: %d"), PacketLength);
//     //     return nullptr;
//     // }
//     // return reinterpret_cast<void*>(const_cast<uint8*>(Data) + sizeof(HEAD));
//     // return reinterpret_cast<HEAD*>(const_cast<uint8*>(Data));
//     return reinterpret_cast<void*>(Header);
// }

void *FTCPSocketClient_Async::GetDataPtr(const uint8 *Data, int32 DataSize)
{
    if (!Data)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid Data Pointer"));
        return nullptr;
    }
    if (DataSize < sizeof(HEAD))
    {
        UE_LOG(LogTemp, Error, TEXT("Insufficient data size. Expected at least : %d bytes, Received: %d"), sizeof(HEAD), DataSize);
        return nullptr;
    }
    HEAD *Header = reinterpret_cast<HEAD *>(const_cast<uint8 *>(Data));
    UE_LOG(LogTemp, Warning, TEXT("Header Address: p, Header Length: %d"), Header, Header->Length);
    return Header;
}

///////////////////////////////////////////////////////////////

void FTCPSocketClient_Async::OnRecvCompleted(short Index, const TArray<uint8> &InPayload)
// void FTCPSocketClient_Async::OnRecvCompleted(short Index, HEAD* InPayload)
{

    GameMode = Cast<ATinySwordGameMode>(GI->GetWorld()->GetAuthGameMode());
    // if (Index > 0 && Index < 22 && PROTOCOLS[Index])
    //     (this->*PROTOCOLS[data->Command])(InPayload.GetData());

    // UE_LOG(LogTemp, Warning, TEXT("Entered in OnRecvCompleted: >>>%d<<<"), (PROTOCOL_IDS)Index);

    // const uint8* Data = InPayload.GetData();
    // int32 DataSize = InPayload.Num();
    // HEAD* Header = reinterpret_cast<HEAD*>(GetDataPtr(Data, DataSize));

    /////////// 왜 게임모드 null?
    if (!GameMode)
    {
        UE_LOG(LogTemp, Fatal, TEXT("[NULLPTR]GAMEMODE IS NULL (Async, OnRecvCompleted)"));
        return;
    }

    switch ((PROTOCOL_IDS)Index)
    {
    case GAMESTART_NOTIFICATION:
    {

        // GameMode->OpenPlayingLevel(); // -> 여기서 Unreal Crash...
        // UE_LOG(LogTemp, Warning, TEXT("[RECEIVED] >>>>>>>>>>>> GAME START <<<<<<<<<<<<<<"));

        // open level
        // UE_LOG(LogTemp, Warning, TEXT("Select Cnt: %d"), SelectCnt);
        if (GameMode)
        {
            GameMode->OpenPlayingLevel();
             
        }
            
        break;
    }
    case CHARACTERSELECT_RESPONSE:
    {
        // UE_LOG(LogTemp, Warning, TEXT("[RECEIVED] CHARACTERSELECT_RESPONSE"));

        break;
    }
    case CHARACTERSELECT_NOTIFICATION:
    {
        SelectCnt++;
        // UE_LOG(LogTemp, Warning, TEXT("[RECEIVED] CHARACTER_NOTIFICATION"));

        CharacterSelect::Notification *Noti =
            const_cast<CharacterSelect::Notification *>(reinterpret_cast<const CharacterSelect::Notification *>(InPayload.GetData()));

        if (Noti == nullptr)
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to parse noti from payload"));
            break;
        }

        /////////////////////
        // GameMode = Cast<ATinySwordGameMode>(GI->GetWorld()->GetAuthGameMode());
        if (!GameMode)
        {
            UE_LOG(LogTemp, Fatal, TEXT("[NULLPTR] GAME MODE IS NULL"));
        }

        GameMode->OnCharacterSelectNotification(Noti);
        /////////////////////////////////////////
        // GameMode->OnCharacterSelectNotification(reinterpret_cast<HEAD*>(GetDataPtr(InPayload.GetData()))); // 올바르게 변환하여 전달
        break;
    }

    case MOVE_RESPONSE:
    {
        // UE_LOG(LogTemp, Warning, TEXT("[RECEIVED] MOVE_RESPONSE"));
        // Move::Response Response;

        // FMemory::Memcpy(&Response, InPayload.GetData(), sizeof(Response));
        // if (Response.bIsSuccess)
        // {

        // }

        break;
    }

    case MOVE_NOTIFICATION:
    {
        // UE_LOG(LogTemp, Log, TEXT("[RECEIVED] MOVE_NOTIFICATION"));
        // 액터 찾아서 위치 이동
        // GameMode->OnMoveNotification(reinterpret_cast<HEAD*>(GetDataPtr(InPayload.GetData())));
        // GameMode->OnMoveNotification(InPayload);

        Move::Notification *Noti =
            const_cast<Move::Notification *>(reinterpret_cast<const Move::Notification *>(InPayload.GetData()));

        GameMode->OnMoveNotification(Noti);

        break;
    }

    case ATTACK_RESPONSE:
    {
        // Attack::Response Response;
        // FMemory::Memcpy(&Response, InPayload.GetData(), sizeof(Response));
        // UE_LOG(LogTemp, Log, TEXT("[RECEIVED] ATTACK_RESPONSE: %d"), Response.bSuccess);
        break;
    }
    case ATTACK_NOTIFICATION:
    {
        UE_LOG(LogTemp, Log, TEXT("[RECEIVED] ATTACK_NOTIFICATION"));
        // 애니메이션 재생
        // 위치 보간

        Attack::Notification *Noti =
            const_cast<Attack::Notification *>(reinterpret_cast<const Attack::Notification *>(InPayload.GetData()));
        GameMode->OnAttackNotification(Noti);
        break;
    }

    case SPAWN_NOTIFICATION:
    {
        UE_LOG(LogTemp, Log, TEXT("[RECEIVED] SPAWN_NOTIFICATION"));
        // 위치에 액터 스폰
        // GameMode->OnSpawnNotification(InPayload); //////////////////

        Spawn::Notification *Noti =
            const_cast<Spawn::Notification *>(reinterpret_cast<const Spawn::Notification *>(InPayload.GetData()));
        GameMode->OnSpawnNotification(Noti);

        // GameMode->SaveAllPathsToTextFile();

        break;
    }

    case GETITEM_NOTIFICATION:
    {
        // 위치 보간
        // 아이템 액터 destroy
        UE_LOG(LogTemp, Log, TEXT("[RECEIVED] GETITEM_NOTIFICATION"));
        // GameMode->OnGetItemNotification(InPayload);

        GetItem::Notification *Noti =
            const_cast<GetItem::Notification *>(reinterpret_cast<const GetItem::Notification *>(InPayload.GetData()));
        GameMode->OnGetItemNotification(Noti);

        break;
    }

    case DEAD_NOTIFICATION:
    {
        // CASTLE, GOLDMINE : collapse 스프라이트 변경

        // BOMB, SHEEP : destroy

        // GOBLIN : Animation 재생
        UE_LOG(LogTemp, Log, TEXT("[RECEIVED] DEAD_NOTIFICATION"));
        Dead::Notification *Noti =
            const_cast<Dead::Notification *>(reinterpret_cast<const Dead::Notification *>(InPayload.GetData()));
        GameMode->OnDeadNotification(Noti);

        break;
    }

    case BOMBEXPLODE_NOTIFICATION:
    {
        UE_LOG(LogTemp, Log, TEXT("[RECEIVED] BOMBEXPLODE_NOTIFICATION"));
        // GameMode->OnBombExpNotification(reinterpret_cast<HEAD*>(GetDataPtr(InPayload.GetData())));

        BombExplode::Notification *Noti =
            const_cast<BombExplode::Notification *>(reinterpret_cast<const BombExplode::Notification *>(InPayload.GetData()));
        GameMode->OnBombExpNotification(Noti);
        break;
    }
    }

    // TArray<uint8> Payload;
    // Payload.Append(InPayload);

    // FString Data(Payload.Num(), (char*)Payload.GetData());
    // UE_LOG(LogTemp, Log, TEXT("OnRecvCompleted recv data success. data : %s Payload : %d size : %d"), *Data, Payload.Num(), Data.Len());

    // CUSTOM
    // Ex. Attack::Notification
    // if (InPayload.Num() < sizeof(Attack::Notification))
    // {
    //     UE_LOG(LogTemp, Error, TEXT("Payload size is too small"));
    //     return;
    // }

    // // Read Data
    // const Attack::Notification* noti = reinterpret_cast<const Attack::Notification*>(InPayload.GetData());
    // UE_LOG(LogTemp, Log, TEXT("Received Attack::Notification"));
    // UE_LOG(LogTemp, Log, TEXT("AttackerActorType: %d"), noti->AttackerType);

    /*struct HEAD* data = reinterpret_cast<struct HEAD*>(InPayload.GetData());
    if (data->Command > 0 && data->Command < 22 && PROTOCOLS[data->Command])
    {
        (this->*PROTOCOLS[data->Command])(data);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid Protocol Command: %d"), data->Command);
    }*/
}

void FTCPSocketClient_Async::OnRecvFailed()
{
    FString OutText;
    FTCPSocketClientUtils::PrintSocketError(OutText);
    UE_LOG(LogTemp, Log, TEXT("OnRecvFailed Error : %s"), *OutText);
}

HEAD FTCPSocketClient_Async::ByteArrayToStruct(const TArray<uint8> &ByteArray)
{
    HEAD StructData;
    if (ByteArray.Num() == sizeof(HEAD))
    {
        FMemory::Memcpy(&StructData, ByteArray.GetData(), sizeof(HEAD));
    }
    return StructData;
}

