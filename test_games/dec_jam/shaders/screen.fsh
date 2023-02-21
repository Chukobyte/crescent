#version 330 core

out vec4 fragColor;
    
in vec2 texCoords;
    
uniform sampler2D screenTexture;
uniform float brightness;
    
void main() {
    vec3 color = texture(screenTexture, texCoords).rgb;
    color *= brightness;
    fragColor = vec4(color, 1.0f);
}
