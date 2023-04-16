#include "Player.h"
#include "TextureManager.h"

Player::Player(): m_currentAnimationState(PlayerAnimationState::PLAYER_IDLE_RIGHT)
{
	TextureManager::Instance().LoadSpriteSheet(
		"../Assets/sprites/Player.txt",
		"../Assets/sprites/Player.png", 
		"Player");


	SetSpriteSheet(TextureManager::Instance().GetSpriteSheet("Player"));
	
	
	// set frame width
	SetWidth(80);

	// set frame height
	SetHeight(120);

	GetTransform()->position = glm::vec2(400.0f, 300.0f);
	GetRigidBody()->velocity = glm::vec2(0.0f, 0.0f);
	GetRigidBody()->acceleration = glm::vec2(0.0f, 0.0f);
	GetRigidBody()->isColliding = false;
	SetType(GameObjectType::PLAYER);

	BuildAnimations();
}

Player::~Player()
= default;

void Player::Draw()
{
	// draw the player according to animation state
	switch(m_currentAnimationState)
	{
	case PlayerAnimationState::PLAYER_IDLE_RIGHT:
		TextureManager::Instance().PlayAnimation("Player", GetAnimation("idle"),
			GetTransform()->position, 0.12f, 0, 255, true);
		break;
	case PlayerAnimationState::PLAYER_IDLE_LEFT:
		TextureManager::Instance().PlayAnimation("Player", GetAnimation("idle"),
			GetTransform()->position, 0.12f, 0, 255, true, SDL_FLIP_HORIZONTAL);
		break;
	case PlayerAnimationState::PLAYER_RUN_RIGHT:
		TextureManager::Instance().PlayAnimation("Player", GetAnimation("run"),
			GetTransform()->position, 0.12f, 0, 255, true);
		break;
	case PlayerAnimationState::PLAYER_RUN_LEFT:
		TextureManager::Instance().PlayAnimation("Player", GetAnimation("run"),
			GetTransform()->position, 0.12f, 0, 255, true, SDL_FLIP_HORIZONTAL);
		break;
	case PlayerAnimationState::PLAYER_ATTACK_CLOSE:
		TextureManager::Instance().PlayAnimation("Player", GetAnimation("close"),
			GetTransform()->position, 0.08f, 0, 255, true, SDL_FLIP_HORIZONTAL);
		break;
	case PlayerAnimationState::PLAYER_ATTACK_FIRE:
		TextureManager::Instance().PlayAnimation("Player", GetAnimation("fire"),
			GetTransform()->position, 0.08f, 0, 255, true, SDL_FLIP_HORIZONTAL);
		break;
	default:
		break;
	}
}

void Player::Update()
{
}

void Player::Clean()
{
}

void Player::SetAnimationState(const PlayerAnimationState new_state)
{
	m_currentAnimationState = new_state;
}

void Player::BuildAnimations()
{
	auto idle_animation = Animation();

	idle_animation.name = "idle";
	idle_animation.frames.push_back(GetSpriteSheet()->GetFrame("PlayerIdle-0"));
	idle_animation.frames.push_back(GetSpriteSheet()->GetFrame("PlayerIdle-1"));
	idle_animation.frames.push_back(GetSpriteSheet()->GetFrame("PlayerIdle-2"));
	idle_animation.frames.push_back(GetSpriteSheet()->GetFrame("PlayerIdle-3"));
	idle_animation.frames.push_back(GetSpriteSheet()->GetFrame("PlayerIdle-4"));
	idle_animation.frames.push_back(GetSpriteSheet()->GetFrame("PlayerIdle-5"));
	idle_animation.frames.push_back(GetSpriteSheet()->GetFrame("PlayerIdle-6"));
	idle_animation.frames.push_back(GetSpriteSheet()->GetFrame("PlayerIdle-7"));

	SetAnimation(idle_animation);

	auto run_animation = Animation();

	run_animation.name = "run";
	run_animation.frames.push_back(GetSpriteSheet()->GetFrame("PlayerRun-0"));
	run_animation.frames.push_back(GetSpriteSheet()->GetFrame("PlayerRun-1"));
	run_animation.frames.push_back(GetSpriteSheet()->GetFrame("PlayerRun-2"));
	run_animation.frames.push_back(GetSpriteSheet()->GetFrame("PlayerRun-3"));
	run_animation.frames.push_back(GetSpriteSheet()->GetFrame("PlayerRun-4"));
	run_animation.frames.push_back(GetSpriteSheet()->GetFrame("PlayerRun-5"));
	run_animation.frames.push_back(GetSpriteSheet()->GetFrame("PlayerRun-6"));
	run_animation.frames.push_back(GetSpriteSheet()->GetFrame("PlayerRun-7"));

	SetAnimation(run_animation);

	auto close_animation = Animation();

	close_animation.name = "close";
	close_animation.frames.push_back(GetSpriteSheet()->GetFrame("AttackClose-0"));
	close_animation.frames.push_back(GetSpriteSheet()->GetFrame("AttackClose-1"));
	close_animation.frames.push_back(GetSpriteSheet()->GetFrame("AttackClose-2"));
	close_animation.frames.push_back(GetSpriteSheet()->GetFrame("AttackClose-3"));
	close_animation.frames.push_back(GetSpriteSheet()->GetFrame("AttackClose-4"));
	close_animation.frames.push_back(GetSpriteSheet()->GetFrame("AttackClose-5"));
	close_animation.frames.push_back(GetSpriteSheet()->GetFrame("AttackClose-6"));
	close_animation.frames.push_back(GetSpriteSheet()->GetFrame("AttackClose-7"));

	SetAnimation(close_animation);

	auto fire_animation = Animation();

	fire_animation.name = "fire";
	fire_animation.frames.push_back(GetSpriteSheet()->GetFrame("AttackFire-0"));
	fire_animation.frames.push_back(GetSpriteSheet()->GetFrame("AttackFire-1"));
	fire_animation.frames.push_back(GetSpriteSheet()->GetFrame("AttackFire-2"));
	fire_animation.frames.push_back(GetSpriteSheet()->GetFrame("AttackFire-3"));
	fire_animation.frames.push_back(GetSpriteSheet()->GetFrame("AttackFire-4"));
	fire_animation.frames.push_back(GetSpriteSheet()->GetFrame("AttackFire-5"));
	fire_animation.frames.push_back(GetSpriteSheet()->GetFrame("AttackFire-6"));
	fire_animation.frames.push_back(GetSpriteSheet()->GetFrame("AttackFire-7"));

	SetAnimation(fire_animation);
}
