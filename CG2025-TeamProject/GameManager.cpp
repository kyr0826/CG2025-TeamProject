#include <memory>

#include "GameManager.h"
#include "SoundManager.h"
#include "EventSystem.h"

#include "SceneManager.h"
#include "GameOverScene.h"

namespace Core {
	void GameManager::AddScore(int score) {
		this->score += score;
		EventSystem::GetInstance().PublishScore(this->score);
	}

	void GameManager::GameStart() {
		EventSystem::GetInstance().PublishGameState(curGameState, GameState::Playing);
		SettingStartFruit();
		curGameState = GameState::Playing;
		score = 0;
	}

	void GameManager::GamePause() {
		EventSystem::GetInstance().PublishGameState(curGameState, GameState::Paused);
		curGameState = GameState::Paused;
	}

	void GameManager::GameResume() {
		EventSystem::GetInstance().PublishGameState(curGameState, GameState::Playing);
		curGameState = GameState::Playing;
	}

	void GameManager::GameOver() {
		if (curGameState == GameState::GameOver)
			return;

		EventSystem::GetInstance().PublishGameState(curGameState, GameState::GameOver);

		curGameState = GameState::GameOver;

		if (highScore < score)
			highScore = score;

		SceneManager::GetInstance().ChangeScene(std::make_shared<GameOverScene>());
	}
}