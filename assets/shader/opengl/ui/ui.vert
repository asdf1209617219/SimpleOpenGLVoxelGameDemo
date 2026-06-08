#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in ivec3 aTexCoord;
uniform mat4 projection;

out vec3 vTexCoord;
void main() {
    gl_Position = projection * vec4(aPos, 0, 1);
    vTexCoord = aTexCoord;
}