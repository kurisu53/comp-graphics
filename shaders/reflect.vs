#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;

out vec3 Position;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	mat3 normalMatrix = mat3(transpose(inverse(model)));
	Normal = normalize(normalMatrix * normals);
	Position = vec3(model * vec4(position, 1.0f));
	gl_Position = projection * view * model * vec4(position, 1.0f);
}