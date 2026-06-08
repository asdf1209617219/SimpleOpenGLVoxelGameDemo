#pragma once
#include <dme/graph/texture/tex_atlas.h>

namespace dme {
	namespace event {
		class ItemUseData;
	}
	namespace graph {
		class ShaderBase;
	}
	namespace ui {
		class ItemIcon;
	}
	namespace item {
		class AllItemProto;
		class BlockItemProto;
		class ItemStack;

		enum class ItemFunc {
			none = 0,
			f1 = 1, //left click
			f2 = 2, //right click
			f3, f4, f5, f6, f7, f8, f9, f10,
			f11, f12, f13, f14, f15, f16, f17, f18, f19, f20,
		};

		class ItemProto {
		private:
			friend AllItemProto;
			friend MultiSizeArr;
			static void setId(ItemProto* proto, uid id) noexcept;
		protected:
			uid id;
			//每摞最大数量
			uint stackMaxCount;
			//纹理在纹理图集里的下标
			graph::TexIndex texIndex;
			//是否必须实例化，比如有耐久的物品
			bool isInstance;
			strid name;

			//由子类实现纹理的加载
			ItemProto(uid id, strid name, uint stackMaxCount, bool isInstance) noexcept;
		public:
			//单个格子默认最大物品数量
			constexpr static uint defaultStackMaxCount = 100;

			//空构造器专门用于None
			ItemProto() noexcept;
			//需传入物品的纹理
			ItemProto(uid id, strid name, uint stackMaxCount, bool isInstance, strid texPath) noexcept;
			//空手使用物品时为None
			bool isNone() const noexcept;
			//空手使用物品时为None
			bool notNone() const noexcept;
			uid getId() const noexcept;
			strid getName() const noexcept;
			uint getStackMaxCount() const noexcept;
			void setStackMaxCount(uint stackMaxCount) noexcept;
			bool getIsInstance() const noexcept;
			void setIsInstance(bool isInstance) noexcept;

			//纹理在纹理图集里的下标
			graph::TexIndex getTexIndex() const noexcept;

			//当使用功能1时，尝试执行挖掘方块
			void digBlock(const event::ItemUseData& data) const;

			//使用物品
			virtual void use(const event::ItemUseData& data) const;
			//创建物品图标
			virtual ui::ItemIcon& createIcon(const ItemStack& itemStack) const;

			bool operator ==(const ItemProto& a) const noexcept;
		};

	}
}