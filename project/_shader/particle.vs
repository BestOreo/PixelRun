#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

out vec2 TexCoords;
out vec4 ParticleColor;

uniform float scale;
uniform vec3 particle_center_worldspace;
uniform vec2 random_offset;
uniform vec4 particle_color;
// Values that stay constant for the whole mesh.
uniform mat4 vp_mat; // Model-View-Projection matrix, but without the Model (the position is in BillboardPos; the orientation depends on the camera)
uniform vec3 camera_right;
uniform vec3 camera_up;

void main()
{
    vec3 vertex_position_worldspace =
        particle_center_worldspace
        + camera_right * (vertex.x * scale + random_offset.x)
        + camera_up * (vertex.y * scale + random_offset.y);

    gl_Position = vp_mat * vec4(vertex_position_worldspace, 1.0f);

    TexCoords = vertex.zw;
    ParticleColor = particle_color;
}
