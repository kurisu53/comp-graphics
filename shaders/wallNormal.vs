#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 tangents;
layout (location = 4) in vec3 bitangents;

out vec3 FragPosition;
out vec2 TexCoord;

out vec3 TangViewPosition;
out vec3 TangLightPosition;
out vec3 TangFragPosition;

out float fogFactor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 viewPosition;
uniform vec3 lightPosition;

uniform float fogDensity;
uniform float fogGradient;

void main()
{
	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec3 Tang = normalize(normalMatrix * tangents);
	vec3 Norm = normalize(normalMatrix * normals);
	Tang = normalize(Tang - dot(Tang, Norm) * Norm);
	vec3 Bitang = cross(Norm, Tang);
	mat3 TBN = transpose(mat3(Tang, Bitang, Norm));

	FragPosition = vec3(model * vec4(position, 1.0));
	TexCoord = texCoords; 
	
	TangViewPosition = TBN * viewPosition;
	TangLightPosition = TBN * lightPosition;
	TangFragPosition = TBN * FragPosition;

	vec4 CameraPosition = view * model * vec4(position, 1.0);
	float distance = length(CameraPosition.xyz);
	fogFactor = exp(-pow((distance * fogDensity), fogGradient));
    fogFactor = clamp(fogFactor, 0.0, 1.0);

	gl_Position = projection * view * model * vec4(position, 1.0);
}