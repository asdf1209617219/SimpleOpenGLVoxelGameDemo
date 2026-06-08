#pragma once
#include <dme/graph/texture/tex_cube.h>
#include <dme/graph/texture/tex2d.h>
#include <dme/graph/texture/tex2d_array.h>
#include <dme/graph/texture/tex_atlas.h>
#include <dme/graph/texture/tex2d_multisample.h>
#include <dme/graph/vertex/vertex_type.h>
namespace dme::graph {

    enum class ShaderType {
        Vertex,
        Fragment,
        Geometry,
    };

    //包装shader文件的路径
    struct ShaderUrl {
        ShaderType type;
        const char* url;

        ShaderUrl(ShaderType type, const char* url) noexcept : type(type), url(url) {}
    };

    //包装shader源码
    struct ShaderSrc {
        ShaderType type;
        const char* src;

        ShaderSrc(ShaderType type, const char* src) noexcept : type(type), src(src) {}
    };


    template<IsVertex T>
    class Shader;

    class ShaderBase {
    protected:
        uint id;
        const VertexType& type; //着色器使用的顶点类型

        static inline const stdstr shaderTypeNameVertex = "VERTEX";
        static inline const stdstr shaderTypeNameFragment = "FRAGMENT";
        static inline const stdstr shaderTypeNameGeometry = "GEOMETRY";
        static inline const stdstr shaderTypeNameProgram = "PROGRAM";
        static inline DynArr<uint> shaderProgramArray;

        static sint shaderTypeToGLShaderType(ShaderType type);

        static const stdstr& getShaderTypeName(ShaderType type);
        static void checkCompileErrors(uint shader, ShaderUrl url);
        static void checkCompileErrors(uint shader, ShaderSrc src);

        template<typename T>
        static stdstr linkErrorsInfo(const T& shader) {
            if constexpr (std::is_same_v<T, ShaderUrl>) {
                return (stdstr("shader type: ") + getShaderTypeName(shader.type) + ", url: " + shader.url + "\n-------------------\n");
            }
            else if constexpr (std::is_same_v<T, ShaderSrc>) {
                return (stdstr("shader type: ") + getShaderTypeName(shader.type) + ", src: ->\n" + shader.src + "\n-------------------\n");
            }
            else {
                static_assert(false, "Type must is ShaderUrl or ShaderSrc");
            }
        }

        template<typename... Args>
        static void checkLinkErrors(uint program, const Args&... args) {
            sint success = 0;
            glGetProgramiv(program, GL_LINK_STATUS, &success);
            if (!success) {
                sint logLength = 0;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
                if (logLength > 0) {
                    stdstr infoLog;
                    infoLog.reserve(logLength);
                    glGetProgramInfoLog(program, logLength, NULL, infoLog.data());
                    Log::Error("Shader: program linking error, program id: ",
                        program, ", failure reason: ", infoLog, ";\n", linkErrorsInfo(args)...,
                        "\n --------------------------------------------------------- \n");
                }
            }
        }

        template<typename T>
        static uint createShader(const T& shader) {
            if constexpr (std::is_same_v<T, ShaderUrl>) {
                uint shaderId = glCreateShader(shaderTypeToGLShaderType(shader.type));
                stdstr shaderCodeStr;
                try {
                    std::ifstream shaderFile;
                    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
                    shaderFile.open(shader.url);
                    std::stringstream shaderStream;
                    shaderStream << shaderFile.rdbuf();
                    shaderFile.close();

                    shaderCodeStr = shaderStream.str();
                }
                catch (std::ifstream::failure& e) {
                    Log::Error("Shader: file reading failed: ",
                        e.what(), "; file path is: ", shader.url);
                }
                const char* shaderCodePtr = shaderCodeStr.c_str();
                glShaderSource(shaderId, 1, &shaderCodePtr, NULL);
                glCompileShader(shaderId);
                checkCompileErrors(shaderId, shader);
                return shaderId;
            } 
            else if constexpr (std::is_same_v<T, ShaderSrc>) {
                uint shaderId = glCreateShader(shaderTypeToGLShaderType(shader.type));
                const char* shaderCodePtr = shader.src;
                glShaderSource(shaderId, 1, &shaderCodePtr, NULL);
                glCompileShader(shaderId);
                checkCompileErrors(shaderId, shader);
                return shaderId;
            }
            else {
                static_assert(false, "Type must is ShaderUrl or ShaderSrc");
            }
        }

        template<typename... Args>
        ShaderBase(const VertexType& type, const Args&... args) : id(0), type(type) {
            constexpr uint count = sizeof...(Args);
            uint ids[count] = {ShaderBase::createShader(args)...};
            // shader Program
            id = glCreateProgram();
            for (uint i = 0; i < count; i++) {
                glAttachShader(id, ids[i]);
            }
            glLinkProgram(id);
            checkLinkErrors(id, args...);
            for (uint i = 0; i < count; i++) {
                glDeleteShader(ids[i]);
            }
            if (id) {
                shaderProgramArray.add(id);
            }
        }
    public:
        template <IsVertex T, typename... Args>
        static Shader<T> Create(Args&&... args) {
            return Shader<T>(args...);
        }

        static void DeleteAllProgram();

        uint getId() const;
        const VertexType& getType() const;
        sint getUniformLocation(const stdstr& name) const;

        void use() const;

        void setTexCube(const stdstr& name, const TexCube& tex, sint texIndex) const;
        void setTexCube(sint uniformLocation, const TexCube& tex, sint texIndex) const;
        void setTex2d(const stdstr& name, const Tex2d& tex, sint texIndex) const;
        void setTex2d(sint uniformLocation, const Tex2d& tex, sint texIndex) const;
        void setTex2dArray(const stdstr& name, const Tex2dArray& tex, sint texIndex) const;
        void setTex2dArray(sint uniformLocation, const Tex2dArray& tex, sint texIndex) const;
        void setTexAtlas(const stdstr& name, const TexAtlas& tex, sint texIndex) const;
        void setTexAtlas(sint uniformLocation, const TexAtlas& tex, sint texIndex) const;
        void setTex2dMultisample(const stdstr& name, const Tex2dMultisample& tex, sint texIndex) const;
        void setTex2dMultisample(sint uniformLocation, const Tex2dMultisample& tex, sint texIndex) const;

        void setBool(const stdstr& name, bool value) const;
        void setBool(sint uniformLocation, bool value) const;

        void setInt(const stdstr& name, sint value) const;
        void setInt(sint uniformLocation, sint value) const;
        void setInt2(const stdstr& name, const Int2& value) const;
        void setInt2(sint uniformLocation, const Int2& value) const;
        void setInt3(const stdstr& name, const Int3& value) const;
        void setInt3(sint uniformLocation, const Int3& value) const;
        void setInt4(const stdstr& name, const Int4& value) const;
        void setInt4(sint uniformLocation, const Int4& value) const;

        void setFloat(const stdstr& name, float value) const;
        void setFloat(sint uniformLocation, float value) const;
        void setFloat2(const stdstr& name, const Float2& value) const;
        void setFloat2(sint uniformLocation, const Float2& value) const;
        void setFloat3(const stdstr& name, const Float3& value) const;
        void setFloat3(sint uniformLocation, const Float3& value) const;
        void setFloat4(const stdstr& name, const Float4& value) const;
        void setFloat4(sint uniformLocation, const Float4& value) const;

        void setMatrix(const stdstr& name, const Matrix4& value) const;
        void setMatrix(sint uniformLocation, const Matrix4& value) const;

        bool operator ==(const ShaderBase& object) const;

    };

    template<IsVertex T>
    class Shader : public ShaderBase {
    private:
        friend class ShaderBase;

        template<typename... Args>
        Shader(const Args&... args) : ShaderBase(T::GetVertexType(), args...) {}
    public:
        using vertex_type = T;

    };

    struct Shaders {
        static Shader<CubeVertex>& CubeBlock();
        static Shader<PlantVertex>& PlantBlock();

        static Shader<VertexPos>& CubeFrame();
        static Shader<VertexPos>& Ray();
        static Shader<VertexPos>& RayPoint();
        static Shader<VertexPos2Rgb>& Test();

        static Shader<VertexPos2Tex>& FramebuffersScreen();
        static Shader<VertexPos2>& ReverseColor();
        static Shader<RectUIVertex>& UI();
        static Shader<VertexBlockItemIcon>& BlockItem();
    };

}