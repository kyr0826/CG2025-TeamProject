#include "Common_Headers.h"
#include "TitleScene.h"
#include "SceneManager.h"
#include "GameScene.h"
#include "GameManager.h"

void TitleScene::OnEnter() {
	Core::SoundManager::GetInstance().Play("Title");
}

void TitleScene::OnExit() {
	Core::SoundManager::GetInstance().StopBGM();
}

void TitleScene::Update(float deltaTime) {
}

void TitleScene::Reshape(int w, int h) {
	Graphics::Camera* main_camera = Core::GameManager::GetInstance().GetGameCamera();
	main_camera->width = w;
	main_camera->height = h;
}

void TitleScene::Render() {
	Graphics::Camera* main_camera = Core::GameManager::GetInstance().GetGameCamera();
	Graphics::UIManager::GetInstance().RenderTitleScene(main_camera->width, main_camera->height);
}

void TitleScene::HandleInput(unsigned char key) {
	switch (key) {
	case KEY_ESC:
		glutLeaveMainLoop();
		break;

	case KEY_SPACE:
		SceneManager::GetInstance().ChangeScene(std::make_shared<GameScene>());
		break;
	}
}
