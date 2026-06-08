#include <dme/graph/texture/tex2d.h>
namespace dme::graph {

    Tex2d::Tex2d() noexcept : Tex() {}
    Tex2d::Tex2d(strid path, TexWrap texWrap, TexFilter texFilter) {
        auto result = Tex::PathMap.get(path);
        if (result) {
            id = result->val;
            return;
        }
        sint param = Tex::TexWrapToGLParam(texWrap);

        glGenTextures(1, &id.v);
        glBindTexture(GL_TEXTURE_2D, id.v);
        // 为当前绑定的纹理对象设置环绕、过滤方式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, param); //GL_REPEAT
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, param); //GL_REPEAT
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        
        //对远处纹理使用mipmap
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Tex::TexFilterToGLParam(texFilter));

        // 加载并生成纹理
        auto data = Tex::LoadImage(path);
        if (data.data()) {
            glTexImage2D(GL_TEXTURE_2D, 0, data.channelCode(), data.width(), data.height(), 0, data.channelCode(), GL_UNSIGNED_BYTE, data.data());
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    /*
    Tex2d::Tex2d(uchar* data, sint width, sint height, sint channel) {
        glGenTextures(1, &id.v);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        sint channelCode = Tex::ChannelToGLFormat(channel);
        glTexImage2D(GL_TEXTURE_2D, 0, channelCode, width, height, 0, channelCode, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        Tex::FreeImage(data);

        glBindTexture(GL_TEXTURE_2D, 0);
    }
    */

    /*
    void Tex2d::bind() const {
        glActiveTexture(GL_TEXTURE0); // 在绑定纹理之前先激活纹理单元
        //shader.setInt("ourTexture", 0); // 手动设置纹理单元为0（只有一个纹理时纹理单元默认是0）
        //glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture"), 0); // 手动设置纹理单元为0（只有一个纹理时纹理单元默认是0）

        glBindTexture(GL_TEXTURE_2D, id);
    }
    */

    void Tex2d::bind(sint location) const {
        glActiveTexture(GL_TEXTURE0 + location); // 在绑定纹理之前先激活纹理单元
        glBindTexture(GL_TEXTURE_2D, id);
    }

}