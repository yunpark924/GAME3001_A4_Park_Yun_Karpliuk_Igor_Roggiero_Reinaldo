#include "RangedCombatEnemy.h"

#include "Game.h"
#include "TextureManager.h"
#include "Util.h"
#include "MoveToLOSAction.h"
#include "MoveToPlayerAction.h"
#include "PatrolAction.h"
#include "EventManager.h"
#include "FleeAction.h"
#include "MoveToCoverAction.h"
#include "MoveToRange.h"
#include "WaitBehindCoverAction.h"
#include "PlaceholderSprite.h"

RangedCombatEnemy::RangedCombatEnemy(Scene* scene) :  m_fireCounter(0), m_fireCounterMax(60)
{				
	TextureManager::Instance().LoadSpriteSheet("../Assets/sprites/mage.txt",
		"../Assets/sprites/mage.png", "mage");

	SetMaxSpeed(20.0f);
	SetTurnRate(5.0f);
	SetAccelerationRate(2.0f);
	SetStartPos(glm::vec2(300.0f, 500.0f));
	SetScene(scene);
	SetSprite(new PlaceholderSprite);

	SetMaxRange(350);
	SetMinRange(100);

	GetSprite()->SetSpriteSheet(TextureManager::Instance().GetSpriteSheet("mage"));

	SetHealth(100);

	const auto size = TextureManager::Instance().GetTextureSize("mage");
	SetWidth(static_cast<int>(size.x));
	SetHeight(static_cast<int>(size.y));
	GetTransform()->position = glm::vec2(0.0f, 0.0f);
	GetRigidBody()->bounds = glm::vec2(GetWidth(), GetHeight());
	GetRigidBody()->velocity = glm::vec2(0.0f, 0.0f);
	GetRigidBody()->acceleration = glm::vec2(0.0f, 0.0f);
	GetRigidBody()->isColliding = false;
	setIsCentered(true);
	SetType(GameObjectType::AGENT);

	
	SetCurrentHeading(0.0f); // Current facing angle
	SetLOSDistance(1000.0f);
	SetWhiskerAngle(45.0f);
	SetLOSColour(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)); // Default LOS Colour = Red

	// New for Lab 7.1
	SetActionState(ActionState::NO_ACTION);
	buildPatrolPath();

	// New for Lab 7.2
	SetTree(new DecisionTree(this));  // Create a new Tree - AI Brain
	m_buildTree();
	GetTree()->Display();
	BuildAnimations();
}

RangedCombatEnemy::~RangedCombatEnemy()
= default;

void RangedCombatEnemy::Draw()
{
	std::string current_anim = "";
	// Draw the enemy based on the animation state.
	switch (GetAnimationState())
	{
	case EnemyAnimationState::ENEMY_IDLE_LEFT:
		current_anim = "idle";
		TextureManager::Instance().PlayAnimation("mage", GetSprite()->GetAnimation(current_anim),
			GetTransform()->position, 0.12f, static_cast<double>(GetCurrentHeading()), 255, true);
		
		break;
	case EnemyAnimationState::ENEMY_IDLE_RIGHT:
		current_anim = "idle";
		TextureManager::Instance().PlayAnimation("mage", GetSprite()->GetAnimation(current_anim),
			GetTransform()->position, 0.12f, static_cast<double>(GetCurrentHeading()), 255, true, SDL_FLIP_HORIZONTAL);
		break;
	case EnemyAnimationState::ENEMY_RUN_LEFT:
		current_anim = "run";
		TextureManager::Instance().PlayAnimation("mage", GetSprite()->GetAnimation(current_anim),
			GetTransform()->position, 0.12f, static_cast<double>(GetCurrentHeading()), 255, true);
		break;
	case EnemyAnimationState::ENEMY_RUN_RIGHT:
		current_anim = "run";
		TextureManager::Instance().PlayAnimation("mage", GetSprite()->GetAnimation(current_anim),
			GetTransform()->position, 0.12f, static_cast<double>(GetCurrentHeading()), 255, true, SDL_FLIP_HORIZONTAL);
		break;
	case EnemyAnimationState::ENEMY_RUN_DOWN:
		current_anim = "run";
		TextureManager::Instance().PlayAnimation("mage", GetSprite()->GetAnimation(current_anim),
			GetTransform()->position, 0.12f, static_cast<double>(GetCurrentHeading()), 255, true);
		break;
	case EnemyAnimationState::ENEMY_RUN_UP:
		current_anim = "run";
		TextureManager::Instance().PlayAnimation("mage", GetSprite()->GetAnimation(current_anim),
			GetTransform()->position, 0.12f, static_cast<double>(GetCurrentHeading()), 255, true);
		break;
	}

	SetWidth(GetSprite()->GetAnimation(current_anim).frames[0].w);
	SetHeight(GetSprite()->GetAnimation(current_anim).frames[0].h);

	if (GetHealth() > 0)
	{
		Util::DrawFilledRect(GetTransform()->position - glm::vec2((GetHealth() / GetMaxHealth() * 100) / 2, 60.0f), GetHealth() / GetMaxHealth() * 100.0f, 10.0f, glm::vec4(0, 1.0f, 0, 1.0f));
	} else
	{
	}

	if (EventManager::Instance().IsIMGUIActive()) 
	{
		// draw the LOS Line
		Util::DrawLine(GetTransform()->position + GetCurrentDirection() * 0.5f * static_cast<float>(GetWidth()),
			GetMiddleLOSEndPoint(), GetLOSColour());
	}

	// If we are in debug mode, draw the collider rect.
	if (Game::Instance().GetDebugMode())
	{
		Util::DrawRect(GetTransform()->position -
			glm::vec2(this->GetWidth() * 0.5f, this->GetHeight() * 0.5f),
			this->GetWidth(), this->GetHeight());
	}
}

void RangedCombatEnemy::Update()
{
	
	GetTree()->MakeDecision();
}

void RangedCombatEnemy::Clean()
{
}



void RangedCombatEnemy::Attack()
{
	auto scene = dynamic_cast<PlayScene*>(GetScene());

	if (GetActionState() != ActionState::ATTACK) 
	{
	
		SetActionState(ActionState::ATTACK);
	}
	// New for lab 8
	glm::vec2 target_direction = Util::Normalize(scene->GetTarget()->GetTransform()->position - GetTransform()->position);
	LookWhereYoureGoing(target_direction, false);

	if (m_fireCounter++ % m_fireCounterMax == 0)
	{
		scene->SpawnEnemyTorpedo(this);
	}
}

void RangedCombatEnemy::BuildAnimations()
{
	Animation idle_animation = Animation();

	idle_animation.name = "idle";
	idle_animation.frames.push_back(GetSprite()->GetSpriteSheet()->GetFrame("idle"));

	GetSprite()->SetAnimation(idle_animation);

	Animation run_animation = Animation();

	run_animation.name = "run";
	run_animation.frames.push_back(GetSprite()->GetSpriteSheet()->GetFrame("run1"));
	run_animation.frames.push_back(GetSprite()->GetSpriteSheet()->GetFrame("run2"));

	GetSprite()->SetAnimation(run_animation);

	Animation run_back_animation = Animation();

	run_back_animation.name = "run";
	run_back_animation.frames.push_back(GetSprite()->GetSpriteSheet()->GetFrame("run1"));
	run_back_animation.frames.push_back(GetSprite()->GetSpriteSheet()->GetFrame("run2"));

	GetSprite()->SetAnimation(run_back_animation);

	Animation run_front_animation = Animation();

	run_front_animation.name = "run";
	run_front_animation.frames.push_back(GetSprite()->GetSpriteSheet()->GetFrame("run1"));
	run_front_animation.frames.push_back(GetSprite()->GetSpriteSheet()->GetFrame("run2"));

	GetSprite()->SetAnimation(run_front_animation);

}


void RangedCombatEnemy::m_buildTree()
{
	// Create and add the root node - Health Condition
	GetTree()->SetEnemyHealthNode(new EnemyHealthCondition(this, true));

	GetTree()->GetTree().push_back(GetTree()->GetEnemyHealthNode());

	m_buildLeftTree();
	m_buildRightTree();
}

void RangedCombatEnemy::m_buildLeftTree()
{
	// Left Subtree Level 1 -> Flee Action
	TreeNode* fleeNode = GetTree()->AddNode(GetTree()->GetEnemyHealthNode(), new FleeAction(this), TreeNodeType::LEFT_TREE_NODE);
	GetTree()->GetTree().push_back(fleeNode);
}

void RangedCombatEnemy::m_buildRightTree()
{
	// Right Subtree Level 1 -> Enemy Hit Condition
	GetTree()->SetEnemyHitNode(new EnemyHitCondition(this, false));
	GetTree()->AddNode(GetTree()->GetEnemyHealthNode(), GetTree()->GetEnemyHitNode(), TreeNodeType::RIGHT_TREE_NODE);
	GetTree()->GetTree().push_back(GetTree()->GetEnemyHitNode());

	// Right Left Subtree Level 2 -> Player Detected Condition
	GetTree()->SetPlayerDetectedNode(new PlayerDetectedCondition(this));
	GetTree()->AddNode(GetTree()->GetEnemyHitNode(), GetTree()->GetPlayerDetectedNode(), TreeNodeType::LEFT_TREE_NODE);

	// Right Subtree Level 2 -> LOS Condition
	const auto LOSNodeRight = new LOSCondition(this);
	GetTree()->AddNode(GetTree()->GetEnemyHitNode(), LOSNodeRight, TreeNodeType::RIGHT_TREE_NODE);
	GetTree()->GetTree().push_back(LOSNodeRight);

	// Left Left Subtree Level 3 -> Patrol Action
	TreeNode* patrolNode = GetTree()->AddNode(GetTree()->GetPlayerDetectedNode(), new PatrolAction(this), TreeNodeType::LEFT_TREE_NODE);
	GetTree()->GetTree().push_back(patrolNode);

	// Left Right Subtree Level 3 -> LOS Condition
	const auto LOSNodeLeft = new LOSCondition(this);
	GetTree()->AddNode(GetTree()->GetPlayerDetectedNode(), LOSNodeLeft, TreeNodeType::RIGHT_TREE_NODE);
	GetTree()->GetTree().push_back(LOSNodeLeft);

	// Right Left Subtree Level 3 -> Wait Behind Cover Action
	TreeNode* waitBehindCoverNode = GetTree()->AddNode(LOSNodeRight, new WaitBehindCoverAction(this), TreeNodeType::LEFT_TREE_NODE);
	GetTree()->GetTree().push_back(waitBehindCoverNode);

	// Right Right Subtree Level 3 -> Move to Cover Action
	TreeNode* moveToCoverNode = GetTree()->AddNode(LOSNodeRight, new MoveToCoverAction(this), TreeNodeType::RIGHT_TREE_NODE);
	GetTree()->GetTree().push_back(moveToCoverNode);

	// Left Left Subtree Level 4 -> Move to LOS Action
	TreeNode* moveToLOS = GetTree()->AddNode(LOSNodeLeft, new MoveToLOSAction(this), TreeNodeType::LEFT_TREE_NODE);
	GetTree()->GetTree().push_back(moveToLOS);

	// Left Right Subtree Level 4 -> Ranged Combat Condition
	GetTree()->SetRangedCombatNode(new RangedCombatCondition(this));
	GetTree()->AddNode(LOSNodeLeft, GetTree()->GetRangedCombatNode(), TreeNodeType::RIGHT_TREE_NODE);
	GetTree()->GetTree().push_back(GetTree()->GetRangedCombatNode());

	// Left Left Subtree Level 5 -> Move to Range Action
	TreeNode* moveToRangeNode = GetTree()->AddNode(GetTree()->GetRangedCombatNode(), new MoveToRangeAction(this), TreeNodeType::LEFT_TREE_NODE);
	GetTree()->GetTree().push_back(moveToRangeNode);

	// Left Right Subtree Level 5 -> Attack Action
	TreeNode* attackNode = GetTree()->AddNode(GetTree()->GetRangedCombatNode(), new AttackAction(this), TreeNodeType::RIGHT_TREE_NODE);
	GetTree()->GetTree().push_back(attackNode);
}
