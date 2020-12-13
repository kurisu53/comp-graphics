#version 330 core
out vec4 FragColor;

in vec3 FragPosition;
in vec3 Normal;
in vec2 TexCoord;

in float fogFactor;

uniform vec3 lightPosition;
uniform vec3 viewPosition;
uniform vec3 lightColor;
uniform vec3 fogColor;
 
uniform sampler2D texture1;

uniform bool lightOn;
uniform bool Blinn;
uniform bool fogOn;
uniform float shininess; 

void main()
{
    if (lightOn) {
        float ambientStrength = 0.1;
        float specularStrength = 0.5;

        vec3 norm = normalize(Normal);
        vec3 lightDirection = normalize(lightPosition - FragPosition);
        vec3 viewDirection = normalize(viewPosition - FragPosition);

        vec3 ambient = ambientStrength * lightColor;
        vec3 diffuse = max(dot(norm, lightDirection), 0.0) * lightColor; 
        vec3 specular = vec3(0.0);

        float spec = 0.0;
        if (Blinn)
            spec = pow(max(dot(norm, normalize(lightDirection + viewDirection)), 0.0), shininess);
        else
            spec = pow(max(dot(viewDirection, reflect(-lightDirection, norm)), 0.0), shininess);

        specular = specularStrength * spec * lightColor;

        vec4 texColor = texture(texture1, TexCoord);
        FragColor = vec4(ambient + diffuse, 1.0) * texColor + vec4(specular, 1.0);
        if (fogOn)
            FragColor = mix(vec4(fogColor, 1.0f), FragColor, fogFactor);
    }
    else {
        FragColor = texture(texture1, TexCoord);
        if (fogOn)
            FragColor = mix(vec4(fogColor, 1.0f), FragColor, fogFactor);
    }
}