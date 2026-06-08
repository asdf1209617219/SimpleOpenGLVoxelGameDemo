#version 330 core
#extension GL_EXT_texture_array : enable
struct Material {  
    float specular;
    float shininess;
};
struct DirLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirLight dirLight;
uniform Material material;

in vec3 vView;
in vec3 vPos;
in vec3 vNormal;
in vec3 vTex;

uniform sampler2DArray textureArray;

out vec4 FragColor;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
void main() {
    vec3 norm = normalize(vNormal);
    vec3 viewDir = normalize(vView - vPos);
    vec3 light = CalcDirLight(dirLight, norm, viewDir);
    light = clamp(light, 0.0, 1);

    //FragColor = texture(textureArray, vTex);
    FragColor = vec4(light, 1) * texture(textureArray, vTex);
}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
    //vec3 lightDir = normalize(-light.direction);
    vec3 lightDir = -light.direction; // is normalizing
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0), material.shininess);
    // combine results
    //vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    //vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    //vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    vec3 ambient = light.ambient;
    vec3 diffuse = light.diffuse * diff;
    vec3 specular = light.specular * (spec * material.specular);
    return ambient + diffuse + specular;
}