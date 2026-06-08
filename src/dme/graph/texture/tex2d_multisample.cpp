#include <dme/graph/texture/tex2d_multisample.h>
#include <stb/stb_image.h>
namespace dme::graph {
    Tex2dMultisample::Tex2dMultisample() : Tex() {}
    Tex2dMultisample::Tex2dMultisample(sint width, sint height, TexWrap texWrap, TexFilter texFilter) {
        sint param = Tex::TexWrapToGLParam(texWrap);

        glGenTextures(1, &id.v);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, id.v);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, Content::MSAALevel, GL_RGB, width, height, GL_TRUE);
        // 为当前绑定的纹理对象设置环绕、过滤方式
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, param);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, param);

        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    }

    void Tex2dMultisample::bind(sint location) const {
        glActiveTexture(GL_TEXTURE0 + location); // 在绑定纹理之前先激活纹理单元
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, id);
    }

}