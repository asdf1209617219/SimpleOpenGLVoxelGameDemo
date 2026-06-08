#pragma once
#include <dme/entity/entity.h>
#include <dme/block/block.h>
namespace dme::event {
	class BlockPlaceData {
	private:
	protected:
		block::Block block;
		Int3 pos;
		sid worldId;
		entity::Entity* entity;
	public:
		BlockPlaceData(block::Block block, const Int3& pos, sid worldId, entity::Entity* entity = null);

		block::Block getBlock() const;
		const Int3& getPos() const;
		sid getWorldId() const;
		entity::Entity* getEntity() const;
	};

}