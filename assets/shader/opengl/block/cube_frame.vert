#version 330 core
layout (location = 0) in vec3 aPos;

//out vec4 aFragColor;

uniform ivec3 pos;
uniform ivec3 cameraPosInt3;
uniform mat4 view;
uniform mat4 projection;

void main() {
    ivec3 p = pos - cameraPosInt3;
    gl_Position = projection * view * vec4(aPos + p, 1);
    //aFragColor = vec4(0.04, 0.28, 0.26, 0.5);
}