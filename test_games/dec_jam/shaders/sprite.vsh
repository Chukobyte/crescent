#version 330 core

layout (location = 0) in float ID;
layout (location = 1) in vec2 VERTEX;
layout (location = 2) in vec2 CRE_TEXTURE_COORDS;
layout (location = 3) in vec4 CRE_TEXTURE_MODULATE;
layout (location = 4) in float CRE_APPLY_NEAREST_NEIGHBOR;

out vec2 UV;
out vec4 TEXTURE_MODULATE;
out float USING_NEAREST_NEIGHBOR;

uniform mat4 CRE_MODELS[100];
uniform mat4 CRE_PROJECTION;

//@@UNIFORMS

//@@FUNCTIONS

void main() {
    int CRE_SPRITE_INT_ID = int(ID);
    UV = CRE_TEXTURE_COORDS;
    TEXTURE_MODULATE = CRE_TEXTURE_MODULATE;
    USING_NEAREST_NEIGHBOR = CRE_APPLY_NEAREST_NEIGHBOR;
    //@@vertex()
    gl_Position = CRE_PROJECTION * CRE_MODELS[CRE_SPRITE_INT_ID] * vec4(VERTEX, 0.0f, 1.0f);
}
