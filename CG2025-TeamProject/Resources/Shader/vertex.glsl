#version 330 core
layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec3 in_Normal;
layout (location = 2) in vec2 in_TexCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 normalMat;

void main(void) {
	FragPos = vec3(model * vec4(in_Position, 1.0));
	Normal = mat3(normalMat) * in_Normal;
	TexCoord = in_TexCoord;

	gl_Position = projection * view * model * vec4(in_Position, 1.0);
}