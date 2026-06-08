#version 330 core
in vec2 vTexCoord;
uniform sampler2D screenTexture;

out vec4 FragColor;
void main() {
    vec3 col = texture(screenTexture, vTexCoord).rgb;
    FragColor = vec4(vec3(1.0 - col), 1.0);
} 