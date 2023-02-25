#version 330 core

in float spriteId;
in vec2 texCoord;
in vec4 spriteColor;
in vec2 spriteDrawSource;
in float isSpritePixelArt;

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
    float brightness = 0.5f;
    vec2 uv = mix(texCoord, apply_nearest_neighbor(texCoord, spriteDrawSource), isSpritePixelArt);
    color = spriteColor * texture(sprite, uv) * brightness;
}
