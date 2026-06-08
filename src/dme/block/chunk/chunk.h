#pragma once
#include <dme/block/block.h>
#include <dme/block/block_data.h>
#include <dme/graph/mesh/cube_mesh.h>
#include <dme/graph/mesh/plant_mesh.h>
namespace dme {
	class Game;
	namespace world {
		class World;
	}
	namespace entity {
		class Entity;
	}
	namespace block {
		class BPos;

		class Chunk : ObjPool<Chunk> {
		private:
			friend Block;
			friend BPos;
			friend graph::CubeMesh;
			friend graph::PlantMesh;
			friend world::World;
			friend Game;
			friend ObjPool<Chunk>;

			union {
				struct {
					Int3 pos; //区块坐标
					sid worldId; //世界ID
				};
				Int4 posW; //含世界ID的坐标
			};
			world::World* world;
			Block blocks[chunk_volume];

			void setPosW(const Int4& posW) noexcept;
			void setWorld(world::World& world) noexcept;
			//添加方块渲染用的数据
			void addBlockRenderData(CBPos cbpos, Block& block, const BlockProto& proto);
			//更新方块渲染用的数据
			void updateBlockRenderData(CBPos cbpos, const Block& block, const BlockProto& proto) const;
			//移除方块渲染用的数据
			void removeBlockRenderData(CBPos cbpos, Block& block, const BlockProto& proto);

			Chunk() noexcept;
			Chunk(const Int4& id) noexcept;
			Chunk(const Int3& pos) noexcept;
			Chunk(const Int3& pos, sid worldId) noexcept;

			//用于world异步加载区块
			static Chunk* Alloc() noexcept {
				return ObjPool<Chunk>::Alloc();
			}
			static void Remove(Chunk* obj) noexcept {
				//obj->unloadChunkMesh(); // TODO 目前该函数存在问题
				return ObjPool<Chunk>::Remove(obj);
			}
		public:
			static Chunk* Create() noexcept {
				return ObjPool<Chunk>::Add();
			}
			static Chunk* Create(const Int4& id) noexcept {
				return ObjPool<Chunk>::Add(id);
			}
			static Chunk* Create(const Int3& pos) noexcept {
				return ObjPool<Chunk>::Add(pos);
			}
			static Chunk* Create(const Int3& pos, sid worldId) noexcept {
				return ObjPool<Chunk>::Add(pos, worldId);
			}

			sid getWorldId() const noexcept;
			world::World* getWorld() const noexcept;
			Int3 getPos() const noexcept;
			Int4 getPosW() const noexcept;
			Int3 getBlockPos(const Block& block) const noexcept;

			//获取当前区块面朝向的区块（通过world的getChunk方法获取，如果区块未加载则返回null）
			Chunk* nearChunk(Face3 face) const;

			//获取方块数组开头（如果修改方块数据，需自行处理物理、渲染等其他逻辑）
			Block* getBlocks() noexcept;
			//获取方块数组开头（如果修改方块数据，需自行处理物理、渲染等其他逻辑）
			const Block* getBlocks() const noexcept;
			//是否存在方块
			bool existBlock(CBPos cbpos) const noexcept;
			//获取方块
			Block getBlock(CBPos cbpos) const noexcept;
			//获取方块指针（如果修改方块数据，需自行处理物理、渲染等其他逻辑）
			Block* getBlockPtr(CBPos cbpos) noexcept;
			//获取方块指针（如果修改方块数据，需自行处理物理、渲染等其他逻辑）
			const Block* getBlockPtr(CBPos cbpos) const noexcept;
			//引用方块（如果修改方块数据，需自行处理物理、渲染等其他逻辑）
			Block& refBlock(CBPos cbpos) noexcept;
			//引用方块（如果修改方块数据，需自行处理物理、渲染等其他逻辑）
			const Block& refBlock(CBPos cbpos) const noexcept;
			//设置方块（直接修改方块数据，需自行处理物理、渲染等其他逻辑）
			void setBlock(CBPos cbpos, Block block) noexcept;
			//移除方块（直接修改方块数据，需自行处理物理、渲染等其他逻辑）
			void removeBlock(CBPos cbpos) noexcept;

			//放置方块，放置成功则返回true
			//bool place(CBPos cbpos, Block block);

			//通过BlockProto的place方法放置方块，放置成功则返回true（外部应该调用此方法，以触发物理和渲染的更新）
			bool place(CBPos cbpos, entity::Entity& entity, const BlockProto& proto);
			//破坏方块，破坏成功则返回true（外部应该调用此方法，以触发物理和渲染的更新）
			bool destroy(CBPos cbpos, entity::Entity& entity);
			//更换方块，更换成功则返回true（如果原本位置没有方块则不会有动作并返回false）
			//bool replace(CBPos cbpos, Block block);
			

			//加载方块数据以及渲染用的数据（需自行处理物理等其他逻辑）
			void loadBlock(CBPos cbpos, Block block);
			//卸载方块数据以及渲染用的数据（需自行处理物理等其他逻辑）
			void unloadBlock(CBPos cbpos);

			//加载整个区块的渲染数据到mesh中（用于世界生成器生成完方块数据后，生成mesh数据）
			void loadChunkMesh();
			//卸载整个区块的渲染数据
			void unloadChunkMesh();

			static void DrawAll();
			static graph::CubeMesh& CubeMesh();
			static graph::PlantMesh& PlantMesh();
			static constexpr size_t obj_pool_size = 512;

			friend constexpr Hash Hasher(const Chunk& a) noexcept {
				return Hasher(a.getPosW());
			}
		};

	}
}

namespace std {
	template<>
	struct hash<dme::block::Chunk> {
		constexpr size_t operator ()(const dme::block::Chunk& a) const noexcept {
			return Hasher(a);
		}
	};
}