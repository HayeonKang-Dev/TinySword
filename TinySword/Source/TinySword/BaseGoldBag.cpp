// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGoldBag.h"

// Sets default values
ABaseGoldBag::ABaseGoldBag()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABaseGoldBag::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseGoldBag::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

