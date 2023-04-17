#pragma once
#ifndef __RANGED_COMBAT_ENEMY_H__
#define __RANGED_COMBAT_ENEMY_H__

#include "Agent.h"
#include "DecisionTree.h"
#include "AttackAction.h"

class RangedCombatEnemy : public Agent
{
public:
	// Constructor Function
	RangedCombatEnemy(Scene* scene);

	// Destructor Function
	~RangedCombatEnemy();

	// LifeCycle Functions
	void Draw() override;
	void Update() override;
	void Clean() override;

	// getters and setters (accessors and mutators)
	[[nodiscard]] float GetMaxSpeed() const;
	[[nodiscard]] float GetTurnRate() const;
	[[nodiscard]] float GetAccelerationRate() const;
	[[nodiscard]] glm::vec2 GetDesiredVelocity() const;
	[[nodiscard]] float GetMinRange() const;
	[[nodiscard]] float GetMaxRange() const;

	void SetMaxSpeed(float speed);
	void SetTurnRate(float angle);
	void SetAccelerationRate(float rate);
	void SetDesiredVelocity(glm::vec2 target_position);

	// public functions
	void Seek();
	void LookWhereYoureGoing(glm::vec2 target_direction, bool direction_hack = true);
	void Reset();

	// New for Lab 7.2
	void Patrol() override;
	DecisionTree* GetTree() const;

	// New for Lab 7.3
	void Flee() override;
	void MoveToLOS() override;
	void MoveToCover() override;
	void WaitBehindCover() override;
	void Attack() override;

	// New for lab 8
	void MoveToRange() override;

private:
	// private movement variables
	float m_maxSpeed;
	float m_turnRate;
	float last_rotation;
	float m_accelerationRate;
	glm::vec2 m_startPosition;

	// where we want to go
	glm::vec2 m_desiredVelocity;

	// private function
	void m_move();

	// New for Lab 7.1
	std::vector<glm::vec2> m_patrolPath;
	int m_wayPoint;
	void m_buildPatrolPath();

	// New for Lab 7.2
	DecisionTree* m_tree;
	void m_buildTree();
	void m_buildLeftTree();
	void m_buildRightTree();

	// New for Lab 8.
	int m_fireCounter; // Number of frames that have elapsed
	int m_fireCounterMax; // Frame delay
	Scene* m_pScene; // PlayScene
	bool m_movingTowardsPlayer;

	float min_range;
	float max_range;
};

#endif /* defined (__RANGED_COMBAT_ENEMY_H__)*/

