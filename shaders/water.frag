#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2D texSampler;
layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec4 colorMod;
layout(location = 3) in vec4 ambient;
layout(location = 4) in vec2 fragTexCoord2;

layout(location = 0) out vec4 outColor;


void main()
{
    vec4 color = mix(texture(texSampler, fragTexCoord), texture(texSampler, fragTexCoord2), 0.5);

    if(color.r > 0.5 && color.g > 0.5 && color.b > 0.5 && color.r < 0.9 && color.g < 0.9 && color.b < 0.9){
        discard;
    } else if(color.r < 0.35 && color.g < 0.35 && color.b < 0.35){
        discard;
    }

    outColor = color + vec4(0.5,0.5,0.5,0.0) * vec4(1.0,1.0,1.0,0.5);
}
