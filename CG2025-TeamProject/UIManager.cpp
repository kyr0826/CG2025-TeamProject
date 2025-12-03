#include "UIManager.h"

namespace Graphics {
	UIManager::UIManager() {
		currentScore = 0;
		isGameOver = false;
		bgTextureID = 0;
	}

	void UIManager::Initialize() {
		// 1. 이벤트 구독
		EventSystem::GetInstance().Subscribe(EventType::ScoreUpdated,
			std::bind(&UIManager::HandleScoreUpdate, this, std::placeholders::_1));

		EventSystem::GetInstance().Subscribe(EventType::GameStateChanged,
			std::bind(&UIManager::HandleGameStateChange, this, std::placeholders::_1));

		// 2. UI 배경 이미지 로드
		bgTextureID = TextureManager::GetInstance().GetTexture("UI_Back");
	}

	void UIManager::Render(int windowWidth, int windowHeight) {
		// 3D 모드 -> 2D 모드 전환
		Begin2D(windowWidth, windowHeight);

		// 1. 배경 이미지 그리기
		if (bgTextureID != 0) {
			glColor3f(1.0f, 1.0f, 1.0f);
			DrawImage(0, 0, (float)windowWidth, (float)windowHeight, bgTextureID);
		}

		// 2. 점수 텍스트 그리기
		glColor3f(0.3f, 0.2f, 0.1f); // 짙은 갈색 글씨
		std::string scoreStr = std::to_string(currentScore);

		// 화면 비율에 따라 위치가 달라질 수 있으므로 비율로 잡거나 절대 좌표 사용
		DrawText(130.0f, windowHeight - 78.0f, scoreStr, GLUT_BITMAP_TIMES_ROMAN_24);

		// 2D 모드 -> 3D 모드 복구
		End2D();
	}

	void UIManager::Begin2D(int width, int height) {
		glUseProgram(0);
		glDisable(GL_DEPTH_TEST); // 깊이 테스트 끔 (UI가 항상 위에)
		glDisable(GL_LIGHTING);   // 조명 끔 (이미지 색상 왜곡 방지)

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0, width, 0, height); // 2D 직교 투영

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
	}

	void UIManager::End2D() {
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

		glEnable(GL_LIGHTING);
		glEnable(GL_DEPTH_TEST);
	}

	void UIManager::DrawImage(float x, float y, float w, float h, GLuint texID) {
		if (texID == 0) return;

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texID);

		// 투명도(Alpha) 처리를 위해 블렌딩 활성화
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex2f(x, y);			// 좌하
		glTexCoord2f(1, 0); glVertex2f(x + w, y);		// 우하
		glTexCoord2f(1, 1); glVertex2f(x + w, y + h);	// 우상
		glTexCoord2f(0, 1); glVertex2f(x, y + h);		// 좌상
		glEnd();

		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
	}

	void UIManager::DrawText(float x, float y, std::string text, void* font) {
		glRasterPos2f(x, y);
		for (char c : text) {
			glutBitmapCharacter(font, c);
		}
	}

	void UIManager::HandleScoreUpdate(const EventData& data) {
		currentScore += data.intVal;
	}

	void UIManager::HandleGameStateChange(const EventData& data) {
		if (data.nextGameState == GameState::GameOver) {
			isGameOver = true;
		}
		else if (data.nextGameState == GameState::Playing) {
			isGameOver = false;
			currentScore = 0;
		}
	}
}