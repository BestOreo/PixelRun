#version 330 core

in vec2 TextCoord;
uniform sampler2D texture_diffuse0;
uniform sampler2D texture_disolve;
uniform vec3 stage1_color;
uniform vec3 stage2_color;
uniform vec3 stage3_color;
uniform float disolve_threshold;
uniform float stage1_threshold;
uniform float stage2_threshold;
uniform float stage3_threshold;

out vec4 color;

void main()
{
    float value = texture(texture_disolve, TextCoord).r;
    if(value < disolve_threshold){
        discard;
    }

    color = texture(texture_diffuse0, TextCoord);


    // if(value < stage3_threshold){
    //     color = vec4(stage3_color, 1.0f);
    // } else{
    //     if(value < stage2_threshold){
    //         color = vec4(stage2_color, 1.0f);
    //     } else{
    //         if(value < stage1_threshold){
    //             color = vec4(stage1_color, 1.0f);
    //         } else{
    //             color = texture(texture_diffuse0, TextCoord);
    //         }
    //     }
    // }
}