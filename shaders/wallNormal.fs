#version 330 core
out vec4 FragColor;

in vec3 FragPosition;
in vec2 TexCoord;

in vec3 TangViewPosition;
in vec3 TangLightPosition;
in vec3 TangFragPosition;

in float fogFactor;

uniform vec3 viewPosition;
uniform vec3 lightPosition;
uniform vec3 fogColor;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;

uniform bool lightOn;
uniform bool Blinn;
uniform bool fogOn;
uniform float shininess; 

void main()
{
    if (lightOn) {
	    vec3 texColor = texture(diffuseMap, TexCoord).rgb;
	    vec3 normal = texture(normalMap, TexCoord).rgb;
	    normal = normalize(normal * 2.0 - 1.0);

	    float ambientStrength = 0.1;
        float specularStrength = 0.2;

        vec3 lightDirection = normalize(TangLightPosition - TangFragPosition);
        vec3 viewDirection = normalize(TangViewPosition - TangFragPosition);

        vec3 ambient = ambientStrength * texColor;
        vec3 diffuse = max(dot(lightDirection, normal), 0.0) * texColor;
        vec3 specular = vec3(0.0);

        float spec = 0.0;
        if (Blinn)
            spec = pow(max(dot(normal, normalize(lightDirection + viewDirection)), 0.0), shininess);
        else
            spec = pow(max(dot(viewDirection, reflect(-lightDirection, normal)), 0.0), shininess);

        specular = vec3(specularStrength) * spec;

        FragColor = vec4(ambient + diffuse + specular, 1.0);
        if (fogOn)
            FragColor = mix(vec4(fogColor, 1.0f), FragColor, fogFactor);
    }
    else {
        FragColor = texture(diffuseMap, TexCoord);
        if (fogOn)
            FragColor = mix(vec4(fogColor, 1.0f), FragColor, fogFactor);
    }
}