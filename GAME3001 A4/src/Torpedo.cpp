#include "Torpedo.h"
#include "TextureManager.h"

TorpedoPool::TorpedoPool()
{

}

TorpedoPool::~TorpedoPool()
= default;

void TorpedoPool::Draw()
{
	for (const auto torpedo : m_pTorpedos)
	{
		torpedo->Draw();
	}
}

void TorpedoPool::Update()
{
	for (size_t i = 0; i < m_pTorpedos.size(); i++)
	{
		if (m_pTorpedos[i]->GetDeleteMe())
		{
			delete m_pTorpedos[i];
			m_pTorpedos[i] = nullptr;
			m_pTorpedos.erase(i + m_pTorpedos.begin());
			m_pTorpedos.shrink_to_fit();
		} else
		{
			m_pTorpedos[i]->Update();
		}
	}
}

void TorpedoPool::Clean()
{
	for (auto torpedo : m_pTorpedos)
	{
		delete torpedo;
		torpedo = nullptr;
	}
	m_pTorpedos.clear();
	m_pTorpedos.shrink_to_fit();
}

void TorpedoPool::FireTorpedo(Torpedo* torpedoToFire)
{
	m_pTorpedos.push_back(torpedoToFire);
}

std::vector<Torpedo*> TorpedoPool::GetPool()
{
	return m_pTorpedos;
}



Torpedo::Torpedo()
{
	m_currentAnimationState = TorpedoAnimationState::FIRED;
	m_speed = 0.0f;
}

Torpedo::~Torpedo()
= default;

// Renders the torpedo based on their position.
void Torpedo::Draw()
{
	// draw the Torpedo according to animation state
	switch (m_currentAnimationState)
	{
	case TorpedoAnimationState::FIRED:
		TextureManager::Instance().PlayAnimation(m_textureKey, GetAnimation("fire"),
			GetTransform()->position, 0.25f, 0, 255, true);
		break;
	default:
		break;
	}
}

// Updates the torpedo's position
void Torpedo::Update()
{
	if (!m_deleteMe)
	{
		GetTransform()->position += m_direction;
		if (GetRigidBody()->isColliding ||
			GetTransform()->position.x < 0 || GetTransform()->position.x > Config::SCREEN_WIDTH ||
			GetTransform()->position.y < 0 || GetTransform()->position.y > Config::SCREEN_HEIGHT)
		{
			m_deleteMe = true;
		}
	}
}

// Cleanup!
void Torpedo::Clean()
{
}

void Torpedo::SetAnimationState(const TorpedoAnimationState new_state)
{
	m_currentAnimationState = new_state;
}

bool Torpedo::GetDeleteMe()
{
	return m_deleteMe;
}

void Torpedo::SetDeleteMe(bool temp)
{
	m_deleteMe = temp;
}

TORPEDO_TYPE Torpedo::GetTorpedoType()
{
	return m_type;
}

void Torpedo::SetTorpedoType(TORPEDO_TYPE type)
{
	m_type = type;
}

// Builds base class animations!
void Torpedo::BuildAnimations()
{
	auto fire_animation = Animation();

	fire_animation.name = "fire";
	fire_animation.frames.push_back(GetSpriteSheet()->GetFrame("fired1"));
	fire_animation.frames.push_back(GetSpriteSheet()->GetFrame("fired2"));
	fire_animation.frames.push_back(GetSpriteSheet()->GetFrame("fired3"));
	fire_animation.frames.push_back(GetSpriteSheet()->GetFrame("fired4"));
	fire_animation.frames.push_back(GetSpriteSheet()->GetFrame("fired5"));
	fire_animation.frames.push_back(GetSpriteSheet()->GetFrame("fired6"));
	fire_animation.frames.push_back(GetSpriteSheet()->GetFrame("fired7"));
	fire_animation.frames.push_back(GetSpriteSheet()->GetFrame("fired8"));

	SetAnimation(fire_animation);
}


