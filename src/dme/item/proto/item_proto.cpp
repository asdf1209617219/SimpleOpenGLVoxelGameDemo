#include <dme/item/proto/item_proto.h>
#include <dme/item/proto/block_item_proto.h>
#include <dme/item/item_stack.h>
#include <dme/event/event.h>
#include <dme/world/world.h>
#include <dme/ui/item/item_icon.h>
namespace dme::item {

	ItemProto::ItemProto()  noexcept : id(0), name("None"), stackMaxCount(0), isInstance(false) {}
	ItemProto::ItemProto(uid id, strid name, uint stackMaxCount, bool isInstance)
		noexcept : id(id), name(name), stackMaxCount(stackMaxCount), isInstance(isInstance) 
	{
	}
	ItemProto::ItemProto(uid id, strid name, uint stackMaxCount, bool isInstance, strid texPath)
		noexcept : id(id), name(name), stackMaxCount(stackMaxCount), isInstance(isInstance)
	{
		texIndex = graph::TexAtlas::Default().addIfAbsent(texPath);
	}

	bool ItemProto::isNone() const noexcept {
		return id == 0;
	}
	bool ItemProto::notNone() const noexcept {
		return id != 0;
	}
	uid ItemProto::getId() const noexcept {
		return id;
	}
	void ItemProto::setId(ItemProto* proto, uid id_) noexcept {
		proto->id = id_;
	}
	strid ItemProto::getName() const noexcept {
		return name;
	}
	uint ItemProto::getStackMaxCount() const noexcept {
		return stackMaxCount;
	}
	void ItemProto::setStackMaxCount(uint stackMaxCount_) noexcept {
		stackMaxCount = stackMaxCount_;
	}
	bool ItemProto::getIsInstance() const noexcept {
		return isInstance;
	}
	void ItemProto::setIsInstance(bool isInstance_) noexcept {
		isInstance = isInstance_;
	}
	graph::TexIndex ItemProto::getTexIndex() const noexcept {
		return texIndex;
	}

	void ItemProto::digBlock(const event::ItemUseData& data) const {
		if (data.getFunc() != item::ItemFunc::f1) return; //左键破坏

		if (!data.hasTarget()) return;
		auto& target = data.getTarget();
		auto& itemStack = data.getStack();
		//auto itemProto = itemStack.getProto();

		auto& pos = target.gridPos;
		auto& entity = data.getEntity();

		auto world = entity.getWorld();
		if (world) {
			world->destroyBlock(pos, entity);
		}
	}
	void ItemProto::use(const event::ItemUseData& data) const {
		digBlock(data);
	}
	ui::ItemIcon& ItemProto::createIcon(const ItemStack& itemStack) const {
		return ui::ItemIcon::Create(itemStack);
	}

	bool ItemProto::operator ==(const ItemProto& a) const noexcept {
		return id == a.id;
	}

}
