#pragma once
#ifndef __ENEMY__
#define __ENEMY__

#include "Agent.h"
#include "PlayerAnimationState.h"

class Enemy final : public Agent {
public:
	Enemy();
	~Enemy() override;

	// Inherited via GameObject
	virtual void Draw() override;
	virtual void Update() override;
	virtual void Clean() override;

	// getters and setters (accessors and mutators)
	float GetMaxSpeed() const;
	float GetTurnRate() const;
	float GetAccelerationRate() const;
	glm::vec2 GetDesiredVelocity() const;

	void SetMaxSpeed(float speed);
	void SetTurnRate(float angle);
	void SetAccelerationRate(float rate);
	void SetDesiredVelocity(glm::vec2 target_position);

	// public functions
	void Seek();
	void LookWhereYoureGoing(glm::vec2 target_direction);
	void IsPatrolling(bool state);
	bool GetPatrollingEnabled();

	// setters
	void SetAnimationState(EnemyAnimatioState new_state);

	float m_detecRadius;
private:
	// private movement variables
	float m_maxSpeed;
	float m_turnRate;
	float m_accelerationRate;

	// patrol for ship
	std::vector<glm::vec2> m_patrol;
	int m_waypoint;
	bool m_isPatrolling;

	// where we want to go
	glm::vec2 m_desiredVelocity;

	// private function
	void m_move();

	void BuildAnimations();

	EnemyAnimatioState m_currentAnimationState;
};


#endif /* defined (__ENEMY__) */