#include <dme/graph/shader.h>
namespace dme::graph {

    sint ShaderBase::shaderTypeToGLShaderType(ShaderType type) {
        switch (type) {
        case ShaderType::Vertex:
            return GL_VERTEX_SHADER;
        case ShaderType::Fragment:
            return GL_FRAGMENT_SHADER;
        case ShaderType::Geometry:
            return GL_GEOMETRY_SHADER;
        default:
            return -1;
        }
    }

    uint ShaderBase::getId() const {
        return id;
    }
    const VertexType& ShaderBase::getType() const {
        return type;
    }
    sint ShaderBase::getUniformLocation(const stdstr& name) const {
        return glGetUniformLocation(id, name.c_str());
    }

    void ShaderBase::use() const {
        glUseProgram(id);
    }

    void ShaderBase::setTexCube(const stdstr& name, const TexCube& tex, sint texIndex) const {
        glUniform1i(getUniformLocation(name), texIndex);
        tex.bind(texIndex);
    }
    void ShaderBase::setTexCube(sint uniformLocation, const TexCube& tex, sint texIndex) const {
        glUniform1i(uniformLocation, texIndex);
        tex.bind(texIndex);
    }
    void ShaderBase::setTex2d(const stdstr& name, const Tex2d& tex, sint texIndex) const {
        glUniform1i(getUniformLocation(name), texIndex);
        tex.bind(texIndex);
    }
    void ShaderBase::setTex2d(sint uniformLocation, const Tex2d& tex, sint texIndex) const {
        glUniform1i(uniformLocation, texIndex);
        tex.bind(texIndex);
    }
    void ShaderBase::setTex2dArray(const stdstr& name, const Tex2dArray& tex, sint texIndex) const {
        glUniform1i(getUniformLocation(name), texIndex);
        tex.bind(texIndex);
    }
    void ShaderBase::setTex2dArray(sint uniformLocation, const Tex2dArray& tex, sint texIndex) const {
        glUniform1i(uniformLocation, texIndex);
        tex.bind(texIndex);
    }
    void ShaderBase::setTexAtlas(const stdstr& name, const TexAtlas& tex, sint texIndex) const {
        glUniform1i(getUniformLocation(name), texIndex);
        tex.bind(texIndex);
    }
    void ShaderBase::setTexAtlas(sint uniformLocation, const TexAtlas& tex, sint texIndex) const {
        glUniform1i(uniformLocation, texIndex);
        tex.bind(texIndex);
    }
    void ShaderBase::setTex2dMultisample(const stdstr& name, const Tex2dMultisample& tex, sint texIndex) const {
        glUniform1i(getUniformLocation(name), texIndex);
        tex.bind(texIndex);
    }
    void ShaderBase::setTex2dMultisample(sint uniformLocation, const Tex2dMultisample& tex, sint texIndex) const {
        glUniform1i(uniformLocation, texIndex);
        tex.bind(texIndex);
    }

    void ShaderBase::setBool(const stdstr& name, bool value) const {
        glUniform1i(getUniformLocation(name), (sint)value);
    }
    void ShaderBase::setBool(sint uniformLocation, bool value) const {
        glUniform1i(uniformLocation, (sint)value);
    }

    void ShaderBase::setInt(const stdstr& name, sint value) const {
        glUniform1i(getUniformLocation(name), value);
    }
    void ShaderBase::setInt(sint uniformLocation, sint value) const {
        glUniform1i(uniformLocation, (sint)value);
    }
    void ShaderBase::setInt2(const stdstr& name, const Int2& value) const {
        glUniform2i(getUniformLocation(name), value.x, value.y);
    }
    void ShaderBase::setInt2(sint uniformLocation, const Int2& value) const {
        glUniform2i(uniformLocation, value.x, value.y);
    }
    void ShaderBase::setInt3(const stdstr& name, const Int3& value) const {
        glUniform3i(getUniformLocation(name), value.x, value.y, value.z);
    }
    void ShaderBase::setInt3(sint uniformLocation, const Int3& value) const {
        glUniform3i(uniformLocation, value.x, value.y, value.z);
    }
    void ShaderBase::setInt4(const stdstr& name, const Int4& value) const {
        glUniform4i(getUniformLocation(name), value.x, value.y, value.z, value.w);
    }
    void ShaderBase::setInt4(sint uniformLocation, const Int4& value) const {
        glUniform4i(uniformLocation, value.x, value.y, value.z, value.w);
    }

    void ShaderBase::setFloat(const stdstr& name, float value) const {
        glUniform1f(getUniformLocation(name), value);
    }
    void ShaderBase::setFloat(sint uniformLocation, float value) const {
        glUniform1f(uniformLocation, value);
    }
    void ShaderBase::setFloat2(const stdstr& name, const Float2& value) const {
        glUniform2f(getUniformLocation(name), value.x, value.y);
    }
    void ShaderBase::setFloat2(sint uniformLocation, const Float2& value) const {
        glUniform2f(uniformLocation, value.x, value.y);
    }
    void ShaderBase::setFloat3(const stdstr& name, const Float3& value) const {
        glUniform3f(getUniformLocation(name), value.x, value.y, value.z);
    }
    void ShaderBase::setFloat3(sint uniformLocation, const Float3& value) const {
        glUniform3f(uniformLocation, value.x, value.y, value.z);
    }
    void ShaderBase::setFloat4(const stdstr& name, const Float4& value) const {
        glUniform4f(getUniformLocation(name), value.x, value.y, value.z, value.w);
    }
    void ShaderBase::setFloat4(sint uniformLocation, const Float4& value) const {
        glUniform4f(uniformLocation, value.x, value.y, value.z, value.w);
    }

    void ShaderBase::setMatrix(const stdstr& name, const Matrix4& value) const {
        glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, value.ptr());
    }
    void ShaderBase::setMatrix(sint uniformLocation, const Matrix4& value) const {
        glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, value.ptr());
    }


    void ShaderBase::DeleteAllProgram() {
        for (uint id : shaderProgramArray) {
            glDeleteProgram(id);
        }
        shaderProgramArray.clear();
    }
    const stdstr& ShaderBase::getShaderTypeName(ShaderType type) {
        switch (type) {
        case ShaderType::Vertex:
            return shaderTypeNameVertex;
        case ShaderType::Fragment:
            return shaderTypeNameFragment;
        default:
            return shaderTypeNameGeometry;
        }
    }


    void ShaderBase::checkCompileErrors(uint shader, ShaderUrl url) {
        sint success = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            sint logLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
            if (logLength > 0) {
                stdstr infoLog;
                infoLog.reserve(logLength);
                glGetProgramInfoLog(shader, logLength, NULL, infoLog.data());
                Log::Error("Shader: shader compile error of type: ",
                    getShaderTypeName(url.type), ", id: ", shader,
                    ", failure reason: ", infoLog,
                    ", url: ", url.url,
                    "\n --------------------------------------------------------- \n");
            }
        }
    }
    void ShaderBase::checkCompileErrors(uint shader, ShaderSrc src) {
        sint success = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            sint logLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
            if (logLength > 0) {
                stdstr infoLog;
                infoLog.reserve(logLength);
                glGetProgramInfoLog(shader, logLength, NULL, infoLog.data());
                Log::Error("Shader: shader compile error of type: ",
                    getShaderTypeName(src.type), ", id: ", shader,
                    ", failure reason: ", infoLog,
                    ", src: ->\n", src.src,
                    "\n --------------------------------------------------------- \n");
            }
        }
    }


    bool ShaderBase::operator ==(const ShaderBase& object) const {
        return id == object.id;
    }

    Shader<CubeVertex>& Shaders::CubeBlock() {
        static auto _shader = ShaderBase::Create<CubeVertex>(
            ShaderUrl(ShaderType::Vertex, DME_PATH_SHADER_BLOCK "cube_block.vert"),
            ShaderUrl(ShaderType::Geometry, DME_PATH_SHADER_BLOCK "cube_block.geom"),
            ShaderUrl(ShaderType::Fragment, DME_PATH_SHADER_BLOCK "cube_block.frag")
        );
        return _shader;
    }

    Shader<PlantVertex>& Shaders::PlantBlock() {
        static auto _shader = ShaderBase::Create<PlantVertex>(
            ShaderUrl(ShaderType::Vertex, DME_PATH_SHADER_BLOCK "plant_block.vert"),
            ShaderUrl(ShaderType::Geometry, DME_PATH_SHADER_BLOCK "plant_block.geom"),
            ShaderUrl(ShaderType::Fragment, DME_PATH_SHADER_BLOCK "plant_block.frag")
        );
        return _shader;
    }

    Shader<VertexPos>& Shaders::CubeFrame() {
        static auto _shader = ShaderBase::Create<VertexPos>(
            ShaderUrl(ShaderType::Vertex, DME_PATH_SHADER_BLOCK "cube_frame.vert"),
            ShaderUrl(ShaderType::Fragment, DME_PATH_SHADER_BLOCK "cube_frame.frag")
        );
        return _shader;
    }

    Shader<VertexPos>& Shaders::Ray() {
        static auto _shader = ShaderBase::Create<VertexPos>(
            ShaderUrl(ShaderType::Vertex, DME_PATH_SHADER_BLOCK "ray.vert"),
            ShaderUrl(ShaderType::Fragment, DME_PATH_SHADER_BLOCK "ray.frag")
        );
        return _shader;
    }

    Shader<VertexPos>& Shaders::RayPoint() {
        static auto _shader = ShaderBase::Create<VertexPos>(
            ShaderUrl(ShaderType::Vertex, DME_PATH_SHADER_BLOCK "ray_point.vert"),
            ShaderUrl(ShaderType::Fragment, DME_PATH_SHADER_BLOCK "ray_point.frag")
        );
        return _shader;
    }

    Shader<VertexPos2Rgb>& Shaders::Test() {
        static auto _shader = ShaderBase::Create<VertexPos2Rgb>(
            ShaderUrl(ShaderType::Vertex, DME_PATH_SHADER_BLOCK "test.vert"),
            ShaderUrl(ShaderType::Geometry, DME_PATH_SHADER_BLOCK "test.geom"),
            ShaderUrl(ShaderType::Fragment, DME_PATH_SHADER_BLOCK "test.frag")
        );
        return _shader;
    }

    Shader<VertexPos2Tex>& Shaders::FramebuffersScreen() {
        static auto _shader = ShaderBase::Create<VertexPos2Tex>(
            ShaderUrl(ShaderType::Vertex, DME_PATH_SHADER_UI "framebuffers_screen.vert"),
            ShaderUrl(ShaderType::Fragment, DME_PATH_SHADER_UI "framebuffers_screen.frag")
        );
        return _shader;
    }

    Shader<VertexPos2>& Shaders::ReverseColor() {
        static auto _shader = ShaderBase::Create<VertexPos2>(
            ShaderUrl(ShaderType::Vertex, DME_PATH_SHADER_UI "reverse_color.vert"),
            ShaderUrl(ShaderType::Fragment, DME_PATH_SHADER_UI "reverse_color.frag")
        );
        return _shader;
    }

    Shader<RectUIVertex>& Shaders::UI() {
        static auto _shader = ShaderBase::Create<RectUIVertex>(
            ShaderUrl(ShaderType::Vertex, DME_PATH_SHADER_UI "ui.vert"),
            ShaderUrl(ShaderType::Fragment, DME_PATH_SHADER_UI "ui.frag")
        );
        return _shader;
    }

    Shader<VertexBlockItemIcon>& Shaders::BlockItem() {
        static auto _shader = ShaderBase::Create<VertexBlockItemIcon>(
            ShaderUrl(ShaderType::Vertex, DME_PATH_SHADER_UI "block_item.vert"),
            ShaderUrl(ShaderType::Fragment, DME_PATH_SHADER_UI "block_item.frag")
        );
        return _shader;
    }

}
