#pragma once
#include <dme/block/chunk/cbpos.h>
#include <dme/block/block_shape.h>
#include <dme/block/block_data.h>
namespace dme {
	namespace world {
		class World;
	}

	namespace block {
		class BlockProto;

		//区块中保存的方块
		class Block {
			friend Chunk;
			friend world::World;
		private:
			static constexpr BlockBase _cube_base_ = BlockBase(BlockShape::Cube);
			//static constexpr uint _is_block_mask_ = int_high_bit_mask >> 1;
			//static constexpr uint _id_mask_ = ~(_is_block_mask_ | int_high_bit_mask);

			static constexpr uint _is_block_mask_ = int_high_bit_mask;
			static constexpr uint _id_mask_ = ~(_is_block_mask_ | int_high_bit_mask);

			//方块原型ID，为uid::Null时表示方块为空，最高位为1时表示方块的第二个字段为block、为0则表示base
			uid id;
			union {
				//id最高位为1时，表示指向方块数据，可以指向uid::Null
				uid block;
				//id最高位为0时，表示用block这个字段保存方块的形状和基础数据，最高8位为形状，其他位为基础数据
				BlockBase base;
			};
			//指向方块渲染数据，为null时表示方块不渲染
			uid graph;
			//指向方块物理数据，为null时表示方块无碰撞
			uid phys;
		protected:
		public:
			constexpr Block() noexcept : id(), block(), graph(), phys() {}
			constexpr Block(uid id, uid block) noexcept : id(id& _id_mask_), block(block), graph(), phys() {}
			constexpr Block(uid id, uid block, uid graph, uid phys) noexcept : id(id& _id_mask_), block(block), graph(graph), phys(phys) {}

			//explicit constexpr Block(uid id) noexcept : id(id & _id_mask_), block(), graph(), phys() {}

			constexpr Block(uid id, BlockShape shape) noexcept : id(id& _id_mask_), base(shape), graph(), phys() {}
			constexpr Block(uid id, BlockBase base) noexcept : id(id& _id_mask_), base(base), graph(), phys() {}

			constexpr Block(const Block& a) noexcept = default;
			constexpr Block(Block&& a) noexcept = default;
			constexpr Block& operator =(const Block& a) noexcept = default;
			constexpr Block& operator =(Block&& a) noexcept = default;

			//将属性置为默认
			inline constexpr void deleteData() noexcept {
				*this = Block();
			}
			//设置为空（id为-1时方块为空）
			inline constexpr void setNull() noexcept {
				id.setNull();
			}
			//是否为空（id为-1时方块为空）
			inline constexpr bool isNull() const noexcept {
				return id.isNull();
			}
			//是否为空（id为-1时方块为空）
			inline constexpr bool notNull() const noexcept {
				return id.notNull();
			}
			inline constexpr uid getId() const noexcept {
				return id & _id_mask_;
			}

			//id最高位是1
			inline constexpr bool isBlock() const noexcept {
				return id & _is_block_mask_;
			}
			//id最高位是0
			inline constexpr bool isBase() const noexcept {
				return !(id & _is_block_mask_);
			}
			//isBlock时才有效
			inline constexpr uid getBlock() const noexcept {
				//DME_ASSERT(isBlock());
				return block.getHighBitZero();
			}
			inline constexpr void setBlock(uid block_) noexcept {
				block = block_.getHighBitOne();
			}
			inline constexpr void setBlockNull() noexcept {
				block.setNull();
			}

			//isBase时才有效
			inline constexpr BlockBase getBase() const noexcept {
				//DME_ASSERT(isBase());
				return base;
			}
			//isBase时才有效
			inline constexpr BlockBase& refBase() noexcept {
				//DME_ASSERT(isBase());
				return base;
			}
			//isBase时才有效
			inline constexpr const BlockBase& refBase() const noexcept {
				//DME_ASSERT(isBase());
				return base;
			}
			inline constexpr void setBase(BlockBase base_) noexcept {
				base = base_;
			}
			//isBase时才有效
			inline constexpr BlockShape getShape() const noexcept {
				//DME_ASSERT(isBase());
				return base.getShape();
			}
			//isBase时才有效
			inline constexpr void setShape(BlockShape shape) noexcept {
				//DME_ASSERT(isBase());
				base.setShape(shape);
			}

			inline constexpr uid getGraph() const noexcept {
				return graph;
			}
			inline constexpr uid& refGraph() noexcept {
				return graph;
			}
			inline constexpr const uid& refGraph() const noexcept {
				return graph;
			}
			inline constexpr void setGraph(uid graph_) noexcept {
				graph = graph_;
			}
			inline constexpr void setGraphNull() noexcept {
				graph.setNull();
			}

			inline constexpr uid getPhys() const noexcept {
				return phys;
			}
			inline constexpr uid& refPhys() noexcept {
				return phys;
			}
			inline constexpr const uid& refPhys() const noexcept {
				return phys;
			}
			inline constexpr void setPhys(uid phys_) noexcept {
				phys = phys_;
			}
			inline constexpr void setPhysNull() noexcept {
				phys.setNull();
			}

			BlockProto* getProto() const;
			inline constexpr static Block createCube(uid id) noexcept {
				return Block(id, _cube_base_);
			}

			constexpr bool operator ==(const Block& a) const noexcept {
				return id == a.id && block == a.block && graph == a.graph && phys == a.phys;
			}
		};

	}
}

namespace std {
	template<>
	struct hash<dme::block::Block> {
		constexpr size_t operator ()(const dme::block::Block& a) const noexcept {
			return dme::Hasher(a);
		}
	};
}