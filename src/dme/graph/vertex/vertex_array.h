#pragma once
#include <dme/graph/vertex/vertex_type.h>

namespace dme::graph {

    //用于申请buffer和调用绘制函数，具体的顶点数据需在其他地方管理
    class VertexArray {
    private:
        uint vao;
        uint vbo;
        uint stride;
        uint vertexCount;
        void* vertex;
    public:
        VertexArray();
        VertexArray(const VertexType& types, void* vertex, uint vertexCount);
        

        void draw() const;
        void release(); //删除申请的vao和vbo
        void rebindBuffer() const; //重新绑定buffer
        void changeCount(uint vertexCount); //更改顶点数量
        void changeVertex(void* vertex, uint vertexCount); //更改顶点以及数量

        void* getVertex() const;
        uint getVertexCount() const;
    };

}