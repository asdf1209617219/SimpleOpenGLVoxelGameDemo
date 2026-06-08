#pragma once
#include <dme/block/chunk/chunk.h>
#include <dme/util/bvh.h>
#include <dme/engine/world_time.h>
namespace dme {
	class Game;
	namespace entity {
		class Entity;
	}
	namespace event {
		class BlockPlaceData;
		class BlockDestroyData;
	}
	namespace world {

		class WorldGenerator;
		class World : private ObjPool<World> {
			friend ObjPool<World>;
		private:
			friend Game;
			friend entity::Entity; //仅用于entityPool中管理entity指针
			//friend block::Chunk; //仅用于chunk删除方块额外数据用
			friend WorldGenerator;
			friend block::BlockProto;

			sid id;
			Int3 spawnPoint; //出生点
			strid name; //世界名称
			block::ChunkMap chunkMap; //区块信息
			util::BVH3f bvh;
			eng::WorldTime time;
			math::Seed* seed; //考虑换成shared_ptr
			WorldGenerator* generator; //考虑换成shared_ptr
			Hive<entity::Entity*> entityPool;

			Hive<entity::Entity*>::comp_ptr addEntity(entity::Entity* ptr) noexcept;
			void removeEntity(Hive<entity::Entity*>::comp_ptr ptr) noexcept;

			void update();
			//同步加载区块
			block::Chunk* getOrGenChunk(const Int3& chunkPos);
			//异步加载区块
			block::Chunk* getOrGenChunkAsync(const Int3& chunkPos);

			static void deleteAllWorld();
			static void _addToAllWorld(World* world);
			static FlatMap<sid, World*>& _allWorld();

			World(sid id, strid name, math::Seed& seed, WorldGenerator& worldGenerator) noexcept;
		public:

			sid getId() const noexcept;
			Int3 getSpawnPoint();
			strid getName() const noexcept;
			const block::ChunkMap& getChunkMap() const noexcept;
			math::Seed& getSeed() const noexcept;

			util::BVH3f& getBVH() noexcept;
			const util::BVH3f& getBVH() const noexcept;
			eng::WorldTime getWorldTime() const noexcept;
			//根据世界时间获取阳光的方向（已归一化）
			Float3 getLightDirection() const noexcept;

			//未加载区块则返回null，不会加载区块
			block::Chunk* getChunk(const Int3& chunkPos) const;
			//同步加载区块，调用时未加载区块则同步加载后返回
			block::Chunk* loadChunk(const Int3& chunkPos);
			//异步加载区块，调用时未加载区块则返回null
			block::Chunk* loadChunkAsync(const Int3& chunkPos);
			//卸载区块，如果有区块并成功移除则返回true
			bool unloadChunk(const Int3& chunkPos);

			//放置方块，未加载区块则放置失败并返回false，并异步加载区块
			bool placeBlock(const Int3& pos, entity::Entity& entity, const block::BlockProto& blockProto);
			//破坏方块，未加载区块则破坏失败并返回false，并异步加载区块
			bool destroyBlock(const Int3& pos, entity::Entity& entity);
			//未加载区块则返回null，并异步加载区块
			block::Block* getBlock(const Int3& pos);

			//仅用于测试
			void test(const Int3& pos);

			//将直接在对象池构造（自动分配世界ID）
			static World* CreateAutoId(strid name, math::Seed& seed, WorldGenerator& worldGenerator);
			//将直接在对象池构造（如果世界ID重复则返回null并添加失败）
			static World* Create(sid worldId, strid name, math::Seed& seed, WorldGenerator& worldGenerator);
			//根据ID获取世界（没这个ID则返回null）
			static World* GetWorld(sid worldId);
			//更新所有世界
			static void UpdateAll();

			//起始世界
			static World& StartWorld();

			static constexpr size_t obj_pool_size = 16;
		};

	}
}