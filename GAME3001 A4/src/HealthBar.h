#pragma once
#ifndef __HEALTH_BAR__
#define __HEALTH_BAR__
#include "Sprite.h"
#include "PlayerAnimationState.h"

class HealthBar: public Sprite
{
public:
	HealthBar();
	~HealthBar() override;

	void Draw() override;
	void Update() override;
	void Clean() override;

	void SetAnimationState(HPAnimationState state);
private:
	void BuildAnimations();
	HPAnimationState m_currentAnimationState;
};

class EnemyHealthBar : public HealthBar
{
public:
	EnemyHealthBar();
	~EnemyHealthBar() override;
	void Draw() override;
};

#endif