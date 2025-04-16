// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetworkMessage.h"
#include "TCPSocketClientUtils.h"
#include "protocol.h"
#include "TinySwordGameInstance.h"
// #include <winsock2.h>


class ATinySwordGameMode; 

////////// 
class TINYSWORD_API FTCPSocketClient_Async 
{
public:
	// FTCPSocketClient_Async(UMyGameInstance* InGameInstance);

	// static TSharedPtr<FBufferArchive> CreatePacket(uint32 InType, const uint8* InPayload, int32 InPayloadSize);
	static TSharedPtr<FBufferArchive> CreatePacket(short MessageType, const void* PayloadData, int32 PayloadSize);
	

	void Connect();
	void Disconnect();

	TSharedPtr<FBufferArchive> CreatePacket(uint32 Type, const FString& Text);

	void Initialize(UWorld* world);

// private:
	// all phase functions will be called at GameThread
	void BeginSendPhase(TSharedPtr<FBufferArchive> Packet);
	void EndSendPhase();

	void BeginRecvPhase();
	void EndRecvPhase();


	void OnSendCompleted();
	void OnSendFailed();

	void OnRecvCompleted(short Index, const TArray<uint8>& InPayload);
	// void OnRecvCompleted(short Index, HEAD* InPayload);
	void OnRecvFailed();

	// Serialize, DeSerialize
	// TArray<uint8> StructToByteArray(const FMyStruct& StructData);
	HEAD ByteArrayToStruct(const TArray<uint8>& ByteArray);

	void SetGameInstance(UTinySwordGameInstance* gameInstance) { GI = gameInstance;};

	TSharedPtr<FSocket> GetSocket() { return Socket; }
	void SetSocket(TSharedPtr<FSocket> InSocket) { Socket = InSocket; }

	void* GetDataPtr(const uint8* Data, int32 DataSize);

private:
	// FSocket* Socket;
	TSharedPtr<FSocket> Socket; 

	UTinySwordGameInstance* GI;

	TSharedPtr<FTCPSocketClient_Async> TCPClient;

	ATinySwordGameMode* GameMode; 

	UWorld* World;
	
	short SelectCnt = 0;

};


