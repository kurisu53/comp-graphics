#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;

out vec2 TexCoord;

uniform mat4 view;
uniform mat4 projection;
uniform vec3 camUp;
uniform vec3 camRight;
uniform vec3 placing;

void main()
{
    TexCoord = texCoords;
    vec3 rotatedModel = camRight * position.x + camUp * position.y;
    vec3 placedModel = 1.25 * rotatedModel + placing;
    gl_Position = projection * view * vec4(placedModel, 1.0);
}