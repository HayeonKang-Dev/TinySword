// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TinySwordGameMode.h"
#include "PaperSpriteActor.h"
#include "Components/CapsuleComponent.h"
#include "PaperSpriteComponent.h" 
#include "PaperFlipbookComponent.h"
#include "BaseGoldBag.generated.h"



UCLASS()
class TINYSWORD_API ABaseGoldBag : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseGoldBag();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	int32 GetTagId() const {return TagId;}

	

private:
	UPROPERTY(VisibleAnywhere)
	int32 TagId; 

	void SetGoldBagId();

	ATinySwordGameMode* GameMode;


	UPROPERTY(EditAnywhere)
	UCapsuleComponent* collision; 

	UPROPERTY(EditAnywhere)
	UPaperFlipbookComponent* paperSprite; 

	// UPROPERTY(EditAnywhere)
	// UPaperZDAnimationComponent* animationComponent; 

	UPROPERTY(EditAnywhere, Category="Animation")
	UPaperFlipbook* SpawnAnim;
	
	void PlaySpawnAnim();
};
