#include "Enemy.h"
#include "Game.h"
#include "TextureManager.h"
#include "Util.h"


Enemy::Enemy()
{
	TextureManager::Instance().LoadSpriteSheet(
		"../Assets/sprites/enemy.txt",
		"../Assets/sprites/Enemy.png",
		"Enemy");
	
	SetSpriteSheet(TextureManager::Instance().GetSpriteSheet("Enemy"));
	
	// set frame width & height
	SetWidth(80);
	SetHeight(100);

	/*TextureManager::Instance().Load("../Assets/textures/reliant_small.png", "starship");
	const auto size = TextureManager::Instance().GetTextureSize("starship");
	SetWidth(static_cast<int>(size.x));
	SetHeight(static_cast<int>(size.y));*/

	//TODO: we need to play animation somewhere

	GetTransform()->position = glm::vec2(100.0f, 100.0f);
	GetRigidBody()->velocity = glm::vec2(0, 0);
	GetRigidBody()->acceleration = glm::vec2(0.0f, 0.0f);
	GetRigidBody()->isColliding = false;
	setIsCentered(true);
	SetType(GameObjectType::AGENT);

	// Starting Motion Properties
	m_maxSpeed = 20.0f; // a maximum number of pixels moved per frame
	m_turnRate = 5.0f; // a maximum number of degrees to turn each time-step
	m_accelerationRate = 0.0f; // a maximum number of pixels to add to the velocity each frame
	m_detecRadius = 350.0f;

	SetCurrentDirection(glm::vec2(1.0f, 0.0f)); // Facing Right

	SetLOSDistance(400.0f);
	SetLOSColour(glm::vec4(1, 0, 0, 1));

	SetActionState(ActionState::NO_ACTION);
	m_isPatrolling = false;
	SetAnimationState(EnemyAnimatioState::ENEMY_IDLE_LEFT);

	// setup patrol
	m_patrol.push_back(glm::vec2(760.0f, 40.0f));
	m_patrol.push_back(glm::vec2(760.0f, 560.0f));
	m_patrol.push_back(glm::vec2(40.0f, 560.0f));
	m_patrol.push_back(glm::vec2(40.0f, 40.0f));
	m_waypoint = 0;
	SetTargetPosition(m_patrol[m_waypoint]);

	BuildAnimations();
}

Enemy::~Enemy()
= default;

void Enemy::Draw()
{
	// draw the target
	//TextureManager::Instance().Draw("Enemy",
	//	GetTransform()->position, static_cast<double>(GetCurrentHeading()), 255, true);
	// draw LOS
	//Util::DrawLine(GetTransform()->position,
	//	GetTransform()->position + GetCurrentDirection() * GetLOSDistance(),
	//	GetLOSColour());

	//draw enemy animation state
	switch (m_currentAnimationState)
	{
	case EnemyAnimatioState::ENEMY_IDLE_RIGHT:
		TextureManager::Instance().PlayAnimation("Enemy", GetAnimation("idle"),
			GetTransform()->position, 0.08f, 0, 255, true);
		break;
	case EnemyAnimatioState::ENEMY_IDLE_LEFT:
		TextureManager::Instance().PlayAnimation("Enemy", GetAnimation("idle"),
			GetTransform()->position, 0.08f, 0, 255, true, SDL_FLIP_HORIZONTAL);
		break;
	case EnemyAnimatioState::ENEMY_RUN_RIGHT:
		TextureManager::Instance().PlayAnimation("Enemy", GetAnimation("walk"),
			GetTransform()->position, 0.25f, 0, 255, true);
		break;
	case EnemyAnimatioState::ENEMY_RUN_LEFT:
		TextureManager::Instance().PlayAnimation("Enemy", GetAnimation("walk"),
			GetTransform()->position, 0.25f, 0, 255, true, SDL_FLIP_HORIZONTAL);
		break;
	case EnemyAnimatioState::ENEMY_HIT:
		TextureManager::Instance().PlayAnimation("Enemy", GetAnimation("hit"),
			GetTransform()->position, 0.08f, 0, 255, true, SDL_FLIP_HORIZONTAL);
		break;
	case EnemyAnimatioState::ENEMY_DIE:
		TextureManager::Instance().PlayAnimation("Enemy", GetAnimation("die"),
			GetTransform()->position, 0.08f, 0, 255, true, SDL_FLIP_HORIZONTAL);
		break;
	default:
		break;
	}
}

void Enemy::Update()
{
	switch (GetActionState())
	{
	case ActionState::PATROL:
		m_isPatrolling = true;
		SetAccelerationRate(4.0f);
		SetAnimationState(EnemyAnimatioState::ENEMY_RUN_LEFT);
		m_move();

		break;
	case ActionState::NO_ACTION:
		m_isPatrolling = false;
		SetAccelerationRate(0.0f);
		SetAnimationState(EnemyAnimatioState::ENEMY_IDLE_LEFT);
		break;
	}
}

void Enemy::Clean()
{
}

float Enemy::GetMaxSpeed() const
{
	return m_maxSpeed;
}

float Enemy::GetTurnRate() const
{
	return m_turnRate;
}

float Enemy::GetAccelerationRate() const
{
	return m_accelerationRate;
}

glm::vec2 Enemy::GetDesiredVelocity() const
{
	return m_desiredVelocity;
}

void Enemy::SetMaxSpeed(const float speed)
{
	m_maxSpeed = speed;
}

void Enemy::SetTurnRate(const float angle)
{
	m_turnRate = angle;
}

void Enemy::SetAccelerationRate(const float rate)
{
	m_accelerationRate = rate;
}

void Enemy::SetDesiredVelocity(const glm::vec2 target_position)
{
	m_desiredVelocity = Util::Normalize(target_position - GetTransform()->position);
}

void Enemy::Seek()
{
	// find next waypoint
	if (Util::Distance(m_patrol[m_waypoint], GetTransform()->position) <= 10.0f)
	{
		if (++m_waypoint == m_patrol.size())
			m_waypoint = 0;
		SetTargetPosition(m_patrol[m_waypoint]);
	}

	SetDesiredVelocity(GetTargetPosition());

	const glm::vec2 steering_direction = GetDesiredVelocity() - GetCurrentDirection();

	LookWhereYoureGoing(steering_direction);

	GetRigidBody()->acceleration = GetCurrentDirection() * GetAccelerationRate();
}

void Enemy::LookWhereYoureGoing(const glm::vec2 target_direction)
{
	float target_rotation = Util::SignedAngle(GetCurrentDirection(), target_direction) - 90.0f;

	const float turn_sensitivity = 3.0f;

	if (abs(target_rotation) > turn_sensitivity)
	{
		if (target_rotation > 0.0f)
		{
			SetCurrentHeading(GetCurrentHeading() + GetTurnRate());
		}
		else if (target_rotation < 0.0f)
		{
			SetCurrentHeading(GetCurrentHeading() - GetTurnRate());
		}
	}

	UpdateWhiskers(GetWhiskerAngle());
}

void Enemy::IsPatrolling(bool state)
{
	m_isPatrolling = state;
	if (m_isPatrolling == false)
		SetActionState(ActionState::NO_ACTION);
	else
		SetActionState(ActionState::PATROL);
}

bool Enemy::GetPatrollingEnabled()
{
	return m_isPatrolling;
}

void Enemy::SetAnimationState(EnemyAnimatioState new_state)
{
	m_currentAnimationState = new_state;
}

void Enemy::m_move()
{
	Seek();

	//                      final Position  Position Term   Velocity      Acceleration Term
	// Kinematic Equation-> Pf            = Pi +            Vi * (time) + (0.5) * Ai * (time * time)

	const float dt = Game::Instance().GetDeltaTime();

	// accessing the position Term
	const glm::vec2 initial_position = GetTransform()->position;

	// compute the velocity Term
	const glm::vec2 velocity_term = GetRigidBody()->velocity * dt;

	// compute the acceleration Term
	const glm::vec2 acceleration_term = GetRigidBody()->acceleration * 0.5f; // * dt * dt


	// compute the new position
	glm::vec2 final_position = initial_position + velocity_term + acceleration_term;

	GetTransform()->position = final_position;

	// add our acceleration to velocity
	GetRigidBody()->velocity += GetRigidBody()->acceleration;

	// clamp our velocity at max speed
	GetRigidBody()->velocity = Util::Clamp(GetRigidBody()->velocity, GetMaxSpeed());
}

void Enemy::BuildAnimations()
{
	auto hit_animation = Animation();
	
	hit_animation.name = "hit";
	hit_animation.frames.push_back(GetSpriteSheet()->GetFrame("EnemyHit-0"));
	hit_animation.frames.push_back(GetSpriteSheet()->GetFrame("EnemyHit-1"));
	hit_animation.frames.push_back(GetSpriteSheet()->GetFrame("EnemyHit-2"));
	hit_animation.frames.push_back(GetSpriteSheet()->GetFrame("EnemyHit-3"));

	SetAnimation(hit_animation);

	auto idle_animation = Animation();

	idle_animation.name = "idle";
	idle_animation.frames.push_back(GetSpriteSheet()->GetFrame("EnemyIdle-0"));
	idle_animation.frames.push_back(GetSpriteSheet()->GetFrame("EnemyIdle-1"));
	idle_animation.frames.push_back(GetSpriteSheet()->GetFrame("EnemyIdle-2"));
	idle_animation.frames.push_back(GetSpriteSheet()->GetFrame("EnemyIdle-3"));

	SetAnimation(idle_animation);

	auto walk_animation = Animation();

	walk_animation.name = "walk";
	walk_animation.frames.push_back(GetSpriteSheet()->GetFrame("EnemyWalk-0"));
	walk_animation.frames.push_back(GetSpriteSheet()->GetFrame("EnemyWalk-1"));
	walk_animation.frames.push_back(GetSpriteSheet()->GetFrame("EnemyWalk-2"));
	walk_animation.frames.push_back(GetSpriteSheet()->GetFrame("EnemyWalk-3"));

	SetAnimation(walk_animation);

	auto die_animation = Animation();

	die_animation.name = "die";
	die_animation.frames.push_back(GetSpriteSheet()->GetFrame("EnemyDie-0"));
	die_animation.frames.push_back(GetSpriteSheet()->GetFrame("EnemyDie-1"));
	die_animation.frames.push_back(GetSpriteSheet()->GetFrame("EnemyDie-2"));
	die_animation.frames.push_back(GetSpriteSheet()->GetFrame("EnemyDie-3"));

	SetAnimation(die_animation);

}