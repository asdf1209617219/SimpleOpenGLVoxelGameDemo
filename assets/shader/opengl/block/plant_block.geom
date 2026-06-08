#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 16) out;

in uint vTex[];
in uint vData[];

out vec3 gTexCoord;

uniform mat4 view;
uniform mat4 projection;
uniform ivec3 cameraPosInt3;
uniform vec3 cameraPosFloat3;
uniform ivec3 chunkPos;

uniform vec2 texCoords[4] = {
    vec2( 0.0, 0.0), //bottom left
    vec2( 0.0, 1.0), //top left
    vec2( 1.0, 0.0), //bottom right
    vec2( 1.0, 1.0), //top right
};

void main() {
    mat4 m = projection * view;
    int d = int(vData[0]); //zzzzyyyyxxxx
    ivec3 blockPosi = chunkPos * 16 + ivec3(d & 15, (d >> 4) & 15, (d >> 8) & 15) - cameraPosInt3;
    //vec3 blockPos = cameraPosFloat3 + vec3(blockPosi);
    vec3 blockPos = vec3(blockPosi);
    float size1 = 0.0;
    float size2 = 1.0;
    float size0 = 0;

    //1
    gTexCoord = vec3(texCoords[0], vTex[0]);
    gl_Position = m * vec4(blockPos + vec3(size1, size1, size0), 1.0);
    EmitVertex();
    
    gTexCoord = vec3(texCoords[1], vTex[0]);
    gl_Position = m * vec4(blockPos + vec3(size1, size1, size2), 1.0);
    EmitVertex();

    gTexCoord = vec3(texCoords[2], vTex[0]);
    gl_Position = m * vec4(blockPos + vec3(size2, size2, size0), 1.0);
    EmitVertex();

    gTexCoord = vec3(texCoords[3], vTex[0]);
    gl_Position = m * vec4(blockPos + vec3(size2, size2, size2), 1.0);
    EmitVertex();

    EndPrimitive();

    //2
    gTexCoord = vec3(texCoords[0], vTex[0]);
    gl_Position = m * vec4(blockPos + vec3(size1, size2, size0), 1.0);
    EmitVertex();
    
    gTexCoord = vec3(texCoords[1], vTex[0]);
    gl_Position = m * vec4(blockPos + vec3(size1, size2, size2), 1.0);
    EmitVertex();

    gTexCoord = vec3(texCoords[2], vTex[0]);
    gl_Position = m * vec4(blockPos + vec3(size2, size1, size0), 1.0);
    EmitVertex();

    gTexCoord = vec3(texCoords[3], vTex[0]);
    gl_Position = m * vec4(blockPos + vec3(size2, size1, size2), 1.0);
    EmitVertex();

    EndPrimitive();

    //3
    gTexCoord = vec3(texCoords[0], vTex[0]);
    gl_Position = m * vec4(blockPos + vec3(size2, size2, size0), 1.0);
    EmitVertex();
    
    gTexCoord = vec3(texCoords[1], vTex[0]);
    gl_Position = m * vec4(blockPos + vec3(size2, size2, size2), 1.0);
    EmitVertex();

    gTexCoord = vec3(texCoords[2], vTex[0]);
    gl_Position = m * vec4(blockPos + vec3(size1, size1, size0), 1.0);
    EmitVertex();

    gTexCoord = vec3(texCoords[3], vTex[0]);
    gl_Position = m * vec4(blockPos + vec3(size1, size1, size2), 1.0);
    EmitVertex();

    EndPrimitive();

    //4
    gTexCoord = vec3(texCoords[0], vTex[0]);
    gl_Position = m * vec4(blockPos + vec3(size2, size1, size0), 1.0);
    EmitVertex();
    
    gTexCoord = vec3(texCoords[1], vTex[0]);
    gl_Position = m * vec4(blockPos + vec3(size2, size1, size2), 1.0);
    EmitVertex();

    gTexCoord = vec3(texCoords[2], vTex[0]);
    gl_Position = m * vec4(blockPos + vec3(size1, size2, size0), 1.0);
    EmitVertex();

    gTexCoord = vec3(texCoords[3], vTex[0]);
    gl_Position = m * vec4(blockPos + vec3(size1, size2, size2), 1.0);
    EmitVertex();

    EndPrimitive();
}