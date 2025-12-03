#pragma once
#include "Common_Headers.h"
#include "EventSystem.h"
#include "TextureManager.h"
#include "GameManager.h"
#include <string>

using namespace Core;

namespace Graphics {
	class UIManager {
	public:
		// 싱글톤 패턴
		static UIManager& GetInstance() {
			static UIManager instance;
			return instance;
		}

		// 초기화: 텍스처 로드 및 이벤트 구독
		void Initialize();

		// 렌더링: 매 프레임 호출되어 UI를 그림
		void Render(int windowWidth, int windowHeight);

	private:
		UIManager();
		~UIManager() {}

		// 상태 변수
		int currentScore = 0;
		bool isGameOver = false;

		// 텍스처 ID 저장용
		GLuint bgTextureID = 0;

		// 이벤트 콜백 함수
		void HandleScoreUpdate(const EventData& data);
		void HandleGameStateChange(const EventData& data);

		// 내부 헬퍼 함수
		void Begin2D(int width, int height);
		void End2D();
		void DrawImage(float x, float y, float w, float h, GLuint texID);
		void DrawText(float x, float y, std::string text, void* font = GLUT_BITMAP_HELVETICA_18);
	};
}