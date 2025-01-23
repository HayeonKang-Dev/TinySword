// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/CapsuleComponent.h"
#include "PaperSpriteComponent.h" 
#include "PaperZDAnimationComponent.h"
#include "BaseMeat.generated.h"

UCLASS()
class TINYSWORD_API ABaseMeat : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseMeat();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	int32 GetTagId() const {return TagId;}
	
	void SetTagId(int32 NewTagId);

private:
	int32 TagId;

	UPROPERTY(EditAnywhere)
	UCapsuleComponent* collision; 

	UPROPERTY(EditAnywhere)
	UPaperSpriteComponent* paperSprite; 

	UPROPERTY(EditAnywhere)
	UPaperZDAnimationComponent* animationComponent; 
	
};
