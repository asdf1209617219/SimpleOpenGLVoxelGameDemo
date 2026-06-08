#include <dme/event/data/block_destroy_data.h>
namespace dme::event {
	BlockDestroyData::BlockDestroyData(const Int3& pos, sid worldId, entity::Entity& entity)
		: pos(pos), worldId(worldId), entity(&entity) {
	}

	const Int3& BlockDestroyData::getPos() const {
		return pos;
	}
	sid BlockDestroyData::getWorldId() const {
		return worldId;
	}
	entity::Entity* BlockDestroyData::getEntity() const {
		return entity;
	}

}