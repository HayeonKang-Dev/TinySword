// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAISheep.h"
#include "BaseMeat.h"
#include "TinySwordGameMode.h"
#include "SheepAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "protocol.h"


void ABaseAISheep::BeginPlay()
{
    Super::BeginPlay();

    paperFlipbookComponent = FindComponentByClass<UPaperFlipbookComponent>();

    Health = MaxHealth; 
    GameMode = Cast<ATinySwordGameMode>(GetWorld()->GetAuthGameMode());

    GameMode->ActiveSheepId.Add(this, GetTagId());
}

void ABaseAISheep::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    UpdateAnimation();

    if (IsDead())
    {
        
        // SendSpawnResponseMsg(GetActorLocation(), TagId, 2);

        
        // Destroy(); ////////////////////////////////////
        /*SendDestroyResponseMsg(2, TagId, GetActorLocation().X, GetActorLocation().Y);
        SendDestroyNotiMsg(2, TagId, GetActorLocation().X, GetActorLocation().Y);*/

        
        // SpawnMeat();
    }

    FVector Velocity = GetVelocity();
    if (Velocity.SizeSquared() > 0.0f)
    {
        FlipCharacter(Velocity.X);
    }

    Timer += GetWorld()->DeltaTimeSeconds;
    if (Timer >= 0.5f)
    {
        // SendMoveResponseMsg(1, TagId, Velocity.Size()); ///////////////////////////////////////////////
        //SendMoveNotiMsg(1, TagId, GetActorLocation().X, GetActorLocation().Y);
        Timer = 0.0f;
    }

    // Fix Rotation
    FRotator CurrentRotation = GetActorRotation();
    CurrentRotation.Pitch = 0.0f; 
    CurrentRotation.Yaw = 0.0f; 
    SetActorRotation(CurrentRotation);
}

float ABaseAISheep::TakeDamage(float DamageAmount, FDamageEvent const &DamageEvent, AController *EventInstigator, AActor *DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    // ActualDamage = FMath::Min(Health, ActualDamage);
    // FVector ActorLocation = GetActorLocation(); 
    // TSubclassOf<ABaseMeat> Meat = ABaseMeat::StaticClass(); 
    // if (!IsDead())
    // {
    //     Health -= ActualDamage;
    //     TakeDamageFrom(DamageCauser); 
    //     if (IsDead())
    //     {
    //         // Destroy(); ////////////////////////////////////
    //         SendDestroyResponseMsg(2, TagId, GetActorLocation().X, GetActorLocation().Y);
    //         SendDestroyNotiMsg(2, TagId, GetActorLocation().X, GetActorLocation().Y);

    //         SpawnMeat();
    //     }
    // }
    return ActualDamage;
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
    //SpawnLocation.Z = 45.f;

    // Spawn Meat & Set Meat TagId
    AActor* SpawnedActor; 
	SpawnedActor = World->SpawnActor<AActor>(GeneratedBP->GeneratedClass, SpawnLocation, GetActorRotation(), SpawnParams);
    ABaseMeat* SpawnedMeat = Cast<ABaseMeat>(SpawnedActor);

    if (SpawnedMeat)
    {
        SpawnedMeat->SetTagId(GetTagId()); 
        GameMode->ActiveMeatId.Add(SpawnedMeat, GetTagId());
        // SendSpawnResponseMsg(); 
        //SendSpawnNotiMsg(2, SpawnedMeat->GetTagId(), SpawnedMeat->GetActorLocation().X, SpawnedMeat->GetActorLocation().Y);
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



void ABaseAISheep::FlipCharacter(float MoveDirec)
{
    UPaperFlipbookComponent* SpriteComponent = FindComponentByClass<UPaperFlipbookComponent>(); 

    if (SpriteComponent)
    {
        if (MoveDirec > 0.0f) SpriteComponent->SetRelativeScale3D(FVector(1.5f, 1.5f, 1.5f));
        else if (MoveDirec < 0.0f) SpriteComponent->SetRelativeScale3D(FVector(-1.5f, 1.5f, 1.5f));
    }
}

void ABaseAISheep::UpdateAnimation()
{
    FVector Velocity = GetVelocity();

    if (!Velocity.IsNearlyZero()) {
        FVector MoveDirec = Velocity.GetSafeNormal();
        //FlipCharacter(MoveDirec);
    }

    bool IsMoving = Velocity.SizeSquared() > KINDA_SMALL_NUMBER; 

    if (IsMoving && MoveAnim && paperFlipbookComponent->GetFlipbook() != MoveAnim)
    {
        paperFlipbookComponent->SetFlipbook(MoveAnim);
 
    }
    else if (!IsMoving && IdleAnim && paperFlipbookComponent->GetFlipbook() != IdleAnim)
    {
        paperFlipbookComponent->SetFlipbook(IdleAnim);
        
        
    }
}

// void ABaseAISheep::HandleDeath()
// {
//     PlayDeadAnim(); 
//     SetActorEnableCollision(false);
// }

// void ABaseAISheep::PlayDeadAnim()
// {
//     if (DeadAnim) 
//     {
//         paperFlipbookComponent->SetLooping(false);
//         paperFlipbookComponent->SetFlipbook(DeadAnim);
//         paperFlipbookComponent->Play();

//         paperFlipbookComponent->OnFinishedPlaying.AddDynamic(this, &ABaseAISheep::OnDeadAnimFinished);

//     }
// }
// void ABaseAISheep::PlayDeadDispAnim()
// {
//     if (DeadDispAnim) 
//     {
//         paperFlipbookComponent->SetLooping(false);
//         paperFlipbookComponent->SetFlipbook(DeadDispAnim);
//         paperFlipbookComponent->Play();

//     }
// }
//
// void ABaseAISheep::SendMoveResponseMsg(int ActorType, int ActorIndex, float Speed)
//{
//    struct Move::Response *response = new Move::Response();
//    response->H.Command = 3;//0x11;
//    response->ActorType = ActorType;
//    response->ActorIndex = ActorIndex;
//    response->Speed = Speed;
//    GameMode->messageQueue.push((struct HEAD *)response);
//}
//
// void ABaseAISheep::SendMoveNotiMsg(int actorType, int actorIndex, float X, float Y)
//{
//    struct Move::Notification *noti = new Move::Notification();
//    noti->H.Command = 4;//0x12;
//    noti->ActorType = actorType;
//    noti->ActorIndex = actorIndex;
//    noti->X = X;
//    noti->Y = Y;
//    GameMode->messageQueue.push((struct HEAD *)noti);
//}
//
// void ABaseAISheep::SendSpawnResponseMsg(FVector spawnLocation, int spawnActorIndex, int spawnType)
//{
//    struct Spawn::Response *response = new Spawn::Response();
//    response->H.Command = 7;//0x31;
//    response->Location = spawnLocation;
//    response->SpawnType = spawnType;
//    response->SpawnActorIndex = spawnActorIndex;
//    response->successyn = 1;
//    GameMode->messageQueue.push((struct HEAD *)response);
//}
//
// void ABaseAISheep::SendSpawnNotiMsg(int spawnType, int spawnActorIndex, float X, float Y)
//{
//    struct Spawn::Notification *noti = new Spawn::Notification();
//	noti->H.Command = 8;//0x32;
//	noti->SpawnType = spawnType;
//	noti->SpawnActorIndex = spawnActorIndex;
//	noti->X = X;
//	noti->Y = Y;
//	GameMode->messageQueue.push((struct HEAD *)noti);
//}
//
// void ABaseAISheep::SendDestroyResponseMsg(int actorType, int actorIndex, float X, float Y)
//{
//    struct Destroy::Response *response = new Destroy::Response();
//    response->H.Command = 15;//0x71;
//    response->ActorType = actorType;
//    response->ActorIndex = actorIndex;
//    response->X = X;
//    response->Y = Y;
//    GameMode->messageQueue.push((struct HEAD *)response);
//}
//
// void ABaseAISheep::SendDestroyNotiMsg(int actorType, int actorIndex, float X, float Y)
//{
//    struct Destroy::Notification *noti = new Destroy::Notification();
//    noti->H.Command = 16;//0x72;
//    noti->ActorType = actorType;
//    noti->ActorIndex = actorIndex;
//    noti->X = X;
//    noti->Y = Y;
//    GameMode->messageQueue.push((struct HEAD *)noti);
//}
