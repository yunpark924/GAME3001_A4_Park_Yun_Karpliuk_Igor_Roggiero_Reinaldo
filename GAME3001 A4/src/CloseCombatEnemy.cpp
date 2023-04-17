#include "CloseCombatEnemy.h"

#include "Game.h"
#include "TextureManager.h"
#include "Util.h"
#include "MoveToLOSAction.h"
#include "MoveToPlayerAction.h"
#include "PatrolAction.h"
#include "EventManager.h"

CloseCombatEnemy::CloseCombatEnemy(Scene* scene)
{
	TextureManager::Instance().LoadSpriteSheet("../Assets/sprites/skelly.txt",
		"../Assets/sprites/skelly.png", "skelly");

	SetMaxSpeed(20.0f);
	SetTurnRate(5.0f);
	SetAccelerationRate(2.0f);
	SetStartPos(glm::vec2(300.0f, 500.0f));
	SetScene(scene);
	SetSprite(new PlaceholderSprite);

	SetHealth(100);

	GetSprite()->SetSpriteSheet(TextureManager::Instance().GetSpriteSheet("skelly"));

	const auto size = TextureManager::Instance().GetTextureSize("skelly");
	SetWidth(static_cast<int>(size.x));
	SetHeight(static_cast<int>(size.y));
	GetTransform()->position = glm::vec2(550.0f, 300.0f);
	GetRigidBody()->bounds = glm::vec2(GetWidth(), GetHeight());
	GetRigidBody()->velocity = glm::vec2(0.0f, 0.0f);
	GetRigidBody()->acceleration = glm::vec2(0.0f, 0.0f);
	GetRigidBody()->isColliding = false;
	setIsCentered(true);
	SetType(GameObjectType::AGENT);

	SetMaxRange(100.0f);
	SetMinRange(5.0f);
	
	SetCurrentHeading(0.0f); // Current facing angle
	SetLOSDistance(400.0f);
	SetWhiskerAngle(45.0f);
	SetLOSColour(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)); // Default LOS Colour = Red
	SoundManager::Instance().Load("../Assets/Audio/deathE.mp3", "deathE", SoundType::SOUND_SFX);
	SoundManager::Instance().Load("../Assets/Audio/hurt.mp3", "hurt", SoundType::SOUND_SFX);
	// New for Lab 7.1
	SetActionState(ActionState::NO_ACTION);
	buildPatrolPath();

	// New for Lab 7.2
	SetTree(new DecisionTree(this));
	m_buildTree();
	GetTree()->Display();
	BuildAnimations();
}

CloseCombatEnemy::~CloseCombatEnemy()
= default;

void CloseCombatEnemy::Draw()
{
	std::string current_anim = "";

	switch (GetAnimationState())
	{
	case EnemyAnimationState::ENEMY_IDLE_LEFT:
		current_anim = "idle";
		TextureManager::Instance().PlayAnimation("skelly", GetSprite()->GetAnimation(current_anim),
			GetTransform()->position, 0.12f, static_cast<double>(GetCurrentHeading()), 255, true);

		break;
	case EnemyAnimationState::ENEMY_IDLE_RIGHT:
		current_anim = "idle";
		TextureManager::Instance().PlayAnimation("skelly", GetSprite()->GetAnimation(current_anim),
			GetTransform()->position, 0.12f, static_cast<double>(GetCurrentHeading()), 255, true, SDL_FLIP_VERTICAL);
		break;
	case EnemyAnimationState::ENEMY_RUN_LEFT:
		current_anim = "run";
		TextureManager::Instance().PlayAnimation("skelly", GetSprite()->GetAnimation(current_anim),
			GetTransform()->position, 0.20, static_cast<double>(GetCurrentHeading()), 255, true);
		break;
	case EnemyAnimationState::ENEMY_RUN_RIGHT:
		current_anim = "run";
		TextureManager::Instance().PlayAnimation("skelly", GetSprite()->GetAnimation(current_anim),
			GetTransform()->position, 0.20, static_cast<double>(GetCurrentHeading()), 255, true, SDL_FLIP_VERTICAL);
		break;
	case EnemyAnimationState::ENEMY_RUN_DOWN:
		current_anim = "run";
		TextureManager::Instance().PlayAnimation("skelly", GetSprite()->GetAnimation(current_anim),
			GetTransform()->position, 0.12f, static_cast<double>(GetCurrentHeading()), 255, true);
		break;
	case EnemyAnimationState::ENEMY_RUN_UP:
		current_anim = "run";
		TextureManager::Instance().PlayAnimation("skelly", GetSprite()->GetAnimation(current_anim),
			GetTransform()->position, 0.12f, static_cast<double>(GetCurrentHeading()), 255, true);
		break;
	case EnemyAnimationState::ENEMY_DEAD:
		current_anim = "death";
		TextureManager::Instance().PlayAnimation("skelly", GetSprite()->GetAnimation(current_anim),
			GetTransform()->position, 0.12f, static_cast<double>(GetCurrentHeading()), 255, true);
		break;
	}

	SetWidth(GetSprite()->GetAnimation(current_anim).frames[0].w);
	SetHeight(GetSprite()->GetAnimation(current_anim).frames[0].h);

	if (GetHealth() > 0)
	{
		Util::DrawFilledRect(GetTransform()->position - glm::vec2((GetHealth() / GetMaxHealth() * 100) / 2, 60.0f), GetHealth() / GetMaxHealth() * 100.0f, 10.0f, glm::vec4(3.0f, 5.0f, 0.0f, 1.0f));
	}
	else
	{
		SoundManager::Instance().PlaySoundFX("deathE");
	}

	if (EventManager::Instance().IsIMGUIActive()) 
	{
		Util::DrawLine(GetTransform()->position + GetCurrentDirection() * 0.5f * static_cast<float>(GetWidth()),
			GetMiddleLOSEndPoint(), GetLOSColour());
	}

	if (Game::Instance().GetDebugMode())
	{
		Util::DrawRect(GetTransform()->position -
			glm::vec2(this->GetWidth() * 0.5f, this->GetHeight() * 0.5f),
			this->GetWidth(), this->GetHeight());
	}
}

void CloseCombatEnemy::Update()
{
	GetTree()->MakeDecision();
	if (GetHealth() <= 0)
	{
		SetAnimationState(EnemyAnimationState::ENEMY_DEAD);
	}
}

void CloseCombatEnemy::Clean()
{
}


void CloseCombatEnemy::Attack()
{
	auto scene = dynamic_cast<PlayScene*>(GetScene());
	if (GetActionState() != ActionState::ATTACK) 
	{
		SetActionState(ActionState::ATTACK);
		
	}
	if (timerUntilHit <= 0)
	{
		timerUntilHit = 1.5f;
		scene->GetTarget()->TakeDamage(10.0f);
		SoundManager::Instance().PlaySoundFX("hurt");
	}
	else {
		timerUntilHit -= Game::Instance().GetDeltaTime();
	}
	
}


void CloseCombatEnemy::m_buildTree()
{
	
	GetTree()->SetEnemyHealthNode(new EnemyHealthCondition(this, true));

	GetTree()->GetTree().push_back(GetTree()->GetEnemyHealthNode());

	TreeNode* fleeNode = GetTree()->AddNode(GetTree()->GetEnemyHealthNode(), new FleeAction(this), TreeNodeType::LEFT_TREE_NODE);
	GetTree()->GetTree().push_back(fleeNode);


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

	// Left Right Subtree Level 4 -> Close Combat Condition
	GetTree()->SetCloseCombatNode(new CloseCombatCondition(this));
	GetTree()->AddNode(LOSNodeLeft, GetTree()->GetCloseCombatNode(), TreeNodeType::RIGHT_TREE_NODE);
	GetTree()->GetTree().push_back(GetTree()->GetCloseCombatNode());

	// Left Left Subtree Level 5 -> Move to Player Action
	TreeNode* moveToPlayer = GetTree()->AddNode(GetTree()->GetCloseCombatNode(), new MoveToPlayerAction(this), TreeNodeType::LEFT_TREE_NODE);
	GetTree()->GetTree().push_back(moveToPlayer);

	// Left Right Subtree Level 5 -> Attack Action
	TreeNode* attackNode = GetTree()->AddNode(GetTree()->GetCloseCombatNode(), new AttackAction(this), TreeNodeType::RIGHT_TREE_NODE);
	GetTree()->GetTree().push_back(attackNode);
}


void CloseCombatEnemy::BuildAnimations()
{
	Animation idle_animation = Animation();

	idle_animation.name = "idle";
	idle_animation.frames.push_back(GetSprite()->GetSpriteSheet()->GetFrame("idle"));

	GetSprite()->SetAnimation(idle_animation);

	Animation run_animation = Animation();

	run_animation.name = "run";
	run_animation.frames.push_back(GetSprite()->GetSpriteSheet()->GetFrame("run1"));
	run_animation.frames.push_back(GetSprite()->GetSpriteSheet()->GetFrame("run2"));
	run_animation.frames.push_back(GetSprite()->GetSpriteSheet()->GetFrame("run3"));

	GetSprite()->SetAnimation(run_animation);

	Animation death_animation = Animation();

	death_animation.name = "death";
	death_animation.frames.push_back(GetSprite()->GetSpriteSheet()->GetFrame("deathE1"));
	death_animation.frames.push_back(GetSprite()->GetSpriteSheet()->GetFrame("deathE2"));
	death_animation.frames.push_back(GetSprite()->GetSpriteSheet()->GetFrame("deathE3"));
	death_animation.frames.push_back(GetSprite()->GetSpriteSheet()->GetFrame("deathE4"));

	GetSprite()->SetAnimation(death_animation);
}
