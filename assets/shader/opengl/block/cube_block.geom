#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in uint vTex[];
in uint vData[];

out vec3 gFragPos;
out vec3 gTexCoord;
out vec3 gNormal;

uniform mat4 view;
uniform mat4 projection;
uniform ivec3 cameraPosInt3;
uniform ivec3 chunkPos;

uniform vec2 texCoords[4] = {
    vec2( 0.0, 0.0), //bottom left
    vec2( 0.0, 1.0), //top left
    vec2( 1.0, 0.0), //bottom right
    vec2( 1.0, 1.0), //top right
};

uniform vec2 rotateTexCoords[16] = {
    //bottom left 
    vec2( 0.0, 0.0), //noRotate         0
    vec2( 1.0, 0.0), //clockwise        2
    vec2( 1.0, 1.0), //centralSymmetry  3
    vec2( 0.0, 1.0), //anticlockwis     1
    //top left
    vec2( 0.0, 1.0), //anticlockwis     1
    vec2( 0.0, 0.0), //noRotate         0
    vec2( 1.0, 0.0), //clockwise        2
    vec2( 1.0, 1.0), //centralSymmetry  3
    //bottom right
    vec2( 1.0, 0.0), //clockwise        2
    vec2( 1.0, 1.0), //centralSymmetry  3
    vec2( 0.0, 1.0), //anticlockwis     1
    vec2( 0.0, 0.0), //noRotate         0
    //top right
    vec2( 1.0, 1.0), //centralSymmetry  3
    vec2( 0.0, 1.0), //anticlockwis     1
    vec2( 0.0, 0.0), //noRotate         0
    vec2( 1.0, 0.0), //clockwise        2
};

uniform vec3 normals[6] = {
    vec3( 1.0, 0.0, 0.0), //x+
    vec3(-1.0, 0.0, 0.0), //x-
    vec3( 0.0, 1.0, 0.0), //y+
    vec3( 0.0,-1.0, 0.0), //y-
    vec3( 0.0, 0.0, 1.0), //z+
    vec3( 0.0, 0.0,-1.0), //z-
};

//triangle_strip, draw 'N'
uniform ivec3 positions[24] = {
    //x+
    ivec3(1, 0, 0),
    ivec3(1, 0, 1),
    ivec3(1, 1, 0),
    ivec3(1, 1, 1),
    //x-
    ivec3(0, 1, 0),
    ivec3(0, 1, 1),
    ivec3(0, 0, 0),
    ivec3(0, 0, 1),
    //y+
    ivec3(1, 1, 0),
    ivec3(1, 1, 1),
    ivec3(0, 1, 0),
    ivec3(0, 1, 1),
    //y-
    ivec3(0, 0, 0),
    ivec3(0, 0, 1),
    ivec3(1, 0, 0),
    ivec3(1, 0, 1),
    //z+
    ivec3(0, 0, 1),
    ivec3(0, 1, 1),
    ivec3(1, 0, 1),
    ivec3(1, 1, 1),
    //z-
    ivec3(0, 1, 0),
    ivec3(0, 0, 0),
    ivec3(1, 1, 0),
    ivec3(1, 0, 0),
};

/*
uniform vec3 positions[24] = {
    //x+
    vec3(1.0, 0.0, 0.0),
    vec3(1.0, 0.0, 1.0),
    vec3(1.0, 1.0, 0.0),
    vec3(1.0, 1.0, 1.0),
    //x-
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 1.0, 1.0),
    vec3(0.0, 0.0, 0.0),
    vec3(0.0, 0.0, 1.0),
    //y+
    vec3(1.0, 1.0, 0.0),
    vec3(1.0, 1.0, 1.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 1.0, 1.0),
    //y-
    vec3(0.0, 0.0, 0.0),
    vec3(0.0, 0.0, 1.0),
    vec3(1.0, 0.0, 0.0),
    vec3(1.0, 0.0, 1.0),
    //z+
    vec3(0.0, 0.0, 1.0),
    vec3(0.0, 1.0, 1.0),
    vec3(1.0, 0.0, 1.0),
    vec3(1.0, 1.0, 1.0),
    //z-
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 0.0),
    vec3(1.0, 1.0, 0.0),
    vec3(1.0, 0.0, 0.0),
};
*/

void main() {
    //gl_Position = m * (gl_in[0].gl_Position + vec4(-0.1, 0.0, 0.0, 0.0)); 

    mat4 m = projection * view;
    int d = int(vData[0]); //zzzzyyyyxxxxrrfff
    int face = d & 7;
    int rotate = (d >> 3) & 3;
    int posIndex = face * 4;
    //这里的方块是相对于摄像机的整数坐标的位置，避免float在过大时产生的精度问题
    ivec3 blockPos = chunkPos * 16 + ivec3((d >> 5) & 15, (d >> 9) & 15, (d >> 13) & 15) - cameraPosInt3;

    gNormal = normals[face];
    gTexCoord = vec3(rotateTexCoords[rotate], vTex[0]);
    gFragPos = blockPos + positions[posIndex];
    gl_Position = m * vec4(gFragPos, 1.0);
    EmitVertex();

    gTexCoord = vec3(rotateTexCoords[4 + rotate], vTex[0]);
    gFragPos = blockPos + positions[posIndex + 1];
    gl_Position = m * vec4(gFragPos, 1.0);
    EmitVertex();

    gTexCoord = vec3(rotateTexCoords[8 + rotate], vTex[0]);
    gFragPos = blockPos + positions[posIndex + 2];
    gl_Position = m * vec4(gFragPos, 1.0);
    EmitVertex();
    
    gTexCoord = vec3(rotateTexCoords[12 + rotate], vTex[0]);
    gFragPos = blockPos + positions[posIndex + 3];
    gl_Position = m * vec4(gFragPos, 1.0);
    EmitVertex();

    EndPrimitive();
}