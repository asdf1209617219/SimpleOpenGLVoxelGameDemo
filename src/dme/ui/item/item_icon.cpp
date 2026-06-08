#include <dme/ui/item/item_icon.h>
#include <dme/ui/item/inventory_ui.h>
namespace dme::ui {
	ItemIcon::ItemIcon(uint meshType, const item::ItemStack& itemStack_) : UI(meshType), itemStack(&itemStack_) {}
	void ItemIcon::removeImpl() noexcept {
		ObjPool<ItemIcon>::Remove(this);
	}
	const item::ItemStack& ItemIcon::getItemStack() const noexcept {
		return *itemStack;
	}
	ItemIcon& ItemIcon::Create(const item::ItemStack& itemStack) noexcept {
		auto& r = *ObjPool<ItemIcon>::Add(ItemIcon::GetMeshType(), itemStack);
		return r;
	}
	void ItemIcon::click(const UIEvent& e) {

	}
	void ItemIcon::hover(const UIEvent& e) {

	}
	void ItemIcon::leave(const UIEvent& e) {

	}
	void ItemIcon::editMesh(graph::MeshEditorBase& meshEditor, const bool isAdd) noexcept {
		auto& editor = meshEditor.cast<graph::RectUIVertex>();
		const item::ItemStack& stack = getItemStack();
		if (stack.isEmpty()) {
			return;
		}
		if (isAdd) {
			editor.setRectIndices(1);
		}
		auto& box = getBox();
		//Log::Info("item icon box: center: ", box.center(), ", half: ", box.extents());
		auto texPos = graph::TexAtlas::Default().getPos(stack.getProto().getTexIndex());
		editor.setVertex({
			graph::RectUIVertex(box.min.x, box.min.y, texPos.x0(), texPos.y0(), texPos.z()),
			graph::RectUIVertex(box.min.x, box.max.y, texPos.x0(), texPos.y1(), texPos.z()),
			graph::RectUIVertex(box.max.x, box.max.y, texPos.x1(), texPos.y1(), texPos.z()),
			graph::RectUIVertex(box.max.x, box.min.y, texPos.x1(), texPos.y0(), texPos.z()),
		});
	}
}
