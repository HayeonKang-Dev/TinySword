// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseBomb.h"
#include "Goblin.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

#include "BaseCastle.h"
#include "BaseGoldMine.h"
#include "BaseAISheep.h"

#include "protocol.h"


ABaseBomb::ABaseBomb()
{
    // bombController = Cast<ABombAIController>(GetController());
}



void ABaseBomb::BeginPlay()
{
    Super::BeginPlay();
    
    Health = MaxHealth; 
    GetCharacterMovement()->MaxWalkSpeed = Speed; 
    Damage = 10.0f; 

    // bombController = Cast<ABombAIController>(GetController());
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

    // if (!bombController)
    // {
    //     bombController = Cast<ABombAIController>(GetController());
    // }
        

}



float ABaseBomb::TakeDamage(float DamageAmount, FDamageEvent const &DamageEvent, AController *EventInstigator, AActor *DamageCauser)
{
    // if (DamageCauser && DamageCauser->IsA(ABaseBomb::StaticClass())) return 0.0f; 
    
    // if (AGoblin* Causer = Cast<AGoblin>(DamageCauser))
    // {
    //     if (Causer && Causer->GetTagId() == TagId) return 0.0f; 
    // }

    // float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser); 
    // if(!IsDead())
    // {
    //     ActualDamage = FMath::Min(Health, ActualDamage); 
    //     Health -= ActualDamage; 

    //     if (IsDead())
    //     {
            
    //     }
    // }
    return 0.0f;
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

    for (AActor * hittedActor : DamagedActors)
    {
        if (AGoblin * goblin = Cast<AGoblin>(hittedActor)) 
        {
            // SendAttackResponseMsg(1, TagId, 0, goblin->GetTagId(), Damage);
            // SendAttackNotiMsg(1, TagId, 0, goblin->GetTagId(), Damage, goblin->GetHealth(), GetActorLocation().X, GetActorLocation().Y);
        }
            
        else if (ABaseBomb* bomb = Cast<ABaseBomb>(hittedActor))
        {
            // SendAttackResponseMsg(1, TagId, 1, bomb->GetTagId(), Damage);
            // SendAttackNotiMsg(1, TagId, 1, bomb->GetTagId(), Damage, bomb->GetHealth(), GetActorLocation().X, GetActorLocation().Y); 
        }
            
        else if (ABaseCastle* castle = Cast<ABaseCastle>(hittedActor))
        {
            // SendAttackResponseMsg(1, TagId, 2, castle->GetTagId(), Damage);
            // SendAttackNotiMsg(1, TagId, 2, castle->GetTagId(), Damage, castle->GetDurability(), GetActorLocation().X, GetActorLocation().Y);
        }
            
        else if (ABaseAISheep* sheep = Cast<ABaseAISheep>(hittedActor))
        {
            // SendAttackResponseMsg(1, TagId, 3, sheep->GetTagId(), Damage);
            // SendAttackNotiMsg(1, TagId, 3, sheep->GetTagId(), Damage, sheep->GetHealth(), GetActorLocation().X, GetActorLocation().Y);
        }
            
        else if (ABaseGoldMine* goldMine = Cast<ABaseGoldMine>(hittedActor))
        {
            // SendAttackResponseMsg(1, TagId, 4, goldMine->GetTagId(), Damage);
            // SendAttackNotiMsg(1, TagId, 4, goldMine->GetTagId(), Damage, goldMine->GetDurability(), GetActorLocation().X, GetActorLocation().Y);
        }
            
    }
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
    FTimerHandle TimerHandle_PlayExplodeAnim;
    if (BrinkAnim && paperFlipbookComponent->GetFlipbook() != BrinkAnim)
    {
        paperFlipbookComponent->SetFlipbook(BrinkAnim);
        paperFlipbookComponent->Play();
    }
    GetWorld()->GetTimerManager().SetTimer(TimerHandle_PlayExplodeAnim, this, &ABaseBomb::PlayExplodeAnim, 1.0f, false);
}

void ABaseBomb::PlayExplodeAnim()
{
    if (ExplodeAnim && paperFlipbookComponent->GetFlipbook() != ExplodeAnim)
    {
        paperFlipbookComponent->SetLooping(false);
        paperFlipbookComponent->SetFlipbook(ExplodeAnim);
        paperFlipbookComponent->Play();
    }
}

void ABaseBomb::PlayDeadAnim()
{
    if (DeadAnim) 
    {
        paperFlipbookComponent->SetLooping(false);
        paperFlipbookComponent->SetFlipbook(DeadAnim);
        paperFlipbookComponent->Play();

        paperFlipbookComponent->OnFinishedPlaying.AddDynamic(this, &ABaseBomb::OnDeadAnimFinished);

    }
}

void ABaseBomb::OnDeadAnimFinished()
{
    if (DeadDispAnim) 
    {
        paperFlipbookComponent->SetLooping(false);
        paperFlipbookComponent->SetFlipbook(DeadDispAnim);
        paperFlipbookComponent->Play();

    }
}

void ABaseBomb::HandleDeath()
{
    PlayDeadAnim(); // 알아서 DeadDispAnim 연결 재생 
    SetActorEnableCollision(false);
}

void ABaseBomb::AddToReuseId(int32 tagId)
{
    // 폭발된 폭탄의 Id 재사용하기 위해 모아둠 
    GameMode->ReuseBombId.Enqueue(tagId);
}

//
//void ABaseBomb::SendAttackResponseMsg(int attackerType, int attackerIndex, int targetType, int targetIndex, int damage)
//{
//    struct Attack::Response *response = new Attack::Response(); 
//    response->H.Command = 5;//0x21; 
//    response->AttackerType = attackerType;
//    response->AttackerIndex = attackerIndex;
//    response->TargetType = targetType; 
//    response->TargetIndex = targetIndex; 
//    response->Damage = damage; 
//    response->hityn = 1; 
//    GameMode->messageQueue.push((struct HEAD *)response);
//}
//
//void ABaseBomb::SendAttackNotiMsg(int attackerType, int attackerIndex, int targetType, int targetIndex, int damage, int targetHp, float X, float Y)
//{
//    struct Attack::Notification *noti = new Attack::Notification(); 
//    noti->H.Command = 6;//0x22; 
//    noti->AttackerType = attackerType; 
//    noti->AttackerIndex = attackerIndex; 
//    noti->AttackerType = attackerType; 
//    noti->targetIndex = targetIndex; 
//    noti->Damage = damage; 
//    noti->targetHp = targetHp; 
//    noti->X = X; 
//    noti->Y = Y; 
//    GameMode->messageQueue.push((struct HEAD *)noti);
//}
