// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PaperSpriteComponent.h" 
#include "BaseGoldMine.generated.h"


class UBoxComponent; 
class UPaperFlipbookComponent; 
class USceneComponent;
class ATinySwordGameMode;

UCLASS()
class TINYSWORD_API ABaseGoldMine : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseGoldMine();

	bool IsCollapse();

	int32 GetTagId() const {return TagId;}

	float GetDurability() const {return Durability;}

	UPROPERTY(EditAnywhere, Category="Sprite")
	UPaperSprite* collapseSprite; 


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void DropGoldBag();

	UPROPERTY(EditAnywhere, Category="Components")
	USceneComponent* RootSceneComponent; 

	UPROPERTY(EditAnywhere)
	UPaperSpriteComponent* paperSprite; 

	UPROPERTY(EditAnywhere, Category="Components")
	UBoxComponent* BoxCollider1; 

	UPROPERTY(EditAnywhere, Category="Components")
	UBoxComponent* BoxCollider2; 

	void UpdateSprite();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;


private:
	UPROPERTY(VisibleAnywhere, Category="State")
	float Durability; 

	float MaxDurability = 100.0f; 

	UPROPERTY(EditAnywhere, Category="TagId")
	int32 TagId; 

	ATinySwordGameMode* GameMode; 

	void SendSpawnResponseMsg(); 
	void SendSpawnNotiMsg(int spawnType, int spawnActorIndex, float X, float Y);
};
