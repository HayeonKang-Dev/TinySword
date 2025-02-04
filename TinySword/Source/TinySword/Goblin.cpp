// Fill out your copyright notice in the Description page of Project Settings.


#include "Goblin.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h" 
#include "BaseMeat.h"
#include "BaseBomb.h"
#include "BaseGoldBag.h"
#include "PlayingWidget.h"
#include "GameFramework/HUD.h"


void AGoblin::BeginPlay()
{
    Super::BeginPlay(); 

    playerController = Cast<ATinySwordPlayerController>(GetController());

    Health = MaxHealth; 
    Money = 0; 

    OverlapComponent = FindComponentByClass<UCapsuleComponent>(); 
    paperFlipbookComponent = FindComponentByClass<UPaperFlipbookComponent>();

    //UPaperFlipbook* WalkAnim = LoadObject<UPaperFlipbook>(nullptr, TEXT("/Script/Paper2D.PaperFlipbook'/Game/Animations/Character/TNT_Blue_Walk.TNT_Blue_Walk'"));

    Timer = 0.0f; 
    // if (playerController && playerController->playingWidget)
    // {
    //     playerController->playingWidget->HPBar->SetPercent(GetHealthPercent());
    //     UpdateMoneyCount(Money);
    // }
    

}


void AGoblin::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    //UE_LOG(LogTemp, Warning, TEXT("bIsAttacking: %s, Velocity: %s"), bIsAttacking ? TEXT("true") : TEXT("false"), *GetVelocity().ToString());

    UpdateAnimation();

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
        FlipCharacter(Value);
    }
}

void AGoblin::UpDown(float Value)
{
    if (Value != 0.0f)
    {
        AddMovementInput(FVector(0.0f, 1.0f, 0.0f), Value);
    }
}

// Animations
void AGoblin::FlipCharacter(int MoveDirec)
{
    UPaperFlipbookComponent* SpriteComponent = FindComponentByClass<UPaperFlipbookComponent>(); 

    if (SpriteComponent)
    {
        if (MoveDirec == 1) SpriteComponent->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
        else if (MoveDirec == -1) SpriteComponent->SetRelativeScale3D(FVector(-1.0f, 1.0f, 1.0f));
    }
}

void AGoblin::UpdateAnimation()
{
    if (bIsAttacking) return; 

    FVector Velocity = GetVelocity();

    if (!Velocity.IsNearlyZero()) {
        FVector MoveDirec = Velocity.GetSafeNormal();
        //FlipCharacter(MoveDirec);
    }

    bool IsMoving = Velocity.SizeSquared() > KINDA_SMALL_NUMBER; 

    if (IsMoving && WalkAnim && paperFlipbookComponent->GetFlipbook() != WalkAnim)
    {
        paperFlipbookComponent->SetFlipbook(WalkAnim);
 
    }
    else if (!IsMoving && IdleAnim && paperFlipbookComponent->GetFlipbook() != IdleAnim)
    {
        paperFlipbookComponent->SetFlipbook(IdleAnim);
        
        
    }
}


void AGoblin::ResetToIdle()
{
    bIsAttacking = false; 
    if (paperFlipbookComponent && IdleAnim) paperFlipbookComponent->SetFlipbook(IdleAnim);
}

void AGoblin::PlayAttackAnimation()
{
    if (bIsAttacking) return; 
    bIsAttacking = true; 

    if (paperFlipbookComponent && AttackAnim) paperFlipbookComponent->SetFlipbook(AttackAnim);

    if (AttackAnim) 
    {
        float AnimDur = AttackAnim->GetTotalDuration();
        GetWorldTimerManager().SetTimer(
            TimerHandleAttack,
            this,
            &AGoblin::ResetToIdle,
            AnimDur,
            false
        );
    }
}


//////////////

//////////////
void AGoblin::NotifyActorBeginOverlap(AActor *OtherActor)
{
    Super::NotifyActorBeginOverlap(OtherActor);

    if (!OtherActor || OtherActor==this) return; 

    if (OtherActor->IsA(ABaseMeat::StaticClass())) 
    {
        IncreaseHealth(10);
        if (playerController && playerController->playingWidget) playerController->playingWidget->HPBar->SetPercent(GetHealthPercent());
        OtherActor->Destroy();
    }
    if (OtherActor->IsA(ABaseGoldBag::StaticClass()))
    {
        IncreaseMoney(10);
        UpdateMoneyCount(Money);
        OtherActor->Destroy();
    }
}

void AGoblin::NotifyActorEndOverlap(AActor *OtherActor)
{
    Super::NotifyActorEndOverlap(OtherActor);
}

/////////////

/////////////////
float AGoblin::TakeDamage(float DamageAmount, FDamageEvent const &DamageEvent, AController *EventInstigator, AActor *DamageCauser)
{
    ABaseBomb* impactedBomb = Cast<ABaseBomb>(DamageCauser); 
    if (impactedBomb && impactedBomb->GetTagId()==TagId) return 0.0f; 

    float DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser); 
    DamageToApply = FMath::Min(Health, DamageToApply); 

    if (!IsDead())
    {
        Health -= DamageToApply; 
        playerController->playingWidget->HPBar->SetPercent(GetHealthPercent());

        if (IsDead())
        {
            HandleDeath(); 
            GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }
    }

    return 0.0f;
}

bool AGoblin::IsDead() const
{
    return Health <= 0;
}

void AGoblin::Attack()
{
    PlayAttackAnimation();
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
    UpdateMoneyCount(Money);
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

void AGoblin::UpdateMoneyCount(int money)
{
    FString moneyStr = FString::Printf(TEXT("%d"), money);
    if (playerController && playerController->playingWidget) playerController->playingWidget->moneyCount->SetText(FText::FromString(moneyStr));
}

void AGoblin::HandleDeath()
{

}