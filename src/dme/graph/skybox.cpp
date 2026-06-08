#include <dme/graph/skybox.h>
#include <dme/graph/shader.h>

namespace dme::graph {

    static const VertexPos _vertices[36] = {
        //right +x
        VertexPos(1.0f,  1.0f,  1.0f),
        VertexPos(1.0f, -1.0f,  1.0f),
        VertexPos(1.0f, -1.0f, -1.0f),
        VertexPos(1.0f,  1.0f,  1.0f),
        VertexPos(1.0f, -1.0f, -1.0f),
        VertexPos(1.0f,  1.0f, -1.0f),
        //left -x
        VertexPos(-1.0f, -1.0f,  1.0f),
        VertexPos(-1.0f,  1.0f,  1.0f),
        VertexPos(-1.0f,  1.0f, -1.0f),
        VertexPos(-1.0f, -1.0f,  1.0f),
        VertexPos(-1.0f,  1.0f, -1.0f),
        VertexPos(-1.0f, -1.0f, -1.0f),
        //front +y
        VertexPos(-1.0f,  1.0f,  1.0f),
        VertexPos(1.0f,  1.0f,  1.0f),
        VertexPos(1.0f,  1.0f, -1.0f),
        VertexPos(-1.0f,  1.0f,  1.0f),
        VertexPos(1.0f,  1.0f, -1.0f),
        VertexPos(-1.0f,  1.0f, -1.0f),
        //back -y
        VertexPos(1.0f, -1.0f,  1.0f),
        VertexPos(-1.0f, -1.0f,  1.0f),
        VertexPos(-1.0f, -1.0f, -1.0f),
        VertexPos(1.0f, -1.0f,  1.0f),
        VertexPos(-1.0f, -1.0f, -1.0f),
        VertexPos(1.0f, -1.0f, -1.0f),
        //top +z
        VertexPos(1.0f, -1.0f,  1.0f),
        VertexPos(1.0f,  1.0f,  1.0f),
        VertexPos(-1.0f,  1.0f,  1.0f),
        VertexPos(1.0f, -1.0f,  1.0f),
        VertexPos(-1.0f,  1.0f,  1.0f),
        VertexPos(-1.0f, -1.0f,  1.0f),
        //bottom -z
        VertexPos(1.0f,  1.0f, -1.0f),
        VertexPos(1.0f, -1.0f, -1.0f),
        VertexPos(-1.0f, -1.0f, -1.0f),
        VertexPos(1.0f,  1.0f, -1.0f),
        VertexPos(-1.0f, -1.0f, -1.0f),
        VertexPos(-1.0f,  1.0f, -1.0f),
    };

    static Shader<VertexPos>& _getShader() {
        static Shader<VertexPos> _shader = ShaderBase::Create<VertexPos>(
            ShaderSrc(ShaderType::Vertex,
                R"(#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 vTexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    vTexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
})"),
ShaderSrc(ShaderType::Fragment,
    R"(#version 330 core
out vec4 FragColor;

in vec3 vTexCoords;

uniform samplerCube skybox;

void main()
{    
    FragColor = texture(skybox, vTexCoords);
})"));
    return _shader;
}


    Skybox::Skybox() : texCube(), vao(0), vbo(0) {}
    Skybox::Skybox(graph::TexCube texCube) : texCube(texCube), vao(0), vbo(0) {
        //shader.setInt("skybox", 0);
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(_vertices), &_vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * float_size, (void*)0);
    }

    void Skybox::draw(const Matrix4& projection, const Matrix4& view) const {
        glDepthFunc(GL_LEQUAL);
        auto& shader = _getShader();
        shader.use();

        shader.setTexCube("skybox", texCube, 0);
        shader.setMatrix("projection", projection);
        Matrix4 v = view;
        v.removeTranslate(); //消除移动矩阵
        shader.setMatrix("view", v);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);
    }
    void Skybox::release() {
        if (vao) {
            glDeleteVertexArrays(1, &vao);
            glDeleteBuffers(1, &vbo);
            texCube.release();
            vao = 0;
            vbo = 0;
        }
    }

}