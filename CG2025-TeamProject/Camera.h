#pragma once
#include "Common_Headers.h"
#include "Shader.h"

namespace Graphics {
	class Camera {
	public:
		glm::vec3 Position;
		glm::vec3 Up;
		int width, height;

		Camera(int width, int height, glm::vec3 position);
		void Matrix(float Fovdeg, float nearPlane, float farPlane, const Shader& shader);
	};
}
