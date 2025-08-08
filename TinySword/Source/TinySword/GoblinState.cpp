// Fill out your copyright notice in the Description page of Project Settings.


#include "GoblinState.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"
#include "GameFramework/PawnMovementComponent.h"
#include "TimerManager.h"

// GoblinState::GoblinState()
// {
// }

// IGoblinState::~IGoblinState()
// {
// }

// Idle 
void FGoblinIdleState::OnEnter(AGoblin *Goblin)
{
    if (Goblin && Goblin->IdleAnim) {
        Goblin->GetPaperFlipbookComponent()->SetFlipbook(Goblin->IdleAnim);
        Goblin->GetPaperFlipbookComponent()->SetLooping(true);
        Goblin->GetPaperFlipbookComponent()->Play();
    }
}

void FGoblinIdleState::Update(AGoblin *Goblin)
{
    if (Goblin && !Goblin->GetVelocity().IsNearlyZero()) {
        Goblin->SetState(Goblin->WalkState.Get());
    }
}

void FGoblinIdleState::OnExit(AGoblin *Goblin)
{
}

// walk
void FGoblinWalkState::OnEnter(AGoblin *Goblin)
{
    if (Goblin && Goblin->WalkAnim) {
        Goblin->GetPaperFlipbookComponent()->SetFlipbook(Goblin->WalkAnim);
        Goblin->GetPaperFlipbookComponent()->SetLooping(true);
        Goblin->GetPaperFlipbookComponent()->Play();
    }
}

void FGoblinWalkState::Update(AGoblin *Goblin)
{
    if (Goblin && Goblin->GetVelocity().IsNearlyZero()) {
        Goblin->SetState(Goblin->IdleState.Get());
    }
}

void FGoblinWalkState::OnExit(AGoblin *Goblin)
{
}

// attack 
void FGoblinAttackState::OnEnter(AGoblin *Goblin)
{
    if (Goblin && Goblin->AttackAnim) {
        UPaperFlipbookComponent* FlipbookComponent = Goblin->GetPaperFlipbookComponent();
        FlipbookComponent->SetFlipbook(Goblin->AttackAnim);
        FlipbookComponent->SetLooping(false);
        FlipbookComponent->Play();

        float AnimDuration = Goblin->AttackAnim->GetTotalDuration();
        FTimerHandle TimerHandle;
        FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(Goblin, &AGoblin::ResetStateBySpeed);
        Goblin->GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, AnimDuration, false);
    }
}

void FGoblinAttackState::Update(AGoblin *Goblin)
{
    
}

void FGoblinAttackState::OnExit(AGoblin *Goblin)
{
}

// dead 
void FGoblinDeadState::OnEnter(AGoblin *Goblin)
{
    if (Goblin && Goblin->DeadAnim) {
        UPaperFlipbookComponent* FlipbookComponent = Goblin->GetPaperFlipbookComponent();
        FlipbookComponent->Stop();
        FlipbookComponent->SetFlipbook(Goblin->DeadAnim);
        FlipbookComponent->SetLooping(false);
        FlipbookComponent->Play();
    }
}

void FGoblinDeadState::Update(AGoblin *Goblin)
{
}

void FGoblinDeadState::OnExit(AGoblin *Goblin)
{
}