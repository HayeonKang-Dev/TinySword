// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Goblin.h"
#include "AsyncNetworking.h"
#include "TinySwordGameInstance.generated.h"

/**
 * 
 */

 class FTCPSocketClient_Async;

UCLASS()
class TINYSWORD_API UTinySwordGameInstance : public UGameInstance
{
	GENERATED_BODY()

public: 
	virtual void Init() override;

	int32 GetTagId() const {return TagId; }
	void SetTagId(int32 newTagId) {TagId = newTagId;}

	AGoblin* GetChar() const {return controlledChar;}
	void SetChar(AGoblin* newChar) {controlledChar = newChar;}

	// 싱글톤 접근 함수
    static UTinySwordGameInstance* GetInstance(UWorld* World);

    // TCPClient에 대한 접근자
    TSharedPtr<FTCPSocketClient_Async> GetTCPClient()
    {
        return TCPClient;
    }

	

private:
	int32 TagId; 

	AGoblin* controlledChar; 

	TSharedPtr<FTCPSocketClient_Async> TCPClient;

    bool bIsConnect = false; 

	int cnt = 0;

};
