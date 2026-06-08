#include <dme/block/proto/all.h>
#include <dme/event/event.h>
#include <dme/world/world.h>
namespace dme::block {
	MultiSizeArr AllBlockProto::prototypes;
	BlockProto* AllBlockProto::GetProto(uid id) {
		return id < prototypes.getPtrCount() ? reinterpret_cast<BlockProto*>(prototypes.getPtr(id)) : null;
	}
	void AllBlockProto::EachProto(const std::function<void(BlockProto& proto)>& func) {
		prototypes.each([func](uchar* ptr) {
			func(*reinterpret_cast<BlockProto*>(ptr));
		});
	}

	BlockProto* AllBlockProto::Stone = null;
	BlockProto* AllBlockProto::Dirt = null;
	BlockProto* AllBlockProto::GrassBlock = null;
	BlockProto* AllBlockProto::Plank = null;
	BlockProto* AllBlockProto::Log = null;
	BlockProto* AllBlockProto::Leaves = null;
	PlantBlockProto* AllBlockProto::Grass = null;

	/*
	static void _defaultBlockPlaceEvent(event::BlockPlaceData data) {
		auto world = world::World::getWorld(data.getWorldId());
		if (world) {
			world->placeBlock(data.getPos(), *data.getEntity(), data.getBlock());
		}
	}
	static void _defaultBlockDestroyEvent(event::BlockDestroyData data) {
		auto world = world::World::getWorld(data.getWorldId());
		if (world) {
			world->destroyBlock(data.getPos(), *data.getEntity());
		}
	}
	static void _defaultBlockLoadEvent(event::BlockLoadData data) {
		data.getChunk()->loadBlock(data.getPos(), data.getBlock());
	}
	static void _defaultBlockUpdateEvent(event::BlockUpdateData data) {
		//data.getBlock().updateEnd(std::move(data));
	}
	*/

	void AllBlockProto::Init() {
		prototypes.setIndexAndPtr(BlockProto::setId,
			Stone,
			Dirt,
			GrassBlock,
			Plank,
			Log,
			Leaves,
			Grass
		);

		new (Stone) BlockProto(Stone->id, "Stone",
			DME_PATH_TEX_BLOCK "stone.png"
		);

		new (Dirt) BlockProto(Dirt->id, "Dirt",
			DME_PATH_TEX_BLOCK "dirt.png"
		);

		new (GrassBlock) BlockProto(GrassBlock->id, "Grass block",
			DME_PATH_TEX_BLOCK "grass_side.png",
			DME_PATH_TEX_BLOCK "grass_top.png",
			DME_PATH_TEX_BLOCK "dirt.png"
		);

		new (Plank) BlockProto(Plank->id, "Plank",
			DME_PATH_TEX_BLOCK "planks_oak.png"
		);

		new (Log) BlockProto(Log->id, "Log",
			DME_PATH_TEX_BLOCK "log_oak.png",
			DME_PATH_TEX_BLOCK "log_oak_top.png"
		);

		new (Leaves) BlockProto(Leaves->id, "Leaves",
			DME_PATH_TEX_BLOCK "leaves_oak_opaque.png"
		);

		new (Grass) PlantBlockProto(Grass->id, "Grass",
			DME_PATH_TEX_BLOCK "double_plant_grass_top.png"
		);
	}

}