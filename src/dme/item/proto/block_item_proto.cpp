#include <dme/item/proto/block_item_proto.h>
#include <dme/block/proto/block_proto.h>
#include <dme/block/cube_face.h>
#include <dme/event/data/item_use_data.h>
#include <dme/event/event.h>
#include <dme/world/world.h>
#include <dme/ui/item/block_item_icon.h>

namespace dme::item {

	BlockItemProto::BlockItemProto(uid id, const block::BlockProto& blockProto, uint stackMaxCount)
		: ItemProto(id, blockProto.getName(), stackMaxCount, false), blockProto(&blockProto)
	{
		texIndex = blockProto.getTexIndex();
	}

	const block::BlockProto& BlockItemProto::getBlockProto() const noexcept {
		return *blockProto;
	}
	void BlockItemProto::use(const event::ItemUseData& data) const {
		auto& target = data.getTarget();
		if (!target.hasTarget) return;
		auto& itemStack = data.getStack();

		auto& entity = data.getEntity();
		auto world = entity.getWorld();
		if (!world) return;
		switch (data.getFunc()) {
		case item::ItemFunc::f1: //左键破坏
			world->destroyBlock(target.gridPos, entity);
			break;
		case item::ItemFunc::f2: //右键放置
			auto& itemProto = itemStack.getProto();
			//if (itemProto == null) return; //理论上来说这里不可能会空
			if (!entity.getIsAdmin()) {
				itemStack.subCount();
			}
			//uid blockId = blockProto.getId();
			//event::BlockPlaceEvent::trigger(blockId, event::BlockPlaceData(block::Block::createCube(blockId), target->getFaceToBlockPos(), entity.getWorldId(), &entity));

			auto& blockProto = *(reinterpret_cast<const BlockItemProto&>(itemProto).blockProto);
			world->placeBlock(target.getFaceNearPos(), entity, blockProto);
			break;
		}
	}
	ui::ItemIcon& BlockItemProto::createIcon(const ItemStack& itemStack) const {
		return blockProto->createIcon(itemStack);
	}
}