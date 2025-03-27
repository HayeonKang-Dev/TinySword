// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "PaperFlipbookComponent.h"
#include "BaseAISheep.generated.h"

/**
 * 
 */
class ATinySwordGameMode;

UCLASS()
class TINYSWORD_API ABaseAISheep : public APaperCharacter
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void TakeDamageFrom(AActor* Attacker);

	bool IsDead() const;

	void SpawnMeat(); 

	int32 GetTagId() const {return TagId;}

	float GetHealth() const {return Health;}
	void SetHealth(float newHealth) {Health = newHealth;}

	void DecreaseHealth(float damage) { Health = FMath::Max(0, Health-damage);}

protected:
	virtual void BeginPlay() override;

	FTimerHandle SpeedBoostTimerHandle; 

	void ResetSpeed(); 

	UPaperFlipbookComponent* paperFlipbookComponent;

private:
	void MoveRight(float AxisValue); 


	ATinySwordGameMode* GameMode;

	UPROPERTY(EditAnywhere, Category="Speed")
	float BoostedSpeed = 140.0f; 

	UPROPERTY(EditAnywhere, Category="Speed")
	float NormalSpeed = 30.0f; 

	UPROPERTY(EditAnywhere, Category="Speed")
	float SpeedBoostDuration = 3.f; 


	UPROPERTY(EditAnywhere, Category="Animation")
	UPaperFlipbook* IdleAnim; 
	
	UPROPERTY(EditAnywhere, Category="Animation")
	UPaperFlipbook* MoveAnim;

	UPROPERTY(EditAnywhere, Category="Animation")
	UPaperFlipbook* DeadAnim;

	UPROPERTY(EditAnywhere, Category="Animation")
	UPaperFlipbook* DeadDispAnim;

	UPROPERTY(EditAnywhere, Category="State")
	int32 TagId; 

	UPROPERTY(EditAnyWhere, Category="State")
	float Health; 

	float MaxHealth = 100; 

	void FlipCharacter(float MoveDirec);

	void UpdateAnimation();
	// void HandleDeath(); 

	// void PlayDeadAnim(); 
	// void PlayDeadDispAnim();

	float Timer = 0.0f; 

	/*void SendMoveResponseMsg(int ActorType, int ActorIndex, float Speed); 
	void SendMoveNotiMsg(int actorType, int actorIndex, float X, float Y);

	void SendSpawnResponseMsg(FVector spawnLocation, int spawnActorIndex, int spawnType); 
	void SendSpawnNotiMsg(int spawnType, int spawnActorIndex, float X, float Y);

	void SendDestroyResponseMsg(int actorType, int actorIndex, float X, float Y); 
	void SendDestroyNotiMsg(int actorType, int actorIndex, float X, float Y);*/
};
