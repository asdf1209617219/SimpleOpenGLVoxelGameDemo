#pragma once
#include <dme/graph/vertex/vertex_type.h>

namespace dme::graph {

    //用于申请buffer和调用绘制函数，具体的顶点数据索引数据需在其他地方管理
    class VertexIndexArray {
    private:
        uint vao;
        uint vbo;
        uint ibo;
        uint stride;
        uint vertexCount;
        uint indexCount;
        void* vertex;
        uint* index;
    public:
        VertexIndexArray(const VertexType& types, void* vertex, uint vertexCount, uint* index, uint indexCount);
        ~VertexIndexArray();

        void draw() const;
        void rebindBuffer() const; //重新绑定buffer
        void changeCount(uint vertexCount, uint indexCount); //更改顶点数量
        void changeCount(void* vertex, uint vertexCount, uint* index, uint indexCount); //更改顶点数量

        void* getVertex() const;
        uint getVertexCount() const;
        uint* getIndex() const;
        uint getIndexCount() const;
    };

}