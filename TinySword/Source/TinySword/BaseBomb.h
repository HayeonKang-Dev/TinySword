// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "BombAIController.h"
#include "TinySwordGameMode.h"
#include "PaperFlipbookComponent.h"
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

	float GetSpeed() const {return Speed;}

	void SetTagId(int32 newTagId);

	int32 GetOwnerTagId() const {return OwnerTagId;}
	void SetOwnerTagId(int32 newOwnerTagId);

	void PlayBrinkAnim();
	void PlayExplodeAnim();

	void AddToReuseId(int32 tagId);


protected:
	virtual void BeginPlay() override; 

	FTimerHandle BombExplosionTimerHandle; 

	float ElapsedTime = 0.0f; 

	UPaperFlipbookComponent* paperFlipbookComponent;

private:
	UPROPERTY(VisibleAnywhere, Category="State")
	float Health; 

	float MaxHealth = 70.0f;

	UPROPERTY(VisibleAnywhere, Category="State")
	float Speed = 80.0f; 

	UPROPERTY(VisibleAnywhere, Category="State")
	float Damage; 

	UPROPERTY(VisibleAnywhere, Category="State")
	int32 TagId;

	UPROPERTY(EditAnywhere, Category="Animation")
	UPaperFlipbook* BrinkAnim; 
	
	UPROPERTY(EditAnywhere, Category="Animation")
	UPaperFlipbook* ExplodeAnim;

	ABombAIController* bombController; 

	ATinySwordGameMode* GameMode;

	int32 OwnerTagId; 	

	void SendAttackResponseMsg(); 
	void SendAttackNotiMsg(int attackerType, int attackerIndex, int targetType, int targetIndex, int damage, int targetHp, float X, float Y);
};
