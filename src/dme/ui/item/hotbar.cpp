#include <dme/ui/item/hotbar.h>
#include <dme/entity/player.h>
#include <dme/ui/item/item_icon.h>
#include <dme/item/inventory/inventory.h>

namespace dme::ui {
	graph::TexIndex Hotbar::backgroundTex = DME_SVAR_DELAY_FUNC(Hotbar::backgroundTex, graph::TexAtlas::Default().addIfAbsent, DME_PATH_TEX_UI "hotbar.png");
	graph::TexIndex Hotbar::activeTex = DME_SVAR_DELAY_FUNC(Hotbar::activeTex, graph::TexAtlas::Default().addIfAbsent, DME_PATH_TEX_UI "hotbar_active.png");

	void Hotbar::removeImpl() noexcept {
		ObjPool<Hotbar>::Remove(this);
	}
	Hotbar& Hotbar::Default() noexcept {
		static Hotbar& _hotbar = Hotbar::Create(Game::GetControlling().getInventory(), 1);
		return _hotbar;
	}
	Hotbar& Hotbar::Create(item::Inventory& inventory, uint row, uint col) noexcept {
		auto& r = *ObjPool<Hotbar>::Add(UI::GetMeshType(), inventory, 3, Hotbar::backgroundTex, row, col);
		auto size = r.getItemFrameTexSize();
		r.setIsTriggerable(false)
			.setZIndex(1)
			.setWidthHeight(UIpx(tof(size * col)), UIpx(tof(size * row)))
			.setPivotAndAnchorsOnLower()
			.setName("hotbar");
		r.updateAllItemStack();
		return r;
	}

	Hotbar::Hotbar(uint meshType, item::Inventory& inventory, ushort itemFrameTexBorder, graph::TexIndex itemFrameTex, uint row, uint col) noexcept 
		: InventoryUI(meshType, inventory, itemFrameTexBorder, itemFrameTex, row, col),
		activeIndex(0)
	{
	}

	sint Hotbar::getMinIndex() const noexcept {
		return 0;
	}
	sint Hotbar::getMaxIndex() const noexcept {
		return getInventoryUIColRow().x - 1;
	}
	sint Hotbar::getActiveIndex() const noexcept{
		return activeIndex;
	}
	sint Hotbar::setActiveIndex(sint activeIndex_) noexcept{
		activeIndex = math::Clamp(activeIndex_, getMinIndex(), getMaxIndex());
		UITree::NeedUpdate(*this);
		return activeIndex;
	}
	sint Hotbar::scollActiveIndex(bool isLeft) noexcept {
		if (isLeft) {
			if (activeIndex == getMinIndex()) {
				activeIndex = getMaxIndex();
			}
			else {
				activeIndex--;
			}
		}
		else {
			if (activeIndex == getMaxIndex()) {
				activeIndex = getMinIndex();
			}
			else {
				activeIndex++;
			}
		}
		UITree::NeedUpdate(*this);
		return activeIndex;
	}
	uid Hotbar::invertoryIndexToInInvertoryUIIndex(uint indexInInventory) {
		if (indexInInventory < getInventoryUISize()) {
			return indexInInventory;
		}
		return uid::Null;
	}
	void Hotbar::settingItemIcon(ItemIcon& icon) {
		icon.setWidthHeight(icon.getWidth() * 0.8f, icon.getHeight() * 0.8f);
	}

	void Hotbar::click(const UIEvent& e) {

	}
	void Hotbar::hover(const UIEvent& e) {

	}
	void Hotbar::editMesh(graph::MeshEditorBase& meshEditor, const bool isAdd) noexcept {
		auto colRow = getInventoryUIColRow();
		auto& editor = meshEditor.cast<graph::RectUIVertex>();
		uint count = colRow.x * colRow.y + 1;
		if (isAdd) {
			editor.resize({ count * 4, count * 6 });
			editor.setRectIndices(count);
		}
		auto& box = getBox();
		Float2 size = box.sideLength() / colRow;
		auto texPos = graph::TexAtlas::Default().getPos(Hotbar::backgroundTex);
		auto texPosActive = graph::TexAtlas::Default().getPos(Hotbar::activeTex);
		uint index = 0;

		float y0 = box.min.y;
		float y1 = y0 + size.y;
		float x0;
		float x1;
		for (uint i = 0; i < colRow.x; ++i) {
			x0 = box.min.x + i * size.x;
			x1 = x0 + size.x;

			editor.vertex(index) = graph::RectUIVertex(x0, y0, texPos.x0(), texPos.y0(), texPos.z());
			++index;
			editor.vertex(index) = graph::RectUIVertex(x0, y1, texPos.x0(), texPos.y1(), texPos.z());
			++index;
			editor.vertex(index) = graph::RectUIVertex(x1, y1, texPos.x1(), texPos.y1(), texPos.z());
			++index;
			editor.vertex(index) = graph::RectUIVertex(x1, y0, texPos.x1(), texPos.y0(), texPos.z());
			++index;


			//Abox2 tbox = Abox2({ x0, y0 }, { x1, y1 });
			//Log::Info("hotbar box: center: ", tbox.center(), ", half: ", tbox.extents());
		}
		x0 = box.min.x + activeIndex * size.x;
		x1 = x0 + size.x;

		size.x = (tof(texPosActive.w() - texPos.w()) / 2) * (x1 - x0) / texPos.w();
		size.y = (tof(texPosActive.h() - texPos.h()) / 2) * (y1 - y0) / texPos.h();

		x1 += size.x;
		y1 += size.y;
		x0 -= size.x;
		y0 -= size.y;

		editor.vertex(index) = graph::RectUIVertex(x0, y0, texPosActive.x0(), texPosActive.y0(), texPosActive.z());
		++index;
		editor.vertex(index) = graph::RectUIVertex(x0, y1, texPosActive.x0(), texPosActive.y1(), texPosActive.z());
		++index;
		editor.vertex(index) = graph::RectUIVertex(x1, y1, texPosActive.x1(), texPosActive.y1(), texPosActive.z());
		++index;
		editor.vertex(index) = graph::RectUIVertex(x1, y0, texPosActive.x1(), texPosActive.y0(), texPosActive.z());

		//Abox2 atbox = Abox2({ x0, y0 }, { x1, y1 });
		//Log::Info("hotbar active box: center: ", atbox.center(), ", half: ", atbox.extents());
	}
}