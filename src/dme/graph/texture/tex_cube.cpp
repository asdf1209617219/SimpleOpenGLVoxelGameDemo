#include <dme/graph/texture/tex_cube.h>

namespace dme::graph {

    TexCube::TexCube() : Tex() {}
    TexCube::TexCube(strid rightPath, strid leftPath,
        strid frontPath, strid backPath,
        strid topPath, strid bottomPath)
    : Tex() 
    {
        glGenTextures(1, &id.v);
        glBindTexture(GL_TEXTURE_CUBE_MAP, id);

        // 为当前绑定的纹理对象设置环绕、过滤方式
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_REPEAT
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); //GL_REPEAT
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); //GL_REPEAT
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        TexCube::loadFace(rightPath, 0, TexTransform::CW);
        TexCube::loadFace(leftPath, 1, TexTransform::CCW);
        TexCube::loadFace(frontPath, 2, TexTransform::N);
        TexCube::loadFace(backPath, 3, TexTransform::CS);
        TexCube::loadFace(topPath, 4, TexTransform::N);
        TexCube::loadFace(bottomPath, 5, TexTransform::CS);

        //glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    void TexCube::bind(sint location) const {
        glActiveTexture(GL_TEXTURE0 + location);
        glBindTexture(GL_TEXTURE_CUBE_MAP, id);
    }
    void TexCube::loadFace(strid path, uint faceIndex, TexTransform transform) {
        auto data = Tex::LoadImage(path, transform, TexChannel::RGBA);
        if (data.data()) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + faceIndex, 0, data.channelCode(), data.width(), data.height(), 0, data.channelCode(), GL_UNSIGNED_BYTE, data.data());
        }
    }

}