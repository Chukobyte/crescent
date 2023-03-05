#version 330 core

in vec2 UV;
in vec4 TEXTURE_MODULATE;
in float USING_NEAREST_NEIGHBOR;

out vec4 COLOR;

uniform sampler2D TEXTURE;

//@@UNIFORMS

//@@FUNCTIONS

vec2 CRE_APPLY_NEAREST_NEIGHBOR(vec2 uv, vec2 texture_size) {
    vec2 pixel = uv * texture_size;

    vec2 seam = floor(pixel + 0.5);
    vec2 dudv = fwidth(pixel);
    pixel = seam + clamp( (pixel - seam) / dudv, -0.5, 0.5);

    return pixel / texture_size;
}

void main() {
    vec2 CRE_SPRITE_TEXTURE_SIZE = textureSize(TEXTURE, 0);
    vec2 CRE_SPRITE_UV = mix(UV, CRE_APPLY_NEAREST_NEIGHBOR(UV, CRE_SPRITE_TEXTURE_SIZE), USING_NEAREST_NEIGHBOR);
    COLOR = TEXTURE_MODULATE * texture(TEXTURE, CRE_SPRITE_UV);
    //@@fragment()
    // float brightness = 0.5f;
    // COLOR *= brightness;
}
