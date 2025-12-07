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

using namespace Graphics;
using namespace Objects;
using namespace Physics;
using namespace Utils;
using namespace Core;

GLvoid DrawScene();
GLvoid ReShape(int w, int h);
GLvoid Timer(int value);
GLvoid Keyboard(unsigned char key, int x, int y);

void CreateDish();
glm::vec3 CalculateLaunchPosition();
glm::vec3 CalculateLaunchVelocity();
void CalculateTrajectory();
void DrawTrajectory();
void SpawnReadyFruit();

void InitResources();

void OnFruitMerge(int nextLevel, glm::vec3 pos);
void OnObjectRemove(GameObject* obj);

void HandleScoreUpdate(const EventData& data);
void HandlePlaySound(const EventData& data);
void HandleGameOver(const EventData& data);

// 전역 변수
Shader* common_shader = nullptr;
Camera* main_camera = nullptr;
PhysicsWorld* physicsWorld = nullptr;

Model* common_sphere_model = nullptr;
float common_sphere_radius = 0.0f;
Model* dish_model = nullptr;

std::vector<GameObject*> renderObjects;


// 카메라 제어 변수
float g_camera_y_rotation = 0.0f;
float g_camera_distance = CAMERA_DISTANCE;

// 발사 제어 변수
float g_launch_pitch = 54.0f;		// 발사 각도 (높게 던져야 포물선이 예쁨)
float g_launch_yaw = 0.0f;			// 카메라 공전 각도
float g_launch_force = LAUNCH_FORCE;	// 발사 힘 (적당히 멀리 날아가도록)
float g_spawn_timer = 0.0f;			// 다음 과일 생성까지의 시간 카운터

GameObject* readyFruit = nullptr;

// 궤적 렌더링용
GLuint g_trajVAO = 0;
GLuint g_trajVBO = 0;
Shader* trajectoryShader = nullptr;

std::vector<glm::vec3> trajectoryPoints;

int g_total_Score = 0;
bool g_isGameOver = false;


void main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Physics_Test");
	glewExperimental = GL_TRUE;
	glewInit();

	trajectoryShader = new Shader("traj_frag.glsl", "traj_vert.glsl");
	glGenVertexArrays(1, &g_trajVAO);
	glGenBuffers(1, &g_trajVBO);

	common_shader = new Shader("fragment.glsl", "vertex.glsl");

	// 카메라 초기 위치 설정 (약간 위에서 바라봄)
	main_camera = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT, glm::vec3(0, CAMERA_Y, g_camera_distance));

	// 물리 월드 생성
	physicsWorld = new PhysicsWorld();
	physicsWorld->SetMergeCallback(OnFruitMerge);
	physicsWorld->SetRemoveCallback(OnObjectRemove);

	EventSystem::GetInstance().Subscribe(EventType::ScoreUpdated, HandleScoreUpdate);
	EventSystem::GetInstance().Subscribe(EventType::PlaySound, HandlePlaySound);
	EventSystem::GetInstance().Subscribe(EventType::GameStateChanged, HandleGameOver);

	InitResources();

	CreateDish();
	SpawnReadyFruit();

	glutDisplayFunc(DrawScene);
	glutReshapeFunc(ReShape);
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(16, Timer, 16);

	SoundManager::GetInstance().Play("BGM");

	glutMainLoop();

	// 종료 시 메모리 해제
	delete physicsWorld;
	delete main_camera;
	delete common_shader;
	for (auto obj : renderObjects) delete obj;
}

void CreateDish() {
	GameObject* dish = new GameObject("Dish", new Physics_Body(0.0f), dish_model, common_shader);
	dish->SetPosition(glm::vec3(0, DISH_Y_OFFSET, 0));
	dish->SetScale(glm::vec3(DISH_SCALE));

	GLuint dishTextureID = TextureManager::GetInstance().GetTexture("Dish");
	if (dishTextureID != 0) { dish->SetTexture(dishTextureID); }

	dish->SetModelColor(glm::vec3(1.0f, 1.0f, 1.0f));

	physicsWorld->SetDish(dish);
	renderObjects.push_back(dish);
}

void InitResources() {
	common_sphere_model = new Model("Fruit_Model.obj");
	common_sphere_model->Recenter();
	common_sphere_radius = common_sphere_model->GetExactRadius() * 0.98f;

	dish_model = new Model("Dish.obj");
	dish_model->Recenter();

	TextureManager::GetInstance().Initialize();
	SoundManager::GetInstance().Initialize();
	Graphics::UIManager::GetInstance().Initialize(common_shader);
}

GLvoid DrawScene() {
	glClearColor(.25, .25, .25, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	for (auto obj : renderObjects)
		obj->RenderModel(*main_camera);

	DrawTrajectory();

	Graphics::UIManager::GetInstance().Render(main_camera->width, main_camera->height);

	glutSwapBuffers();
}

GLvoid Timer(int value) {
	if (g_spawn_timer > 0.0f) {
		g_spawn_timer -= 0.016f;
		if (g_spawn_timer < 0.0f) {
			g_spawn_timer = 0.0f;
			SpawnReadyFruit();
		}
	}

	// 1. 물리 엔진 업데이트
	physicsWorld->Step(0.016f);

	// 2. 카메라 위치 갱신 (공전)
	main_camera->Position.x = sin(glm::radians(g_launch_yaw)) * g_camera_distance;
	main_camera->Position.z = cos(glm::radians(g_launch_yaw)) * g_camera_distance;

	// 3. 대기 구슬(ReadyBall) 위치 동기화
	if (readyFruit && g_spawn_timer == 0.0f) {
		glm::vec3 launchPos = CalculateLaunchPosition();
		readyFruit->SetPosition(launchPos);

		// 대기 중엔 물리 영향 안 받음
		readyFruit->GetPhysicsBody()->vel = glm::vec3(0);
		readyFruit->SetAcceleration(glm::vec3(0));

		// 궤적 계산
		CalculateTrajectory();
	}

	glutPostRedisplay();
	glutTimerFunc(16, Timer, 16);
}

GLvoid ReShape(int w, int h) {
	glViewport(0, 0, w, h);
	main_camera->width = w;
	main_camera->height = h;
}

GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'q': glutLeaveMainLoop(); return;

	case 32: // Space: 발사
	{
		if (readyFruit) {
			Physics_Body* body = readyFruit->GetPhysicsBody();
			body->vel = CalculateLaunchVelocity();
			readyFruit->SetBillboard(false);
			readyFruit->SetAcceleration(glm::vec3(0, -GRAVITY, 0));

			physicsWorld->AddBall(readyFruit);
			g_spawn_timer = SPAWN_DELAY;
			readyFruit = nullptr;
		}
	}
	break;

	case 'a':
	case 'A':
	case 'd':
	case 'D':
		g_launch_yaw += ((key == 'a' || key == 'A') ? -1 : 1) * CAMERA_ROTATION_SPEED;
		EventSystem::GetInstance().PublishSound("Camera_Rotate.wav");
		break;

		// 각도 제한 (너무 낮거나 너무 높지 않게)
	case 'w':
	case 'W':
		g_launch_pitch = std::min(g_launch_pitch + 2.0f, 68.0f);
		break;

	case 's':
	case 'S':
		g_launch_pitch = std::max(g_launch_pitch - 2.0f, 14.0f);
		break;

	case 'h':
		GameManager::GetInstance().GameStart();
		break;

	case 'j':
		GameManager::GetInstance().GamePause();
		break;

	case 'k':
		GameManager::GetInstance().GameResume();
		break;

	case 'l':
		GameManager::GetInstance().GameOver();
		break;
	}
}

glm::vec3 CalculateLaunchPosition() {
	glm::vec3 camPos = main_camera->Position;

	glm::vec3 target = glm::vec3(0, 0, 0);
	glm::vec3 forward = glm::normalize(target - camPos);
	glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));
	glm::vec3 up = glm::cross(right, forward);

	glm::vec3 offset =
		(forward * READY_FRUIT_OFFSET.z) +
		(right * READY_FRUIT_OFFSET.x) +
		(up * READY_FRUIT_OFFSET.y);

	return camPos + offset;
}

glm::vec3 CalculateLaunchVelocity() {
	glm::vec3 launchPos = CalculateLaunchPosition();
	glm::vec3 target = glm::vec3(0, 0, 0);

	glm::vec3 dir = glm::normalize(target - launchPos);
	glm::vec3 horizontalDir = glm::normalize(glm::vec3(dir.x, 0, dir.z));

	float radPitch = glm::radians(g_launch_pitch);

	// 위쪽으로 쏘는 벡터 (Pitch 적용)
	glm::vec3 launchDir = horizontalDir * cos(radPitch) + glm::vec3(0, 1, 0) * sin(radPitch);

	return glm::normalize(launchDir) * g_launch_force;
}

void CalculateTrajectory() {
	trajectoryPoints.clear();
	glm::vec3 startPos;
	if (readyFruit) startPos = readyFruit->GetPosition();
	else startPos = CalculateLaunchPosition();

	glm::vec3 vel = CalculateLaunchVelocity();
	glm::vec3 cur = startPos;
	trajectoryPoints.push_back(cur);

	for (int i = 0; i < TRAJECTORY_POINTS_COUNT; ++i) {
		vel += glm::vec3(0, -GRAVITY, 0) * TRAJECTORY_TIME_STEP;
		cur += vel * TRAJECTORY_TIME_STEP;
		trajectoryPoints.push_back(cur);
		if (cur.y < -2.0f) break;
	}
}

void DrawTrajectory() {
	if (trajectoryPoints.size() < 2) return;
	if (g_spawn_timer > 0.0f) return;

	trajectoryShader->Activate();

	glm::mat4 view = glm::lookAt(
		main_camera->Position,
		glm::vec3(0, 0, 0),
		main_camera->Up
	);

	glm::mat4 proj = glm::perspective(
		glm::radians(45.0f),
		(float)main_camera->width / (float)main_camera->height,
		0.1f,
		100.0f
	);

	trajectoryShader->SetMat4("view", view);
	trajectoryShader->SetMat4("projection", proj);
	trajectoryShader->SetVec3("lineColor", glm::vec3(0.2, 0.9f, 1.0f));

	glBindVertexArray(g_trajVAO);
	glBindBuffer(GL_ARRAY_BUFFER, g_trajVBO);

	glBufferData(GL_ARRAY_BUFFER, trajectoryPoints.size() * sizeof(glm::vec3), trajectoryPoints.data(), GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

	glLineWidth(4.0f);
	glDrawArrays(GL_LINE_STRIP, 0, trajectoryPoints.size());
	glLineWidth(1.0f);

	glBindVertexArray(0);
}

void SpawnReadyFruit() {
	FruitInfo info = GameManager::GetInstance().GetCurrentFruit();

	Physics_Body* body = new Physics_Body(info.mass);
	body->colliderRadius = common_sphere_radius;

	readyFruit = new GameObject(info.name, body, common_sphere_model, common_shader);

	readyFruit->SetScale(glm::vec3(info.scale));
	readyFruit->SetTexture(TextureManager::GetInstance().GetTexture(info.name));
	readyFruit->SetModelColor(info.color);
	readyFruit->fruitLevel = info.level;
	readyFruit->SetBillboard(true);
	renderObjects.push_back(readyFruit);
}

void OnFruitMerge(int nextLevel, glm::vec3 pos) {
	const FruitInfo& info = GameManager::GetInstance().GetFruitInfo(nextLevel);

	Physics_Body* body = new Physics_Body(info.mass);
	body->colliderRadius = common_sphere_radius * 0.95f;

	body->pos = pos;
	body->vel = glm::vec3(0.0f);
	body->acc = glm::vec3(0, -GRAVITY, 0);

	GameObject* newFruit = new GameObject(info.name, body, common_sphere_model, common_shader);
	newFruit->SetScale(glm::vec3(info.scale));
	newFruit->SetModelColor(info.color);
	newFruit->SetTexture(TextureManager::GetInstance().GetTexture(info.name));
	newFruit->fruitLevel = nextLevel;
	newFruit->PlayMergeAnimation();

	physicsWorld->AddBall(newFruit);
	renderObjects.push_back(newFruit);
	printf("Merge! Level %d -> %d\n", nextLevel - 1, nextLevel);
}

void OnObjectRemove(GameObject* obj) {
	auto it = std::remove(renderObjects.begin(), renderObjects.end(), obj);
	renderObjects.erase(it, renderObjects.end());
	delete obj; // 메모리 해제
}

void HandleScoreUpdate(const EventData& data) {
	g_total_Score += data.intVal;
	std::cout << "Score Up! Current Score: " << g_total_Score << std::endl;
}

void HandlePlaySound(const EventData& data) {
	std::cout << "[Sound System] Playing: " << data.strVal << std::endl;
	SoundManager::GetInstance().Play(data.strVal);
}

void HandleGameOver(const EventData& data) {
	if (g_isGameOver) return;
	if (data.nextGameState != GameState::GameOver) return;

	g_isGameOver = true;
	std::cout << "!!! GAME OVER !!!" << std::endl;
	glutLeaveMainLoop();
}