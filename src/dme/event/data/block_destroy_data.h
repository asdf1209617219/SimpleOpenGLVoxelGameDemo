#pragma once
#include <dme/entity/entity.h>
#include <dme/block/block.h>
namespace dme::event {
	class BlockDestroyData {
	private:
	protected:
		Int3 pos;
		sid worldId;
		entity::Entity* entity;
	public:
		//TODO 需要重做
		BlockDestroyData(const Int3& pos, sid worldId, entity::Entity& entity);

		const Int3& getPos() const;
		sid getWorldId() const;
		entity::Entity* getEntity() const;
	};

}