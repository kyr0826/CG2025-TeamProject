#pragma once
#include "Common_Headers.h"
#include "GameManager.h"

#include <functional>
#include <map> 
namespace Core {

	enum class EventType {
		ScoreUpdated,
		PlaySound,
		GameStateChanged
	};

	struct EventData {
		int intVal = 0;									// 정수 값 전달용	: 점수
		float floatVal = 0.0f;							// 실수 값 전달용
		std::string strVal = "";						// 문자열 값 전달용	: 파일 명
		glm::vec3 vecVal = glm::vec3(0.0f);				// 벡터 값 전달용	: 위치
		GameState curGameState = GameState::None;		// 현재 게임 상태
		GameState nextGameState = GameState::None;		// 다음 게임 상태
	};

	// 이벤트 구독 인터페이스
	using EventHandler = std::function<void(const EventData&)>;

	class EventSystem {
	public:
		static EventSystem& GetInstance();

		void Subscribe(EventType type, EventHandler handler);
		void Publish(EventType type, const EventData& data = EventData());
		void PublishScore(int score);
		void PublishSound(const std::string& sound_url);
		void PublishGameState(GameState current, GameState next);
		void Clear() {
			listeners.clear();
		}
	private:
		EventSystem() {}

		std::map<EventType, std::vector<EventHandler>> listeners;
	};
}