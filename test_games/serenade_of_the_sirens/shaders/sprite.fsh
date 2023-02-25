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

vec2 uv_iq(vec2 uv, vec2 texture_size) {
    vec2 pixel = uv * texture_size;

    vec2 seam = floor(pixel + 0.5);
    vec2 dudv = fwidth(pixel);
    pixel = seam + clamp( (pixel - seam) / dudv, -0.5, 0.5);

    return pixel / texture_size;
}

void main() {
    vec2 res = vec2(400.0f, 225.0f);
//     vec2 uv = uv_cstantos(texCoord, res);
    vec2 uv = uv_iq(texCoord, res);
    color = spriteColor * texture(sprite, uv);
//     color = spriteColor * texture(sprite, texCoord);
}
