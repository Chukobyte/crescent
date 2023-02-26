#version 330 core

in float spriteId;
in vec2 texCoord;
in vec4 spriteColor;
in float spriteApplyNearestNeighbor;

out vec4 color;

uniform sampler2D sprite;

vec2 apply_nearest_neighbor(vec2 uv, vec2 texture_size) {
    vec2 pixel = uv * texture_size;

    vec2 seam = floor(pixel + 0.5);
    vec2 dudv = fwidth(pixel);
    pixel = seam + clamp( (pixel - seam) / dudv, -0.5, 0.5);

    return pixel / texture_size;
}

void main() {
    vec2 spriteTextureSize = textureSize(sprite, 0);
    vec2 uv = mix(texCoord, apply_nearest_neighbor(texCoord, spriteTextureSize), spriteApplyNearestNeighbor);
    color = spriteColor * texture(sprite, uv);
}
