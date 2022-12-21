#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2D texSampler;
layout(binding = 2) uniform sampler2D texSamplerDisplacement;
layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec4 colorMod;
layout(location = 3) in vec4 ambient;
layout(location = 4) in vec2 displacemenTexCoord;

layout(location = 0) out vec4 outColor;


void main()
{
    //vec3 lightVector = vec3(0,0,1);
    //float cosTheta = dot( fragNormal,lightVector );
    vec2 displacement = texture(texSamplerDisplacement, displacemenTexCoord).xy;
    displacement.x - 1.0;
    displacement.y - 1.0;

    vec4 color = texture(texSampler, fragTexCoord + (displacement) * 0.3);

    outColor = color * colorMod;
}
