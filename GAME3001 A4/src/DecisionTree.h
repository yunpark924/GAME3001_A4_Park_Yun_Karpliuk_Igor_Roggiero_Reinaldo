#pragma once
#ifndef __DECISION_TREE__
#define __DECISION_TREE__
#include "Agent.h"
#include "CloseCombatCondition.h"
#include "LOSCondition.h"
#include "DetectCondition.h"
#include "TreeNode.h"
#include "TreeNodeType.h"

class DecisionTree
{
public:
	// constructors and destructor
	DecisionTree();
	DecisionTree(Agent* agent);
	~DecisionTree();

	// getter and setters
	Agent* GetAgent() const;
	LOSCondition* GetLOSNode() const;
	DetectCondition* GetDetectNode();
	CloseCombatCondition* GetCloseCombatNode() const;
	void SetAgent(Agent* agent);

	// convenience functions
	TreeNode* AddNode(TreeNode* parent, TreeNode* child, TreeNodeType type);
	void Display();
	void Update(); // may be used, or we'll use Update of PlayScene
	void Clean();

	void MakeDecision(); // traverse tree in order

private:
	void BuildTree();

	Agent* m_agent;
	LOSCondition* m_LOSNode;
	DetectCondition* m_detectNode;
	CloseCombatCondition* m_closeCombatNode;
	std::vector<TreeNode*> m_treeNodeList;
};

#endif /* defined (__DECISION_TRE__) */