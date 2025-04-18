// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DamageEvents.h"
#include "PaperZDCharacter.h"
#include "PaperFlipbookComponent.h"
#include "TinySwordPlayerController.h"
#include "AsyncNetworking.h"
#include "Serialization/ArrayWriter.h"
#include "protocol.h"
#include "Goblin.generated.h"

/**
 * 
 */
class ATinySwordGameMode;


UCLASS()
class TINYSWORD_API AGoblin : public APaperZDCharacter
{
	GENERATED_BODY()
	
public:
	AGoblin(); 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UCharacterMovementComponent* GoblinMovement;

	// override
	virtual void Tick(float DeltaTime) override; 

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	// Control variable
	void IncreaseMoney(float Amount);

	void IncreaseHealth(float Amount);

	bool DecreaseMoney(float Amount);

	void DecreaseHealth(float Amount);// { Health = FMath::Max(0, Health-Amount);}
	
	// variable
	UPROPERTY(EditDefaultsOnly)
	float MaxHealth = 100.0f; 

	UPROPERTY(EditAnywhere, Category="State")
	float Health;

	UFUNCTION(BlueprintPure)
	bool IsDead() const;

	UPROPERTY(VisibleAnywhere, Category="State")
	float Money = 0; 

	UPROPERTY(EditAnywhere, Category="Combat")
	bool IsAttack = false; 

	UPROPERTY(EditAnywhere, Category="TagId")
	int TagId = 0; 

	int32 GetTagId() const {return TagId;}
	void SetTagId(int32 newTagId);
	
	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* OverlapComponent;

	// ATinySwordPlayerController* GetPlayerController(); //  { return playerController; }

	// for UI 
	UFUNCTION(BlueprintCallable)
	float GetHealth() const {return Health;}
	void SetHealth(float newHealth) { Health = newHealth;}


	UFUNCTION(BlueprintPure)
	float GetHealthPercent() const; 

	UFUNCTION(BlueprintPure)
	int GetMoneyCount() const; 
	void SetMoney(int newMoney) {Money = newMoney;}

	void UpdateMoneyCount_(int money);
	

	void UpdateAnimation();
	void PlayAttackAnimation();

	
	// Actions
	UFUNCTION()
	void Attack();

	void MoveRight(float Value);
	void UpDown(float Value);
	// FVector LastRecvLocation;

	void HandleDeath();

	void SetPlayerController(ATinySwordPlayerController* newPlayerController);

	// bool bIsMoving; // Is Move::Notification AVAILABLE
	

	// void SetMoveDir(FVector2D &newMoveDir) { MoveDir = newMoveDir; }

	FVector LastTargetLocation; 
	bool bIsMovingToTarget= false; 

	void FlipCharacter(int MoveDirec);
	
	// void SetLocation(const FVector &newLocation) { SetActorLocation(newLocation);}
	/*void SendMoveResponseMsg(int ActorType, int ActorIndex, bool bMoveUp, bool bMoveDown, bool bMoveRight, bool bMoveLeft); 
	void SendMoveNotiMsg(int actorType, int actorIndex, float X, float Y);*/


///////////////////////////////////////
	void ShowLoseWidget(); 
	void ShowWinWidget(); 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
	TSubclassOf<UUserWidget> LoseWidgetClass; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
	TSubclassOf<UUserWidget> WinWidgetClass; 

	bool IsMyChar(); 

protected:
	virtual void BeginPlay() override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

	UPROPERTY(EditAnywhere)
	float Damage = 10; 

	UPaperFlipbookComponent* paperFlipbookComponent;

	FVector2D MoveDir; 

private:
	UPROPERTY(EditAnywhere)
	float Speed = 100.0f; // 80

	

	void PlayDeadAnim();

	float Timer; 

	ATinySwordGameMode* GameMode; 
	UTinySwordGameInstance* GI;

	UPROPERTY(EditAnywhere, Category="Animation")
	UPaperFlipbook* IdleAnim; 
	
	UPROPERTY(EditAnywhere, Category="Animation")
	UPaperFlipbook* WalkAnim;

	UPROPERTY(EditAnywhere, Category="Animation")
	UPaperFlipbook* AttackAnim;

	UPROPERTY(EditAnywhere, Category="Animation")
	UPaperFlipbook* DeadAnim;

	FTimerHandle TimerHandleAttack;


	bool bIsAttacking; 
	

	

	void ResetToIdle();

	void SendMoveRequestMsg(short ActorTagId, bool bMoveUp, bool bMoveDown, bool bMoveRight, bool bMoveLeft);
	void SendAttackRequestMsg(short ActorTagId, ActorType TargetActorType, short TargetTagId, Vector TargetLocation, Vector AttackLocation, int damage);
	void SendGetItemRequestMsg(ActorType ItemType, short ItemTagId);

	ATinySwordPlayerController* playerController;

	

	/*void SendAttackResponseMsg(int attackerType, int attackerIndex, int targetType, int targetIndex, int damage); 
	void SendAttackNotiMsg(int attackerType, int attackerIndex, int targetType, int targetIndex, int damage, int targetHp, float X, float Y);

	void SendGetItemResponseMsg(int itemType); 
	void SendGetItemNotiMsg(int itemType, int itemIndex, float X, float Y);

	void SendDestroyResponseMsg(int actorType, int actorIndex, float X, float Y); 
	void SendDestroyNotiMsg(int actorType, int actorIndex, float X, float Y);*/
 
};
