#pragma once
#include <dme/graph/vertex/vertex_type.h>

namespace dme {
    class Game;
    namespace block {
        class BlockProto;
    }
    namespace block {
        class Block;
    }
    namespace graph {
        //一个world使用一个mesh
        class CubeMesh {
            friend Game;
        private:
            //容量大小的可能数量，128，256，512，1024，2048，4096
            static constexpr uint capacity_count = 6;
            //默认每个面最大顶点数，超过了会扩充容量(乘2)
            static constexpr ushort default_max = block::chunk_volume >> (capacity_count - 1);
            static constexpr uint default_max_bit_width = BitWidth(default_max);
            //block的graph指向该结构，表示方块每个面对应的顶点相对于区块相同面第一个顶点的偏移
            struct GraphInfo {
                union {
                    ushort face[Face3c::Len]; //12bit表示面对应顶点所在下标，1bit表示该面是否显示(dzzzzyyyyxxxx)
                    uid freeList;
                };

                constexpr GraphInfo() noexcept : face{} {}

                inline constexpr bool isDisplay(Face3 face3) const noexcept {
                    return face[face3] & block::chunk_volume;
                }
                inline constexpr void displayFace(Face3 face3) noexcept {
                    face[face3] |= block::chunk_volume;
                }
                inline constexpr void hideFace(Face3 face3) noexcept {
                    face[face3] &= block::cbpos_max;
                }
                inline constexpr void deleteFace(Face3 face3) noexcept {
                    face[face3] = 0;
                }
                inline constexpr ushort getIndexInFace(Face3 face3) const noexcept {
                    return face[face3] & block::cbpos_max;
                }
                inline constexpr void setIndexInFace(Face3 face3, ushort indexInFace) noexcept {
                    face[face3] = indexInFace | block::chunk_volume;
                }
            };
            // 区块信息
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
            DynArr<CubeVertex> vertex;
            //方块每个面的顶点的位置信息
            DynArr<GraphInfo> graphInfo;

            //每面在vertex的起始点，每个区块的六个面的起始点的下标在faceStart中是连续的，faceCount和faceMaxCount同理
            DynArr<uint> faceStart;
            //面数
            DynArr<ushort> faceCount;
            //每一面的最大面数
            DynArr<ushort> faceCapacity;

            //每个区块在faceStart的下标与区块总容量(Int2的x表示faceStart的下标，y表示区块总容量)
            using ChunkInfoMap = FlatMap<Int3, ChunkInfo>;
            ChunkInfoMap chunkInfoMap;

            //每种大小的空闲空间的开头
            uid vertexFreeList[capacity_count];
            //graphInfo的空闲空间
            uid graphInfoFreeList;
            //chunk的空闲空间
            uid faceStartFreeList;

            //在draw前判断是否需要重置缓存
            bool needRefreshBuffer;

            //初始化vao和vbo等，只允许在Game::init()里调用
            void init();
            //重新设置缓冲大小以及开始位置
            void resizeBuffer() const;
            //（弃用该函数，目前通过缓存顶点相对下标来实现O(1)时间复杂度的查找）查找该区块相同位置相同面上是否有数据，通过vertexIndex返回vertex数组下标
            bool findSamePos(const uint thisFaceStart, const uint thisFaceCount, const block::CBPos pos, uint& vertexIndex);
            //将oldFaceCapacity扩充一倍，查找空闲空间并将数据移动到新空间
            void findAndMoveToFreeSpace(const uint chunkFaceStartIndex, const ushort oldFaceCapacity, uint& vertexStart);
            //查询chunk的信息，如果chunk还没加入chunkInfoMap则对chunk申请空间
            ChunkInfoMap::pointer findOrApplyChunk(const Int3& chunkPos);
            //查询chunk的信息，如果chunk还没加入chunkInfoMap则返回null
            ChunkInfo* findChunk(const Int3& chunkPos);
            //添加方块的GraphInfo，同时设置方块的graph属性
            void pushBlock(block::Block& block);
            //删除方块的GraphInfo，将所占空间加入graphInfoFreeList，同时删除方块的graph属性
            void removeBlock(block::Block& block);
            //将新的顶点添加到当前顶点数组最后，修改GraphInfo对应面的数据
            void pushVertex(const ChunkInfoMap::pointer it, GraphInfo& info, const block::CBPos pos, const Face3 face3, const uint tex, const FRotate rotate);
            //修改面数据
            void updateVertex(const ChunkInfoMap::pointer it, const GraphInfo& info, const Face3 face3, const uint tex, const FRotate rotate);
            //移除面并将最后的面数据移动到移除的地方，并将GraphInfo对应面的数据置空
            void removeVertex(const ChunkInfoMap::pointer it, const block::Chunk& chunk, GraphInfo& info, const Face3 face3);
            //计算该容量扩容了几次
            inline ushort getCapacityIndex(ushort capacity) const;

            //仅供loadChunkMesh和unloadChunkMesh使用，是需要合并面优化的block则返回true
            bool chunkMeshPushVertex(block::Chunk& chunk, const block::CBPos cbpos);
            //仅供loadChunkMesh和unloadChunkMesh使用，是需要合并面优化的block则返回true
            bool chunkMeshRemoveVertex(block::Chunk& chunk, const block::CBPos cbpos);
            //仅供loadChunkMesh和unloadChunkMesh使用
            void chunkMeshOtherChunkFace(const bool isPush, block::Chunk& chunk, block::Chunk& otherChunk, const block::CBPos cbpos, const Face3 face);
            
            template<bool isPush, Face3... Faces>
            void chunkMeshBlock(block::Chunk& chunk, block::Chunk** otherChunks, const block::CBPos cbpos) {
                bool r;
                if constexpr (isPush) {
                    r = chunkMeshRemoveVertex(chunk, cbpos);
                }
                else {
                    r = chunkMeshPushVertex(chunk, cbpos);
                }
                if (r) {
                    if constexpr (((Faces == Face3c::Right) || ...)) {
                        if (otherChunks[Face3::Right]) {
                            chunkMeshOtherChunkFace(isPush, chunk, *otherChunks[Face3::Right], cbpos.getXpNearChunk(), Face3::Right);
                        }
                    }
                    if constexpr (((Faces == Face3c::Left) || ...)) {
                        if (otherChunks[Face3::Left]) {
                            chunkMeshOtherChunkFace(isPush, chunk, *otherChunks[Face3::Right], cbpos.getXnNearChunk(), Face3::Left);
                        }
                    }
                    if constexpr (((Faces == Face3c::Front) || ...)) {
                        if (otherChunks[Face3::Front]) {
                            chunkMeshOtherChunkFace(isPush, chunk, *otherChunks[Face3::Front], cbpos.getYpNearChunk(), Face3::Front);
                        }
                    }
                    if constexpr (((Faces == Face3c::Back) || ...)) {
                        if (otherChunks[Face3::Back]) {
                            chunkMeshOtherChunkFace(isPush, chunk, *otherChunks[Face3::Back], cbpos.getYnNearChunk(), Face3::Back);
                        }
                    }
                    if constexpr (((Faces == Face3c::Top) || ...)) {
                        if (otherChunks[Face3::Top]) {
                            chunkMeshOtherChunkFace(isPush, chunk, *otherChunks[Face3::Top], cbpos.getZpNearChunk(), Face3::Top);
                        }
                    }
                    if constexpr (((Faces == Face3c::Bottom) || ...)) {
                        if (otherChunks[Face3::Bottom]) {
                            chunkMeshOtherChunkFace(isPush, chunk, *otherChunks[Face3::Bottom], cbpos.getZnNearChunk(), Face3::Bottom);
                        }
                    }
                }
            }

            template<bool isPush, Face3... Faces>
            void chunkMeshEachBlockX(block::Chunk& chunk, block::Chunk** otherChunks, block::CBPos cbpos) {
                chunkMeshBlock<isPush, Face3c::Left, Faces...>(chunk, otherChunks, cbpos);
                cbpos += block::cbpos_x_unit;
                for (ushort x = 1; x < block::chunk_side_max; x++) {
                    chunkMeshBlock<isPush, Faces...>(chunk, otherChunks, cbpos);
                    cbpos += block::cbpos_x_unit;
                }
                chunkMeshBlock<isPush, Face3c::Right, Faces...>(chunk, otherChunks, cbpos);
            }
            template<bool isPush, Face3... Faces>
            void chunkMeshEachBlockY(block::Chunk& chunk, block::Chunk** otherChunks, block::CBPos cbpos) {
                chunkMeshEachBlockX<isPush, Face3c::Back, Faces...>(chunk, otherChunks, cbpos);
                cbpos += block::cbpos_y_unit;
                for (ushort y = 1; y < block::chunk_side_max; y++) {
                    chunkMeshEachBlockX<isPush, Faces...>(chunk, otherChunks, cbpos);
                    cbpos += block::cbpos_y_unit;
                }
                chunkMeshEachBlockX<isPush, Face3c::Front, Faces...>(chunk, otherChunks, cbpos);
            }
            template<bool isPush>
            void chunkMeshEachBlockZ(block::Chunk& chunk, block::Chunk** otherChunks, block::CBPos cbpos) {
                chunkMeshEachBlockY<isPush, Face3c::Bottom>(chunk, otherChunks, cbpos);
                cbpos += block::cbpos_z_unit;
                for (ushort z = 1; z < block::chunk_side_max; z++) {
                    chunkMeshEachBlockY<isPush>(chunk, otherChunks, cbpos);
                    cbpos += block::cbpos_z_unit;
                }
                chunkMeshEachBlockY<isPush, Face3c::Top>(chunk, otherChunks, cbpos);
            }

        public:
            //传入方块的数据以及在区块中的位置以添加进入mesh，并将顶点索引更新到block的graph，如果剩余容量不够会对buffer进行扩容，这会改变顶点指针位置
            void add(const block::ChunkMap& chunks, const block::Chunk& chunk, const block::CBPos pos, block::Block& block, const block::BlockProto& proto);
            //直接更新顶点中除了位置的数据
            void update(const block::Chunk& chunk, const block::CBPos pos, const block::Block& block, const block::BlockProto& proto);
            //移除方块对应的顶点
            void remove(const block::ChunkMap& chunks, const block::Chunk& chunk, const block::CBPos pos, block::Block& block, const block::BlockProto& proto);

            //批量加载整个区块的block的顶点
            void loadChunkMesh(block::Chunk& chunk);
            //卸载区块信息，并释放顶点所占空间
            void unloadChunkMesh(block::Chunk& chunk);

            //绘制
            void draw();

            CubeMesh();
            CubeMesh(const block::ChunkMap& chunks);
            ~CubeMesh();
        };

    }
}