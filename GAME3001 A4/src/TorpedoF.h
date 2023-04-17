#pragma once
#ifndef __TORPEDOF_H__
#define __TORPEDOF_H__

#include "Torpedo.h"
#include "TextureManager.h"

class TorpedoFederation final : public Torpedo
{
public:
	TorpedoFederation(float speed, glm::vec2 direction);
private:
	void BuildAnimations() override;

};

#endif //!__TORPEDOF_H__