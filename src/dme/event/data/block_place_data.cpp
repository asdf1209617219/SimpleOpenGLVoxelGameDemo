#include <dme/event/data/block_place_data.h>
#include <dme/world/world.h>
namespace dme::event {
	BlockPlaceData::BlockPlaceData(block::Block block, const Int3& pos, sid worldId, entity::Entity* entity) :
		block(std::move(block)), pos(pos), worldId(worldId), entity(entity)
	{
	}

	block::Block BlockPlaceData::getBlock() const {
		return block;
	}
	const Int3& BlockPlaceData::getPos() const {
		return pos;
	}
	sid BlockPlaceData::getWorldId() const {
		return worldId;
	}
	entity::Entity* BlockPlaceData::getEntity() const {
		return entity;
	}

}