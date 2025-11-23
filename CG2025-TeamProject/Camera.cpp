#include "Camera.h"

namespace Graphics {
	Camera::Camera(int width, int height, glm::vec3 position) :
		width{ width },
		height{ height },
		Position{ position } {
		Up = glm::vec3(0, 1, 0);
	}

	void Camera::Matrix(float Fovdeg, float nearPlane, float farPlane, const Shader& shader) {
		glm::mat4 view = glm::lookAt(Position, glm::vec3(0, 0, 0), Up);
		glm::mat4 projection = glm::perspective(
			glm::radians(Fovdeg),
			(float)width / (float)height,
			nearPlane,
			farPlane
		);

		GLuint v = glGetUniformLocation(shader.GetShaderProgramID(), "view");
		GLuint p = glGetUniformLocation(shader.GetShaderProgramID(), "projection");
		GLuint vp = glGetUniformLocation(shader.GetShaderProgramID(), "viewPos");

		glUniformMatrix4fv(v, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(p, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(vp, 1, GL_FALSE, glm::value_ptr(Position));
	}
}

