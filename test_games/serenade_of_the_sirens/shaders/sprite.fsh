#version 330 core

in float spriteId;
in vec2 texCoord;
in vec4 spriteColor;

out vec4 color;

uniform sampler2D sprite;

void main() {
    color = spriteColor * texture(sprite, texCoord);
}