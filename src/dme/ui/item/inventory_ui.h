#pragma once
#include <dme/ui/ui.h>

namespace dme {
	namespace item {
		class ItemStack;
		class Inventory;
	}
	namespace ui {
		class ItemIcon;

		class InventoryUI : public UI, private ObjPool<InventoryUI> {
			friend class ObjPool<InventoryUI>;
		public:
			//默认宽度为10个物品
			inline constexpr static uint default_col = 10;
			//物品图标默认大小
			inline constexpr static uint item_icon_size = 64;
		private:
			static IntrusiveListNode<InventoryUI>& _get_list_node(InventoryUI& obj) noexcept;
			using list_type = IntrusiveList<InventoryUI, _get_list_node>;

			item::Inventory* inventory;
			IntrusiveListNode<InventoryUI> listNode;
			CArr<ItemIcon*> icons;
			//物品列行数
			Uint2 colRow;
			//物品边框的纹理（要求纹理宽高必须相等）
			graph::TexIndex itemFrameTex;
			//边框纹理的整体宽度
			ushort itemFrameTexSize;
			//边框纹理的边框宽度
			ushort itemFrameTexBorder;

			virtual void removeImpl() noexcept override;
		protected:
			item::Inventory* getInventory() const noexcept;
			graph::TexIndex getItemFrameTex() const noexcept;

			ItemIcon& createAndSetItemIcon(uint index, const item::ItemStack& stack) noexcept;

			InventoryUI(uint meshType, item::Inventory& inventory, ushort itemFrameTexBorder, graph::TexIndex itemFrameTex, uint row, uint col = default_col) noexcept;
		public:
			//辅助函数，将index转换为列下标行下标（从0开始算），参数2为UI的列数
			static Uint2 IndexToColRowIndex(uint index, uint colCount) noexcept;
			static InventoryUI& Create(item::Inventory& inventory, ushort itemFrameTexBorder, graph::TexIndex itemFrameTex, uint row, uint col = default_col) noexcept;

			InventoryUI* getNextInventoryUI() const noexcept;

			//index从0计算
			ItemIcon* getIcon(uint index) const noexcept;
			//index从0计算
			ItemIcon* getIcon(Uint2 colIndexRowIndex) const noexcept;
			//index从0计算
			item::ItemStack& getItemStack(uint index) const noexcept;
			//index从0计算
			item::ItemStack& getItemStack(Uint2 colIndexRowIndex) const noexcept;

			Uint2 getInventoryUIColRow() const noexcept;
			uint getInventoryUISize() const noexcept;
			//返回行数除以列数，如果列数为0则返回0
			float getRowColRatio() const noexcept;
			Float2 getItemFrameSize() const noexcept;
			ushort getItemFrameTexSize() const noexcept;
			ushort getItemFrameTexBorder() const noexcept;

			//由item::Inventory调用
			void updateItemStack(uint index);
			void updateAllItemStack();

			//将图标放置到InventoryUI的哪个位置中，如果返回uid::Null，则不设置图标
			virtual uid invertoryIndexToInInvertoryUIIndex(uint indexInInventory);
			//创建图标后对图标进行设置
			virtual void settingItemIcon(ItemIcon& icon);

			virtual void click(const UIEvent& e) override;
			virtual void hover(const UIEvent& e) override;
			virtual void editMesh(graph::MeshEditorBase& meshEditor, const bool isAdd) noexcept override;
		};
	}
}
