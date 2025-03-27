// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGoldMine.h"
#include "BaseGoldBag.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "protocol.h"
#include "TinySwordGameMode.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABaseGoldMine::ABaseGoldMine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
    RootComponent = RootSceneComponent;

	paperSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("paperSprite")); 
	paperSprite->SetupAttachment(RootComponent);

	BoxCollider1 = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider1"));
	BoxCollider1->SetupAttachment(RootComponent);
	
	BoxCollider2 = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider2"));
	BoxCollider2->SetupAttachment(RootComponent);


}


// Called when the game starts or when spawned
void ABaseGoldMine::BeginPlay()
{
	Super::BeginPlay();

	Durability = MaxDurability;
	GameMode = Cast<ATinySwordGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
}


// Called every frame
void ABaseGoldMine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsCollapse()) 
	{
		UpdateSprite();
	}
	

}

float ABaseGoldMine::TakeDamage(float DamageAmount, FDamageEvent const &DamageEvent, AController *EventInstigator, AActor *DamageCauser)
{
	DamageAmount = FMath::Min(Durability, DamageAmount);
	// Durability -= DamageAmount;
	if (Durability > 0) 
	{
		// SendSpawnResponseMsg(TagId, 3, GoldBagSpawnLocation());
		// DropGoldBag(GoldBagSpawnLocation());
	}

    return DamageAmount;
}


void ABaseGoldMine::DropGoldBag(FVector spawnLocation)
{
	int RandomValue = FMath::RandRange(0, 1);
	if (RandomValue == 0)
	{
		// drop goldbag logic
		UObject* SpawnActor = StaticLoadObject(UObject::StaticClass(), NULL, TEXT("/Script/Engine.Blueprint'/Game/Blueprints/BP_GoldBag.BP_GoldBag'"));
		UBlueprint* GeneratedBP = Cast<UBlueprint>(SpawnActor);
		UWorld* World = GetWorld();
		if (!SpawnActor || !GeneratedBP || !GeneratedBP->GeneratedClass || !World) return;

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this; 
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; 



		AActor* SpawnedActor; 
		SpawnedActor = World->SpawnActor<AActor>(GeneratedBP->GeneratedClass, spawnLocation, GetActorRotation(), SpawnParams);
		ABaseGoldBag* SpawnedGoldBag = Cast<ABaseGoldBag>(SpawnedActor);

		if (GameMode)
		{
			// SendSpawnResponseMsg(2, SpawnLocation); 
			// SendSpawnNotiMsg(3, SpawnedGoldBag->GetTagId(), spawnLocation.X, spawnLocation.Y);
		}
		else UE_LOG(LogTemp, Warning, TEXT("Game Mode is null (in goldmine.cpp)"));
		

	}
}

FVector ABaseGoldMine::GoldBagSpawnLocation()
{
	// fix z
	FVector SpawnLocation = GetActorLocation(); 
	SpawnLocation.Y += 60.0f; 
	SpawnLocation.Z = -291.0f;
	float RandomOffsetX = UKismetMathLibrary::RandomFloatInRange(-30.0f, 30.0f);
	float RandomOffsetY = UKismetMathLibrary::RandomFloatInRange(-10.0f, 10.0f);
	SpawnLocation.X += RandomOffsetX;
	SpawnLocation.Y += RandomOffsetY;

    return SpawnLocation;
}


bool ABaseGoldMine::IsCollapse()
{
    return Durability <= 0;
}



void ABaseGoldMine::UpdateSprite()
{
	if (IsCollapse() && collapseSprite != nullptr)
		paperSprite->SetSprite(collapseSprite);
}


//void ABaseGoldMine::SendSpawnResponseMsg(int spawnActorIndex, int SpawnType, FVector location)
//{
//
//	struct Spawn::Response *response = new Spawn::Response(); 
//	response->H.Command = 7;//0x31; 
//	response->successyn = 1; 
//	response->SpawnActorIndex = spawnActorIndex; 
//	response->SpawnType = SpawnType;
//	response->Location = location;
//	GameMode->messageQueue.push((struct HEAD *)response);
//
//}
//
//void ABaseGoldMine::SendSpawnNotiMsg(int spawnType, int spawnActorIndex, float X, float Y)
//{
//	struct Spawn::Notification *noti = new Spawn::Notification(); 
//	noti->H.Command = 8;//0x32; 
//	noti->SpawnType = spawnType; 
//	noti->SpawnActorIndex = spawnActorIndex; 
//	noti->X = X; 
//	noti->Y = Y; 
//	GameMode->messageQueue.push((struct HEAD *)noti);
//}


