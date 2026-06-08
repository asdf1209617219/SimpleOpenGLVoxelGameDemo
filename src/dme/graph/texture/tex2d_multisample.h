#pragma once
#include <dme/graph/texture/tex.h>
namespace dme::graph {
    //用于离屏渲染的多重采样
    class Tex2dMultisample : public Tex {
    private:
    public:
        Tex2dMultisample();
        explicit Tex2dMultisample(sint width, sint height, TexWrap texWrap = TexWrap::ClampToEdge, TexFilter texFilter = TexFilter::Nearest);
        void bind(sint location = 0) const;
    };

}
