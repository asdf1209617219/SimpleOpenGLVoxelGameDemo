#pragma once
#include <dme/graph/texture/tex.h>
namespace dme::graph {
    class TexCube : public Tex {
    private:
        static void loadFace(strid path, uint faceIndex, TexTransform transform);
    public:
        TexCube();
        TexCube(strid rightPath, strid leftPath,
            strid frontPath, strid backPath,
            strid topPath, strid bottomPath);
        void bind(sint location = 0) const;
    };

}