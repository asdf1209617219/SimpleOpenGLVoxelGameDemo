#version 330 core
#extension GL_EXT_texture_array : enable
in vec3 vTexCoord;
uniform sampler2DArray aTexture;
uniform int aTextureSize;

out vec4 FragColor;
void main() {
    vec3 texCoord = vec3(vTexCoord.xy / float(aTextureSize), vTexCoord.z);
    FragColor = texture(aTexture, texCoord);
}