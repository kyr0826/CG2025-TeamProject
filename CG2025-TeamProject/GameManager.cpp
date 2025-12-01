#include "GameManager.h"
#include "EventSystem.h"

namespace Core {
	void GameManager::AddScore(int score) {
		this->score += score;
		EventSystem::GetInstance().PublishScore(this->score);
	}

	void GameManager::GameStart() {
		EventSystem::GetInstance().PublishGameState(curGameState, GameState::Playing);
		curGameState = GameState::Playing;
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
		EventSystem::GetInstance().PublishGameState(curGameState, GameState::GameOver);
		curGameState = GameState::GameOver;
	}
}