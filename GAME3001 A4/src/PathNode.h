#pragma once
#ifndef __PATHNODE__
#define __PATHNODE__
#include "Agent.h"

class PathNode : public Agent
{
public:
	PathNode();
	~PathNode();

	// Lifecycle functions
	void Draw() override;
	void Update() override;
	void Clean() override;
private:
};

#endif