#pragma once
#ifndef __START_SCENE__
#define __START_SCENE__

#include "Background.h"
#include "Scene.h"
#include "Label.h"
#include "Button.h"
#include "Player.h"
#include "Target.h"

class StartScene final : public Scene
{
public:
	StartScene();
	~StartScene() override;

	// Inherited via Scene
	virtual void Draw() override;
	virtual void Update() override;
	virtual void Clean() override;
	virtual void HandleEvents() override;
	virtual void Start() override;
	
private:
	Label* m_pStartLabel{};
	Label* m_pInstructionsLabel{};
	Label* m_pReinaldo{};
	Label* m_pYun{};
	Label* m_pIgor{};
	Target* m_pTarget{};
	Background* m_pBackground;
	Button* m_pStartButton{};
};

#endif /* defined (__START_SCENE__) */