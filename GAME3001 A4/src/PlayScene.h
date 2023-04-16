#pragma once
#ifndef __PLAY_SCENE__
#define __PLAY_SCENE__

#include "Scene.h"
#include "Enemy.h"
#include "Player.h"
#include "Obstacle.h"
#include "HealthBar.h"
#include "Background.h"
#include "DecisionTree.h"
#include "PathNode.h"

class PlayScene : public Scene
{
public:
	PlayScene();
	~PlayScene() override;

	// Scene LifeCycle Functions
	virtual void Draw() override;
	virtual void Update() override;
	virtual void Clean() override;
	virtual void HandleEvents() override;
	virtual void Start() override;
private:
	// IMGUI Function
	void GUI_Function();
	std::string m_guiTitle;
	glm::vec2 m_mousePosition;

	// game objects
	Background* m_pBG;
	Enemy* m_pEnemy;
	Player* m_pPlayer;
	HealthBar* m_pHealthBar;
	int m_enemyPosition[2];
	int m_playerPosition[2];
	int m_angle;
	bool m_playerFacingRight{};
	int m_pEnemyHealth = 5;
	int m_pPlayerHealth = 5;

	// decision tree
	DecisionTree* m_decTree;

	// UI Items
	// these are causing problems and idk why??
	//Button* m_pBackButton{};
	//Button* m_pNextButton{};
	//Label* m_pInstructionsLabel{};

	// Input Control
	int m_pCurrentInputType{};
	void GetPlayerInput();
	void GetKeyboardInput();

	// Grid members
	std::vector<PathNode*> m_pGrid;
	bool m_isGridEnabled;
	void BuildGrid();
	void ToggleGrid(bool state);
	void SetGridEnabled(bool state);
	[[nodiscard]] bool GetGridEnabled() const;

	// void CheckShipLOS(DisplayObject* target_object);
	bool CheckAgentLOS(Agent* agent, DisplayObject* target_object);
	bool CheckPathNodeLOS(PathNode* path_node, DisplayObject* target_object);
	void CheckAllNodesWithTarget(DisplayObject* target_object);
	void CheckAllNodesWithBoth();
	void SetPathNodeLOSDistance(int dist);
	int m_pathNodeLOSDistance;
	int m_LOSMode; // 0: nodes visible to target, 1: nodes visible to player, 2: nodes visible to both
	bool m_LOSWithBoth = false;
	bool m_isRange = false;

	// Obstacle members
	std::vector<Obstacle*> m_pObstacles;
	void StoreObstacles();

	// Convenience members
	void ClearNodes();
	bool m_bDebugView;
};

#endif /* defined (__PLAY_SCENE__) */