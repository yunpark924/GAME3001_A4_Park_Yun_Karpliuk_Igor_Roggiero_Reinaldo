#pragma once
#ifndef __PLAYER__
#define __PLAYER__

#include "Label.h"
#include "Obstacle.h"
#include "PlayerAnimationState.h"
#include "Sprite.h"
#include "PlaceholderSprite.h"

class Player final : public Sprite
{
public:
	Player();
	~Player() override;

	// Life Cycle Methods
	virtual void Draw() override;
	virtual void Update() override;
	virtual void Clean() override;
	void Move();
	void MeleeAttack();


	// setters
	void SetAnimationState(PlayerAnimationState new_state);
	void SetFlip(SDL_RendererFlip flip);
	void SetHealth(float health);
	void TakeDamage(float dmg);
	void SetRangeOfAttack(float value);

	// Getters
	float GetHealth() const;
	[[nodiscard]] float GetRangeOfAttack();
	[[nodiscard]] float GetDamage();
private:
	void BuildAnimations();
	PlayerAnimationState m_currentAnimationState;
	SDL_RendererFlip m_flip;
	float m_Health;
	float m_maxHealth;
	float m_rangeOfAttack;
	float m_damage;
};

#endif /* defined (__PLAYER__) */