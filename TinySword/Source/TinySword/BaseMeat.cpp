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

	paperSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("paperSprite")); 
	paperSprite->SetupAttachment(RootComponent);

	animationComponent =CreateDefaultSubobject<UPaperZDAnimationComponent>(TEXT("Animation")); 

}

// Called when the game starts or when spawned
void ABaseMeat::BeginPlay()
{
	Super::BeginPlay();
	
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
