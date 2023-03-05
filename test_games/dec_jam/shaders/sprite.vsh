#version 330 core

layout (location = 0) in float ID;
layout (location = 1) in vec2 VERTEX;
layout (location = 2) in vec2 CRE_TEXTURE_COORDS;
layout (location = 3) in vec4 CRE_TEXTURE_MODULATE;
layout (location = 4) in float CRE_APPLY_NEAREST_NEIGHBOR;

out vec2 texCoord;
out vec4 spriteColor;
out float spriteApplyNearestNeighbor;

uniform mat4 CRE_MODELS[100];
uniform mat4 CRE_PROJECTION;

//@@UNIFORMS

//@@FUNCTIONS

void main() {
    int spriteIntId = int(ID);
    texCoord = CRE_TEXTURE_COORDS;
    spriteColor = CRE_TEXTURE_MODULATE;
    spriteApplyNearestNeighbor = CRE_APPLY_NEAREST_NEIGHBOR;
    //@@vertex()
    gl_Position = CRE_PROJECTION * CRE_MODELS[spriteIntId] * vec4(VERTEX, 0.0f, 1.0f);
}
