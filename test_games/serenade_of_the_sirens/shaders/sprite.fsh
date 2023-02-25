#version 330 core

in float spriteId;
in vec2 texCoord;
in vec4 spriteColor;

out vec4 color;

uniform sampler2D sprite;

vec2 uv_iq(vec2 uv, vec2 texture_size) {
    vec2 pixel = uv * texture_size;

    vec2 seam = floor(pixel + 0.5);
    vec2 dudv = fwidth(pixel);
    pixel = seam + clamp( (pixel - seam) / dudv, -0.5, 0.5);

    return pixel / texture_size;
}

void main() {
    float isPixelArt = 1.0f;
    vec2 drawSource = vec2(32.0f, 32.0f);
    vec2 uv = mix(texCoord, uv_iq(texCoord, drawSource), isPixelArt);
    color = spriteColor * texture(sprite, uv);
}
