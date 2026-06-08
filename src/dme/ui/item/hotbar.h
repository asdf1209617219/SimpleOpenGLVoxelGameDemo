#pragma once
#include <dme/ui/item/inventory_ui.h>
namespace dme::ui {

	class Hotbar : public InventoryUI, private ObjPool<Hotbar> {
		friend ObjPool<Hotbar>;
	private:
		static graph::TexIndex backgroundTex; //背景图片
		static graph::TexIndex activeTex; //活动背景图片

		sint activeIndex; //当前活动下标（0为第一个）

		virtual void removeImpl() noexcept override;
		Hotbar(uint meshType, item::Inventory& inventory, ushort itemFrameTexBorder, graph::TexIndex itemFrameTex, uint row, uint col = default_col) noexcept;
	public:
		//默认快捷栏
		static Hotbar& Default() noexcept;

		static Hotbar& Create(item::Inventory& inventory, uint row, uint col = default_col) noexcept;

		//快捷栏最小下标
		sint getMinIndex() const noexcept;
		//快捷栏最大下标
		sint getMaxIndex() const noexcept;
		//当前选中快捷栏下标
		sint getActiveIndex() const noexcept;
		//直接设置当前选中快捷栏
		sint setActiveIndex(sint activeIndex) noexcept;
		//滚轮当前选中快捷栏
		sint scollActiveIndex(bool isLeft) noexcept;

		virtual uid invertoryIndexToInInvertoryUIIndex(uint indexInInventory) override;
		virtual void settingItemIcon(ItemIcon& icon) override;
		virtual void click(const UIEvent& e) override;
		virtual void hover(const UIEvent& e) override;
		virtual void editMesh(graph::MeshEditorBase& meshEditor, const bool isAdd) noexcept override;
	};

}