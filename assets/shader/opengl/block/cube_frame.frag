#version 330 core
//in vec4 aFragColor;
out vec4 FragColor;

uniform vec3 color;
void main() {
    FragColor = vec4(color, 1);
    //FragColor = vec4(0.04, 0.28, 0.26, 1.0);
    //FragColor = aFragColor;
    //FragColor = vec4(tPos, 1.0);
}