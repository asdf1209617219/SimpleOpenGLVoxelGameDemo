#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUIPos;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTex;
uniform mat4 mvp;
uniform mat4 projection;
uniform vec3 cameraPos;

out vec3 vView;
out vec3 vPos;
out vec3 vNormal;
out vec3 vTex;
void main() {
    vPos = vec3(mvp * vec4(aPos, 1));
    vec4 uPos = projection * vec4(aUIPos, 0, 0);
    gl_Position = vec4(vPos, 1) + uPos;
    vNormal = aNormal;
    vTex = aTex;
    vView = cameraPos + vec3(uPos.xy, 0);
}