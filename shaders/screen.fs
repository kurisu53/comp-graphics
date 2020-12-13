#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D screenTexture;

void main()
{
    FragColor = texture(screenTexture, TexCoord);
    float avg = (FragColor.r + FragColor.g + FragColor.b) / 3.0;
    FragColor = vec4(avg, avg, avg, 1.0);
} 