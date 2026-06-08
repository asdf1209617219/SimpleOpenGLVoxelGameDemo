#include <dme/ui/item/inventory_ui.h>
#include <dme/ui/item/item_icon.h>
#include <dme/item/inventory/inventory.h>
namespace dme::ui {
	IntrusiveListNode<InventoryUI>& InventoryUI::_get_list_node(InventoryUI& obj) noexcept {
		return obj.listNode;
	}

	InventoryUI* InventoryUI::getNextInventoryUI() const noexcept {
		return listNode.next;
	}
	void InventoryUI::removeImpl() noexcept {
		if (!listNode.prev) {
			inventory->inventoryUI = listNode.next;
		}
		InventoryUI::list_type::Remove(*this);
		ObjPool<InventoryUI>::Remove(this);
	}

	InventoryUI::InventoryUI(uint meshType, item::Inventory& inventory_, ushort itemFrameTexBorder, graph::TexIndex itemFrameTex, uint row, uint col) noexcept
		: UI(meshType), inventory(&inventory_), itemFrameTex(itemFrameTex), itemFrameTexSize(item_icon_size + itemFrameTexBorder * 2), itemFrameTexBorder(itemFrameTexBorder), icons(), colRow() {
		if (inventory_.inventoryUI) {
			InventoryUI::list_type::InsertPrev(*inventory_.inventoryUI, *this);
		}
		inventory_.inventoryUI = this;
		if (col == 0 || row == 0) {
			return;
		}
		colRow.x = col;
		colRow.y = row;
		icons.newArr(col * row);
	}


	Uint2 InventoryUI::IndexToColRowIndex(uint index, uint colCount) noexcept {
		return Uint2(index % colCount, index / colCount);
	}
	InventoryUI& InventoryUI::Create(item::Inventory& inventory, ushort border, graph::TexIndex itemFrameTex, uint row, uint col) noexcept {
		auto& r = *ObjPool<InventoryUI>::Add(UI::GetMeshType(), inventory, border, itemFrameTex, col, row);
			r.setPivotAndAnchorsOnCenter()
			.setWidthHeight(UIpx(tof(r.itemFrameTexSize * col)), UIpx(tof(r.itemFrameTexSize * row)));
			;
		return r;
	}


	ItemIcon* InventoryUI::getIcon(uint index) const noexcept {
		return icons[index];
	}
	ItemIcon* InventoryUI::getIcon(Uint2 colIndexRowIndex) const noexcept {
		return icons[colIndexRowIndex.y * colRow.x + colIndexRowIndex.x];
	}
	item::ItemStack& InventoryUI::getItemStack(uint index) const noexcept {
		return (*inventory)[index];
	}
	item::ItemStack& InventoryUI::getItemStack(Uint2 colIndexRowIndex) const noexcept {
		return (*inventory)[colIndexRowIndex.y * colRow.x + colIndexRowIndex.x];
	}

	Uint2 InventoryUI::getInventoryUIColRow() const noexcept {
		return colRow;
	}
	uint InventoryUI::getInventoryUISize() const noexcept {
		return colRow.x * colRow.y;
	}
	float InventoryUI::getRowColRatio() const noexcept {
		if (colRow.x == 0) return 0;
		return tof(colRow.y) / tof(colRow.x);
	}
	item::Inventory* InventoryUI::getInventory() const noexcept {
		return inventory;
	}
	graph::TexIndex InventoryUI::getItemFrameTex() const noexcept {
		return itemFrameTex;
	}
	Float2 InventoryUI::getItemFrameSize() const noexcept {
		if (colRow.x == 0) return Float2();
		return getBox().sideLength() / colRow;
	}
	ushort InventoryUI::getItemFrameTexSize() const noexcept {
		return itemFrameTexSize;
	}
	ushort InventoryUI::getItemFrameTexBorder() const noexcept {
		return itemFrameTexBorder;
	}

	ItemIcon& InventoryUI::createAndSetItemIcon(uint index, const item::ItemStack& stack) noexcept {
		float size = tof(itemFrameTexSize - itemFrameTexBorder * 2);
		float sizeHalf = size / 2;
		float frameHalf = tof(itemFrameTexSize) / 2;
		auto cri = IndexToColRowIndex(index, colRow.x);
		float ratioX = 1.0f / colRow.x;
		float ratioY = 1.0f / colRow.y;

		auto& r = stack.getProto().createIcon(stack);
		r.setZIndex(getZIndex() + 1)
			.setParent(*this)
			.setWidthHeight(UIpx(size), UIpx(size))
			.setPivot(0.5_rt, 0.5_rt)
			.setAnchors(UIrt((cri.x + 0.5f) * ratioX), UIrt((colRow.y - cri.y - 0.5f) * ratioY));
		icons[index] = &r;
		settingItemIcon(r);
		return r;
	}
	void InventoryUI::updateItemStack(uint index) {
		if (!icons.empty()) {
			uid i = invertoryIndexToInInvertoryUIIndex(index);
			if (i.notNull() && i < getInventoryUISize()) {
				ItemIcon* oldIcon = icons[i];
				item::ItemStack* stack = inventory->getItem(index);
				if (oldIcon) {
					if (!stack || stack->isEmpty()) {
						oldIcon->removeThis();
						icons[i] = null;
					}
					else {
						if (oldIcon->getItemStack().getProtoId() != stack->getProtoId()) { //当前格子物品改变
							oldIcon->removeThis();
							createAndSetItemIcon(i, *stack);
						}
						else {
							// TODO 更新物品数量或状态
						}
					}
				}
				else {
					if (stack && !stack->isEmpty()) {
						createAndSetItemIcon(i, *stack);
					}
				}
			}
		}
	}
	void InventoryUI::updateAllItemStack() {
		for (uint i = 0; i < icons.count(); ++i) {
			updateItemStack(i);
		}
	}
	uid InventoryUI::invertoryIndexToInInvertoryUIIndex(uint indexInInventory) {
		return indexInInventory;
	}
	void InventoryUI::settingItemIcon(ItemIcon& icon) {

	}

	void InventoryUI::click(const UIEvent& e) {

	}
	void InventoryUI::hover(const UIEvent& e) {

	}
	void InventoryUI::editMesh(graph::MeshEditorBase& meshEditor, const bool isAdd) noexcept {
		if (colRow.x == 0 || itemFrameTexSize == 0) return;
		auto& editor = meshEditor.cast<graph::RectUIVertex>();
		uint count = colRow.x * colRow.y;
		if (isAdd) {
			editor.resize({ count * 4, count * 6 });
			editor.setRectIndices(count);
		}
		auto& box = getBox();
		Float2 size = getItemFrameSize();
		float startX = box.min.x;
		float startY = box.max.y;
		auto texPos = graph::TexAtlas::Default().getPos(itemFrameTex);
		uint index = 0;
		for (uint j = 0; j < colRow.y; ++j) {
			float y1 = startY - j * size.y;
			float y0 = y1 - size.y;
			for (uint i = 0; i < colRow.x; ++i) {
				float x0 = startX + i * size.x;
				float x1 = x0 + size.x;

				editor.vertex(index) = graph::RectUIVertex(x0, y0, texPos.x0(), texPos.y0(), texPos.z());
				++index;
				editor.vertex(index) = graph::RectUIVertex(x0, y1, texPos.x0(), texPos.y1(), texPos.z());
				++index;
				editor.vertex(index) = graph::RectUIVertex(x1, y1, texPos.x1(), texPos.y1(), texPos.z());
				++index;
				editor.vertex(index) = graph::RectUIVertex(x1, y0, texPos.x1(), texPos.y0(), texPos.z());
				++index;
			}
		}
	}
}