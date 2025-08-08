
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Goblin.h"

/**
 * 
 */
class TINYSWORD_API IGoblinState
{
public:
	// GoblinState();
	virtual ~IGoblinState() {};

	virtual void OnEnter(AGoblin* Goblin) = 0; 
	virtual void Update(AGoblin* Goblin) = 0; 
	virtual void OnExit(AGoblin* Goblin) = 0; 
};


class FGoblinIdleState : public IGoblinState
{
public:
	virtual void OnEnter(AGoblin* Goblin) override;
	virtual void Update(AGoblin* Goblin) override; 
	virtual void OnExit(AGoblin* Goblin) override; 
};

class FGoblinWalkState : public IGoblinState
{
public:
	virtual void OnEnter(AGoblin* Goblin) override;
	virtual void Update(AGoblin* Goblin) override; 
	virtual void OnExit(AGoblin* Goblin) override; 
};

class FGoblinAttackState : public IGoblinState
{
public:
	virtual void OnEnter(AGoblin* Goblin) override;
	virtual void Update(AGoblin* Goblin) override; 
	virtual void OnExit(AGoblin* Goblin) override; 
};

class FGoblinDeadState : public IGoblinState
{
public:
	virtual void OnEnter(AGoblin* Goblin) override;
	virtual void Update(AGoblin* Goblin) override; 
	virtual void OnExit(AGoblin* Goblin) override; 
};