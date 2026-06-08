#version 330 core
#extension GL_EXT_texture_array : enable
in vec3 gTexCoord;

uniform sampler2DArray textureArray;

out vec4 FragColor;

void main() {
    FragColor = texture(textureArray, gTexCoord);
    if(FragColor.a == 0){
        discard;
    }
}