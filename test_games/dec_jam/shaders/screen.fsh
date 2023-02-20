#version 330 core

out vec4 fragColor;
    
in vec2 texCoords;
    
uniform sampler2D screenTexture;
    
void main() {
    vec3 color = texture(screenTexture, texCoords).rgb;
    fragColor = vec4(color, 1.0f);
}
