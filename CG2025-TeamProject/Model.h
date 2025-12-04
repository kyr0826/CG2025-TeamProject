#pragma once
#include "Common_Headers.h"
#include "Shader.h"

namespace Graphics {
	struct Vertex {
		glm::vec3 pos, normal;
		glm::vec2 uv;
	};

	class Model {
	public:
		Model(std::string file_url);
		~Model();

		std::vector<Vertex> GetVertices() const { return vertices; }

		void SetModelMatrix(const glm::mat4& modelMatrix);
		void SetShader(Shader* shader) { this->model_shader = shader; }
		void Render();

		void Recenter();
		float GetExactRadius();

	private:
		Shader* model_shader;

		std::vector<Vertex> vertices;
		GLuint VBO, VAO;

		glm::mat4 modelMatrix;

		void ReadObjFile(const char* fileName);

		void ReadNewLine(char* str) {
			char* pos;
			if ((pos = strchr(str, '\n')) != NULL)
				*pos = '\0';
		}
	};
}