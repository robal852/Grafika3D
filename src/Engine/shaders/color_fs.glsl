#version 450

layout(location=0) out vec4 vFragColor;

#if __VERSION__ > 410
layout(std140, binding=0) uniform Modifiers {
#else
    layout(std140) uniform Color {
    #endif
    vec4  Kd;
    bool use_map_Kd;
};

in vec2 vertex_texcoords;

uniform sampler2D map_Kd;

void main() {
    if(use_map_Kd == false){
        vFragColor = Kd;
    }else{
        vFragColor = Kd*texture(map_Kd, vertex_texcoords);
    }
}