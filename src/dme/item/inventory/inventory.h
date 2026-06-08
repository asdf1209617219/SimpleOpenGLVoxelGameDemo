#pragma once
#include <dme/item/item_stack.h>
namespace dme {
	namespace ui {
		class InventoryUI;
	}
	namespace item {
		class Inventory {
			friend ui::InventoryUI;
		private:
		protected:
			ui::InventoryUI* inventoryUI; //由ui来设置该变量
			ItemStack* arr;
			uint size;

			void updateInventoryUI(uint index) const;
		public:
			Inventory(uint size) noexcept;

			void setSize(uint size) noexcept;
			uint getSize() const noexcept;
			//添加物品，自动查找空闲格子，优先和没装满的格子合并
			uint addItem(const ItemStack& item) noexcept;
			//减少物品，自动查找拥有该物品的格子
			uint subItem(const ItemStack& item) noexcept;
			//与传入的物品互相交换
			void swapItem(uint index, ItemStack& item) noexcept;

			//获取物品，如果超出范围则返回null
			ItemStack* getItem(uint a) noexcept;
			//获取物品，如果超出范围则返回null
			const ItemStack* getItem(uint a) const noexcept;

			//获取物品，如果超出范围则返回最后一个物品
			ItemStack& operator[](uint a) noexcept;
			//获取物品，如果超出范围则返回最后一个物品
			const ItemStack& operator[](uint a) const noexcept;
		};
	}
}