// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PaperFlipbookComponent.h"
#include "BaseCastle.generated.h"


class UBoxComponent; 
class UPaperFlipbookComponent; 
class USceneComponent;

UCLASS()
class TINYSWORD_API ABaseCastle : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseCastle();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void OnCollapse();

	UPROPERTY(EditAnywhere, Category="Components")
	USceneComponent* RootSceneComponent; 

	UPROPERTY(EditAnywhere, Category="Components")
	UPaperFlipbookComponent* PaperFlipbookComponent; 

	UPROPERTY(EditAnywhere, Category="Components")
	UBoxComponent* BoxCollider1; 

	UPROPERTY(EditAnywhere, Category="Components")
	UBoxComponent* BoxCollider2; 

	UPROPERTY(EditAnywhere, Category="Animation")
	UPaperFlipbook* CollapseFlipbook;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	int32 GetTagId() const {return TagId;}

	float GetDurability() const {return Durability;}

	bool IsCollapse();

	
private:
	UPROPERTY(VisibleAnywhere)
	float Durability; 

	float MaxDurability = 100.0f; 
	
	UPROPERTY(EditAnywhere, Category="TagId")
	int TagId = 0; 
};
