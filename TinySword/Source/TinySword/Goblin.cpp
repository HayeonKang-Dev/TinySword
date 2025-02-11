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
#include "BaseCastle.h"
#include "BaseAISheep.h"
#include "BaseGoldMine.h"
#include "BaseGoldBag.h"

#include "PlayingWidget.h"
#include "GameFramework/HUD.h"
#include "TinySwordGameMode.h"
#include "protocol.h"
#include "GameFramework/CharacterMovementComponent.h"


void AGoblin::BeginPlay()
{
    Super::BeginPlay(); 

    //playerController = Cast<ATinySwordPlayerController>(GetController());
    GameMode = Cast<ATinySwordGameMode>(GetWorld()->GetAuthGameMode());

    Health = MaxHealth; 
    Money = 0; 
    GetCharacterMovement()->MaxWalkSpeed = Speed; 

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
        Timer += GetWorld()->DeltaTimeSeconds;
        if (Timer >= 0.5f)
        {
            // SendMoveResponseMsg(0, TagId, ); 
            SendMoveNotiMsg(0, TagId, GetActorLocation().X, GetActorLocation().Y);

            Timer = 0.0f;
        }
        // AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Value);
        FlipCharacter(Value);
    }
}

void AGoblin::UpDown(float Value)
{
    if (Value != 0.0f)
    {
        Timer += GetWorld()->DeltaTimeSeconds; 
        if (Timer >= 0.5f)
        {
            // SendMoveResponseMsg(); 
            SendMoveNotiMsg(0, TagId, GetActorLocation().X, GetActorLocation().Y);

            Timer = 0.0f;
        }
        // AddMovementInput(FVector(0.0f, 1.0f, 0.0f), Value);
    }
}

// Animations
void AGoblin::FlipCharacter(int MoveDirec)
{
    UPaperFlipbookComponent* SpriteComponent = FindComponentByClass<UPaperFlipbookComponent>(); 

    if (SpriteComponent)
    {
        if (MoveDirec == 1) SpriteComponent->SetRelativeScale3D(FVector(1.5f, 1.5f, 1.5f));
        else if (MoveDirec == -1) SpriteComponent->SetRelativeScale3D(FVector(-1.5f, 1.5f, 1.5f));
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
void AGoblin::SetTagId(int32 newTagId)
{
    TagId = newTagId;
}
//////////////
void AGoblin::NotifyActorBeginOverlap(AActor *OtherActor)
{
    Super::NotifyActorBeginOverlap(OtherActor);

    if (!OtherActor || OtherActor==this) return; 

    if (OtherActor->IsA(ABaseMeat::StaticClass())) 
    {
        UE_LOG(LogTemp, Warning, TEXT("Overlap with Meat"));
        ABaseMeat* meat = Cast<ABaseMeat>(OtherActor); 
        SendGetItemResponseMsg(); 
        SendGetItemNotiMsg(0, meat->GetTagId(), GetActorLocation().X, GetActorLocation().Y);

        SendDestroyResponseMsg(3, meat->GetTagId(), meat->GetActorLocation().X, meat->GetActorLocation().Y); 
        SendDestroyNotiMsg(3, meat->GetTagId(), meat->GetActorLocation().X, meat->GetActorLocation().Y);

        IncreaseHealth(10);
        if (playerController && playerController->playingWidget) playerController->playingWidget->UpdateHealthBar(GetHealthPercent());
        // OtherActor->Destroy(); ///////////////////////////////////////////////////////////
        
    }
    if (OtherActor->IsA(ABaseGoldBag::StaticClass()))
    {
        UE_LOG(LogTemp, Warning, TEXT("Overlap with GoldBag"));
        ABaseGoldBag* gold = Cast<ABaseGoldBag>(OtherActor); 
        SendGetItemResponseMsg(); 
        SendGetItemNotiMsg(1, gold->GetTagId(), GetActorLocation().X, GetActorLocation().Y);
        IncreaseMoney(10);
        //UpdateMoneyCount_(Money);

        SendDestroyResponseMsg(1, gold->GetTagId(), gold->GetActorLocation().X, gold->GetActorLocation().Y);
        SendDestroyNotiMsg(1, gold->GetTagId(), gold->GetActorLocation().X, gold->GetActorLocation().Y);

        // OtherActor->Destroy(); /////////////////////////////////////////////////////////
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
        playerController->playingWidget->UpdateHealthBar(GetHealthPercent());

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
    SendAttackResponseMsg();

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
                SendAttackNotiMsg(0, TagId, 0, targetPlayer->GetTagId(), Damage, targetPlayer->GetHealth(), GetActorLocation().X, GetActorLocation().Y);
            }
            else if (ABaseBomb* targetBomb = Cast<ABaseBomb>(HitActor))
            {
                SendAttackNotiMsg(0, TagId, 1, targetBomb->GetTagId(), Damage, targetBomb->GetHealth(), GetActorLocation().X, GetActorLocation().Y);
            }
            else if (ABaseCastle* targetCastle = Cast<ABaseCastle>(HitActor))
            {
                SendAttackNotiMsg(0, TagId, 2, targetCastle->GetTagId(), Damage, targetCastle->GetDurability(), GetActorLocation().X, GetActorLocation().Y);
            }
            else if (ABaseAISheep* targetSheep = Cast<ABaseAISheep>(HitActor))
            {
                SendAttackNotiMsg(0, TagId, 3, targetSheep->GetTagId(), Damage, targetSheep->GetHealth(), GetActorLocation().X, GetActorLocation().Y);
            }
            else if (ABaseGoldMine* targetGoldMine = Cast<ABaseGoldMine>(HitActor))
            {
                SendAttackNotiMsg(0, TagId, 4, targetGoldMine->GetTagId(), Damage, targetGoldMine->GetDurability(), GetActorLocation().X, GetActorLocation().Y);
            }

        }
    }

}

//////////////////

// Control Variable
void AGoblin::IncreaseMoney(float Amount)
{
    Money += Amount; 
    UpdateMoneyCount_(Money);
}

bool AGoblin::DecreaseMoney(float Amount)
{
    if (Money < 10) return false;
    Money -= Amount;
    UpdateMoneyCount_(Money);
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

void AGoblin::UpdateMoneyCount_(int money)
{
    UE_LOG(LogTemp, Warning, TEXT("Entered in UpdateMoneyCount"));

    if (playerController)
    {
        UE_LOG(LogTemp, Warning, TEXT("Goblin -> PlayerController is valid"));
        UPlayingWidget* playing = playerController->GetPlayingWidget(); 
        if (playing) 
        {
            UE_LOG(LogTemp, Warning, TEXT("Goblin-> playingWidget is not null"));
            playing->UpdateMoneyCount(money);
        }
    }
    
    // playerController가 널인지, playingWidget이 널인지 분리해서 판별해야 함. 

}

void AGoblin::HandleDeath()
{

}

void AGoblin::SetPlayerController(ATinySwordPlayerController *newPlayerController)
{
    playerController = newPlayerController;
}




/////////////////////////////////////////////////////
void AGoblin::SendMoveResponseMsg(int ActorType, int ActorIndex, bool bMoveUp, bool bMoveDown, bool bMoveRight, bool bMoveLeft)
{
    struct Move::Response *response = new Move::Response(); 
    response->H.Command = 0x11; 
    response->ActorType = ActorType;
    response->ActorIndex = ActorIndex; 
    response->bMoveUp = bMoveUp; 
    response->bMoveDown = bMoveDown; 
    response->bMoveRight = bMoveRight; 
    response->bMoveLeft = bMoveLeft; 
    response->Speed = Speed;
    GameMode->messageQueue.push((struct HEAD *)response);
}

void AGoblin::SendMoveNotiMsg(int actorType, int actorIndex, float X, float Y)
{
    struct Move::Notification *noti = new Move::Notification(); 
    noti->H.Command = 0x12; 
    noti->ActorType = actorType; 
    noti->ActorIndex = actorIndex; 
    noti->X = X; 
    noti->Y = Y; 
    GameMode->messageQueue.push((struct HEAD *)noti);
}

void AGoblin::SendAttackResponseMsg()
{
    struct Attack::Response *response = new Attack::Response(); 
    response->H.Command = 0x21; 
    response->hityn = 1;
    GameMode->messageQueue.push((struct HEAD *)response);
} 

void AGoblin::SendAttackNotiMsg(int attackerType, int attackerIndex, int targetType, int targetIndex, int damage, int targetHp, float X, float Y)
{
    struct Attack::Notification *noti = new Attack::Notification(); 
    noti->H.Command = 0x22; 
    noti->AttackerType = attackerType; 
    noti->AttackerIndex = attackerIndex; 
    noti->targetType = targetType; 
    noti->targetIndex = targetIndex; 
    noti->Damage = damage; 
    noti->targetHp = targetHp; 
    noti->X = X; 
    noti->Y = Y; 
    GameMode->messageQueue.push((struct HEAD *)noti);
}

void AGoblin::SendGetItemResponseMsg()
{
    struct GetItem::Response *response = new GetItem::Response(); 
    response->H.Command = 0x41; 
    response->successyn = 1; 
    response->playerHp = GetHealth(); 
    response->playerCoin = GetMoneyCount(); 
    GameMode->messageQueue.push((struct HEAD *)response);
}

void AGoblin::SendGetItemNotiMsg(int itemType, int itemIndex, float X, float Y)
{
    struct GetItem::Notification *noti = new GetItem::Notification(); 
    noti->H.Command =0x42; 
    noti->playerIndex = GetTagId(); 
    noti->playerHp = GetHealth(); 
    noti->playerCoin = GetMoneyCount(); 
    noti->ItemType = itemType; 
    noti->ItemIndex = itemIndex; 
    noti->X = X; 
    noti->Y = Y; 
    GameMode->messageQueue.push((struct HEAD *)noti);
}

void AGoblin::SendDestroyResponseMsg(int actorType, int actorIndex, float X, float Y)
{
    struct Destroy::Response *response = new Destroy::Response(); 
    response->H.Command = 0x71; 
    response->ActorType = actorType; 
    response->ActorIndex = actorIndex; 
    response->X = X; 
    response->Y = Y; 
    GameMode->messageQueue.push((struct HEAD *)response);
    UE_LOG(LogTemp, Warning, TEXT("Send Destroy Response Message..."));

}

void AGoblin::SendDestroyNotiMsg(int actorType, int actorIndex, float X, float Y)
{
    struct Destroy::Notification *noti = new Destroy::Notification(); 
    noti->H.Command = 0x72;
    noti->ActorType = actorType; 
    noti->ActorIndex = actorIndex; 
    noti->X = X; 
    noti->Y = Y; 
    GameMode->messageQueue.push((struct HEAD *)noti);

}

