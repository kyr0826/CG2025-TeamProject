#pragma once
// 윈도우
const int WINDOW_WIDTH				= 800;
const int WINDOW_HEIGHT				= 600;

// 계산용
const float M_PI					= 3.14159265358979323846f;
const float GRAVITY					= 9.81f;

// 카메라
const float CAMERA_DISTANCE			= 5.0f;
const float CAMERA_ROTATION_SPEED	= 5.0f;
const float INITIAL_CAMERA_Y		= 3.0f;

// 조명
const glm::vec3 LIGHT_POSITION		= glm::vec3(0.0f, 5.0f, 0.0f);
const glm::vec3 LIGHT_COLOR			= glm::vec3(1.0f, 1.0f, 1.0f);

// 발사 관련 상수
const float LAUNCH_FORCE_MIN		= 10.0f;
const float LAUNCH_FORCE_MAX		= 30.0f;
const int TRAJECTORY_POINTS			= 30;		// 궤적 점 개수
const float TRAJECTORY_TIME_STEP	= 0.05f;	// 점 사이의 시간 간격
const int TRAJECTORY_POINTS_COUNT	= 100;
const float TRAJECTORY_STEP			= 0.03f;