#version 330 core 
layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec3 in_Normal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 normalMat;

void main(void)	{ 
	FragPos = vec3(model * vec4(in_Position, 1.0));
	Normal = mat3(normalMat) * in_Normal;

	gl_Position = projection *  view * model * vec4 (in_Position, 1.0);
}