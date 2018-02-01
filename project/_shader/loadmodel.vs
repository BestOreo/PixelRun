#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 textCoord;
layout(location = 2) in vec3 normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TextCoord;
    vec4 FragPosLightSpace;
} vs_out;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    vs_out.FragPos = vec3(model * vec4(position, 1.0));
    vs_out.Normal = transpose(inverse(mat3(model))) * normal;
    vs_out.TextCoord = textCoord;
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
}