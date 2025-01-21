// Fill out your copyright notice in the Description page of Project Settings.


#include "Goblin.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h" 



void AGoblin::BeginPlay()
{
    Super::BeginPlay(); 

    Health = MaxHealth; 
    Money = 0; 

    OverlapComponent = FindComponentByClass<UCapsuleComponent>(); 
    paperFlipbookComponent = FindComponentByClass<UPaperFlipbookComponent>();

    //UPaperFlipbook* WalkAnim = LoadObject<UPaperFlipbook>(nullptr, TEXT("/Script/Paper2D.PaperFlipbook'/Game/Animations/Character/TNT_Blue_Walk.TNT_Blue_Walk'"));

    Timer = 0.0f; 
}


void AGoblin::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    
}

// Control Character
void AGoblin::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &AGoblin::Attack);

    PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AGoblin::MoveRight);
    PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &AGoblin::UpDown);
}

void AGoblin::MoveRight(float Value)
{
    if (Value != 0.0f)
    {
        AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Value);
        //paperFlipbookComponent->SetFlipbook(WalkAnim);
    }
}

void AGoblin::UpDown(float Value)
{
    if (Value != 0.0f)
    {
        AddMovementInput(FVector(0.0f, 1.0f, 0.0f), Value);
    }
}

void AGoblin::FlipCharacter(bool MoveDirec)
{
    UPaperFlipbookComponent* SpriteComponent = FindComponentByClass<UPaperFlipbookComponent>(); 

    if (SpriteComponent)
    {
        if (MoveDirec) SpriteComponent->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
        else SpriteComponent->SetRelativeScale3D(FVector(-1.0f, 1.0f, 1.0f));
    }
}


//////////////

//////////////
void AGoblin::NotifyActorBeginOverlap(AActor *OtherActor)
{
    Super::NotifyActorBeginOverlap(OtherActor);
}

void AGoblin::NotifyActorEndOverlap(AActor *OtherActor)
{
    Super::NotifyActorEndOverlap(OtherActor);
}

/////////////

/////////////////
float AGoblin::TakeDamage(float DamageAmount, FDamageEvent const &DamageEvent, AController *EventInstigator, AActor *DamageCauser)
{
    return 0.0f;
}

bool AGoblin::IsDead() const
{
    return Health <= 0;
}

void AGoblin::Attack()
{
    IsAttack = true; 
    FVector Start, End;

    // Get Dynamite's collision
    USphereComponent* SphereCollision = FindComponentByClass<USphereComponent>();
    if (SphereCollision)
    {
        float Radius = SphereCollision->GetUnscaledSphereRadius();
        Start = SphereCollision->GetComponentLocation();
        Start.X -= Radius; 
        End = Start; 
        End.X += Radius; 
    }

    FHitResult HitResult; 
    FCollisionQueryParams CollisionParams; 
    CollisionParams.AddIgnoredActor(this);

    // Line Trace
    if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, CollisionParams))
    {
        AActor* HitActor = Cast<AActor>(HitResult.GetActor());
        if (HitActor)
        {
            FDamageEvent DamageEvent; 
            HitActor->TakeDamage(Damage, DamageEvent, nullptr, this);

            if (AGoblin* targetPlayer = Cast<AGoblin>(HitActor))
            {

            }

            // else if .. != my castle, my bomb 
        }
    }

}

//////////////////

// Control Variable
void AGoblin::IncreaseMoney(float Amount)
{
    Money += Amount; 
}

bool AGoblin::DecreaseMoney(float Amount)
{
    if (Money < 10) return false;
    Money -= Amount;
    return true;
}

void AGoblin::IncreaseHealth(float Amount)
{
    if (Health+Amount > MaxHealth) Health = MaxHealth;
    else Health += Amount; 
}
/////////////



// Connection with UI
float AGoblin::GetHealthPercent() const
{
    return Health / MaxHealth;
}

int AGoblin::GetMoneyCount() const
{
    return Money;
}


void AGoblin::HandleDeath()
{

}