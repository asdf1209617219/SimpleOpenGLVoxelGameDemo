#pragma once
#include <dme/block/bpos.h>
#include <dme/block/chunk/cbpos.h>
#include <dme/block/chunk/chunk.h>
#include <dme/block/cube_face.h>
#include <dme/graph/texture/tex2d_array.h>
#include <dme/graph/texture/tex_atlas.h>
namespace dme{
	namespace entity {
		class Entity;
	}
	namespace item {
		class ItemStack;
	}
	namespace ui {
		class ItemIcon;
	}

	namespace block {
		class AllBlockProto;
		//class Block;
		//class BPos;
		class Chunk;

		//TODO 目前未实现非固体方块逻辑
		enum class BlockType {
			Solid,
			Liquid,
		};

		//决定碰撞后如何处理
		enum class BlockCollisionType {
			None,
			Solid,
			Liquid,
		};

		//决定通过什么shader渲染以及数据的组织方式
		enum class BlockRenderType {
			None,
			Cube,
			Plant,
		};

		//方块原型，保存方块的各种属性信息
		class BlockProto {
		private:
			friend AllBlockProto;
			friend MultiSizeArr;
			static void setId(BlockProto* proto, uid id) noexcept;
		protected:
			uid id;
			//方块类别
			BlockType type;
			//渲染方式
			BlockRenderType renderType;
			//碰撞类型
			BlockCollisionType collisionType;
			//名称
			strid name;
			//用于保存各种标识
			uint flags;

			//纹理在纹理数组里的下标
			graph::TexIndex texArrIndex[Face3::Len];

			//纹理在纹理图集里的下标
			graph::TexIndex texIndex[Face3::Len];

			//放置方块，直接对区块里的方块进行修改，参数block一定为空，且放置后block一定不为空
			virtual void placeBlock(Chunk& chunk, Block& block, entity::Entity& entity) const;
			//破坏方块，直接对区块里的方块进行修改，参数block一定 不 为空，函数结束后chunk会自动把block置为空
			virtual void destroyBlock(Chunk& chunk, Block& block, entity::Entity& entity) const;
			//使用方块，参数block一定 不 为空
			virtual void useBlock(Chunk& chunk, Block& block, entity::Entity& entity) const;
		public:
			//是否透明（如植物类的方块）
			static constexpr uint flags_transparent = 1;

			BlockProto(uid id, strid name, strid tex,
				uint flags = 0, BlockRenderType renderType = BlockRenderType::Cube, BlockCollisionType collisionType = BlockCollisionType::Solid);
		
			BlockProto(uid id, strid name, strid texSide, strid texTopBottom,
				uint flags = 0, BlockRenderType renderType = BlockRenderType::Cube, BlockCollisionType collisionType = BlockCollisionType::Solid);

			BlockProto(uid id, strid name, strid texSide, strid texTop, strid texBottom,
				uint flags = 0, BlockRenderType renderType = BlockRenderType::Cube, BlockCollisionType collisionType = BlockCollisionType::Solid);

			BlockProto(uid id, strid name, strid texRight, strid texLeft, strid texFront, strid texBack, strid texTop, strid texBottom,
				uint flags = 0, BlockRenderType renderType = BlockRenderType::Cube, BlockCollisionType collisionType = BlockCollisionType::Solid);

			uid getId() const noexcept;
			BlockType getType() const noexcept;
			BlockCollisionType getCollisionType() const noexcept;
			BlockRenderType getRenderType() const noexcept;
			bool getTransparent() const noexcept;
			strid getName() const noexcept;
			//纹理在纹理数组里的下标
			graph::TexIndex getTexArrIndex(Face3 face = Face3c::Min) const noexcept;
			//纹理在纹理图集里的下标
			graph::TexIndex getTexIndex(Face3 face = Face3c::Min) const noexcept;

			//放置方块，该方法供外部调用
			bool place(const BPos& pos, entity::Entity& entity) const;
			//放置方块，该方法供外部调用
			bool place(Chunk& chunk, Block& block, entity::Entity& entity) const;
			//破坏方块，该方法供外部调用
			bool destroy(const BPos& pos, entity::Entity& entity) const;
			//破坏方块，该方法供外部调用
			bool destroy(Chunk& chunk, Block& block, entity::Entity& entity) const;
			//对方块使用互动键，该方法供外部调用
			bool use(const BPos& pos, entity::Entity& entity) const;
			//对方块使用互动键，该方法供外部调用
			bool use(Chunk& chunk, Block& block, entity::Entity& entity) const;

			//创建方块对应的物品图标（由item::BlockItemProto调用）
			virtual ui::ItemIcon& createIcon(const item::ItemStack& itemStack) const;

			bool operator ==(const BlockProto& a) const noexcept;
			friend constexpr Hash Hasher(const BlockProto& a) noexcept {
				return Hasher(a.id);
			}
		};
	}
}