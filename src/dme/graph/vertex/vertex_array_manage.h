#pragma once
#include <dme/graph/vertex/vertex_type.h>
namespace dme::graph {

    //用于在Mesh中将vao分组
    class VertexArrayManage {
        /*
            用于记录顶点数组中哪些空间是空闲的，一段空闲空间在Map中存4次，其中包括顶点、索引空间的开头结尾，
            key为顶点空间开头，value为顶点空间结尾+1，然后key和value相反再存一次，
            再将key和value的最高位取反表示索引的空间，再这么存两次
            不允许顶点或索引的个数超过2^31
        */
        using FreeMap = std::unordered_map<uint, uint>;

    protected:
        uint vao;
        uint vbo;
        uint ibo;
        //一个顶点的大小
        uint stride;
        //在draw前判断是否需要重置缓存
        bool needResizeBuffer;

        VertexIndex vertexIndex;
        uint vertexMaxCount;
        uint indexMaxCount;

        FreeMap freeMap;

        //查找空闲空间
        void findFreeMap(MeshSegment& segment, uint& newVertexCount, uint& newIndexCount, uint addVertexCount, uint addIndexCount);
        //对顶点以及索引数组扩容
        void expandBuffer(uint oldVertexCount, uint newVertexCount, uint oldIndexCount, uint newIndexCount);
        //重新设置缓冲大小以及开始位置
        void resizeBuffer() const;
    public:
        //通过调用MeshObjBase的willAddVertexIndexCount函数获取将要添加的顶点及索引数量，如果剩余容量不够会对buffer进行扩容，这会改变顶点及索引指针位置。然后调用editMesh函数对顶点及索引数据进行赋值
        void startAdd(MeshObjBase& obj);
        //更新顶点及索引，只允许更新该对象之前添加的顶点及索引，即无法删除和添加
        void startUpdate(MeshObjBase& obj);
        //移除对象对应的顶点，这里只是将该对象的顶点坐标设置为0
        void remove(MeshObjBase& obj);
        void draw();
        //删除该对象下的数据
        void release();
        
        //~VertexArrayManage();

        VertexArrayManage(const VertexType& type);
        VertexArrayManage(const VertexType& type, uint vertexMaxCount, uint indexMaxCount);

        template<IsMeshObj T>
        static VertexArrayManage Create() {
            return VertexArrayManage(T::vertex_type::GetVertexType());
        }
        template<IsMeshObj T>
        static VertexArrayManage Create(uint vertexMaxCount, uint indexMaxCount) {
            return VertexArrayManage(T::vertex_type::GetVertexType(), vertexMaxCount, indexMaxCount);
        }
    };

}