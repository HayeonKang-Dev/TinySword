// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
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
	// virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void TakeDamageFrom(AActor* Attacker);

	bool IsDead() const;

	void SpawnMeat(); 

	int32 GetTagId() const {return TagId;}

protected:
	virtual void BeginPlay() override;

	FTimerHandle SpeedBoostTimerHandle; 

	void ResetSpeed(); 

private:
	void MoveRight(float AxisValue); 

	ATinySwordGameMode* GameMode;

	float BoostedSpeed = 100.0f; 

	float NormalSpeed = 30.0f; 

	float SpeedBoostDuration = 3.f; 

	int32 TagId; 

	float Health; 

	float MaxHealth = 100; 
};
