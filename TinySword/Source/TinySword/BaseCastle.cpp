// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCastle.h"
#include "Components/BoxComponent.h"
#include "PaperFlipbookComponent.h"
#include "Goblin.h"


// Sets default values
ABaseCastle::ABaseCastle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
    RootComponent = RootSceneComponent;

	PaperFlipbookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("PaperFlipbook"));
	PaperFlipbookComponent->SetupAttachment(RootComponent);

	BoxCollider1 = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider1"));
	BoxCollider1->SetupAttachment(PaperFlipbookComponent);
	
	BoxCollider2 = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider2"));
	BoxCollider2->SetupAttachment(PaperFlipbookComponent);
}

float ABaseCastle::TakeDamage(float DamageAmount, FDamageEvent const &DamageEvent, AController *EventInstigator, AActor *DamageCauser)
{
	if (AGoblin* CauserPlayer = Cast<AGoblin>(DamageCauser))
	{
		if (CauserPlayer->GetTagId() == GetTagId())
		{
			return 0.0f; 
		}
	}

	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	ActualDamage = FMath::Min(Durability, ActualDamage);
	if (!IsCollapse())
	{
		Durability -= ActualDamage;
		if (IsCollapse()) OnCollapse();
	}
    return ActualDamage;
}

// Called when the game starts or when spawned
void ABaseCastle::BeginPlay()
{
	Super::BeginPlay();
	Durability = MaxDurability;
}


// Called every frame
void ABaseCastle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// if (IsCollapse())
	// {
	// 	OnCollapse();
	// }
	
}




// Collapse
bool ABaseCastle::IsCollapse()
{
    return Durability <= 0;
}

void ABaseCastle::OnCollapse()
{
	if (CollapseFlipbook) PaperFlipbookComponent->SetFlipbook(CollapseFlipbook);
	if (BoxCollider1) BoxCollider1->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}