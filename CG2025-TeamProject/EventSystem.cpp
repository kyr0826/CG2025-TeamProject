#include "EventSystem.h"

namespace Core {
	EventSystem& EventSystem::GetInstance() {
		static EventSystem instance;
		return instance;
	}

	void EventSystem::Subscribe(EventType type, EventHandler handler) { listeners[type].push_back(handler); }

	void EventSystem::Publish(EventType type, const EventData& data) {
		if (listeners.find(type) != listeners.end()) {
			for (const auto& handler : listeners[type]) {
				handler(data);
			}
		}
	}

	void EventSystem::PublishScore(int score) {
		EventData data;
		data.intVal = score;
		Publish(EventType::ScoreUpdated, data);
	}

	void EventSystem::PublishSound(const std::string& sound_url) {
		EventData data;
		data.strVal = sound_url;
		Publish(EventType::PlaySound, data);
	}

	void EventSystem::PublishGameState(GameState current, GameState next) {
		EventData data;
		data.curGameState = current;
		data.nextGameState = next;

		std::string temp_str[] = { "None", "Playing", "Paused", "GameOver" };
		std::cout << "CUR : " << temp_str[(int)current+1] << " NEXT : " << temp_str[(int)next+1] << std::endl;

		Publish(EventType::GameStateChanged, data);
	}
}