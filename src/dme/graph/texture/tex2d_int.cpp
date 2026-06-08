#include <dme/graph/texture/tex2d_int.h>
namespace dme::graph {
    Tex2dInt::Tex2dInt() noexcept : data(null), count(0), size(0), width(0), height(0) {};
    Tex2dInt::Tex2dInt(uint width, uint height) : data(null), count(0), size(width * height), width(width), height(height) {
		data = Malloc<uint>(size);
		Memset<uint>(data, 0, size);

        glGenTextures(1, &id.v);
        glBindTexture(GL_TEXTURE_2D, id.v);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, width, height, 0, GL_RED_INTEGER, GL_INT, data);

        glBindTexture(GL_TEXTURE_2D, 0);
    };

    Tex2dInt::Tex2dInt(Tex2dInt&& a) noexcept : data(a.data), count(a.count), size(a.size), width(a.width), height(a.height) {
        id = a.id;
        a.id.setNull();
        a.data = null;
        a.count = 0;
        a.size = 0;
        a.width = 0;
        a.height = 0;
    }
    Tex2dInt& Tex2dInt::operator=(Tex2dInt&& a) noexcept {
        if(this == &a) {
            return *this;
		}
        if (data) {
            Free(data);
        }
        id = a.id;
        data = a.data;
        count = a.count;
        size = a.size;
        width = a.width;
        height = a.height;
        a.id.setNull();
        a.data = null;
        a.count = 0;
        a.size = 0;
        a.width = 0;
        a.height = 0;
		return *this;
    }

    Tex2dInt::~Tex2dInt() {
        if (data) {
            Free(data);
            data = null;
            count = 0;
            size = 0;
            width = 0;
            height = 0;
            Tex::release();
		}
    }
}