#version 330 core

in float spriteId;
in vec2 texCoord;
in vec4 spriteColor;

out vec4 color;

uniform sampler2D sprite;

vec2 uv_cstantos(vec2 uv, vec2 res) {
    vec2 pixels = uv * res;
    vec2 alpha = 0.7f * fwidth(pixels);
    vec2 pixels_fract = fract(pixels);
    vec2 pixels_diff = clamp(0.5f / alpha * pixels_fract, 0.0f, 0.5f) +
                       clamp(0.5f / alpha * (pixels_fract - 1) + 0.5f, 0.0f, 0.5f);
    pixels = floor(pixels) + pixels_diff;
    return pixels / res;
}

void main() {
    vec2 uv = uv_cstantos(texCoord, vec2(400.0f, 225.0f));
    color = spriteColor * texture(sprite, uv);
//     color = spriteColor * texture(sprite, texCoord);
}
