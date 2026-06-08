#pragma once
#include <dme/block/chunk/chunk.h>
namespace dme::world {

	class World;
	//世界生成器接口，最小生成单位为区块
	class WorldGenerator {
	protected:
		block::Chunk* getOrGenChunk(World& world, const Int3& chunkPos);
	public:
		//获取世界出生点坐标
		virtual Int3 getSpawnPoint(World& world) = 0;
		//在该函数内生成chunk的各种方块数据
		virtual void editChunk(block::Chunk& chunk, const Int3& chunkPos, const World& world) = 0;

		//仅用于测试
		virtual void test(const Int3& pos) {

		}
	};

	//默认的世界生成器
	class StartWorldGen : public WorldGenerator {
	private:
		using HeightData = std::array<sint, block::chunk_area>;
		using HeightMap = FlatKeyMap<Int2, HeightData>;
		HeightData& getOrGenHeightData(const Int3& chunkPos, const Int3& chunkStartPos);
	protected:
		math::PerlinNoise perlin;
		HeightMap heightMap;
	public:
		StartWorldGen(math::Seed& seed);

		virtual Int3 getSpawnPoint(World& world) override;
		virtual void editChunk(block::Chunk& chunk, const Int3& chunkPos, const World& world) override;
	};


	//默认的世界生成器2
	class StartWorldGen2 : public WorldGenerator {
	protected:
		using DepthData = std::array<uchar, block::chunk_volume>;
		using DepthMap = FlatKeyMap<Int3, DepthData>;

		math::PerlinNoise perlin;
		bool hasSpawnPoint = false;
		Int3 spawnPoint;
		DepthMap depthMap;

		DepthData* getOrGenDepthData(const Int3& chunkPos) noexcept;
	public:
		StartWorldGen2(math::Seed& seed);

		virtual Int3 getSpawnPoint(World& world) override;
		virtual void editChunk(block::Chunk& chunk, const Int3& chunkPos, const World& world) override;
		virtual void test(const Int3& pos) override;
	};

}