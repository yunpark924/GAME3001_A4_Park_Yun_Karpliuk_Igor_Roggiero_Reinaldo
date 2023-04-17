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
	TextureManager::Instance().Draw("background",
		GetTransform()->position, 0, 255, true);
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
		TextureManager::Instance().Load("../Assets/sprites/badBG.png", "background");

		const auto size = TextureManager::Instance().GetTextureSize("background");
		SetWidth(size.x);
		SetHeight(size.y);
		GetTransform()->position = glm::vec2(size.x / 2, size.y / 2);
		// Preload music
		SoundManager::Instance().Load("../Assets/Audio/GameOver.mp3", "GameOver", SoundType::SOUND_MUSIC);
		SoundManager::Instance().SetMusicVolume(20);

		// Play Music
		SoundManager::Instance().PlayMusic("GameOver");
	}

	else
	{
		TextureManager::Instance().Load("../Assets/sprites/goodBG.png", "background");

		const auto size = TextureManager::Instance().GetTextureSize("background");
		SetWidth(size.x);
		SetHeight(size.y);
		GetTransform()->position = glm::vec2(size.x / 2, size.y / 2);

		// Preload music
		SoundManager::Instance().Load("../Assets/Audio/Victory.mp3", "Victory", SoundType::SOUND_MUSIC);
		SoundManager::Instance().SetMusicVolume(20);

		// Play Music
		SoundManager::Instance().PlayMusic("Victory");
	}

	const SDL_Color blue = { 100, 0, 255, 255 };
	m_label = (m_wincondition) ? new  Label("Hyrule is saved!", "Wild", 80, blue, glm::vec2(400.0f, 140.0f)) : new  Label("Hyrule has Fallen.", "Wild", 80, blue, glm::vec2(400.0f, 140.0f));
	m_label->SetParent(this);
	AddChild(m_label);

	// Restart Button
	m_pRestartButton = new Button("../Assets/textures/restartButton.png", "restartButton", GameObjectType::RESTART_BUTTON);
	m_pRestartButton->GetTransform()->position = glm::vec2(400.0f, 300.0f);
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
