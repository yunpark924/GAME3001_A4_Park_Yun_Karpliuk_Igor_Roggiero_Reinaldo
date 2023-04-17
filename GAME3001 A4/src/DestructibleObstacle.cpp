﻿#include "DestructibleObstacle.h"

#include <iostream>

#include "TextureManager.h"
#include "Util.h"

DestructibleObstacle::DestructibleObstacle()
{
    TextureManager::Instance().Load("../Assets/sprites/rock.png", "destruct_obstacle");

    const auto size = TextureManager::Instance().GetTextureSize("destruct_obstacle");
    SetWidth(static_cast<int>(size.x));
    SetHeight(static_cast<int>(size.y));
    SetHealth(100);
    SetMaxHealth(100);
    GetTransform()->position = glm::vec2(400.0f, 300.0f);
    SetType(GameObjectType::DESTRUCT_OBSTACLE);
    GetRigidBody()->isColliding = false;
}

void DestructibleObstacle::Draw()
{
    TextureManager::Instance().Draw("destruct_obstacle", GetTransform()->position, this, 0, 255, true);
  
}

void DestructibleObstacle::Update()
{
    if(GetHealth()<=0)
    {
        SetDeleteMe(true);
    }
}

void DestructibleObstacle::Clean()
{
    
}

void DestructibleObstacle::SetHealth(float health)
{
    m_health = health;
}

float DestructibleObstacle::GetHealth()
{
    return m_health;
}

void DestructibleObstacle::SetMaxHealth(float health)
{
    m_maxHealth = health;
}

float DestructibleObstacle::GetMaxHealth()
{
    return m_maxHealth;
}

void DestructibleObstacle::TakeDamage(float dmg)
{
    std::cout << "Obstacle: " << dmg << " damage.\n";
    m_health-=dmg;
}


