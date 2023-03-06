#version 330 core

out vec4 fragColor;
    
in vec2 texCoords;
    
uniform sampler2D TEXTURE;
uniform float brightness;
    
void main() {
    vec3 color = texture(TEXTURE, texCoords).rgb;
    color *= brightness;
    fragColor = vec4(color, 1.0f);
}
