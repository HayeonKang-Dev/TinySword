// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseMeat.h"

// Sets default values
ABaseMeat::ABaseMeat()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    RootComponent = SceneRoot;

	collision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("collision")); 
	collision->SetupAttachment(RootComponent);

	paperSprite = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("PaperSprite")); 
	paperSprite->SetupAttachment(RootComponent);



}

// Called when the game starts or when spawned
void ABaseMeat::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(PickupEnableTimerHandle, this, &ABaseMeat::EnablePickup, 0.5f, false); 
	PlaySpawnAnim();
}

// Called every frame
void ABaseMeat::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseMeat::SetTagId(int32 NewTagId)
{
	TagId = NewTagId;
}

void ABaseMeat::PlaySpawnAnim()
{
	if (paperSprite && SpawnAnim)
	{
		paperSprite->SetLooping(false);
		paperSprite->SetFlipbook(SpawnAnim);
		paperSprite->Play();
	}
}
