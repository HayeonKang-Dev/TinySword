// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGoldBag.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ABaseGoldBag::ABaseGoldBag()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    RootComponent = SceneRoot;

	collision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("collision")); 
	collision->SetupAttachment(RootComponent);

	paperSprite = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("paperSprite")); 
	paperSprite->SetupAttachment(RootComponent);

	// animationComponent =CreateDefaultSubobject<UPaperZDAnimationComponent>(TEXT("Animation")); 
	

}

// Called when the game starts or when spawned
void ABaseGoldBag::BeginPlay()
{
	Super::BeginPlay();

	PlaySpawnAnim();
	
	GameMode = Cast<ATinySwordGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	SetGoldBagId();
}

// Called every frame
void ABaseGoldBag::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseGoldBag::SetGoldBagId()
{
	int32 id = 0; 
	if (!GameMode->ReuseGoldBagId.IsEmpty()) GameMode->ReuseGoldBagId.Dequeue(id);
	else id = GameMode->ActiveGoldBagId.Num() + 1; 

	TagId = id; 
	
	GameMode->ActiveGoldBagId.Add(this, TagId);
}

void ABaseGoldBag::PlaySpawnAnim()
{
	if (paperSprite && SpawnAnim)
	{
		paperSprite->SetLooping(false);
		paperSprite->SetFlipbook(SpawnAnim);
		paperSprite->Play();
	}
}
