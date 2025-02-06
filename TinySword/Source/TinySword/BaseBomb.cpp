// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseBomb.h"
#include "Goblin.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"


void ABaseBomb::BeginPlay()
{
    Super::BeginPlay();
    
    Health = MaxHealth; 
    GetCharacterMovement()->MaxWalkSpeed = Speed; 
    Damage = 10.0f; 

    bombController = Cast<ABombAIController>(GetController());
    GameMode = Cast<ATinySwordGameMode>(GetWorld()->GetAuthGameMode());

    paperFlipbookComponent = FindComponentByClass<UPaperFlipbookComponent>();
}



void ABaseBomb::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    FRotator CurrentRotation = GetActorRotation();
    CurrentRotation.Pitch = 0.0f; 
    CurrentRotation.Yaw = 0.0f; 
    SetActorRotation(CurrentRotation);
}



float ABaseBomb::TakeDamage(float DamageAmount, FDamageEvent const &DamageEvent, AController *EventInstigator, AActor *DamageCauser)
{
    if (DamageCauser && DamageCauser->IsA(ABaseBomb::StaticClass())) return 0.0f; 
    
    if (AGoblin* Causer = Cast<AGoblin>(DamageCauser))
    {
        if (Causer && Causer->GetTagId() == TagId) return 0.0f; 
    }

    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser); 
    if(!IsDead())
    {
        ActualDamage = FMath::Min(Health, ActualDamage); 
        Health -= ActualDamage; 

        if (IsDead())
        {
            
        }
    }
    return ActualDamage;
}


void ABaseBomb::DealRadialDamage()
{
    FVector Origin = GetActorLocation(); 
    float DamageRadius = 100.0f; 
    TArray<AActor*> IgnoreActors;
    IgnoreActors.Add(this);

    AController* InstigatedByController = nullptr;

    TArray<AActor*> DamagedActors; 

    UGameplayStatics::ApplyRadialDamage(
        this,
        Damage, 
        Origin, 
        DamageRadius, 
        UDamageType::StaticClass(),
        DamagedActors, 
        this, 
        GetInstigatorController(), 
        true
    );

    // for (int32 i = DamagedActors.Num()-1; i>=0; --i)
    // {
    //     AActor* Actor = DamagedActors[i];
    //     if (!Actor || Actor->IsPendingKill())
    //     {
    //         DamagedActors.RemoveAt(i);
    //     }
    // }
}


bool ABaseBomb::IsDead() const
{
    return Health <= 0;
}



void ABaseBomb::SetTagId(int32 newTagId)
{
    TagId = newTagId;
}

void ABaseBomb::SetOwnerTagId(int32 newOwnerTagId)
{
    OwnerTagId = newOwnerTagId;
}

void ABaseBomb::PlayBrinkAnim()
{
    if (BrinkAnim && paperFlipbookComponent->GetFlipbook() != BrinkAnim)
    {
        paperFlipbookComponent->SetFlipbook(BrinkAnim);
    }
}

void ABaseBomb::PlayExplodeAnim()
{
    if (ExplodeAnim && paperFlipbookComponent->GetFlipbook() != ExplodeAnim)
    {
        paperFlipbookComponent->SetFlipbook(ExplodeAnim);
    }
}
