#include "EndScene.h"
#include <algorithm>
#include "Game.h"
#include "glm/gtx/string_cast.hpp"
#include "EventManager.h"

EndScene::EndScene(bool win) :m_wincondition(win)
{
	EndScene::Start();
}

EndScene::~EndScene()
= default;

void EndScene::Draw()
{
	DrawDisplayList();
}

void EndScene::Update()
{
	UpdateDisplayList();
}

void EndScene::Clean()
{
	RemoveAllChildren();
	SoundManager::Instance().Unload("GameOver", SoundType::SOUND_MUSIC);
	SoundManager::Instance().Unload("Victory", SoundType::SOUND_MUSIC);
}

void EndScene::HandleEvents()
{
	EventManager::Instance().Update();

	// Button Events
	m_pRestartButton->Update();

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

void EndScene::Start()
{
	if (!m_wincondition)
	{
		// Preload music
		SoundManager::Instance().Load("../Assets/Audio/GameOver.mp3", "GameOver", SoundType::SOUND_MUSIC);
		SoundManager::Instance().SetMusicVolume(20);

		// Play Music
		SoundManager::Instance().PlayMusic("GameOver");
	}

	else
	{
		TextureManager::Instance().Load("../Assets/sprites/gameover.png", "game over");

		// Preload music
		SoundManager::Instance().Load("../Assets/Audio/Victory.mp3", "Victory", SoundType::SOUND_MUSIC);
		SoundManager::Instance().SetMusicVolume(20);

		// Play Music
		SoundManager::Instance().PlayMusic("Victory");
	}

	const SDL_Color blue = { 0, 0, 255, 255 };
	m_label = (m_wincondition) ? new  Label("Hyrule is saved!", "Dock51", 80, blue, glm::vec2(400.0f, 40.0f)) : new  Label("Hyrule has Fallen.", "Consolas", 80, blue, glm::vec2(400.0f, 40.0f));
	m_label->SetParent(this);
	AddChild(m_label);

	// Restart Button
	m_pRestartButton = new Button("../Assets/textures/restartButton.png", "restartButton", GameObjectType::RESTART_BUTTON);
	m_pRestartButton->GetTransform()->position = glm::vec2(400.0f, 400.0f);
	m_pRestartButton->AddEventListener(Event::CLICK, [&]()-> void
	{
		m_pRestartButton->SetActive(false);
		Game::Instance().ChangeSceneState(SceneState::PLAY);
	});

	m_pRestartButton->AddEventListener(Event::MOUSE_OVER, [&]()->void
	{
		m_pRestartButton->SetAlpha(128);
	});

	m_pRestartButton->AddEventListener(Event::MOUSE_OUT, [&]()->void
	{
		m_pRestartButton->SetAlpha(255);
	});

	AddChild(m_pRestartButton);

	ImGuiWindowFrame::Instance().SetDefaultGuiFunction();
}
