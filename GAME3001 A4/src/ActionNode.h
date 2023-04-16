#pragma once
#ifndef __ACTION_NODE__
#define __ACTION_NODE__
#include "TreeNode.h"
#include "Agent.h"

// Abstract Class
class ActionNode : public TreeNode
{
public:
	ActionNode() : m_agent(nullptr) { m_isLeaf = true; }
	virtual ~ActionNode() = default;
	virtual void Action() = 0; // abstract method (function) -> Specialization

	Agent* GetAgent() { return m_agent; }
	void SetAgent(Agent* agent) { m_agent = agent; }
private:
	Agent* m_agent;
};

#endif /* defined (__ACTION_NODE__) */