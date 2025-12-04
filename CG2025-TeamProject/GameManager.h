#pragma once  
#include "Common_Headers.h"  
#include "CommonUtils.h"  

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
	GameState GetCurrentGameState() const { return curGameState; }

	FruitInfo GetCurrentFruit() {  
		FruitInfo returnFruit = curFruit;  
		curFruit = nextFruit;  
		int nextLevel = CommonUtils::RandomInt(0, 4);  
		nextFruit = fruitTypes[nextLevel];  
		return curFruit;  
	}

	FruitInfo CurFruit() const { return curFruit; }
	FruitInfo NextFruit() const { return nextFruit; }

	FruitInfo GetFruitInfo(int fruit_level) const {
		return fruitTypes[fruit_level];
	}

private:  
	GameManager() : score{ 0 }, curGameState{ GameState::None } {  
		InitFruitData();
		curFruit = fruitTypes[0];  
		nextFruit = fruitTypes[1];  
	}  
	int score;  
	std::vector<FruitInfo> fruitTypes;  
	GameState curGameState;  
	FruitInfo curFruit, nextFruit;  

	void InitFruitData() {  
		fruitTypes.clear();  

		// 단계 | 이름 | 스케일 | 색상(RGB) | 질량(스케일에 비례)  
		fruitTypes.push_back({ "Cherry",		0, 0.15f, glm::vec3(0.8f, 0.0f, 0.0f), 1.0f });		// 0. 체리  
		fruitTypes.push_back({ "Strawberry",	1, 0.23f, glm::vec3(1.0f, 0.2f, 0.2f), 2.0f });		// 1. 딸기  
		fruitTypes.push_back({ "Grape",			2, 0.32f, glm::vec3(0.6f, 0.2f, 0.8f), 3.0f });		// 2. 포도  
		fruitTypes.push_back({ "Hallabong",		3, 0.40f, glm::vec3(1.0f, 0.6f, 0.0f), 4.0f });		// 3. 한라봉 (귤)  
		fruitTypes.push_back({ "Persimmon",		4, 0.50f, glm::vec3(1.0f, 0.4f, 0.0f), 5.0f });		// 4. 감  
		fruitTypes.push_back({ "Apple",			5, 0.62f, glm::vec3(0.9f, 0.1f, 0.1f), 7.0f }); 	// 5. 사과  
		fruitTypes.push_back({ "Pear",			6, 0.75f, glm::vec3(0.9f, 0.9f, 0.6f), 10.0f });	// 6. 참외 (노랑)  
		fruitTypes.push_back({ "Peach",			7, 0.90f, glm::vec3(1.0f, 0.7f, 0.8f), 13.0f });	// 7. 복숭아  
		fruitTypes.push_back({ "Pineapple",		8, 1.10f, glm::vec3(0.9f, 0.8f, 0.2f), 18.0f });	// 8. 파인애플  
		fruitTypes.push_back({ "Melon",			9, 1.35f, glm::vec3(0.5f, 0.9f, 0.5f), 25.0f });	// 9. 멜론  
		fruitTypes.push_back({ "Watermelon",	10, 1.60f, glm::vec3(0.1f, 0.7f, 0.2f), 35.0f });	// 10. 수박  
	}  

};  
}