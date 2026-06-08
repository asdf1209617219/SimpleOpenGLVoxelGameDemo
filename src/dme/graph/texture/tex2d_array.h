#pragma once
#include <dme/graph/texture/tex.h>

namespace dme::graph {
    //纹理数组的每个图形宽高必须相同
    class Tex2dArray : public Tex {
    private:
        TexChannel texChannel;
        DynArr<TexData> dataArr; //每层的纹理数据
        DynArr<strid> pathArr; //通过索引查图片路径
        FlatMap<strid, TexIndex> texIndexMap; //通过图片路径查询纹理索引
    public:
        Tex2dArray(TexChannel texChannel = TexChannel::RGBA) noexcept;

        //禁止复制
        Tex2dArray(const Tex2dArray&) = delete;
        Tex2dArray& operator=(const Tex2dArray&) = delete;

        //加载一个新纹理，如果已经存在则返回原有的索引，否则添加到数组中并返回新索引（添加后需要调用merge函数将新纹理合并到数组中）
        TexIndex addIfAbsent(strid path) noexcept;
        //通过图片路径查询纹理索引
        TexIndex getIndex(strid path) const noexcept;

        //将所有纹理合并成一个纹理数组，返回是否成功（纹理的宽高必须相同）
        bool merge();

        void bind(sint location = 0) const;

        //Block默认使用纹理
        static Tex2dArray& Cube();
    };

}