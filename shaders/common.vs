#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 texCoords;

out vec3 FragPosition;
out vec3 Normal;
out vec2 TexCoord;

out float fogFactor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float fogDensity;
uniform float fogGradient;

void main()
{
	mat3 normalMatrix = mat3(transpose(inverse(model)));

	FragPosition = vec3(model * vec4(position, 1.0f));
	Normal = normalize(normalMatrix * normals);
	TexCoord = texCoords; 

	vec4 CameraPosition = view * model * vec4(position, 1.0f);
	float distance = length(CameraPosition.xyz);
	fogFactor = exp(-pow((distance * fogDensity), fogGradient));
    fogFactor = clamp(fogFactor, 0.0f, 1.0f);

	gl_Position = projection * view * model * vec4(position, 1.0f);
}