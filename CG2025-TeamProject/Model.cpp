#include <cstdio>
#include <sstream>
#include "Model.h"

namespace Graphics {
	Model::Model(std::string file_url) {
		modelMatrix = glm::mat4(1.0f);
		ReadObjFile(file_url.c_str());

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(Vertex), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, face_count * sizeof(Face), faces, GL_STATIC_DRAW);

		glBindVertexArray(0);
	}

	Model::~Model() {
		if (vertices) free(vertices);
		if (faces) free(faces);
	}

	void Model::SetModelMatrix(const glm::mat4& modelMatrix) { this->modelMatrix = modelMatrix; }

	void Model::Render() {
		glBindVertexArray(VAO);
		GLuint id = model_shader->GetShaderProgramID();

		glUniform3fv(glGetUniformLocation(id, "lightPos"), 1, glm::value_ptr(LIGHT_POSITION));
		glUniform3fv(glGetUniformLocation(id, "lightColor"), 1, glm::value_ptr(LIGHT_COLOR));
		glUniformMatrix4fv(glGetUniformLocation(id, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));

		glm::mat4 normalMat = glm::transpose(glm::inverse(modelMatrix));
		glUniformMatrix4fv(glGetUniformLocation(id, "normalMat"), 1, GL_FALSE, glm::value_ptr(normalMat));

		glDrawElements(GL_TRIANGLES, face_count * 3, GL_UNSIGNED_INT, 0);
	}

	/// <summary>
	/// 중심점을 (0,0,0)으로 강제 이동시키는 함수
	/// </summary>
	void Model::Recenter() {
		glm::vec3 minPos(FLT_MAX), maxPos(-FLT_MAX);
		for (size_t i = 0; i < vertex_count; ++i) {
			minPos = glm::min(minPos, vertices[i].pos);
			maxPos = glm::max(maxPos, vertices[i].pos);
		}

		glm::vec3 center = (minPos + maxPos) * 0.5f;
		for (size_t i = 0; i < vertex_count; ++i) vertices[i].pos -= center;

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_count * sizeof(Vertex), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	/// <summary>
	/// 정확한 물리 반지름 계산
	/// </summary>
	float Model::GetExactRadius() {
		float maxDistSq = 0.0f;
		for (size_t i = 0; i < vertex_count; ++i) {
			float d = glm::dot(vertices[i].pos, vertices[i].pos);
			if (d > maxDistSq) maxDistSq = d;
		}
		return std::sqrt(maxDistSq);
	}

	void Model::ReadObjFile(const char* fileName) {
		FILE* file;
		fopen_s(&file,fileName, "r");
		if (!file) exit(EXIT_FAILURE);

		char line[256];

		vertex_count = 0;
		face_count = 0;
		size_t normal_count = 0;

		while (fgets(line, sizeof(line), file)) {
			ReadNewLine(line);

			if (line[0] == 'v' && line[1] == ' ') vertex_count++;
			else if (line[0] == 'v' && line[1] == 'n') normal_count++;
			else if (line[0] == 'f' && line[1] == ' ') face_count++;
		}
		fseek(file, 0, SEEK_SET);

		vertices = (Vertex*)calloc(vertex_count, sizeof(Vertex));
		faces = (Face*)calloc(face_count, sizeof(Face));
		glm::vec3* temp_n = (glm::vec3*)calloc(normal_count, sizeof(glm::vec3));

		size_t v_idx = 0, n_idx = 0, f_idx = 0;
		while (fgets(line, sizeof(line), file)) {
			ReadNewLine(line);

			if (line[0] == 'v' && line[1] == ' ') {
				sscanf_s(line + 2, "%f %f %f", &vertices[v_idx].pos.x, &vertices[v_idx].pos.y, &vertices[v_idx].pos.z);
				v_idx++;
			}
			else if (line[0] == 'v' && line[1] == 'n') {
				sscanf_s(line + 2, "%f %f %f", &temp_n[n_idx].x, &temp_n[n_idx].y, &temp_n[n_idx].z);
				n_idx++;
			}
			else if (line[0] == 'f' && line[1] == ' ') {
				unsigned int v[3], vn[3], temp;
				int matches = sscanf_s(line + 2, "%d/%d/%d %d/%d/%d %d/%d/%d"
					, &v[0], &temp, &vn[0]
					, &v[1], &temp, &vn[1]
					, &v[2], &temp, &vn[2]);

				if (matches != 9) {
					sscanf_s(line + 2, "%d//%d %d//%d %d//%d"
						, &v[0], &vn[0]
						, &v[1], &vn[1]
						, &v[2], &vn[2]);
				}

				faces[f_idx].v1 = v[0] - 1;
				faces[f_idx].v2 = v[1] - 1;
				faces[f_idx].v3 = v[2] - 1;

				if (vn[0] > 0) vertices[v[0] - 1].normal = temp_n[vn[0] - 1];
				if (vn[1] > 0) vertices[v[1] - 1].normal = temp_n[vn[1] - 1];
				if (vn[2] > 0) vertices[v[2] - 1].normal = temp_n[vn[2] - 1];
				f_idx++;
			}
		}
		fclose(file);
		free(temp_n);
	}
}