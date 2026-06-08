#pragma once
#include <dme/core.h>
namespace dme::graph {
    //纹理环绕方式(Texture Wrapping)
    enum class TexWrap {
        Repeat, //对纹理的默认行为。重复纹理图像。
        MirroredRepeat, //和Repeat一样，但每次重复图片是镜像放置的。
        ClampToEdge, //纹理坐标会被约束在0到1之间，超出的部分会重复纹理坐标的边缘，产生一种边缘被拉伸的效果。
        ClampToBorder, //超出的坐标为用户指定的边缘颜色。
    };

    //纹理过滤(Texture Filtering)
    enum class TexFilter {
        Nearest, //默认的纹理过滤方式。选择中心点最接近纹理坐标的像素。
        Linear, //会基于纹理坐标附近的纹理像素，计算出一个插值。
    };

    enum class TexChannel {
	    AUTO = 0, //以图片文件的通道数为准
        RED = 1,
        RGB = 3,
        RGBA = 4,
    };

    enum class TexTransform {
        N = 0, //不做处理(none)
        CW, //顺时针(clockwise)
        CS, //中心对称(central symmetry)
        CCW, //逆时针(counterclockwise)
        //FlipHorizontal, //水平翻转
        //FlipVertical, //垂直翻转
    };

    class Tex;
    class TexAtlas;
	//纹理数据类，包含图片数据指针、宽高、通道数等信息，通过RAII自动释放内存
    class TexData {
		friend class Tex;
        friend class TexAtlas;
    private:
        uchar* _data;
        sint _width;
        sint _height;
        sint _channel;
        sint _channelCode;

        TexData(uchar* data, sint width, sint height, sint channel, sint channelCode) noexcept;
    public:
        //会同时申请空间，内存初始化为0
        static TexData AllocTexData(sint width, sint height, TexChannel channel = TexChannel::RGBA) noexcept;

        TexData(TexData&& a) noexcept;
        TexData& operator=(TexData&& a) noexcept;
        ~TexData();
        //禁止复制
        TexData(const TexData&) = delete;
        TexData& operator=(const TexData&) = delete;

		//返回纹理数据指针
        uchar* data() const noexcept {
            return _data;
	    }
		//返回纹理宽度
        sint width() const noexcept {
            return _width;
		}
		//返回纹理高度
        sint height() const noexcept {
            return _height;
        }
		//返回纹理通道数
        sint channel() const noexcept {
            return _channel;
		}
		//返回纹理通道数对应的OpenGL格式代码，如GL_RGB、GL_RGBA等
        sint channelCode() const noexcept {
            return _channelCode;
		}

        //返回纹理面积
        sint area() const noexcept {
            return _width * _height;
        }
		//返回数据大小，等于宽乘高乘通道数
        sint dataSize() const noexcept {
            return _width * _height * _channel;
        }

        //将纹理导出到文件，路径为相对于游戏可执行文件的路径，不要包含文件扩展名，返回是否成功
        bool exportToFile(strid path) const;

        //用于纹理图集中排序，越宽越高的在后面
        constexpr bool operator<(const TexData& a) const noexcept {
            if (_width < a._width) {
                return true;
            }
            if (_width > a._width) {
                return false;
            }
            if (_height < a._height) {
                return true;
            }
            if (_height > a._height) {
                return false;
            }
            if (_data < a._data) {
                return true;
            }
            return false;
        }
    };
    //用于记录纹理图中每个纹理在图集中的位置，不支持旋转
    class TexPos {
    private:
        sint _posZ;
        Abox2i _box; //矩形的左下角和右上角
    public:
        constexpr TexPos() noexcept
            : _posZ(0), _box() {}
        //从矩形的左下和右上两个顶点位置创建TexPos
        constexpr TexPos(const Int2& lowerLeft, const Int2& upperRight, const sint posZ) noexcept
            : _posZ(posZ), _box(lowerLeft, upperRight) {}

        //纹理数组中的层数
        constexpr sint z() const noexcept {
            return _posZ;
        }

        //包围盒
        constexpr const Abox2i& box() const noexcept {
            return _box;
        }

        //左下
        constexpr const Int2& min() const noexcept {
            return _box.min;
        }
        //左下x
        constexpr sint x0() const noexcept {
            return _box.min.x;
        }
        //左下y
        constexpr sint y0() const noexcept {
            return _box.min.y;
        }

        //右上
        constexpr const Int2& max() const noexcept {
            return _box.max;
        }
        //右上x
        constexpr sint x1() const noexcept {
            return _box.max.x;
        }
        //右上y
        constexpr sint y1() const noexcept {
            return _box.max.y;
        }

        //宽
        constexpr sint w() const noexcept {
            return _box.max.x - _box.min.x;
        }
        //高
        constexpr sint h() const noexcept {
            return _box.max.y - _box.min.y;
        }
    };

    //用于记录纹理在图集中的索引
    class TexIndex {
    private:
        uid index;
    public:
        explicit TexIndex(uid index = uid::Null) noexcept : index(index) {}
        constexpr uid get() const noexcept {
            return index;
        }
        constexpr operator uint() const noexcept {
            return index.v;
        }
    };

    class Tex {
    private:
    protected:
        uid id;

        inline static FlatMap<strid, uid> PathMap = FlatMap<strid, uid>(16);
    public:
        Tex() noexcept;

        //删除纹理，将id设置为null
        void release() noexcept;
        //如果id为null则申请纹理id
        void init() noexcept;

        uid getId() const noexcept;

        //纹理id是否为空
        bool isNull() const noexcept;
        //纹理id是否非空
        bool notNull() const noexcept;

        static TexData LoadImage(strid path, TexTransform transform = TexTransform::N, TexChannel texChannel = TexChannel::RGBA);
		//目前不会自动创建目录，如果路径中的目录不存在会导致写入失败
		static bool ExportToFile(strid path, const TexData& data);

        static constexpr sint TexWrapToGLParam(TexWrap param) noexcept {
            switch (param) {
            case TexWrap::Repeat:
                return GL_REPEAT;
            case TexWrap::MirroredRepeat:
                return GL_MIRRORED_REPEAT;
            case TexWrap::ClampToEdge:
                return GL_CLAMP_TO_EDGE;
            case TexWrap::ClampToBorder:
                return GL_CLAMP_TO_BORDER;
            default:
                return -1;
            }
        }
        static constexpr sint TexFilterToGLParam(TexFilter param) noexcept {
            switch (param) {
            case TexFilter::Nearest:
                return GL_NEAREST;
            case TexFilter::Linear:
                return GL_LINEAR;
            default:
                return -1;
            }
        }
        static constexpr sint ChannelToGLFormat(sint channel) noexcept {
            switch (channel) {
            case static_cast<sint>(TexChannel::RED):
                return GL_RED;
            case static_cast<sint>(TexChannel::RGB):
                return GL_RGB;
            case static_cast<sint>(TexChannel::RGBA):
                return GL_RGBA;
            default:
                return -1;
            }
        }
        static constexpr sint ChannelToGLFormat(TexChannel channel) noexcept {
            switch (channel) {
            case TexChannel::RED:
                return GL_RED;
            case TexChannel::RGB:
                return GL_RGB;
            case TexChannel::RGBA:
                return GL_RGBA;
            default:
                return -1;
            }
        }
    };

}