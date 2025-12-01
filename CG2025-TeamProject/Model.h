#pragma once
#include "Common_Headers.h"
#include "Shader.h"

namespace Graphics {
	struct Vertex {
		glm::vec3 pos, normal;
		glm::vec2 uv;
	};

	struct Face { unsigned int v1, v2, v3; };

	class Model {
	public:
		Model(std::string file_url);
		~Model();

		Vertex* GetVertices() const { return vertices; }
		Face* GetFaces() const { return faces; }

		size_t GetVertCount() const { return vertex_count; }
		size_t GetFaceCount() const { return face_count; }

		void SetModelMatrix(const glm::mat4& modelMatrix);
		void SetShader(Shader* shader) { this->model_shader = shader; }
		void Render();

		void Recenter();
		float GetExactRadius();

	private:
		Shader* model_shader;

		Vertex* vertices;
		size_t vertex_count;

		Face* faces;
		size_t face_count;

		GLuint VBO, VAO, EBO;
		glm::mat4 modelMatrix;

		void ReadObjFile(const char* fileName);

		void ReadNewLine(char* str) {
			char* pos;
			if ((pos = strchr(str, '\n')) != NULL)
				*pos = '\0';
		}
	};
}