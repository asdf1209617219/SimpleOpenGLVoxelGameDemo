#pragma once
#include <dme/graph/mesh/cube_mesh.h>
namespace dme::graph {
    // 一个world使用一个mesh
    class PlantMesh {
    private:
        //容量大小的可能数量，32，64，128，256，512，1024，2048，4096
        static constexpr uint capacity_count = 8;
        //默认每个面最大顶点数，超过了会扩充容量(乘2)
        static constexpr ushort default_max = block::chunk_volume >> (capacity_count - 1);
        static constexpr uint default_max_bit_width = BitWidth(default_max);
        //区块信息
        struct ChunkInfo {
            uint chunkIndex; //区块第一个面在faceStart的下标
            //ushort capacity; //区块总容量
            constexpr ChunkInfo() noexcept : chunkIndex(0) {}
            constexpr ChunkInfo(uint chunkIndex) noexcept : chunkIndex(chunkIndex) {}
        };

        //顶点数组对象id
        uint vao;
        //顶点缓冲对象id
        uint vbo;
        //顶点数组
        DynArr<PlantVertex> vertex;
        //每个区块在vertex的起始点
        DynArr<uint> vertexStart;
        //每个区块的顶点数量
        DynArr<ushort> vertexCount;
        //每个区块的最大顶点数量
        DynArr<ushort> vertexCapacity;
        //每个区块在vertexStart的下标
        using ChunkInfoMap = std::unordered_map<Int3, ChunkInfo>;
        ChunkInfoMap chunkInfoMap;

        //每种大小的空闲空间的开头
        uid vertexFreeList[capacity_count];
        //chunk的空闲空间
        uid vertexStartFreeList;

        //在draw前判断是否需要重置缓存
        bool needRefreshBuffer;

        friend Game;
        //初始化vao和vbo等，只允许在Game::init()里调用
        void init();
        //重新设置缓冲大小以及开始位置
        void resizeBuffer() const;
        //（弃用该函数，目前通过缓存顶点相对下标来实现O(1)时间复杂度的查找）查找该区块相同位置相同面上是否有数据，通过vertexIndex返回vertex数组下标
        bool findSamePos(const uint start, const uint count, const block::CBPos pos, uint& vertexIndex);
        //将oldFaceCapacity扩充一倍，查找空闲空间并将数据移动到新空间
        void findAndMoveToFreeSpace(const uint chunkIndex, const uint oldCapacity, uint& start);
        //查询chunk的信息，如果chunk还没加入chunkInfoMap则对chunk申请空间
        ChunkInfoMap::iterator findOrApplyChunk(const Int3& chunkPos);
        //将新的顶点添加到当前顶点数组最后，修改block的graph为顶点相对下标
        void pushVertex(const ChunkInfoMap::iterator& it, uid& graph, const block::CBPos pos, const uint tex);
        //修改面数据
        void updateVertex(const ChunkInfoMap::iterator& it, const uid& graph, const uint tex);
        //移除面并将后面的数据向前移动
        void removeVertex(const ChunkInfoMap::iterator& it, block::Chunk& chunk, uid& graph);
        //计算该容量扩容了几次
        inline ushort getCapacityIndex(ushort capacity) const;
    public:
        //传入方块的数据以及在区块中的位置以添加进入mesh，如果剩余容量不够会对buffer进行扩容，这会改变顶点指针位置
        void add(const block::Chunk& chunk, const block::CBPos pos, block::Block& block, const block::BlockProto& proto);
        //直接更新顶点中除了位置的数据
        void update(const block::Chunk& chunk, const block::Block& block, const block::BlockProto& proto);
        //移除方块对应的顶点
        void remove(block::Chunk& chunk, const block::CBPos pos, block::Block& block);
        //卸载区块信息，并释放顶点所占空间
        void unloadChunkInfo(const block::Chunk& chunk);
        //绘制
        void draw();

        PlantMesh();
        PlantMesh(const block::ChunkMap& chunks);
        //~PlantMesh();
    };

}
