#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 projection;
uniform mat4 view;
uniform vec3 offset;
uniform vec4 pColor;

void main()
{
    float scale = 2.0f;
    TexCoords = vertex.zw;
    ParticleColor = pColor;

    gl_Position = projection * view * vec4((vertex.xy * scale) + offset.xy, offset.z, 1.0);
}