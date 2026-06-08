#version 330 core
layout (location = 0) in vec2 aPos;
uniform mat4 projection;

out vec2 vTexCoord;
void main() {
    gl_Position = projection * vec4(aPos, 0, 1);
    vTexCoord = gl_Position.xy * 0.5f + 0.5f; //由标准化设备坐标转为纹理坐标[-1,1]->[0,1]
}