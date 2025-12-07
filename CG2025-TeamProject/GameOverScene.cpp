#include <string>
#include <memory>

#include "GameOverScene.h"
#include "SoundManager.h"
#include "UIManager.h"
#include "Camera.h"
#include "Constants.h"
#include "SceneManager.h"
#include "GameScene.h"
#include "TitleScene.h"

void GameOverScene::OnEnter() {
	Core::SoundManager::GetInstance().Play("GameOver");
}

void GameOverScene::OnExit() {
	Core::SoundManager::GetInstance().Stop("GameOver");
}

void GameOverScene::Update(float deltaTime) {
}

void GameOverScene::Reshape(int w, int h) {
	Graphics::Camera* main_camera = Core::GameManager::GetInstance().GetGameCamera();
	main_camera->width = w;
	main_camera->height = h;
}

void GameOverScene::Render() {
	Graphics::Camera* main_camera = Core::GameManager::GetInstance().GetGameCamera();
	Graphics::UIManager::GetInstance().RenderGameOverScene(main_camera->width, main_camera->height);
}

void GameOverScene::HandleInput(unsigned char key) {
	switch (key)
	{
	case KEY_ESC:
		SceneManager::GetInstance().ChangeScene(std::make_shared<TitleScene>());
		break;

	case 'R':
	case 'r':
		SceneManager::GetInstance().ChangeScene(std::make_shared<GameScene>());
		break;
	}
}