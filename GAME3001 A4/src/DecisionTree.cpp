#include "DecisionTree.h"
#include "AttackAction.h"
#include "MoveToLOSAction.h"
#include "MoveToPlayerAction.h"
#include "PatrolAction.h"
#include <iostream>

DecisionTree::DecisionTree()
{
	BuildTree();
}

DecisionTree::DecisionTree(Agent* agent)
{
	m_agent = agent;
	BuildTree();
}

DecisionTree::~DecisionTree() = default;

Agent* DecisionTree::GetAgent() const
{
	return m_agent;
}

LOSCondition* DecisionTree::GetLOSNode() const
{
	return m_LOSNode;
}

DetectCondition* DecisionTree::GetDetectNode()
{
	return m_detectNode;
}

CloseCombatCondition* DecisionTree::GetCloseCombatNode() const
{
	return m_closeCombatNode;
}

void DecisionTree::SetAgent(Agent* agent)
{
	m_agent = agent;
}

TreeNode* DecisionTree::AddNode(TreeNode* parent, TreeNode* child, TreeNodeType type)
{
	switch (type)
	{
	case TreeNodeType::LEFT_TREE_NODE:
		parent->m_pLeft = child;
		break;
	case TreeNodeType::RIGHT_TREE_NODE:
		parent->m_pRight = child;
		break;
	}
	child->m_pParent = parent;
	return child;
}

void DecisionTree::Display()
{
	for (auto node : m_treeNodeList)
	{
		std::cout << node->m_name << std::endl;
	}
}

void DecisionTree::Update()
{
}

void DecisionTree::Clean()
{
	for (auto node : m_treeNodeList)
	{
		delete node;
		node = nullptr;
	}
	m_treeNodeList.clear();
	m_treeNodeList.shrink_to_fit();
	// wrangle the remaining dangling pointers. optional
	m_LOSNode = nullptr;
	m_detectNode = nullptr;
	m_closeCombatNode = nullptr;
}

void DecisionTree::MakeDecision()
{
	auto currentNode = m_treeNodeList[0]; // start at root node
	while(!currentNode->m_isLeaf)
	{
		currentNode = dynamic_cast<ConditionNode*>(currentNode)->Condition() ? 
			(currentNode->m_pRight) : (currentNode->m_pLeft);
	}
	static_cast<ActionNode*>(currentNode)->Action(); // invoke the action node
}

void DecisionTree::BuildTree()
{
	// create and add root node
	m_detectNode = new DetectCondition();
	m_treeNodeList.push_back(m_detectNode);

	m_LOSNode = new LOSCondition();
	AddNode(m_detectNode, m_LOSNode, TreeNodeType::RIGHT_TREE_NODE);
	m_treeNodeList.push_back(m_LOSNode);

	m_closeCombatNode = new CloseCombatCondition();
	AddNode(m_LOSNode, m_closeCombatNode, TreeNodeType::RIGHT_TREE_NODE);
	m_treeNodeList.push_back(m_closeCombatNode);

	TreeNode* patrolNode = AddNode(m_detectNode, new PatrolAction(),TreeNodeType::LEFT_TREE_NODE);
	static_cast<PatrolAction*>(patrolNode)->SetAgent(m_agent);
	m_treeNodeList.push_back(patrolNode);

	TreeNode* moveToLOSNode = AddNode(m_LOSNode, new MoveToLOSAction(), TreeNodeType::LEFT_TREE_NODE);
	m_treeNodeList.push_back(moveToLOSNode);

	TreeNode* moveToPlayerNode = AddNode(m_closeCombatNode, new MoveToLOSAction(), TreeNodeType::LEFT_TREE_NODE);
	m_treeNodeList.push_back(moveToPlayerNode);

	TreeNode* attackNode = AddNode(m_closeCombatNode, new AttackAction(), TreeNodeType::RIGHT_TREE_NODE);
	m_treeNodeList.push_back(attackNode);
}
