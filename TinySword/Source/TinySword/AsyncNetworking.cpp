// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
// #include "AsyncNetworking.h"

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


/// @brief 소켓 연결 확인
/// @param Socket 
/// @return 
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

/// @brief GameInstance, GameMode 할당 
/// @param world AsyncNetworking 코드에서는 world 참조가 불가능하므로, 파라미터로 받아 저장해둘 것
void FTCPSocketClient_Async::Initialize(UWorld *world)
{
    // IP, Port 할당
    FString IPAddress = TEXT("127.0.0.1");
    uint16 PortNumber = 25000;

    // GameInstance, GameMode, World 할당 
    GI = UTinySwordGameInstance::GetInstance(world);
    GameMode = Cast<ATinySwordGameMode>(world->GetAuthGameMode());
    World = world;

    if (!GameMode)
    {
        UE_LOG(LogTemp, Error, TEXT("[NULLPTR] GAMEMODE IS NULL"));
    }

    // OpenWorld 시 GameMode가 초기화되므로, GameInstance에 네트워크 객체 저장해 관리
    // 레벨 변경 후 GameInstance에 저장했던 네트워크 객체 가져와 재할당
    if (GI)
    {
        // 네트워크 객체, 소켓 할당
        TCPClient = GI->GetTCPClient();
        TSharedPtr<FSocket> socket = TSharedPtr<FSocket>(FTcpSocketBuilder(TEXT("ClientSocket")).Build()); // 스마트 포인터로 소켓 관리 
        if (!socket.IsValid())
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to Create the SOCKET"));
            return;
        }

        // Nagle 비활성화 : 패킷 즉시 전송하도록 소켓 세팅 변경
        socket->SetNoDelay(true);
        if (TCPClient)
        {
            TCPClient->SetSocket(socket);
            TCPClient->GetSocket()->SetNonBlocking(true); // 비동기 설정
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
    // 새로운 패킷 버퍼 생성
    TSharedPtr<FBufferArchive> Packet = MakeShareable(new FBufferArchive());

    // 헤더 생성:
    // MessageType는 전달받은 메시지 타입,
    // Size는 payload의 크기 (unsigned short로 캐스팅; 헤더 크기 4바이트를 포함해서 계산)
    FMessageHeader Header(MessageType, static_cast<unsigned short>(PayloadSize));

    // 헤더 직렬화 (operator<<를 통해 HEAD 형식으로 기록됨)
    *Packet << Header;

    // UE_LOG(LogTemp, Warning, TEXT("Before Serialize"));
    // 이어서 payload를 버퍼에 기록
    Packet->Serialize(const_cast<void *>(PayloadData), PayloadSize);

    return Packet;
}

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
    TSharedPtr<FBufferArchive> Packet = CreatePacket(static_cast<short>(Type), reinterpret_cast<const void *>(Convert.Get()), PayloadSize);

    return Packet;
}

void FTCPSocketClient_Async::BeginSendPhase(TSharedPtr<FBufferArchive> Packet)
{


    if (!TCPClient.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("TCPClient is NULL"));
        return;
    }

    if (!TCPClient->GetSocket().IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("Socket is NULL"));
        return;
    }

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
}

void FTCPSocketClient_Async::EndSendPhase()
{
    // UE_LOG(LogTemp, Log, TEXT("EndSendPhase"));
    // BeginRecvPhase();
}

void FTCPSocketClient_Async::BeginRecvPhase()
{

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

        // 1. RECV "header"
        bool bSuccessRecvHeader = false; 
        int32 NumRead = 0; 

        bSuccessRecvHeader = LocalTCPClient->GetSocket().Get()->Recv( // Socket
                HeaderBuffer.GetData(), HeaderBuffer.Num(), NumRead, ESocketReceiveFlags::Type::None); // WaitAll
        
        // UE_LOG(LogTemp, Warning, TEXT("******************** bSuccessRecvHeader: %s"), bSuccessRecvHeader ? TEXT("true") : TEXT("false"));
        
        int32 ErrorCode = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLastErrorCode();
        if (ErrorCode == SE_EWOULDBLOCK) return; 

        
        if (bSuccessRecvHeader)
        {
            // 2. RECV "payload"
            HEAD Header;
            FMemory::Memcpy(&Header, HeaderBuffer.GetData(), sizeof(Header));
            // 이 시점에 game mode null 


            int32 PayloadSize = Header.Length - 4; // sizeof(Size) + sizeof(Type) = 4 

            if (PayloadSize < 0)
            {
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
                if (!LocalTCPClient->GetSocket().IsValid()) return;
                if (bSuccessRecvPayload)
                {
                    OnRecvCompleted(Header.Index, Payload);
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
    // BeginSendPhase(); 
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
{

    GameMode = Cast<ATinySwordGameMode>(GI->GetWorld()->GetAuthGameMode());
    if (!GameMode)
    {
        UE_LOG(LogTemp, Fatal, TEXT("[NULLPTR]GAMEMODE IS NULL (Async, OnRecvCompleted)"));
        return;
    }

    switch ((PROTOCOL_IDS)Index)
    {
    case GAMESTART_NOTIFICATION:
    {
        if (GameMode)
        {
            GameMode->OpenPlayingLevel();
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("GAME MODE IS NULL (ASYNC)"));
        }
            
        break;
    }
    // case CHARACTERSELECT_RESPONSE:
    // {
    //     // UE_LOG(LogTemp, Warning, TEXT("[RECEIVED] CHARACTERSELECT_RESPONSE"));

    //     break;
    // }
    case CHARACTERSELECT_NOTIFICATION:
    {
        SelectCnt++;

        CharacterSelect::Notification *Noti =
            const_cast<CharacterSelect::Notification *>(reinterpret_cast<const CharacterSelect::Notification *>(InPayload.GetData()));

        if (Noti == nullptr)
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to parse noti from payload"));
            break;
        }

        if (!GameMode)
        {
            UE_LOG(LogTemp, Fatal, TEXT("[NULLPTR] GAME MODE IS NULL"));
        }

        GameMode->OnCharacterSelectNotification(Noti);
        break;
    }

    // case MOVE_RESPONSE:
    // {
    //     // UE_LOG(LogTemp, Warning, TEXT("[RECEIVED] MOVE_RESPONSE"));
    //     // Move::Response Response;

    //     // FMemory::Memcpy(&Response, InPayload.GetData(), sizeof(Response));
    //     // if (Response.bIsSuccess)
    //     // {

    //     // }

    //     break;
    // }

    case MOVE_NOTIFICATION:
    {
        Move::Notification *Noti =
            const_cast<Move::Notification *>(reinterpret_cast<const Move::Notification *>(InPayload.GetData()));

        GameMode->OnMoveNotification(Noti);

        break;
    }

    // case ATTACK_RESPONSE:
    // {
    //     // Attack::Response Response;
    //     // FMemory::Memcpy(&Response, InPayload.GetData(), sizeof(Response));
    //     // UE_LOG(LogTemp, Log, TEXT("[RECEIVED] ATTACK_RESPONSE: %d"), Response.bSuccess);
    //     break;
    // }
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

        Spawn::Notification *Noti =
            const_cast<Spawn::Notification *>(reinterpret_cast<const Spawn::Notification *>(InPayload.GetData()));
        GameMode->OnSpawnNotification(Noti);


        break;
    }

    case GETITEM_NOTIFICATION:
    {
        UE_LOG(LogTemp, Log, TEXT("[RECEIVED] GETITEM_NOTIFICATION"));

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

        BombExplode::Notification *Noti =
            const_cast<BombExplode::Notification *>(reinterpret_cast<const BombExplode::Notification *>(InPayload.GetData()));
        GameMode->OnBombExpNotification(Noti);
        break;
    }

    case WIN_NOTIFICATION: 
    {
        UE_LOG(LogTemp, Log, TEXT("[RECEIVED] WIN_NOTIFICATION"));
        Win::Notification *Noti =
            const_cast<Win::Notification *>(reinterpret_cast<const Win::Notification *>(InPayload.GetData()));
        GameMode->OnWinNotification(Noti);
        break;
    }

    case LOSE_NOTIFICATION: 
    {
        UE_LOG(LogTemp, Log, TEXT("[RECEIVED] LOSE_NOTIFICATION"));
        Lose::Notification *Noti =
            const_cast<Lose::Notification *>(reinterpret_cast<const Lose::Notification *>(InPayload.GetData()));
        GameMode->OnLoseNotification(Noti);
        break;
    }
    }
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

