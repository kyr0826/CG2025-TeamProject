#pragma once
#include <memory>
#include "Scene.h"
#include "EventSystem.h"

class SceneManager {
public:
	static SceneManager& GetInstance() {
		static SceneManager instance;
		return instance;
	}

	void ChangeScene(std::shared_ptr<Scene> newScene) {
		Core::EventSystem::GetInstance().Clear();
		if (currentScene) { currentScene->OnExit(); }
		currentScene = newScene;
		currentScene->OnEnter();
	}

	std::shared_ptr<Scene> GetScene() { return currentScene; }

private:
	SceneManager() {}
	std::shared_ptr<Scene> currentScene;
};