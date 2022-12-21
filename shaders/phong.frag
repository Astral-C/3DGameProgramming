#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2D texSampler;
layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragPos;

layout(location = 3) in vec3 cameraPos;
layout(location = 4) in vec3 lightDirection;
layout(location = 5) in vec4 lightDiffuse;
layout(location = 6) in vec4 lightAmbient;
layout(location = 7) in vec4 lightSpecular;

layout(location = 0) out vec4 outColor;

void main()
{
    vec3 lightDir = normalize(-lightDirection);
    
    float diff = max(dot(fragNormal,lightDir), 0.0);
    
    vec3 viewDir = normalize(cameraPos - vec3(fragPos));
    vec3 reflectDir = reflect(-lightDir,fragNormal);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), lightSpecular.w);

    vec4 baseColor = texture(texSampler, fragTexCoord);
    vec4 diffuse = diff * lightDiffuse;
    vec4 specular = vec4(lightSpecular.xyz, 1.0) * spec;

    outColor = (lightAmbient + diffuse + specular) * baseColor;
}
