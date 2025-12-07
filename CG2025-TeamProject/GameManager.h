#pragma once  
#include "Common_Headers.h"  
#include "CommonUtils.h"  
#include "Camera.h"

using namespace Utils;

namespace Core {
	struct FruitInfo {
		std::string name;
		int level;
		float scale;
		glm::vec3 color;
		float mass;

		FruitInfo& operator=(const FruitInfo& other) {
			if (this != &other) {
				name = other.name;
				level = other.level;
				scale = other.scale;
				color = other.color;
				mass = other.mass;
			}
			return *this;
		}
	};

	enum class GameState {
		None = -1,
		Playing,
		Paused,
		GameOver
	};

	class GameManager {
	public:
		static GameManager& GetInstance() {
			static GameManager instance;
			return instance;
		}

		void AddScore(int score);

		void GameStart();
		void GamePause();
		void GameResume();
		void GameOver();

		int GetScore() const { return score; }
		int GetHighScore() const { return highScore; }

		GameState GetCurrentGameState() const { return curGameState; }

		void UpdateFruit() {
			curFruit = nextFruit;
			int randLevel = CommonUtils::RandomInt(0, 4);
			nextFruit = fruitTypes[randLevel];
		}

		FruitInfo CurFruit() const { return curFruit; }
		FruitInfo NextFruit() const { return nextFruit; }

		FruitInfo GetFruitInfo(int fruit_level) const { return fruitTypes[fruit_level]; }

		Graphics::Camera* GetGameCamera() { return game_camera; }

		void Initialize() { 
			score = highScore = 0; 
			game_camera = new Graphics::Camera(WINDOW_WIDTH, WINDOW_HEIGHT, glm::vec3(0, CAMERA_Y, CAMERA_DISTANCE));
		}

	private:
		GameManager() : score{ 0 }, curGameState{ GameState::None }, game_camera(nullptr){
			InitFruitData();
			SettingStartFruit();
		}

		int score;
		int highScore;
		std::vector<FruitInfo> fruitTypes;
		GameState curGameState;
		FruitInfo curFruit, nextFruit;
		Graphics::Camera* game_camera;

		void InitFruitData() {
			fruitTypes.clear();

			// 단계 | 이름 | 스케일 | 색상(RGB) | 질량(스케일에 비례)  
			fruitTypes.push_back({ "Blueberry",  0, 0.14f, glm::vec3(0.3f, 0.2f, 0.6f), 1.0f });		// 0. 블루베리
			fruitTypes.push_back({ "Cherry",     1, 0.22f, glm::vec3(0.9f, 0.1f, 0.1f), 2.0f });		// 1. 체리
			fruitTypes.push_back({ "Plum",       2, 0.33f, glm::vec3(0.45f, 0.15f, 0.55f), 3.0f });		// 2. 자두
			fruitTypes.push_back({ "Mandarin",   3, 0.45f, glm::vec3(1.0f, 0.55f, 0.0f), 4.0f });		// 3. 귤

			fruitTypes.push_back({ "Persimmon",  4, 0.50f, glm::vec3(0.95f, 0.4f, 0.1f), 5.0f });		// 4. 감
			fruitTypes.push_back({ "Apple",      5, 0.55f, glm::vec3(0.9f, 0.15f, 0.1f), 7.0f });		// 5. 사과
			fruitTypes.push_back({ "Pear",       6, 0.65f, glm::vec3(0.95f, 0.95f, 0.7f), 10.0f });		// 6. 배
			fruitTypes.push_back({ "Peach",      7, 0.78f, glm::vec3(1.0f, 0.75f, 0.85f), 13.0f });		// 7. 복숭아
			fruitTypes.push_back({ "Papaya",     8, 0.90f, glm::vec3(0.95f, 0.75f, 0.25f), 18.0f });	// 8. 파파야
			fruitTypes.push_back({ "Melon",      9, 1.0f, glm::vec3(0.55f, 0.9f, 0.55f), 25.0f });		// 9. 멜론
			fruitTypes.push_back({ "Watermelon", 10, 1.20f, glm::vec3(0.15f, 0.7f, 0.2f), 35.0f });		// 10. 수박
		}

		void SettingStartFruit() {
			int curRand = CommonUtils::RandomInt(0, 4);
			int nextRand = CommonUtils::RandomInt(0, 4);
			curFruit = fruitTypes[curRand];
			nextFruit = fruitTypes[nextRand];
		}
	};
}