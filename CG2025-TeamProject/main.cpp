#define _CRT_SECURE_NO_WARNINGS 

#include "Common_Headers.h"
#include "Shader.h"
#include "GameObject.h"
#include "Camera.h"
#include "PhysicsWorld.h"
#include "CommonUtils.h"
#include "EventSystem.h"
#include "GameManager.h"
#include "TextureManager.h"
#include "SoundManager.h"
#include "UIManager.h"
#include "ShaderManager.h"

#include "SceneManager.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "LoadingScene.h"

using namespace Graphics;
using namespace Objects;
using namespace Physics;
using namespace Utils;
using namespace Core;

GLvoid DrawScene();
GLvoid ReShape(int w, int h);
GLvoid Timer(int value);
GLvoid Keyboard(unsigned char key, int x, int y);

void main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("CG2025-TeamProject");
	glewExperimental = GL_TRUE;
	glewInit();

	ShaderManager::GetInstance().Initialize();
	Core::GameManager::GetInstance().Initialize();
	UIManager::GetInstance().Initialize();
	SoundManager::GetInstance().Initialize();
	/*Core::SoundManager::GetInstance().Initialize();
	Core::TextureManager::GetInstance().Initialize();*/
	SceneManager::GetInstance().ChangeScene(std::make_shared<LoadingScene>());

	glutDisplayFunc(DrawScene);
	glutReshapeFunc(ReShape);
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(16, Timer, 16);

	glutMainLoop();
}

GLvoid DrawScene() {
	glClearColor(.25, .25, .25, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	SceneManager::GetInstance().GetScene()->Render();

	glutSwapBuffers();
}

GLvoid Timer(int value) {
	SceneManager::GetInstance().GetScene()->Update(0.016f);

	glutPostRedisplay();
	glutTimerFunc(16, Timer, 16);
}

GLvoid ReShape(int w, int h) {
	glViewport(0, 0, w, h);
	SceneManager::GetInstance().GetScene()->Reshape(w, h);
}

GLvoid Keyboard(unsigned char key, int x, int y) {
	SceneManager::GetInstance().GetScene()->HandleInput(key);
}