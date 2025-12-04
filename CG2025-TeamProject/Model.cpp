#include <cstdio>
#include <sstream>
#include "Model.h"

namespace Graphics {
	Model::Model(std::string file_url) {
		modelMatrix = glm::mat4(1.0f);

		std::string full_path = MODEL_ROOT + "/" + file_url;
		ReadObjFile(full_path.c_str());

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, uv));
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);
	}

	Model::~Model() { }

	void Model::SetModelMatrix(const glm::mat4& modelMatrix) { this->modelMatrix = modelMatrix; }

	void Model::Render() {
		glBindVertexArray(VAO);
		GLuint id = model_shader->GetShaderProgramID();
		model_shader->SetVec3("lightPos", LIGHT_POSITION);
		model_shader->SetVec3("lightColor", LIGHT_COLOR);
		model_shader->SetMat4("model", modelMatrix);

		glm::mat4 normalMat = glm::transpose(glm::inverse(modelMatrix));
		model_shader->SetMat4("normalMat", normalMat);

		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		glBindVertexArray(0);
	}

	/// <summary>
	/// 중심점을 (0,0,0)으로 강제 이동시키는 함수
	/// </summary>
	void Model::Recenter() {
		glm::vec3 minPos(FLT_MAX), maxPos(-FLT_MAX);
		for (size_t i = 0; i < vertices.size(); ++i) {
			minPos = glm::min(minPos, vertices[i].pos);
			maxPos = glm::max(maxPos, vertices[i].pos);
		}

		glm::vec3 center = (minPos + maxPos) * 0.5f;
		for (size_t i = 0; i < vertices.size(); ++i) vertices[i].pos -= center;

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), &vertices[0]);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	/// <summary>
	/// 정확한 물리 반지름 계산
	/// </summary>
	float Model::GetExactRadius() {
		float maxDistSq = 0.0f;
		for (size_t i = 0; i < vertices.size(); ++i) {
			float d = glm::dot(vertices[i].pos, vertices[i].pos);
			if (d > maxDistSq) maxDistSq = d;
		}
		return std::sqrt(maxDistSq);
	}

	void Model::ReadObjFile(const char* fileName) {
		FILE* file;
		fopen_s(&file, fileName, "r");
		if (!file) exit(EXIT_FAILURE);

		char line[256];

		size_t vertex_count = 0;
		size_t face_count = 0;
		size_t normal_count = 0;
		size_t uv_count = 0;

		while (fgets(line, sizeof(line), file)) {
			ReadNewLine(line);

			if (line[0] == 'v' && line[1] == ' ') vertex_count++;
			else if (line[0] == 'v' && line[1] == 'n') normal_count++;
			else if (line[0] == 'v' && line[1] == 't') uv_count++;
			else if (line[0] == 'f' && line[1] == ' ') face_count++;
		}
		fseek(file, 0, SEEK_SET);

		glm::vec3* temp_v = (glm::vec3*)calloc(vertex_count, sizeof(glm::vec3));
		glm::vec3* temp_n = (glm::vec3*)calloc(normal_count, sizeof(glm::vec3));
		glm::vec2* temp_uv = (glm::vec2*)calloc(uv_count, sizeof(glm::vec2));

		size_t v_idx = 0, n_idx = 0, f_idx = 0, uv_idx = 0;
		while (fgets(line, sizeof(line), file)) {
			ReadNewLine(line);

			if (line[0] == 'v' && line[1] == ' ') {
				sscanf_s(line + 2, "%f %f %f", &temp_v[v_idx].x, &temp_v[v_idx].y, &temp_v[v_idx].z);
				v_idx++;
				continue;
			}
			else if (line[0] == 'v' && line[1] == 'n') {
				sscanf_s(line + 2, "%f %f %f", &temp_n[n_idx].x, &temp_n[n_idx].y, &temp_n[n_idx].z);
				n_idx++;
				continue;
			}
			else if (line[0] == 'v' && line[1] == 't') {
				sscanf_s(line + 2, "%f %f", &temp_uv[uv_idx].x, &temp_uv[uv_idx].y);
				uv_idx++;
				continue;
			}
			else if (line[0] == 'f' && line[1] == ' ') {
				unsigned int v[3], vt[3], vn[3];
				int matches = sscanf_s(line + 2, "%d/%d/%d %d/%d/%d %d/%d/%d"
					, &v[0], &vt[0], &vn[0]
					, &v[1], &vt[1], &vn[1]
					, &v[2], &vt[2], &vn[2]);

				if (matches != 9) {
					sscanf_s(line + 2, "%d//%d %d//%d %d//%d"
						, &v[0], &vn[0]
						, &v[1], &vn[1]
						, &v[2], &vn[2]);
					vt[0] = vt[1] = vt[2] = 0;
				}

				for (int i = 0; i < 3; ++i) {
					Vertex vertex = {
						temp_v[v[i] - 1],
						temp_n[vn[i] - 1],
						temp_uv[vt[i] - 1],
					};
					vertices.push_back(vertex);
				}
			}
		}
		fclose(file);

		free(temp_v);
		free(temp_n);
		free(temp_uv);

		if (strstr(fileName, "Fruit") || strstr(fileName, "Fruit")) {
			for (auto& v : vertices) {
				v.normal = glm::normalize(v.pos);
			}
		}
		else {
			for (size_t i = 0; i < vertices.size(); i += 3) {
				glm::vec3& p0 = vertices[i + 0].pos;
				glm::vec3& p1 = vertices[i + 1].pos;
				glm::vec3& p2 = vertices[i + 2].pos;

				glm::vec3 N = glm::normalize(glm::cross(p1 - p0, p2 - p0));

				vertices[i + 0].normal += N;
				vertices[i + 1].normal += N;
				vertices[i + 2].normal += N;
			}

			for (auto& v : vertices) {
				v.normal = glm::normalize(v.normal);
			}
		}
	}
}