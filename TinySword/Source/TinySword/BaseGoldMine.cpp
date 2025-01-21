// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGoldMine.h"

// Sets default values
ABaseGoldMine::ABaseGoldMine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}


// Called when the game starts or when spawned
void ABaseGoldMine::BeginPlay()
{
	Super::BeginPlay();

	Durability = MaxDurability;
	
}



// Called every frame
void ABaseGoldMine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float ABaseGoldMine::TakeDamage(float DamageAmount, FDamageEvent const &DamageEvent, AController *EventInstigator, AActor *DamageCauser)
{
	DamageAmount = FMath::Min(Durability, DamageAmount);
	Durability -= DamageAmount;
	if (Durability > 0) DropGoldBag();

    return DamageAmount;
}


void ABaseGoldMine::DropGoldBag()
{
	int RandomValue = FMath::RandRange(0, 1);
	if (RandomValue == 0)
	{
		// goldbag drop logic
	}
}



bool ABaseGoldMine::IsCollapse()
{
    return Durability <= 0;
}
