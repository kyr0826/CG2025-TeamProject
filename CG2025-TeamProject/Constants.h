#pragma once
// 윈도우 상수
const int WINDOW_WIDTH				= 800;
const int WINDOW_HEIGHT				= 600;

// 계산용
const float M_PI					= 3.14159265358979323846f;
const float GRAVITY					= 9.81f;

// 카메라 상수
const float CAMERA_DISTANCE			= 7.0f;
const float CAMERA_ROTATION_SPEED	= 5.0f;
const float CAMERA_Y				= 3.0f;

// 조명 상수
const glm::vec3 LIGHT_POSITION		= glm::vec3(0.0f, 10.0f, 0.0f);
const glm::vec3 LIGHT_COLOR			= glm::vec3(1.0f, 1.0f, 1.0f);

// 발사 상수
const float LAUNCH_FORCE			= 5.0f;
const int TRAJECTORY_POINTS_COUNT	= 30;		// 궤적 점 개수
const float TRAJECTORY_TIME_STEP	= 0.05f;	// 점 사이의 시간 간격
const float SPAWN_DELAY 			= 0.5f;		// 다음 과일 생성까지의 지연 시간

// 애니메이션 상수
const float MERGE_ANIMATION_TIME = 0.2f;		// 병합 애니메이션 시간

// 리소르 루트
const std::string MODEL_ROOT = "Resources/Model";
const std::string SOUND_ROOT = "Resources/Sound";
const std::string TEXTURE_ROOT = "Resources/Texture";