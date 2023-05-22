#version 330 core

out vec4 color;

in vec2 texCoord;

uniform sampler2D ourTexture;
uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 viewPosition;
uniform float ambientStrength;
uniform float specularStrength;
uniform float shininess;

void main()
{
    vec3 ambient = ambientStrength * lightColor;
    
    vec3 norm = normalize(vec3(0.0, 0.0, 1.0)); // Normal vector (assuming the sun is facing the positive Z-axis)
    vec3 lightDir = normalize(lightPosition - vec3(gl_FragCoord.xy, 0.0)); // Direction from the fragment to the light source
    
    float diffuseStrength = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * lightColor;
    
    vec3 viewDir = normalize(viewPosition - vec3(gl_FragCoord.xy, 0.0)); // Direction from the fragment to the viewer
    
    vec3 reflectDir = reflect(-lightDir, norm); // Reflection direction
    float specularStrength = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * specularStrength * lightColor;
    
    vec4 texColor = texture(ourTexture, texCoord);
    
    vec3 result = (ambient + diffuse) * texColor.rgb + specular;
    
    color = vec4(result, texColor.a);
}
