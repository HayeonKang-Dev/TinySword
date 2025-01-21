// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DamageEvents.h"
#include "PaperZDCharacter.h"
#include "PaperFlipbookComponent.h"
#include "Goblin.generated.h"

/**
 * 
 */
UCLASS()
class TINYSWORD_API AGoblin : public APaperZDCharacter
{
	GENERATED_BODY()
	
public:
	// override
	virtual void Tick(float DeltaTime) override; 

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	// Control variable
	void IncreaseMoney(float Amount);

	void IncreaseHealth(float Amount);

	bool DecreaseMoney(float Amount);

	// variable
	UPROPERTY(EditDefaultsOnly)
	float MaxHealth = 100.0f; 

	UPROPERTY(EditAnywhere, Category="State")
	float Health;

	UFUNCTION(BlueprintPure)
	bool IsDead() const;

	UPROPERTY(VisibleAnywhere, Category="State")
	float Money; 

	UPROPERTY(EditAnywhere, Category="Combat")
	bool IsAttack = false; 

	UPROPERTY(EditAnywhere, Category="TagId")
	int TagId = 0; 

	int32 GetTagId() const {return TagId;}
	
	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* OverlapComponent;

	// for UI 
	UFUNCTION(BlueprintCallable)
	float GetHealth() const {return Health;}

	UFUNCTION(BlueprintPure)
	float GetHealthPercent() const; 

	UFUNCTION(BlueprintPure)
	int GetMoneyCount() const; 


	// Actions
	UFUNCTION()
	void Attack();

	void MoveRight(float Value);
	void UpDown(float Value);

	void HandleDeath();

protected:
	virtual void BeginPlay() override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

	UPROPERTY(EditAnywhere)
	float Damage = 10; 

	UPaperFlipbookComponent* paperFlipbookComponent;


private:
	void FlipCharacter(bool MoveDirec);

	// ATinySwordGameModeBase* GameMode; 

	float Timer; 

};
