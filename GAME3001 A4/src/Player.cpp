#include "Player.h"

#include "EventManager.h"
#include "Game.h"
#include "TextureManager.h"
#include "Util.h"

Player::Player() : m_currentAnimationState(PlayerAnimationState::PLAYER_IDLE_RIGHT)
{
	TextureManager::Instance().LoadSpriteSheet(
		"../Assets/sprites/linkP.txt",
		"../Assets/sprites/linkP.png",
		"linkP");

	SetSpriteSheet(TextureManager::Instance().GetSpriteSheet("linkP"));

	SetRangeOfAttack(50.0f);

	// set frame width
	SetWidth(56);

	// set frame height
	SetHeight(52);

	GetTransform()->position = glm::vec2(400.0f, 300.0f);
	GetRigidBody()->velocity = glm::vec2(0.0f, 0.0f);
	GetRigidBody()->velocityDampening = glm::vec2(0.90f, 0.90f);
	GetRigidBody()->acceleration = glm::vec2(0.0f, 0.0f);
	GetRigidBody()->isColliding = false;
	SetType(GameObjectType::PLAYER);
	SetHealth(100.0f);

	m_damage = 10.0f;

	BuildAnimations();
}

Player::~Player()
= default;

void Player::Draw()
{
	// If we are in debug mode, draw the collider rect.
	if (Game::Instance().GetDebugMode())
	{
		Util::DrawRect(GetTransform()->position -
			glm::vec2(this->GetWidth() * 0.5f, this->GetHeight() * 0.5f),
			this->GetWidth(), this->GetHeight());

		Util::DrawCircle(GetTransform()->position, GetRangeOfAttack());

	}

	//// Drawing the health bar for the player based on the health count.
	//if (m_Health > 0)
	//{
	//	Util::DrawFilledRect(GetTransform()->position - glm::vec2((m_Health / m_maxHealth * 100) / 2, 60.0f), m_Health / m_maxHealth * 100, 10.0f, glm::vec4(0, 1.0f, 0, 1.0f));
	//} else
	//{
	//	
	//}

	// draw the player according to animation state
	switch (m_currentAnimationState)
	{
	case PlayerAnimationState::PLAYER_IDLE_RIGHT:
		TextureManager::Instance().PlayAnimation("linkP", GetAnimation("idle"),
			GetTransform()->position, 0.12f, 0, 255, true);
		break;
	case PlayerAnimationState::PLAYER_IDLE_LEFT:
		TextureManager::Instance().PlayAnimation("linkP", GetAnimation("idle"),
			GetTransform()->position, 0.12f, 0, 255, true);
		break;
	case PlayerAnimationState::PLAYER_RUN_RIGHT:
		TextureManager::Instance().PlayAnimation("linkP", GetAnimation("run_right"),
			GetTransform()->position, 0.85f, 0, 255, true);
		break;
	case PlayerAnimationState::PLAYER_RUN_LEFT:
		TextureManager::Instance().PlayAnimation("linkP", GetAnimation("run"),
			GetTransform()->position, 0.85f, 0, 255, true);
		break;
	case PlayerAnimationState::PLAYER_RUN_DOWN:
		TextureManager::Instance().PlayAnimation("linkP", GetAnimation("run_back"),
			GetTransform()->position, 0.85f, 0, 255, true);
		break;
	case PlayerAnimationState::PLAYER_RUN_UP:
		TextureManager::Instance().PlayAnimation("linkP", GetAnimation("run_front"),
			GetTransform()->position, 0.95f, 0, 255, true);
		break;
	default:
		break;
	}
}

void Player::Update()
{
	Move();
	if (Util::Magnitude(GetRigidBody()->velocity) <= 10)
	{
		if (m_currentAnimationState != PlayerAnimationState::PLAYER_IDLE_LEFT && m_currentAnimationState != PlayerAnimationState::PLAYER_IDLE_RIGHT)
		{
			if (m_currentAnimationState == PlayerAnimationState::PLAYER_RUN_LEFT)
			{
				SetAnimationState(PlayerAnimationState::PLAYER_IDLE_LEFT);
			}
			else
			{
				SetAnimationState(PlayerAnimationState::PLAYER_IDLE_RIGHT);
			}
		}

	}
}

void Player::Clean()
{
}

void Player::Move()
{
	if (GetRigidBody()->isColliding)
	{
		GetRigidBody()->velocity = -GetRigidBody()->velocity * 1.05f;
	}
	const float dt = Game::Instance().GetDeltaTime();
	const glm::vec2 initial_position = GetTransform()->position;
	const glm::vec2 velocity_term = GetRigidBody()->velocity * dt;
	const glm::vec2 acceleration_term = GetRigidBody()->acceleration * 0.5f;
	const glm::vec2 final_position = initial_position + velocity_term + acceleration_term;
	GetTransform()->position = final_position;
	GetRigidBody()->velocity += GetRigidBody()->acceleration;
	GetRigidBody()->velocity *= GetRigidBody()->velocityDampening;
}

void Player::MeleeAttack()
{
	std::cout << "Melee Attack successful on enemy!\n\n";
}


void Player::SetAnimationState(const PlayerAnimationState new_state)
{
	m_currentAnimationState = new_state;
}

void Player::SetHealth(float health)
{
	m_Health = health;
	m_maxHealth = m_Health;
}


float Player::GetHealth() const
{
	return m_Health;
}

float Player::GetRangeOfAttack()
{
	return m_rangeOfAttack;
}

float Player::GetDamage()
{
	return m_damage;
}

void Player::TakeDamage(float dmg)
{
	m_Health -= dmg;
	std::cout << "Player took " << dmg << " damage.\n";
}

void Player::SetRangeOfAttack(float value)
{
	m_rangeOfAttack = value;
}

void Player::BuildAnimations()
{
	auto idle_animation = Animation();

	idle_animation.name = "idle";
	idle_animation.frames.push_back(GetSpriteSheet()->GetFrame("idle1"));


	SetAnimation(idle_animation);

	auto run_back_animation = Animation();

	run_back_animation.name = "run_back";
	run_back_animation.frames.push_back(GetSpriteSheet()->GetFrame("run_back1"));
	run_back_animation.frames.push_back(GetSpriteSheet()->GetFrame("run_back2"));
	run_back_animation.frames.push_back(GetSpriteSheet()->GetFrame("run_back3"));
	run_back_animation.frames.push_back(GetSpriteSheet()->GetFrame("run_back4"));
	run_back_animation.frames.push_back(GetSpriteSheet()->GetFrame("run_back5"));
	run_back_animation.frames.push_back(GetSpriteSheet()->GetFrame("run_back6"));
	run_back_animation.frames.push_back(GetSpriteSheet()->GetFrame("run_back7"));

	SetAnimation(run_back_animation);

	auto run_animation = Animation();

	run_animation.name = "run";
	run_animation.frames.push_back(GetSpriteSheet()->GetFrame("run1"));
	run_animation.frames.push_back(GetSpriteSheet()->GetFrame("run2"));
	run_animation.frames.push_back(GetSpriteSheet()->GetFrame("run3"));
	run_animation.frames.push_back(GetSpriteSheet()->GetFrame("run4"));
	run_animation.frames.push_back(GetSpriteSheet()->GetFrame("run5"));
	run_animation.frames.push_back(GetSpriteSheet()->GetFrame("run6"));
	run_animation.frames.push_back(GetSpriteSheet()->GetFrame("run7"));

	SetAnimation(run_animation);

	auto run_righta_nimation = Animation();

	run_righta_nimation.name = "run_right";
	run_righta_nimation.frames.push_back(GetSpriteSheet()->GetFrame("run_right1"));
	run_righta_nimation.frames.push_back(GetSpriteSheet()->GetFrame("run_right2"));
	run_righta_nimation.frames.push_back(GetSpriteSheet()->GetFrame("run_right3"));
	run_righta_nimation.frames.push_back(GetSpriteSheet()->GetFrame("run_right4"));
	run_righta_nimation.frames.push_back(GetSpriteSheet()->GetFrame("run_right5"));
	run_righta_nimation.frames.push_back(GetSpriteSheet()->GetFrame("run_right6"));
	run_righta_nimation.frames.push_back(GetSpriteSheet()->GetFrame("run_right7"));

	SetAnimation(run_righta_nimation);

	auto run_front_animation = Animation();

	run_front_animation.name = "run_front";
	run_front_animation.frames.push_back(GetSpriteSheet()->GetFrame("run_front1"));
	run_front_animation.frames.push_back(GetSpriteSheet()->GetFrame("run_front2"));
	run_front_animation.frames.push_back(GetSpriteSheet()->GetFrame("run_front3"));
	run_front_animation.frames.push_back(GetSpriteSheet()->GetFrame("run_front4"));
	run_front_animation.frames.push_back(GetSpriteSheet()->GetFrame("run_front5"));
	run_front_animation.frames.push_back(GetSpriteSheet()->GetFrame("run_front6"));
	run_front_animation.frames.push_back(GetSpriteSheet()->GetFrame("run_front7"));
	run_front_animation.frames.push_back(GetSpriteSheet()->GetFrame("run_front8"));

	SetAnimation(run_front_animation);
}
