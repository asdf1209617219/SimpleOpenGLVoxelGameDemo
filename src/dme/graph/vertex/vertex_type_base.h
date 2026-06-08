#pragma once
#include <dme/core.h>
namespace dme::graph {

    template<typename T>
    constexpr uint GetGLenum = 0;
    template<>
    constexpr uint GetGLenum<schar> = GL_BYTE;
    template<>
    constexpr uint GetGLenum<uchar> = GL_UNSIGNED_BYTE;
    template<>
    constexpr uint GetGLenum<sshort> = GL_SHORT;
    template<>
    constexpr uint GetGLenum<ushort> = GL_UNSIGNED_SHORT;
    template<>
    constexpr uint GetGLenum<sint> = GL_INT;
    template<>
    constexpr uint GetGLenum<uint> = GL_UNSIGNED_INT;
    template<>
    constexpr uint GetGLenum<float> = GL_FLOAT;
    template<>
    constexpr uint GetGLenum<double> = GL_DOUBLE;

    //VertexField表示每个顶点中的位置或颜色或法线等单一字段，一个顶点是多个字段组合起来的
    struct VertexField {
        const uchar typeId; //单个数据类型id，通过base_type_id<T>获得
        const uchar typeSize; //单个数据类型的大小
        const uchar count; //表示同一个数据类型一共有几个
        const uchar totalSize; //该属性总大小
        const uint glenum; //OpenGL里对应的类型

        constexpr bool operator ==(const VertexField& a) const noexcept {
            return typeId == a.typeId && count == a.count;
        }
        friend constexpr Hash Hasher(const VertexField& a) noexcept {
            return Hash::Combine(a.typeId, a.count);
        }

    private:
        constexpr VertexField(uchar typeId_, uchar typeSize_, uchar count_, uint glenum_) noexcept : typeId(typeId_), count(count_), typeSize(typeSize_), totalSize(typeSize_ * count_), glenum(glenum_) {}
    public:
        template<typename T>
        static constexpr VertexField create(uchar count_) noexcept {
            return VertexField(base_type_id<T>, sizeof(T), count_, GetGLenum<T>);
        }
    };

    struct VertexFieldc {
        static constexpr VertexField b1 = VertexField::create<schar>(1);
        static constexpr VertexField b2 = VertexField::create<schar>(2);
        static constexpr VertexField b3 = VertexField::create<schar>(3);
        static constexpr VertexField b4 = VertexField::create<schar>(4);
        static constexpr VertexField ub1 = VertexField::create<uchar>(1);
        static constexpr VertexField ub2 = VertexField::create<uchar>(2);
        static constexpr VertexField ub3 = VertexField::create<uchar>(3);
        static constexpr VertexField ub4 = VertexField::create<uchar>(4);
        static constexpr VertexField s1 = VertexField::create<sshort>(1);
        static constexpr VertexField s2 = VertexField::create<sshort>(2);
        static constexpr VertexField s3 = VertexField::create<sshort>(3);
        static constexpr VertexField s4 = VertexField::create<sshort>(4);
        static constexpr VertexField us1 = VertexField::create<ushort>(1);
        static constexpr VertexField us2 = VertexField::create<ushort>(2);
        static constexpr VertexField us3 = VertexField::create<ushort>(3);
        static constexpr VertexField us4 = VertexField::create<ushort>(4);
        static constexpr VertexField i1 = VertexField::create<sint>(1);
        static constexpr VertexField i2 = VertexField::create<sint>(2);
        static constexpr VertexField i3 = VertexField::create<sint>(3);
        static constexpr VertexField i4 = VertexField::create<sint>(4);
        static constexpr VertexField ui1 = VertexField::create<uint>(1);
        static constexpr VertexField ui2 = VertexField::create<uint>(2);
        static constexpr VertexField ui3 = VertexField::create<uint>(3);
        static constexpr VertexField ui4 = VertexField::create<uint>(4);
        static constexpr VertexField f1 = VertexField::create<float>(1);
        static constexpr VertexField f2 = VertexField::create<float>(2);
        static constexpr VertexField f3 = VertexField::create<float>(3);
        static constexpr VertexField f4 = VertexField::create<float>(4);
        static constexpr VertexField d1 = VertexField::create<double>(1);
        static constexpr VertexField d2 = VertexField::create<double>(2);
        static constexpr VertexField d3 = VertexField::create<double>(3);
        static constexpr VertexField d4 = VertexField::create<double>(4);
    };

    //获取顶点类型
    template<typename T>
    constexpr VertexField GetVertexField;

    template<>
    constexpr VertexField GetVertexField<schar> = VertexFieldc::b1;
    template<>
    constexpr VertexField GetVertexField<Char2> = VertexFieldc::b2;
    template<>
    constexpr VertexField GetVertexField<Char3> = VertexFieldc::b3;
    template<>
    constexpr VertexField GetVertexField<Char4> = VertexFieldc::b4;

    template<>
    constexpr VertexField GetVertexField<uchar> = VertexFieldc::ub1;
    template<>
    constexpr VertexField GetVertexField<Uchar2> = VertexFieldc::ub2;
    template<>
    constexpr VertexField GetVertexField<Uchar3> = VertexFieldc::ub3;
    template<>
    constexpr VertexField GetVertexField<Uchar4> = VertexFieldc::ub4;

    template<>
    constexpr VertexField GetVertexField<sshort> = VertexFieldc::s1;
    template<>
    constexpr VertexField GetVertexField<Short2> = VertexFieldc::s2;
    template<>
    constexpr VertexField GetVertexField<Short3> = VertexFieldc::s3;
    template<>
    constexpr VertexField GetVertexField<Short4> = VertexFieldc::s4;

    template<>
    constexpr VertexField GetVertexField<ushort> = VertexFieldc::us1;
    template<>
    constexpr VertexField GetVertexField<Ushort2> = VertexFieldc::us2;
    template<>
    constexpr VertexField GetVertexField<Ushort3> = VertexFieldc::us3;
    template<>
    constexpr VertexField GetVertexField<Ushort4> = VertexFieldc::us4;

    template<>
    constexpr VertexField GetVertexField<sint> = VertexFieldc::i1;
    template<>
    constexpr VertexField GetVertexField<Int2> = VertexFieldc::i2;
    template<>
    constexpr VertexField GetVertexField<Int3> = VertexFieldc::i3;
    template<>
    constexpr VertexField GetVertexField<Int4> = VertexFieldc::i4;

    template<>
    constexpr VertexField GetVertexField<uint> = VertexFieldc::ui1;
    template<>
    constexpr VertexField GetVertexField<Uint2> = VertexFieldc::ui2;
    template<>
    constexpr VertexField GetVertexField<Uint3> = VertexFieldc::ui3;
    template<>
    constexpr VertexField GetVertexField<Uint4> = VertexFieldc::ui4;

    template<>
    constexpr VertexField GetVertexField<float> = VertexFieldc::f1;
    template<>
    constexpr VertexField GetVertexField<Float2> = VertexFieldc::f2;
    template<>
    constexpr VertexField GetVertexField<Float3> = VertexFieldc::f3;
    template<>
    constexpr VertexField GetVertexField<Float4> = VertexFieldc::f4;

    template<>
    constexpr VertexField GetVertexField<double> = VertexFieldc::d1;
    template<>
    constexpr VertexField GetVertexField<Double2> = VertexFieldc::d2;
    template<>
    constexpr VertexField GetVertexField<Double3> = VertexFieldc::d3;
    template<>
    constexpr VertexField GetVertexField<Double4> = VertexFieldc::d4;

    //表示一个顶点中所有属性的类型
    struct VertexType {
        const VertexField* fields; //属性数组
        const uint count; //属性个数
        const uint stride; //整个顶点对象的大小
        const uint id; //顶点对象类型ID
	    const stdstr name; //顶点对象类型名称

    private:
        constexpr VertexType(const VertexField* fields, const uint count, const uint stride, const uint vertexId, const stdstr name) noexcept : fields(fields), count(count), stride(stride), id(vertexId), name(name) {}

        inline static constinit uint _id = 0;
        static uint _createVertexId() noexcept {
            uint i = _id;
            ++_id;
            return i;
        }
        static DynArr<VertexType>& _getArr() noexcept {
            static DynArr<VertexType> _arr = DynArr<VertexType>(16);
            return _arr;
        }
    public:
        static const VertexType& GetById(uint id) noexcept {
            return _getArr()[id];
        }

        //注册顶点类型，第一个参数为顶点对象，后面的参数为顶点的各个属性
        //注意顶点的内存对齐
        template<typename VertexT, typename ...Fields>
        static VertexType RegisterVertex() noexcept {
            static VertexField _fields[sizeof...(Fields)] = { GetVertexField<Fields>... };
            auto type = VertexType(
                _fields,
                sizeof...(Fields),
                sizeof(VertexT),
                _createVertexId(),
			    typeid(VertexT).name()
            );
            _getArr().add(type);
            return type;
        }

        //设定顶点属性，调用前需先绑定vao并绑定vbo及其数据
        void bindVertexAttr() const noexcept {
            uint typeIndex = 0;
            uchar* pointer = null;
            for (uint i = 0; i < count; i++) {
                const VertexField& vertexType = fields[i];
                if (vertexType.typeId == BaseTypeId::Float) {
                    glVertexAttribPointer(typeIndex, vertexType.count, vertexType.glenum, GL_FALSE, stride, pointer);
                }
                //else if (vertexType.typeId == BaseTypeId::Double) { //TODO OpenGL 4.1后才支持Double类型
                //    glVertexAttribLPointer(typeIndex, vertexType.count, vertexType.glenum, stride, pointer);
                //}
                else {
                    glVertexAttribIPointer(typeIndex, vertexType.count, vertexType.glenum, stride, pointer);
                }
                glEnableVertexAttribArray(typeIndex);
                typeIndex++;
                pointer += vertexType.totalSize;
            }
        }

        bool operator ==(const VertexType& a) const noexcept {
            return id == a.id;
        }
        bool operator <(const VertexType& a) const noexcept {
            return id < a.id;
        }
        friend constexpr Hash Hasher(const VertexType& a) noexcept {
            return Hasher(a.id);
        }
    };

    //表示顶点个数和索引个数
    struct VertexIndexCount {
        uint vertexCount;
        uint indexCount;

        constexpr VertexIndexCount() noexcept : vertexCount(0), indexCount(0) {}
        constexpr VertexIndexCount(uint vertexCount, uint indexCount) noexcept : vertexCount(vertexCount), indexCount(indexCount) {}
        //个数为0
        constexpr bool empty() const noexcept {
            return vertexCount == 0 && indexCount == 0;
        }
        //添加矩形
        constexpr void addRectangle(uint count = 1) noexcept {
            vertexCount += count * 4;
            indexCount += count * 6;
        }
        //添加三角形
        constexpr void addTriangle(uint count = 1) noexcept {
            vertexCount += count * 3;
            indexCount += count * 3;
        }
    };

    //用于表示某个对象的顶点和索引在mesh中的位置
    struct MeshSegment {
        uint vertexStart; //在顶点数组中的顶点下标
        uint vertexEnd; //等于顶点下标加顶点个数
        uint indexStart; //在索引数组中的索引下标
        uint indexEnd; //等于索引下标加索引个数

        constexpr MeshSegment() noexcept : vertexStart(0), vertexEnd(0), indexStart(0), indexEnd(0) {}
        constexpr MeshSegment(uint vertexStart, uint vertexEnd, uint indexStart, uint indexEnd) noexcept : vertexStart(vertexStart), vertexEnd(vertexEnd), indexStart(indexStart), indexEnd(indexEnd) {}
        //个数为0
        constexpr bool empty() const noexcept {
            return vertexStart == vertexEnd && indexStart == indexEnd;
        }
        //设置为空，这里只是将end设置为start
        constexpr void setEmpty() noexcept {
            vertexEnd = vertexStart;
            indexEnd = indexStart;
        }
        constexpr uint vertexCount() const noexcept {
            return vertexEnd - vertexStart;
        }
        constexpr uint indexCount() const noexcept {
            return indexEnd - indexStart;
        }

        bool operator ==(const MeshSegment& a) const noexcept {
            return vertexStart == a.vertexStart && vertexEnd == a.vertexEnd && indexStart == a.indexStart && indexEnd == a.indexEnd;
        }
        friend constexpr Hash Hasher(const MeshSegment& a) noexcept {
            return Hash::Combine(a.vertexStart, a.vertexEnd, a.indexStart, a.indexEnd);
        }
    };
    
    //存储mesh中空闲的段落，根据个数排序
    struct FreeStartCount {
        uint start; //数组下标起点
        uint count; //个数

        constexpr FreeStartCount() noexcept : start(0), count(0) {}
        constexpr FreeStartCount(uint start, uint count) noexcept : start(start), count(count) {}
        //个数为0
        constexpr bool empty() const noexcept {
            return count == 0;
        }

        bool operator ==(const FreeStartCount& a) const noexcept {
            return start == a.start && count == a.count;
        }
        bool operator <(const FreeStartCount& a) const noexcept {
            return count < a.count || (count == a.count && start < a.start);
        }
        friend constexpr Hash Hasher(const FreeStartCount& a) noexcept {
            return Hash::Combine(a.start, a.count);
        }
    };
    //存储mesh中空闲的段落，根据起点排序
    struct FreeSegment {
        uint start; //数组下标起点
        uint end; //数组下标加个数

        constexpr FreeSegment() noexcept : start(0), end(0) {}
        constexpr FreeSegment(uint start, uint end) noexcept : start(start), end(end) {}
        //个数为0
        constexpr bool empty() const noexcept {
            return end - start == 0;
        }

        bool operator ==(const FreeSegment& a) const noexcept {
            return start == a.start && end == a.end;
        }
        bool operator <(const FreeSegment& a) const noexcept {
            return start < a.start;
        }
        friend constexpr Hash Hasher(const FreeSegment& a) noexcept {
            return Hash::Combine(a.start, a.end);
        }
    };

    //表示顶点数组和索引数组，传入的vertex和index必须是通过new[]创建的
    struct VertexIndex {
        uchar* vertex;
        uint* index;
        uint vertexCount;
        uint indexCount;

        constexpr VertexIndex() noexcept : vertex(null), vertexCount(0), index(null), indexCount(0) {}
        constexpr VertexIndex(uchar* vertex, uint vertexCount, uint* index, uint indexCount) noexcept : vertex(vertex), vertexCount(vertexCount), index(index), indexCount(indexCount) {}
        constexpr bool isNull() const noexcept {
            return vertex == null && index == null;
        }
        constexpr bool isEmpty() const noexcept {
            return vertex == null && vertexCount == 0 && index == null && indexCount == 0;
        }
        void release() noexcept {
            //std::free(vertex);
            //delete[] vertex;
            delete[] vertex;
            delete[] index;
            vertex = null;
            index = null;
            vertexCount = 0;
            indexCount = 0;
        }
    };

    //描述顶点字段的模板类，顶点字段要与Fields参数一一对应
    template<typename VertexT, typename ...Fields>
    class VertexBase {
    private:
        inline static VertexType _vertexType = VertexType::RegisterVertex<VertexT, Fields...>();
    public:
        static const VertexType& GetVertexType() noexcept {
            return _vertexType;
        }
    };

    template <typename T>
    concept IsVertex = requires() {
        // 检查是否存在返回const VertexType&的静态函数
        { T::GetVertexType() } -> std::same_as<const VertexType&>;
    };

    template <typename T>
    concept UsingVertexType = requires() {
        // 检查是否using了vertex_type类型且符合IsVertex
        typename T::vertex_type;
        requires IsVertex<typename T::vertex_type>;
    };
}

namespace std {
    template<>
    struct hash<dme::graph::VertexField> {
        size_t operator ()(const dme::graph::VertexField& a) const noexcept {
            return Hasher(a);
        }
    };
    template<>
    struct hash<dme::graph::VertexType> {
        size_t operator ()(const dme::graph::VertexType& a) const noexcept {
            return Hasher(a);
        }
    };
}