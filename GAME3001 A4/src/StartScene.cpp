#include "StartScene.h"
#include <algorithm>
#include "Game.h"
#include "glm/gtx/string_cast.hpp"
#include "EventManager.h"

StartScene::StartScene()
{
	StartScene::Start();
}

StartScene::~StartScene()
= default;

void StartScene::Draw()
{
	DrawDisplayList();
}

void StartScene::Update()
{
	UpdateDisplayList();
}

void StartScene::Clean()
{
	RemoveAllChildren();
}

void StartScene::HandleEvents()
{
	EventManager::Instance().Update();

	// Keyboard Events
	if (EventManager::Instance().IsKeyDown(SDL_SCANCODE_ESCAPE))
	{
		Game::Instance().Quit();
	}

	if (EventManager::Instance().IsKeyDown(SDL_SCANCODE_1))
	{
		Game::Instance().ChangeSceneState(SceneState::PLAY);
	}
}

void StartScene::Start()
{
	m_pBackground = new Background();
	AddChild(m_pBackground, 0);
	const SDL_Color blue = { 100, 0, 255, 255 };
	m_pStartLabel = new Label("Legend of Link", "Wild", 100, blue, glm::vec2(400.0f, 140.0f));
	m_pStartLabel->SetParent(this);
	AddChild(m_pStartLabel);

	m_pInstructionsLabel = new Label("Press Start to play", "Wild", 40, blue, glm::vec2(400.0f, 200.0f));
	m_pInstructionsLabel->SetParent(this);
	AddChild(m_pInstructionsLabel);

	//Added Names and ID'S for group members. - Reinaldo
	
	m_pReinaldo = new Label("Reinaldo Roggiero - 101369223", "Wild", 30, blue, glm::vec2(280.0f, 470.0f));
	m_pReinaldo->SetParent(this);
	AddChild(m_pReinaldo);

	m_pYun = new Label("Yun Park - 101426966", "Wild", 30, blue, glm::vec2(280.0f, 520.0f));
	m_pYun->SetParent(this);
	AddChild(m_pYun);

	m_pIgor = new Label("Igor Karpliuk - 101330933", "Wild", 30, blue, glm::vec2(280.0f, 570.0f));
	m_pIgor->SetParent(this);
	AddChild(m_pIgor);

	m_pTarget = new Target();
	m_pTarget->GetTransform()->position = glm::vec2(400.0f, 300.0f);
	AddChild(m_pTarget);

	// Start Button
	m_pStartButton = new Button();
	m_pStartButton->GetTransform()->position = glm::vec2(400.0f, 400.0f);

	m_pStartButton->AddEventListener(Event::CLICK, [&]()-> void
		{
			m_pStartButton->SetActive(false);
			Game::Instance().ChangeSceneState(SceneState::PLAY);
		});

	m_pStartButton->AddEventListener(Event::MOUSE_OVER, [&]()->void
		{
			m_pStartButton->SetAlpha(128);
		});

	m_pStartButton->AddEventListener(Event::MOUSE_OUT, [&]()->void
		{
			m_pStartButton->SetAlpha(255);
		});
	AddChild(m_pStartButton);

	ImGuiWindowFrame::Instance().SetDefaultGuiFunction();
}