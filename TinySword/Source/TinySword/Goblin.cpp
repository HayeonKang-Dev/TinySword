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


AGoblin::AGoblin()
{
    // GetCharacterMovement();    
}


void AGoblin::BeginPlay()
{
    Super::BeginPlay(); 

    UE_LOG(LogTemp, Error, TEXT(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Goblin is Reseted. : %d"), TagId);
    //playerController = Cast<ATinySwordPlayerController>(GetController());
    GameMode = Cast<ATinySwordGameMode>(GetWorld()->GetAuthGameMode());
    GI = Cast<UTinySwordGameInstance>(GetWorld()->GetGameInstance());

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
    

    // GI

    // 생성자나 BeginPlay에서 설정
    SetReplicates(true);
    SetReplicateMovement(true); // 위치, 회전 등 자동 동기화


}


void AGoblin::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    //UE_LOG(LogTemp, Warning, TEXT("bIsAttacking: %s, Velocity: %s"), bIsAttacking ? TEXT("true") : TEXT("false"), *GetVelocity().ToString());

    if (!IsDead())
    {
        UpdateAnimation();
    }
    

    if (!playerController)
    {
        playerController = Cast<ATinySwordPlayerController>(GetController());
    }

    // if (!IsDead() && playerController)
    // {
    //     // playerController->playingWidget->UpdateHealthBar(GetHealthPercent());

    //     if (IsDead())
    //     {
    //         // HandleDeath(); 
    //         // GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    //     }
    // }


    if (bIsMovingToTarget && FVector::Dist(GetActorLocation(), LastTargetLocation) < 0.01f)
    {
        bIsMovingToTarget = false; 
    }

    
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
    if (FMath::Abs(Value) > KINDA_SMALL_NUMBER) // if (Value != 0.0f)
    {

        SendMoveRequestMsg(GetTagId(), 0, 0, Value>0.0, Value<0.0);

        AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Value);

        FlipCharacter(Value);
    }
}

void AGoblin::UpDown(float Value)
{
    if (FMath::Abs(Value) > KINDA_SMALL_NUMBER)
    {
        SendMoveRequestMsg(GetTagId(), Value<0.0, Value>0.0, 0, 0);
        AddMovementInput(FVector(0.0f, 1.0f, 0.0f), Value);
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

void AGoblin::PlayDeadAnim()
{
    if (DeadAnim)
    {
        UE_LOG(LogTemp, Warning, TEXT("PLAY DEAD ANIM! && DeadAnim IS AVAILABLE"));
        paperFlipbookComponent->Stop();
        paperFlipbookComponent->SetLooping(false);
        paperFlipbookComponent->SetFlipbook(DeadAnim);
        paperFlipbookComponent->Play();
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

    if (!playerController || !playerController->IsA(ATinySwordPlayerController::StaticClass()))
    {
        //UE_LOG(LogTemp, Error, TEXT("PLAYER CONTROLLER IS NOT A TINY SWORD'S"));
        return;
    }

    if (OtherActor->IsA(ABaseMeat::StaticClass())) 
    {
        UE_LOG(LogTemp, Warning, TEXT("Overlap with Meat"));
        ABaseMeat* meat = Cast<ABaseMeat>(OtherActor); 
        
        SendGetItemRequestMsg(MEAT, meat->GetTagId());
        IncreaseHealth(10);
        // meat->Destroy();
       
        if (playerController && playerController->playingWidget) 
        {
            playerController->playingWidget->UpdateHealthBar(GetHealthPercent());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("PLAYER CONTROLLER IS NULL"));
        }

        

    }
    if (OtherActor->IsA(ABaseGoldBag::StaticClass()))
    {
        UE_LOG(LogTemp, Warning, TEXT("Overlap with GoldBag"));
        ABaseGoldBag* gold = Cast<ABaseGoldBag>(OtherActor); 

        /////////////////////
        // IncreaseMoney(10);
        // gold->Destroy();
        ////////////////////

        SendGetItemRequestMsg(GOLDBAG, gold->GetTagId());
        IncreaseMoney(10); 
        // gold->Destroy(); 

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
    UE_LOG(LogTemp, Warning, TEXT(">>>>>>>>>>>!!!!! Called TakeDamage"));
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
        UE_LOG(LogTemp, Warning, TEXT(">>>>>>>>>>>>>>>>>>>>>>>>>>> ATTACKING"));
        AActor* HitActor = Cast<AActor>(HitResult.GetActor());
        if (HitActor)
        {
            UE_LOG(LogTemp, Warning, TEXT(">>>>>>>>>>>>>>>>>>> HIT ACTOR AVAILABLE"));
            
            FDamageEvent DamageEvent; 
            //HitActor->TakeDamage(Damage, DamageEvent, nullptr, this);

            if (AGoblin* targetPlayer = Cast<AGoblin>(HitActor))
            {
                UE_LOG(LogTemp, Warning, TEXT("Target is PLAYER"));
                if (targetPlayer)
                {
                    SendAttackRequestMsg(GetTagId(), GOBLIN, targetPlayer->GetTagId(), 
                                        Vector(targetPlayer->GetActorLocation()), Vector(GetActorLocation()), Damage);
                    return;
                    
                }
                    
                
            }
            else if (ABaseBomb* targetBomb = Cast<ABaseBomb>(HitActor))
            {
                UE_LOG(LogTemp, Warning, TEXT("Target is BOMB"));
                if (targetBomb->GetTagId() == GetTagId()) 
                {
                    SendAttackRequestMsg(GetTagId(), NONEACTOR, 0, Vector(0, 0, 0), Vector(GetActorLocation()), Damage);
                    return; 
                }

                if (targetBomb) 
                {
                    SendAttackRequestMsg(GetTagId(), BOMB, targetBomb->GetTagId(), 
                                    Vector(targetBomb->GetActorLocation()), Vector(GetActorLocation()), Damage);
                    return;
                }
                    
            }
            else if (ABaseCastle* targetCastle = Cast<ABaseCastle>(HitActor))
            {
                UE_LOG(LogTemp, Warning, TEXT("Target is CASTLE"));

                if (targetCastle->GetTagId() == GetTagId()) 
                {
                    SendAttackRequestMsg(GetTagId(), NONEACTOR, 0, Vector(0, 0, 0), Vector(GetActorLocation()), Damage);
                    return; 
                }


                if (targetCastle)
                {
                    SendAttackRequestMsg(GetTagId(), CASTLE, targetCastle->GetTagId(), 
                                    Vector(targetCastle->GetActorLocation()), Vector(GetActorLocation()), Damage);
                    return;
                }
                    
            }
            else if (ABaseAISheep* targetSheep = Cast<ABaseAISheep>(HitActor))
            {
                UE_LOG(LogTemp, Warning, TEXT("Target is SHEEP"));
                if (targetSheep)
                {
                    SendAttackRequestMsg(GetTagId(), SHEEP, targetSheep->GetTagId(), 
                                    Vector(targetSheep->GetActorLocation()), Vector(GetActorLocation()), Damage);
                    return;
                }
                    
            }
            else if (ABaseGoldMine* targetGoldMine = Cast<ABaseGoldMine>(HitActor))
            {
                UE_LOG(LogTemp, Warning, TEXT("Target is GOLDMINE"));
                if (targetGoldMine)
                {
                    SendAttackRequestMsg(GetTagId(), GOLDMINE, targetGoldMine->GetTagId(),
                                    Vector(targetGoldMine->GetActorLocation()), Vector(GetActorLocation()), Damage);
                    return;
                }
                    
            }
            
        }
        
    }
    SendAttackRequestMsg(GetTagId(), NONEACTOR, 0, Vector(0, 0, 0), Vector(GetActorLocation()), Damage);
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

void AGoblin::DecreaseHealth(float Amount)
{
    Health = FMath::Max(0, Health-Amount);
    if (!playerController) playerController = Cast<ATinySwordPlayerController>(GetController());
    if (playerController) playerController->playingWidget->UpdateHealthBar(GetHealthPercent());
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
    if (!playerController)
    {
        playerController = Cast<ATinySwordPlayerController>(GetController()); 
    }

    if (playerController)
    {
        UE_LOG(LogTemp, Warning, TEXT("Goblin -> PlayerController is valid"));
        UPlayingWidget* playing = playerController->GetPlayingWidget(); 
        if (playing) 
        {
            UE_LOG(LogTemp, Warning, TEXT("Goblin-> playingWidget is not null"));
            playing->UpdateMoneyCount(money);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("[PLAYING WIDGET IS NULL]"));
            playerController->FindPlayingWidget(); 
            playerController->GetPlayingWidget()->UpdateMoneyCount(money);
        }
    }
    
    // playerController가 널인지, playingWidget이 널인지 분리해서 판별해야 함. 

}

void AGoblin::HandleDeath()
{
    // play anim 
    PlayDeadAnim();

    // enable collision
    SetActorEnableCollision(false);

    if (!playerController) playerController = Cast<ATinySwordPlayerController>(GetController());
    if (playerController) playerController->DisableInput(GetWorld()->GetFirstPlayerController());

    //////////// 본진도 부서지면 Lose 위젯 띄우기 ////////////////
}

void AGoblin::SetPlayerController(ATinySwordPlayerController *newPlayerController)
{
    playerController = newPlayerController;
}



void AGoblin::SendMoveRequestMsg(short ActorTagId, bool bMoveUp, bool bMoveDown, bool bMoveRight, bool bMoveLeft)
{

    Move::Request Request; 
    Request.MoveActorType = GOBLIN;
    Request.MoveActorTagId = ActorTagId; 
    Request.bMoveUp = bMoveUp; 
    Request.bMoveDown = bMoveDown; 
    Request.bMoveRight = bMoveRight; 
    Request.bMoveLeft = bMoveLeft;
    Request.Location = GetActorLocation(); /////////////

    FArrayWriter WriterArray; 
    WriterArray.Serialize((UTF8CHAR*)&Request, sizeof(Request));
    TSharedPtr<FBufferArchive> Packet = FTCPSocketClient_Async::CreatePacket((short)MOVE_REQUEST, WriterArray.GetData(), WriterArray.Num());

    // GameMode->GetTCPSocketClient().BeginSendPhase(Packet);
    GI->GetTCPClient()->BeginSendPhase(Packet);
}

void AGoblin::SendAttackRequestMsg(short ActorTagId, ActorType TargetActorType, short TargetTagId, Vector TargetLocation, Vector AttackLocation, int damage)
{

    UE_LOG(LogTemp, Warning, TEXT("[SEND] ATTACK REQUEST MSG"));

    Attack::Request Request; 
    Request.TargetActorType = TargetActorType; 
    Request.AttackerActorType = GOBLIN; 
    Request.TargetTagId = TargetTagId; 
    Request.AttackerTagId = ActorTagId;// GetTagId(); 
    Request.TargetLocation = TargetLocation; 
    Request.AttackLocation = AttackLocation; 
    Request.Damage = damage; 

    UE_LOG(LogTemp, Warning, TEXT("[ATTACK REQUEST MSG] Success input Attack::Request value."));
    FArrayWriter WriterArray; 
    WriterArray.Serialize((UTF8CHAR*)&Request, sizeof(Request));
    TSharedPtr<FBufferArchive> Packet = FTCPSocketClient_Async::CreatePacket((short)ATTACK_REQUEST, WriterArray.GetData(), WriterArray.Num());

    // UE_LOG(LogTemp, Warning, TEXT("End Create Packet / Before Call BeginSendPhase..."));
    // GameMode->GetTCPSocketClient().BeginSendPhase(Packet);
    GI->GetTCPClient()->BeginSendPhase(Packet);
    //GI->GetTCPClient()->BeginRecvPhase(); 

}

void AGoblin::SendGetItemRequestMsg(ActorType ItemType, short ItemTagId)
{
    UE_LOG(LogTemp, Warning, TEXT("[SEND] GETITEM REQUEST MSG"));
    GetItem::Request Request; 
    Request.ItemType = ItemType;
    Request.ItemTagId = ItemTagId;
    Request.PlayerTagId = GetTagId(); 
    Request.Location = Vector(GetActorLocation()); 
    Request.IncreaseVal = 10; 

    FArrayWriter WriterArray; 
    WriterArray.Serialize((UTF8CHAR*)&Request, sizeof(Request));
    TSharedPtr<FBufferArchive> Packet = FTCPSocketClient_Async::CreatePacket((short)GETITEM_REQUEST, WriterArray.GetData(), WriterArray.Num());
    // GameMode->GetTCPSocketClient().BeginSendPhase(Packet);
    GI->GetTCPClient()->BeginSendPhase(Packet);
}


/////////////////////////////////////////////////////
//void AGoblin::SendMoveResponseMsg(int ActorType, int ActorIndex, bool bMoveUp, bool bMoveDown, bool bMoveRight, bool bMoveLeft)
//{
//    struct Move::Response *response = new Move::Response(); 
//    response->H.Command = 3;//0x11; 
//    response->ActorType = ActorType;
//    response->ActorIndex = ActorIndex; 
//    response->bMoveUp = bMoveUp; 
//    response->bMoveDown = bMoveDown; 
//    response->bMoveRight = bMoveRight; 
//    response->bMoveLeft = bMoveLeft; 
//    response->Speed = Speed;
//    GameMode->messageQueue.push((struct HEAD *)response);
//}
//
//void AGoblin::SendMoveNotiMsg(int actorType, int actorIndex, float X, float Y)
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
//void AGoblin::SendAttackResponseMsg(int attackerType, int attackerIndex, int targetType, int targetIndex, int damage)
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
//void AGoblin::SendAttackNotiMsg(int attackerType, int attackerIndex, int targetType, int targetIndex, int damage, int targetHp, float X, float Y)
//{
//    struct Attack::Notification *noti = new Attack::Notification(); 
//    noti->H.Command = 6;//0x22; 
//    noti->AttackerType = attackerType; 
//    noti->AttackerIndex = attackerIndex; 
//    noti->targetType = targetType; 
//    noti->targetIndex = targetIndex; 
//    noti->Damage = damage; 
//    noti->targetHp = targetHp; 
//    noti->X = X; 
//    noti->Y = Y; 
//    GameMode->messageQueue.push((struct HEAD *)noti);
//}
//
//void AGoblin::SendGetItemResponseMsg(int itemType)
//{
//    struct GetItem::Response *response = new GetItem::Response(); 
//    response->H.Command = 9;//0x41; 
//    response->successyn = 1; 
//    response->playerIndex = GetTagId();
//    response->ItemType = itemType; 
//    GameMode->messageQueue.push((struct HEAD *)response);
//}
//
//void AGoblin::SendGetItemNotiMsg(int itemType, int itemIndex, float X, float Y)
//{
//    struct GetItem::Notification *noti = new GetItem::Notification(); 
//    noti->H.Command = 10;//0x42; 
//    noti->playerIndex = GetTagId(); 
//    noti->playerHp = GetHealth(); 
//    noti->playerCoin = GetMoneyCount(); 
//    noti->ItemType = itemType; 
//    noti->ItemIndex = itemIndex; 
//    noti->X = X; 
//    noti->Y = Y; 
//    GameMode->messageQueue.push((struct HEAD *)noti);
//}
//
//void AGoblin::SendDestroyResponseMsg(int actorType, int actorIndex, float X, float Y)
//{
//    struct Destroy::Response *response = new Destroy::Response(); 
//    response->H.Command = 15;//0x71; 
//    response->ActorType = actorType; 
//    response->ActorIndex = actorIndex; 
//    response->X = X; 
//    response->Y = Y; 
//    GameMode->messageQueue.push((struct HEAD *)response);
//    UE_LOG(LogTemp, Warning, TEXT("Send Destroy Response Message..."));
//
//}
//
//void AGoblin::SendDestroyNotiMsg(int actorType, int actorIndex, float X, float Y)
//{
//    struct Destroy::Notification *noti = new Destroy::Notification(); 
//    noti->H.Command = 16;//0x72;
//    noti->ActorType = actorType; 
//    noti->ActorIndex = actorIndex; 
//    noti->X = X; 
//    noti->Y = Y; 
//    GameMode->messageQueue.push((struct HEAD *)noti);
//
//}

