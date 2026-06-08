#pragma once
#include <dme/graph/texture/tex.h>
namespace dme::graph {
    class Tex2d : public Tex {
    private:
    public:
        Tex2d() noexcept;
        explicit Tex2d(strid path, TexWrap texWrap = TexWrap::ClampToEdge, TexFilter texFilter = TexFilter::Nearest);
        //Tex2d(uchar* data, sint width, sint height, sint channel);

        void bind(sint location = 0) const;
    };

}