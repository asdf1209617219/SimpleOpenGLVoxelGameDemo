#include <dme/graph/texture/tex.h>
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

namespace dme::graph {
    TexData::TexData(uchar* data, sint width, sint height, sint channel, sint channelCode) noexcept
        : _data(data), _width(width), _height(height), _channel(channel), _channelCode(channelCode) {}
    TexData::TexData(TexData&& a) noexcept : _data(a._data), _width(a._width), _height(a._height), _channel(a._channel), _channelCode(a._channelCode)
    {
        a._data = null;
        a._width = 0;
        a._height = 0;
        a._channel = 0;
        a._channelCode = -1;
    }
    TexData& TexData::operator=(TexData&& a) noexcept {
        if (_data) {
            DME_FREE(_data);
        }
        _data = a._data;
        a._data = null;
        a._width = 0;
        a._height = 0;
        a._channel = 0;
        a._channelCode = -1;
        return *this;
    }
    TexData::~TexData() {
        if (_data) {
            DME_FREE(_data);
        }
    }
    TexData TexData::AllocTexData(sint width, sint height, TexChannel channel) noexcept {
        sint c = static_cast<sint>(channel);
        if (width <= 0 || height <= 0 || c <= 0) {
            return TexData(null, 0, 0, 0, -1);
        }
		size_t dataSize = tost(width) * height * c;
        uchar* data = reinterpret_cast<uchar*>(DME_MALLOC(dataSize));
        if (!data) {
            return TexData(null, 0, 0, 0, -1);
        }
        MemsetByte(data, 0, dataSize);
        return TexData(data, width, height, c, Tex::ChannelToGLFormat(channel));
    }
    bool TexData::exportToFile(strid path) const {
        Log::Info("TexData: start exporting to file");
        bool success = true;
        strid filePath = strid(path.str() + ".png");
        if (!Tex::ExportToFile(filePath, *this)) {
            success = false;
        }
        Log::Info("TexData: exporting end");
        return success;
    }

    Tex::Tex() noexcept : id() {}
    //Tex::Tex(const Tex& tex) noexcept : id(tex.id) {}
    //Tex::Tex(Tex&& tex) noexcept : id(tex.id) {
    //    tex.id.setNull();
    //}

    void Tex::release() noexcept {
        if (id.isNull()) {
            return;
        }
        glDeleteTextures(1, &id.v);
        id.setNull();
    }

    void Tex::init() noexcept {
        if (id.notNull()) {
            return;
        }
        glGenTextures(1, &id.v);
    }

    uid Tex::getId() const noexcept {
        return id;
    }
    bool Tex::isNull() const noexcept {
        return id.isNull();
    }
    bool Tex::notNull() const noexcept {
        return id.notNull();
    }

    TexData Tex::LoadImage(strid path, TexTransform transform, TexChannel texChannel) {
        sint width = 0, height = 0, channel = 0, channelCode = -1;
        uchar* data = stbi_load(path.cstr(), &width, &height, &channel, static_cast<sint>(texChannel));
        if (texChannel != TexChannel::AUTO) {
			channel = static_cast<sint>(texChannel);
        }
        if (data) {
			sint maxSize = Content::GetMaxTextureSize();
			if (width > maxSize || height > maxSize) {
                Log::Error("Texture: image size exceeds the maximum texture size at path: ", path,
                    "\nwidth: ", width, ", height: ", height, ", max size: ", maxSize);
                DME_FREE(data);
                return TexData(null, 0, 0, 0, -1);
            }

            channelCode = ChannelToGLFormat(channel);

            switch (transform) {
                case TexTransform::N:
                break;
                case TexTransform::CW:
                {
                    sint area = width * height;
                    uchar* newData = reinterpret_cast<uchar*>(DME_MALLOC(tost(area * channel)));
                    sint newWidth = height;
                    sint newHeight = width;
                    sint newIndex = (newWidth - 1) * channel;
                    sint oldIndex = 0;
                    for (sint y = 0; y < height; ++y, newIndex = (newWidth - y - 1) * channel) {
                        for (sint x = 0; x < width; ++x, oldIndex += channel, newIndex += channel * newWidth) {
                            for (sint j = 0; j < channel; j++) {
                                newData[newIndex + j] = data[oldIndex + j];
                            }
                        }
                    }
                    DME_FREE(data);
                    data = newData;
                    width = newWidth;
                    height = newHeight;
                }
                break;
                case TexTransform::CS:
                {
                    sint area = width * height;
                    sint newIndex = (area - 1) * channel;
                    sint oldIndex = 0;
                    area /= 2;
                    for (sint i = 0; i < area; ++i, oldIndex += channel, newIndex -= channel) {
                        for (sint j = 0; j < channel; ++j) {
                            Swap(data[newIndex + j], data[oldIndex + j]);
                        }
                    }
                }
                break;
                case TexTransform::CCW:
                {
                    sint area = width * height;
                    uchar* newData = reinterpret_cast<uchar*>(DME_MALLOC(tost(area * channel)));
                    sint newWidth = height;
                    sint newHeight = width;
                    sint newIndex = (area - newWidth) * channel;
                    sint oldIndex = 0;
                    for (sint y = 0; y < height; ++y, newIndex = (area - newWidth + y) * channel) {
                        for (sint x = 0; x < width; ++x, oldIndex += channel, newIndex -= channel * newHeight) {
                            for (sint j = 0; j < channel; ++j) {
                                newData[newIndex + j] = data[oldIndex + j];
                            }
                        }
                    }
                    DME_FREE(data);
                    data = newData;
                    width = newWidth;
                    height = newHeight;
                }
                break;
            }
        }
        else {
		    Log::Error("Texture: loading image failed at path: ", path, "\nfailure reason: ", stbi_failure_reason());
        }
        return TexData(data, width, height, channel, channelCode);
    }

    bool Tex::ExportToFile(strid path, const TexData& data) {
		auto d = data.data();
        if (!d) {
            Log::Error("Texture: export image is empty");
            return false;
        }
		CFile file = CFile().Open(path.cstr(), "rb");
        if (file.file) {
            file.close();
            Log::Error("Texture: the file already exists at path: ", path);
            return false;
        }
        sint stride_in_bytes = data.width() * data.channel();
        sint result = stbi_write_png(path.cstr(), data.width(), data.height(), data.channel(), d, stride_in_bytes);
        if (result == 0) {
            Log::Error("Texture: failed to write image to path: ", path, "\nfailure reason: ", file.lastErrStr());
            return false;
        }
        return true;
    }
}