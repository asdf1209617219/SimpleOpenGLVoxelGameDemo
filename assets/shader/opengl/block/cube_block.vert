#version 330 core
layout (location = 0) in uint aTex;
layout (location = 1) in uint aData;

out uint vTex;
out uint vData;

void main() {
    vTex = aTex;
    vData = aData;
    //gl_Position = vec4(0, 0, 0, 1);
}