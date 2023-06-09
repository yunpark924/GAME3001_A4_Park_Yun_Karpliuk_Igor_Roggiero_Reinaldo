#pragma once
#ifndef __RIGID_BODY__
#define __RIGID_BODY__
#include <glm/vec2.hpp>

struct RigidBody
{
	float mass = 0.0f;
	glm::vec2 bounds;

	glm::vec2 velocity;
	glm::vec2 velocityDampening;
	glm::vec2 acceleration;
	bool isColliding = false;
};
#endif /* defined (__RIGID_BODY__) */