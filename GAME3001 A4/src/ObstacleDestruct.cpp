#include "ObstacleDestruct.h"

#include <iostream>

#include "TextureManager.h"
#include "Util.h"

ObstacleDestruct::ObstacleDestruct()
{
    TextureManager::Instance().Load("../Assets/sprites/box.png", "ObstacleDestruct");

    const auto size = TextureManager::Instance().GetTextureSize("ObstacleDestruct");
    SetWidth(static_cast<int>(size.x));
    SetHeight(static_cast<int>(size.y));
    SetHealth(100);
    SetMaxHealth(100);
    GetTransform()->position = glm::vec2(400.0f, 300.0f);
    SetType(GameObjectType::DESTRUCT_OBSTACLE);
    GetRigidBody()->isColliding = false;
}

void ObstacleDestruct::Draw()
{
    TextureManager::Instance().Draw("ObstacleDestruct", GetTransform()->position, this, 0, 255, true);
    if (GetHealth() > 0)
    {
        Util::DrawFilledRect(GetTransform()->position - glm::vec2((GetHealth() / GetMaxHealth() * 100) / 2, 60.0f), GetHealth() / GetMaxHealth() * 100, 10.0f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    }
}

void ObstacleDestruct::Update()
{
    if(GetHealth()<=0)
    {
        SetDeleteMe(true);
    }
}

void ObstacleDestruct::Clean()
{
    
}

void ObstacleDestruct::SetHealth(float health)
{
    m_health = health;
}

float ObstacleDestruct::GetHealth()
{
    return m_health;
}

void ObstacleDestruct::SetMaxHealth(float health)
{
    m_maxHealth = health;
}

float ObstacleDestruct::GetMaxHealth()
{
    return m_maxHealth;
}

void ObstacleDestruct::TakeDamage(float dmg)
{
    std::cout << "Obstacle took: " << dmg << " damage.\n";
    m_health-=dmg;
}


