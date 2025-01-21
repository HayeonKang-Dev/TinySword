// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAISheep.h"
#include "BaseMeat.h"
#include "TinySwordGameMode.h"
#include "SheepAIController.h"
#include "GameFramework/CharacterMovementComponent.h"


void ABaseAISheep::BeginPlay()
{
    Super::BeginPlay();

    Health = MaxHealth; 
    GameMode = Cast<ATinySwordGameMode>(GetWorld()->GetAuthGameMode());

    GameMode->ActiveSheepId.Add(this, GetTagId());
}

// void ABaseAISheep::Tick(float DeltaTime)
// {
//     Super::Tick(float DeltaTime);
// }

float ABaseAISheep::TakeDamage(float DamageAmount, FDamageEvent const &DamageEvent, AController *EventInstigator, AActor *DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    ActualDamage = FMath::Min(Health, ActualDamage);
    FVector ActorLocation = GetActorLocation(); 
    TSubclassOf<ABaseMeat> Meat = ABaseMeat::StaticClass(); 
    if (!IsDead())
    {
        Health -= ActualDamage; 
        if (IsDead())
        {
            SpawnMeat();
        }
    }
    return ActualDamage;
}

// Move 
void ABaseAISheep::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ABaseAISheep::MoveRight);
}



void ABaseAISheep::MoveRight(float AxisValue)
{
    AddMovementInput(GetActorRightVector() * AxisValue);
}


bool ABaseAISheep::IsDead() const
{
    return Health <= 0;
}

void ABaseAISheep::SpawnMeat()
{
    UObject* SpawnActor = StaticLoadObject(UObject::StaticClass(), NULL, TEXT("/Game/Blueprints/BP_Meat.BP_Meat")); 
    UBlueprint* GeneratedBP = Cast<UBlueprint>(SpawnActor);
    UWorld* World = GetWorld(); 

    if (!SpawnActor || !GeneratedBP || !GeneratedBP->GeneratedClass || !World) return;

    FActorSpawnParameters SpawnParams; 
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // Adjust Spawn Location (Lower Z)
    FVector SpawnLocation = GetActorLocation(); 
    SpawnLocation.Z = 45.f;

    // Spawn Meat & Set Meat TagId
    AActor* SpawnedActor; 
	SpawnedActor = World->SpawnActor<AActor>(GeneratedBP->GeneratedClass, SpawnLocation, GetActorRotation(), SpawnParams);
    ABaseMeat* SpawnedMeat = Cast<ABaseMeat>(SpawnedActor);

    if (SpawnedMeat)
    {
        SpawnedMeat->SetTagId(GetTagId()); 
        GameMode->ActiveMeatId.Add(SpawnedMeat, GetTagId());
    }

}


void ABaseAISheep::TakeDamageFrom(AActor *Attacker)
{
    if (Attacker == nullptr) return; 

    FVector AttackLocation = Attacker->GetActorLocation(); 
    ASheepAIController* sheepAIController = Cast<ASheepAIController>(GetController()); 
    if (sheepAIController)
    {
        GetCharacterMovement()->MaxWalkSpeed = BoostedSpeed; 
        GetWorld()->GetTimerManager().SetTimer(SpeedBoostTimerHandle, this, &ABaseAISheep::ResetSpeed, SpeedBoostDuration, false);
        sheepAIController->FleeFrom(AttackLocation);
    }
}

void ABaseAISheep::ResetSpeed()
{
    GetCharacterMovement()->MaxWalkSpeed = NormalSpeed; 
}