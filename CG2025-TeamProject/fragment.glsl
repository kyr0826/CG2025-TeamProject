#version 330 core 
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

uniform sampler2D outTexture;
uniform int useTexture;	
// 0 : 텍스처 없음 ( object color 사용 )
// 1 : 텍스처 있음
uniform int isUI;

void main(void)  { 

	// Ambient
	float ambientStrength = 0.3;
	vec3 ambient = ambientStrength * lightColor;
	
	// Diffuse 
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;
	
	// Specular
	int shininess = 32;
	float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);  
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	vec3 specular = specularStrength * spec * lightColor;  
		
	vec4 baseColor;
	if(useTexture == 0) {
		baseColor = vec4(objectColor, 1.0);
	} else {
		baseColor = texture(outTexture, TexCoord);
	}

	if (isUI == 1) {
        FragColor = baseColor;
        return; 
    }

	vec3 result = (ambient + diffuse + specular) * baseColor.rgb;
	FragColor = vec4(result, 1.0);
}