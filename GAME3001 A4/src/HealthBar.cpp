#include "HealthBar.h"
#include "TextureManager.h"

HealthBar::HealthBar()
{
	TextureManager::Instance().LoadSpriteSheet(
		"../Assets/sprites/Heart Meter.txt",
		"../Assets/sprites/Heart Meter Sprites.png",
		"Health Bar");


	SetSpriteSheet(TextureManager::Instance().GetSpriteSheet("Health Bar"));

	// set frame width & height
	SetWidth(64);
	SetHeight(16);

	GetTransform()->position = glm::vec2(40.0f, 20.0f);
	GetRigidBody()->bounds = glm::vec2(GetWidth(), GetHeight());
	setIsCentered(true);
	GetRigidBody()->isColliding = false;

	SetAnimationState(HPAnimationState::FULL_HP);

	BuildAnimations();
}

HealthBar::~HealthBar() = default;

void HealthBar::Draw()
{
	switch (m_currentAnimationState)
	{
	case HPAnimationState::FULL_HP:
		TextureManager::Instance().PlayAnimation("Health Bar", GetAnimation("fullhp"),
			GetTransform()->position, 0.0f, 0, 255, true);
		break;
	case HPAnimationState::THREE_QUARTERS_HP:
		TextureManager::Instance().PlayAnimation("Health Bar", GetAnimation("75hp"),
			GetTransform()->position, 0.0f, 0, 255, true);
		break;
	case HPAnimationState::HALF_HP:
		TextureManager::Instance().PlayAnimation("Health Bar", GetAnimation("50hp"),
			GetTransform()->position, 0.0f, 0, 255, true);
		break;
	case HPAnimationState::QUARTER_HP:
		TextureManager::Instance().PlayAnimation("Health Bar", GetAnimation("25hp"),
			GetTransform()->position, 0.0f, 0, 255, true);
		break;
	case HPAnimationState::NO_HP:
		TextureManager::Instance().PlayAnimation("Health Bar", GetAnimation("0hp"),
			GetTransform()->position, 0.0f, 0, 255, true);
		break;
	}
}

void HealthBar::Update()
{
}

void HealthBar::Clean()
{
}

void HealthBar::SetAnimationState(HPAnimationState state)
{
	m_currentAnimationState = state;
}

void HealthBar::BuildAnimations()
{
	auto full_hp = Animation();

	full_hp.name = "fullhp";
	full_hp.frames.push_back(GetSpriteSheet()->GetFrame("FullHP"));

	SetAnimation(full_hp);

	auto three_quarter_hp = Animation();

	three_quarter_hp.name = "75hp";
	three_quarter_hp.frames.push_back(GetSpriteSheet()->GetFrame("75HP"));

	SetAnimation(three_quarter_hp);

	auto half_hp = Animation();

	half_hp.name = "50hp";
	half_hp.frames.push_back(GetSpriteSheet()->GetFrame("50HP"));

	SetAnimation(half_hp);

	auto quarter_hp = Animation();

	quarter_hp.name = "25hp";
	quarter_hp.frames.push_back(GetSpriteSheet()->GetFrame("25HP"));

	SetAnimation(quarter_hp);

	auto no_hp = Animation();

	no_hp.name = "0hp";
	no_hp.frames.push_back(GetSpriteSheet()->GetFrame("0HP"));

	SetAnimation(no_hp);
}

EnemyHealthBar::EnemyHealthBar()
{
}

EnemyHealthBar::~EnemyHealthBar()
{
}

void EnemyHealthBar::Draw()
{
	HealthBar::Draw();
}