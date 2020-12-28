#version 330 core
out vec4 FragColor;

in vec3 Position;
in vec3 Normal;

uniform vec3 viewPosition;
uniform samplerCube skybox;

void main()
{
	vec3 viewDirection = normalize(Position - viewPosition);
	vec3 reflection = reflect(viewDirection, normalize(Normal));
	FragColor = vec4(texture(skybox, reflection).rgb, 1.0);
}