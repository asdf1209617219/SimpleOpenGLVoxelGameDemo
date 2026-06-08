#pragma once
#include <dme/graph/texture/tex.h>
namespace dme::graph {

    class Tex2dInt : public Tex {
    private:
        uint* data;
        uint count;
        uint size;
        uint width;
        uint height;
    public:
        Tex2dInt() noexcept;
        Tex2dInt(uint width, uint height);

        Tex2dInt(Tex2dInt&& a) noexcept;
        Tex2dInt& operator=(Tex2dInt&& a) noexcept;

        ~Tex2dInt();

        Tex2dInt(const Tex2dInt&) = delete;
        Tex2dInt& operator=(const Tex2dInt&) = delete;

        void bind(sint location = 0) const;
    };
}