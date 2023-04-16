#include "PlayScene.h"
#include "Game.h"
#include "EventManager.h"
#include "InputType.h"

// required for IMGUI
#include <fstream>

#include "imgui.h"
#include "imgui_sdl.h"
#include "Renderer.h"
#include "Util.h"

PlayScene::PlayScene()
{
	PlayScene::Start();
}

PlayScene::~PlayScene()
= default;

void PlayScene::Draw()
{
	DrawDisplayList();
	if(m_bDebugView)
	{
		Util::DrawRect(m_pPlayer->GetTransform()->position -
			glm::vec2(m_pPlayer->GetWidth() * 0.5f, m_pPlayer->GetHeight() * 0.5f),
			m_pPlayer->GetWidth(), m_pPlayer->GetHeight());
		Util::DrawRect(m_pEnemy->GetTransform()->position -
			glm::vec2(m_pEnemy->GetWidth() * 0.5f, m_pEnemy->GetHeight() * 0.5f),
			m_pEnemy->GetWidth(), m_pEnemy->GetHeight());
		Util::DrawCircle(m_pEnemy->GetTransform()->position, m_pEnemy->m_detecRadius * 0.5f);
		Util::DrawLine(m_pEnemy->GetTransform()->position,
			m_pEnemy->GetTransform()->position + m_pEnemy->GetCurrentDirection() * m_pEnemy->GetLOSDistance(),
			glm::vec4(1, 0, 0, 1));

		for(auto obstacle : m_pObstacles)
		{
			Util::DrawRect(obstacle->GetTransform()->position -
				glm::vec2(obstacle->GetWidth() * 0.5f, obstacle->GetHeight() * 0.5f),
				obstacle->GetWidth(), obstacle->GetHeight());

		}
	}
	SDL_SetRenderDrawColor(Renderer::Instance().GetRenderer(), 255, 255, 255, 255);
}

void PlayScene::Update()
{
	UpdateDisplayList();

	if(m_pPlayerHealth == 0)
	{
		m_pPlayer->SetEnabled(false);
	}
	if(m_pEnemyHealth == 0)
	{
		SoundManager::Instance().PlaySound("enemy death");
		m_pEnemy->SetAnimationState(EnemyAnimatioState::ENEMY_DIE);
		m_pEnemy->SetEnabled(false);
	}

	float distance = Util::Distance(m_pPlayer->GetTransform()->position, m_pEnemy->GetTransform()->position);
	if (distance < m_pEnemy->m_detecRadius)
	{
		std::cout << "Player is in radius!\n";
		m_isRange = true;
	}
	else
		m_isRange = false;

	// obstacle blocking,,,,,,, sigh
	for(unsigned i = 0; i < m_pObstacles.size(); i++)
	{
		if(CollisionManager::AABBCheck(m_pPlayer, m_pObstacles[i]))
		{
			m_pPlayer->GetRigidBody()->isColliding = true;
	
			if (m_pPlayer->GetTransform()->position.y >= m_pObstacles[i]->GetTransform()->position.y) // If bottom of player < top of platform in "previous frame"
				// Colliding with top side of tile. Or collided from top.
				m_pPlayer->GetTransform()->position.y = m_pObstacles[i]->GetTransform()->position.y + 100.0f;
			else if (m_pPlayer->GetTransform()->position.x >= m_pObstacles[i]->GetTransform()->position.x) // If bottom of player < top of platform in "previous frame"
				// Colliding with top side of tile. Or collided from top.
				m_pPlayer->GetTransform()->position.x = m_pObstacles[i]->GetTransform()->position.x + 100.0f;
		}
	}

	CheckAgentLOS(m_pPlayer, m_pEnemy);
	m_decTree->MakeDecision();

	if (m_isGridEnabled)
	{
		switch (m_LOSMode)
		{
		case 0:
			CheckAllNodesWithTarget(m_pEnemy);
			break;
		case 1:
			CheckAllNodesWithTarget(m_pPlayer);
			break;
		case 2:
			CheckAllNodesWithBoth();
			break;
		}
	}
}

void PlayScene::Clean()
{
	RemoveAllChildren();
}


void PlayScene::HandleEvents()
{
	EventManager::Instance().Update();

	GetPlayerInput();

	GetKeyboardInput();
}

void PlayScene::GetPlayerInput()
{
	// debug controls
	if (EventManager::Instance().KeyPressed(SDL_SCANCODE_H) && m_bDebugView == false)
	{
		SetGridEnabled(true);
		m_bDebugView = true;
		ToggleGrid(true);
	}
	else if (EventManager::Instance().KeyPressed(SDL_SCANCODE_H) && m_bDebugView == true)
	{
		SetGridEnabled(false);
		m_bDebugView = false;
		ToggleGrid(false);
	}
	if(EventManager::Instance().KeyPressed(SDL_SCANCODE_K))
	{
		m_pEnemy->SetAnimationState(EnemyAnimatioState::ENEMY_HIT);
		SoundManager::Instance().PlaySound("enemy hit");
		m_pEnemyHealth--;
	}
	if (EventManager::Instance().KeyPressed(SDL_SCANCODE_P) && m_pEnemy->GetPatrollingEnabled() == true)
		m_pEnemy->IsPatrolling(false);
	else if (EventManager::Instance().KeyPressed(SDL_SCANCODE_P) && m_pEnemy->GetPatrollingEnabled() == false)
		m_pEnemy->IsPatrolling(true);


	switch (m_pCurrentInputType)
	{
	case static_cast<int>(InputType::GAME_CONTROLLER):
	{
		// handle player movement with GameController
		if (SDL_NumJoysticks() > 0)
		{
			if (EventManager::Instance().GetGameController(0) != nullptr)
			{
				constexpr auto dead_zone = 10000;
				if (EventManager::Instance().GetGameController(0)->STICK_LEFT_HORIZONTAL > dead_zone)
				{
					m_pPlayer->SetAnimationState(PlayerAnimationState::PLAYER_RUN_RIGHT);
					m_playerFacingRight = true;
				}
				else if (EventManager::Instance().GetGameController(0)->STICK_LEFT_HORIZONTAL < -dead_zone)
				{
					m_pPlayer->SetAnimationState(PlayerAnimationState::PLAYER_RUN_LEFT);
					m_playerFacingRight = false;
				}
				else
				{
					if (m_playerFacingRight)
					{
						m_pPlayer->SetAnimationState(PlayerAnimationState::PLAYER_IDLE_RIGHT);
					}
					else
					{
						m_pPlayer->SetAnimationState(PlayerAnimationState::PLAYER_IDLE_LEFT);
					}
				}
			}
		}
	}
	break;
	case static_cast<int>(InputType::KEYBOARD_MOUSE):
	{
		// handle player movement with mouse and keyboard
		if (EventManager::Instance().IsKeyDown(SDL_SCANCODE_A))
		{
			m_pPlayer->SetAnimationState(PlayerAnimationState::PLAYER_RUN_LEFT);
			m_pPlayer->GetTransform()->position -= glm::vec2(5.0f, 0.0f);
			m_playerFacingRight = false;
			SoundManager::Instance().PlaySound("footstep");
		}
		else if (EventManager::Instance().IsKeyDown(SDL_SCANCODE_D))
		{
			m_pPlayer->SetAnimationState(PlayerAnimationState::PLAYER_RUN_RIGHT);
			m_pPlayer->GetTransform()->position += glm::vec2(5.0f, 0.0f);
			m_playerFacingRight = true;
			SoundManager::Instance().PlaySound("footstep");
		}
		else if (EventManager::Instance().IsKeyDown(SDL_SCANCODE_W))
		{
			m_pPlayer->SetAnimationState(PlayerAnimationState::PLAYER_RUN_LEFT);
			m_pPlayer->GetTransform()->position -= glm::vec2(0.0f, 5.0f);
			m_playerFacingRight = false;
			SoundManager::Instance().PlaySound("footstep");
		}
		else if (EventManager::Instance().IsKeyDown(SDL_SCANCODE_S))
		{
			m_pPlayer->SetAnimationState(PlayerAnimationState::PLAYER_RUN_RIGHT);
			m_pPlayer->GetTransform()->position += glm::vec2(0.0f, 5.0f);
			m_playerFacingRight = true;
			SoundManager::Instance().PlaySound("footstep");
		}
		else if (EventManager::Instance().MousePressed(1))
		{
			m_pPlayer->SetAnimationState(PlayerAnimationState::PLAYER_ATTACK_CLOSE);
			std::cout << "close combat\n";
			SoundManager::Instance().PlaySound("player attack");
			if(CollisionManager::AABBCheck(m_pPlayer, m_pEnemy))
			{
				std::cout << "enemy takes damage\n";
				m_pEnemy->SetAnimationState(EnemyAnimatioState::ENEMY_HIT);
				SoundManager::Instance().PlaySound("enemy hit");
				m_pEnemyHealth--;
			}
		}
		else if (EventManager::Instance().MousePressed(3))
		{
			m_pPlayer->SetAnimationState(PlayerAnimationState::PLAYER_ATTACK_FIRE);
			std::cout << "ranged combat\n";
			SoundManager::Instance().PlaySound("player firing");
		}
		else
		{
			if (m_playerFacingRight)
			{
				m_pPlayer->SetAnimationState(PlayerAnimationState::PLAYER_IDLE_RIGHT);
			}
			else
			{
				m_pPlayer->SetAnimationState(PlayerAnimationState::PLAYER_IDLE_LEFT);
			}
		}
	}
	break;
	case static_cast<int>(InputType::ALL):
	{
		if (SDL_NumJoysticks() > 0)
		{
			if (EventManager::Instance().GetGameController(0) != nullptr)
			{
				constexpr auto dead_zone = 10000;
				if (EventManager::Instance().GetGameController(0)->STICK_LEFT_HORIZONTAL > dead_zone
					|| EventManager::Instance().IsKeyDown(SDL_SCANCODE_D))
				{
					m_pPlayer->SetAnimationState(PlayerAnimationState::PLAYER_RUN_RIGHT);
					m_playerFacingRight = true;
				}
				else if (EventManager::Instance().GetGameController(0)->STICK_LEFT_HORIZONTAL < -dead_zone
					|| EventManager::Instance().IsKeyDown(SDL_SCANCODE_A))
				{
					m_pPlayer->SetAnimationState(PlayerAnimationState::PLAYER_RUN_LEFT);
					m_playerFacingRight = false;
				}
				else
				{
					if (m_playerFacingRight)
					{
						m_pPlayer->SetAnimationState(PlayerAnimationState::PLAYER_IDLE_RIGHT);
					}
					else
					{
						m_pPlayer->SetAnimationState(PlayerAnimationState::PLAYER_IDLE_LEFT);
					}
				}
			}
		}
		else if (EventManager::Instance().IsKeyDown(SDL_SCANCODE_A))
		{
			m_pPlayer->SetAnimationState(PlayerAnimationState::PLAYER_RUN_LEFT);
			m_playerFacingRight = false;
		}
		else if (EventManager::Instance().IsKeyDown(SDL_SCANCODE_D))
		{
			m_pPlayer->SetAnimationState(PlayerAnimationState::PLAYER_RUN_RIGHT);
			m_playerFacingRight = true;
		}
		else
		{
			if (m_playerFacingRight)
			{
				m_pPlayer->SetAnimationState(PlayerAnimationState::PLAYER_IDLE_RIGHT);
			}
			else
			{
				m_pPlayer->SetAnimationState(PlayerAnimationState::PLAYER_IDLE_LEFT);
			}
		}
	}
	break;
	}
}

void PlayScene::GetKeyboardInput()
{
	if (EventManager::Instance().IsKeyDown(SDL_SCANCODE_ESCAPE))
	{
		Game::Instance().Quit();
	}

	if (EventManager::Instance().IsKeyDown(SDL_SCANCODE_1))
	{
		Game::Instance().ChangeSceneState(SceneState::START);
	}

	if (EventManager::Instance().IsKeyDown(SDL_SCANCODE_2))
	{
		Game::Instance().ChangeSceneState(SceneState::END);
	}
}

void PlayScene::Start()
{
	// Set GUI Title
	m_guiTitle = "Play Scene";

	// Set Input Type
	m_pCurrentInputType = static_cast<int>(InputType::KEYBOARD_MOUSE);

	m_pBG = new Background();
	AddChild(m_pBG);
	
	// Player Sprite
	m_pPlayer = new Player();
	AddChild(m_pPlayer, 3);
	m_playerFacingRight = true;

	m_pEnemy = new Enemy();
	m_pEnemy->GetTransform()->position = glm::vec2(650.f, 200.f);
	AddChild(m_pEnemy, 2);

	// New Obstacle creation
	std::ifstream inFile("../Assets/data/obstacles.txt");
	while (!inFile.eof())
	{
		Obstacle* temp = new Obstacle();
		float x, y, w, h;
		inFile >> x >> y >> w >> h;
		temp->GetTransform()->position = glm::vec2(x, y);
		temp->SetWidth(w);
		temp->SetHeight(h);
		AddChild(temp, 4);
		m_pObstacles.push_back(temp);
	}
	inFile.close();

	m_pHealthBar = new HealthBar();
	AddChild(m_pHealthBar, 5);

	// Setup a few fields
	m_LOSMode = 0;
	m_pathNodeLOSDistance = 1000;
	SetPathNodeLOSDistance(m_pathNodeLOSDistance);

	// Setup the grid
	m_isGridEnabled = false;
	BuildGrid();
	ToggleGrid(m_isGridEnabled);

	// create decision tree
	m_decTree = new DecisionTree(m_pEnemy);
	m_decTree->Display();
	// m_decTree->MakeDecision(); // if you don't want every frame

	// sound loads
	SoundManager::Instance().Load("../Assets/Audio/bgmusic.wav", "bg", SoundType::SOUND_MUSIC);
	SoundManager::Instance().Load("../Assets/Audio/enemydie.wav", "enemy death", SoundType::SOUND_SFX);
	SoundManager::Instance().Load("../Assets/Audio/enemyhit.wav", "enemy hit", SoundType::SOUND_SFX);
	SoundManager::Instance().Load("../Assets/Audio/footstep.wav", "footstep", SoundType::SOUND_SFX);
	SoundManager::Instance().Load("../Assets/Audio/playerattack.wav", "player attack", SoundType::SOUND_SFX);
	SoundManager::Instance().Load("../Assets/Audio/playerfiring.wav", "player firing", SoundType::SOUND_SFX);

	SoundManager::Instance().PlayMusic("bg");
	SoundManager::Instance().SetMusicVolume(8);
	SoundManager::Instance().AllocateChannels(16);
	SoundManager::Instance().SetSoundVolume(36);

	/* DO NOT REMOVE */
	ImGuiWindowFrame::Instance().SetGuiFunction([this] { GUI_Function(); });
}

void PlayScene::GUI_Function()
{
	auto offset = glm::vec2(Config::TILE_SIZE * 0.5f, Config::TILE_SIZE * 0.5f);

	// Always open with a NewFrame
	ImGui::NewFrame();

	// See examples by uncommenting the following - also look at imgui_demo.cpp in the IMGUI filter
	//ImGui::ShowDemoWindow();

	ImGui::Begin("SDL Trek II: The Wrath of Tom", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar);

	ImGui::Separator();

	// Debug Properties
	if (ImGui::Checkbox("Toggle Grid", &m_bDebugView))
	{
		ToggleGrid(m_bDebugView);
	}

	ImGui::Separator();

	if (ImGui::Button("Node LOS to Enemy", { 300, 20 }))
	{
		m_LOSMode = 0;
	}
	if (m_LOSMode == 0)
	{
		ImGui::SameLine();
		ImGui::Text("<Active>");
	}

	if (ImGui::Button("Node LOS to Player", { 300, 20 }))
	{
		m_LOSMode = 1;
	}
	if (m_LOSMode == 1)
	{
		ImGui::SameLine();
		ImGui::Text("<Active>");
	}

	if (ImGui::Button("Node LOS to both", { 300, 20 }))
	{
		m_LOSMode = 2;
	}
	if (m_LOSMode == 2)
	{
		ImGui::SameLine();
		ImGui::Text("<Active>");
	}

	if (ImGui::SliderInt("PathNode LOS Distance", &m_pathNodeLOSDistance, 0, 1000))
	{
		SetPathNodeLOSDistance(m_pathNodeLOSDistance);
	}

	ImGui::Separator();

	// Static Position Variables
	m_enemyPosition[0] = m_pEnemy->GetTransform()->position.x;
	m_enemyPosition[1] = m_pEnemy->GetTransform()->position.y;

	// StarShip Properties
	m_playerPosition[0] = m_pPlayer->GetTransform()->position.x;
	m_playerPosition[1] = m_pPlayer->GetTransform()->position.y;

	if (ImGui::SliderInt2("Player Position", m_playerPosition, 0, 800))
	{
		m_pPlayer->GetTransform()->position.x = m_playerPosition[0];
		m_pPlayer->GetTransform()->position.y = m_playerPosition[1];

	}

	ImGui::Separator();

	// Target Properties
	if (ImGui::SliderInt2("Enemy Position", m_enemyPosition, 0, 800))
	{
		m_pEnemy->GetTransform()->position.x = m_enemyPosition[0];
		m_pEnemy->GetTransform()->position.y = m_enemyPosition[1];
	}

	ImGui::Separator();

	bool isPatrolling = m_pEnemy->GetPatrollingEnabled();
	if (ImGui::Checkbox("Enemy Patrol", &isPatrolling))
	{
		m_pEnemy->IsPatrolling(isPatrolling);
	}


	ImGui::End();
}

void PlayScene::BuildGrid()
{
	const auto tile_size = Config::TILE_SIZE;
	const auto offset = glm::vec2(Config::TILE_SIZE * 0.5f, Config::TILE_SIZE * 0.5f);

	ClearNodes();

	// layout a grid of tiles
	for (int row = 0; row < Config::ROW_NUM; ++row)
	{
		for (int col = 0; col < Config::COL_NUM; ++col)
		{
			PathNode* pathNode = new PathNode();
			pathNode->GetTransform()->position = glm::vec2((col * tile_size) + offset.x,
				(row * tile_size) + offset.y);
			// Collision check with node against all obstacles.
			bool keepNode = true;
			for (auto obstacle : m_pObstacles)
			{
				if (CollisionManager::AABBCheck(pathNode, obstacle))
					keepNode = false;
			}
			if (keepNode == true) // Add path node to grid.
			{
				AddChild(pathNode);
				m_pGrid.push_back(pathNode);
			}
			else // Discard path node.
				delete pathNode;
		}
	}
}

void PlayScene::ToggleGrid(bool state)
{
	for (auto pathNode : m_pGrid)
	{
		pathNode->SetVisible(state);
	}
}

bool PlayScene::GetGridEnabled() const
{
	return m_isGridEnabled;
}

bool PlayScene::CheckPathNodeLOS(PathNode* path_node, DisplayObject* target_object)
{
	return CheckAgentLOS(path_node, target_object);
}

void PlayScene::CheckAllNodesWithTarget(DisplayObject* target_object)
{
	for (auto path_node : m_pGrid)
	{
		// Check angle to target for LOS distance.
		auto targetDirection = target_object->GetTransform()->position - path_node->GetTransform()->position;
		auto normalizedDirection = Util::Normalize(targetDirection);
		path_node->SetCurrentDirection(normalizedDirection);
		CheckPathNodeLOS(path_node, target_object);
	}
}

void PlayScene::CheckAllNodesWithBoth()
{
	for (auto path_node : m_pGrid)
	{
		auto targetDirection = m_pPlayer->GetTransform()->position - path_node->GetTransform()->position;
		auto normalizedDirection = Util::Normalize(targetDirection);
		path_node->SetCurrentDirection(normalizedDirection);

		bool LOSWithStarShip = CheckPathNodeLOS(path_node, m_pPlayer);

		targetDirection = m_pEnemy->GetTransform()->position - path_node->GetTransform()->position;
		normalizedDirection = Util::Normalize(targetDirection);
		path_node->SetCurrentDirection(normalizedDirection);

		bool LOSWithTarget = CheckPathNodeLOS(path_node, m_pEnemy);

		path_node->SetHasLOS((LOSWithStarShip && LOSWithTarget));

		if (LOSWithStarShip && LOSWithTarget)
		{
			std::cout << "LOS between player and enemy.\n";
			m_LOSWithBoth = true;
		}
		else
			m_LOSWithBoth = false;
	}
}

void PlayScene::SetPathNodeLOSDistance(int dist)
{
	for (auto path_node : m_pGrid)
	{
		path_node->SetLOSDistance((float)dist);
	}
}

void PlayScene::SetGridEnabled(const bool state)
{
	m_isGridEnabled = state;
}

bool PlayScene::CheckAgentLOS(Agent* agent, DisplayObject* target_object)
{
	bool hasLOS = false;
	agent->SetHasLOS(hasLOS);
	// If ship to target distance is <= LOS distance
	auto AgentToTargetDist = Util::GetClosestEdge(agent->GetTransform()->position, target_object);
	if (AgentToTargetDist <= agent->GetLOSDistance())
	{
		std::vector<DisplayObject*> contactList;
		for (auto object : GetDisplayList())
		{

			if (object->GetType() == GameObjectType::OBSTACLE)
			{
				auto AgentToObjectDist = Util::GetClosestEdge(agent->GetTransform()->position, object);
				if (AgentToObjectDist > AgentToTargetDist) continue;
				contactList.push_back(object);
			}
		}
		const glm::vec2 agentEndPoint = agent->GetTransform()->position + agent->GetCurrentDirection() * agent->GetLOSDistance();
		hasLOS = CollisionManager::LOSCheck(agent, agentEndPoint, contactList, target_object);
		agent->SetHasLOS(hasLOS);
	}
	return hasLOS;
}

void PlayScene::StoreObstacles()
{

}

void PlayScene::ClearNodes()
{
	for (auto object : GetDisplayList())
	{
		if (object->GetType() == GameObjectType::PATH_NODE)
		{
			RemoveChild(object);
		}
	}
	m_pGrid.clear();
}