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
uniform sampler2D bumpMap;
uniform float bumpScale;

uniform bool lightOn;
uniform bool Blinn;
uniform bool fogOn;
uniform bool parallaxOn;
uniform float shininess; 

void main()
{
    if (lightOn) {
        vec2 coords = vec2(0.0);
        vec3 viewDirection = normalize(TangViewPosition - TangFragPosition);

        if (parallaxOn) {
            coords = TexCoord;
            float minLayers = 8, maxLayers = 32;
            float layersCount = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDirection)));
            vec2 P = viewDirection.xy / viewDirection.z * bumpScale;
            vec2 deltaCoords = P / layersCount;
            float lDepth = 1.0 / layersCount;
            
            float currDepth = 0.0;
            float currValue = texture(bumpMap, coords).r;
            while (currDepth < currValue) {
                coords -= deltaCoords;
                currValue = texture(bumpMap, coords).r;
                currDepth += lDepth;
            }

            vec2 prev = coords + deltaCoords;
            float depthAfter = currValue - currDepth;
            float depthBefore = texture(bumpMap, prev).r - currDepth + lDepth;
            float weight = depthAfter / (depthAfter - depthBefore);
            coords = prev * weight + coords * (1.0 - weight);

            if (coords.x > 1.0 || coords.y > 1.0 || coords.x < 0.0 || coords.y < 0.0)
                discard;
        }
        else 
            coords = TexCoord;
        
	    vec3 texColor = texture(diffuseMap, coords).rgb;
	    vec3 normal = texture(normalMap, coords).rgb;
	    normal = normalize(normal * 2.0 - 1.0);

	    float ambientStrength = 0.1;
        float specularStrength = 0.2;

        vec3 lightDirection = normalize(TangLightPosition - TangFragPosition);
        
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