// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "BombAIController.h"
#include "TinySwordGameMode.h"
#include "BaseBomb.generated.h"

/**
 * 
 */
// class ABombAIController;
// class ATinySwordGameMode;


UCLASS()
class TINYSWORD_API ABaseBomb : public APaperCharacter
{
	GENERATED_BODY()

public: 
	bool IsDead() const; 

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void Tick(float DeltaTime) override;

	void DealRadialDamage();

	float GetHealth() const {return Health;}

	int32 GetTagId() const {return TagId;}

	void SetTagId(int32 newTagId);

protected:
	virtual void BeginPlay() override; 

	FTimerHandle BombExplosionTimerHandle; 

	float ElapsedTime = 0.0f; 

private:
	UPROPERTY(VisibleAnywhere, Category="State")
	float Health; 

	float MaxHealth = 70.0f;

	UPROPERTY(VisibleAnywhere, Category="State")
	float Speed = 50.0f; 

	UPROPERTY(VisibleAnywhere, Category="State")
	float Damage; 

	UPROPERTY(VisibleAnywhere, Category="State")
	int32 TagId;

	ABombAIController* bombController; 

	ATinySwordGameMode* GameMode;
};
