#include <dme/item/proto/all.h>
#include <dme/block/proto/all.h>
#include <dme/event/event.h>
#include <dme/world/world.h>

namespace dme::item {

	MultiSizeArr AllItemProto::prototypes;
	ItemProto* AllItemProto::GetProto(uid id) {
		return id < prototypes.getPtrCount() ? reinterpret_cast<ItemProto*>(prototypes.getPtr(id)) : null;
	}
	void AllItemProto::EachProto(const std::function<void(ItemProto& proto)>& func) {
		prototypes.each([func](uchar* ptr) {
			func(*reinterpret_cast<ItemProto*>(ptr));
		});
	}

	ItemProto* AllItemProto::None = null;
	ItemProto* AllItemProto::Apple = null;
	BlockItemProto* AllItemProto::Stone = null;
	BlockItemProto* AllItemProto::Dirt = null;
	BlockItemProto* AllItemProto::GrassBlock = null;
	BlockItemProto* AllItemProto::Plank = null;
	BlockItemProto* AllItemProto::Log = null;
	BlockItemProto* AllItemProto::Leaves = null;
	BlockItemProto* AllItemProto::Grass = null;

	/*
	static void _defaultItemUseEvent(event::ItemUseData data) {
		auto proto = data.getStack().getProto();
		if (proto) {
			proto->use(data);
		}
	}
	*/

	void AllItemProto::Init() {
		prototypes.setIndexAndPtr(ItemProto::setId,
			None,
			Apple,
			Stone,
			Dirt,
			GrassBlock,
			Plank,
			Log,
			Leaves,
			Grass
		);

		new (None) ItemProto();

		new (Apple) ItemProto(Apple->id, "Apple",
			ItemProto::defaultStackMaxCount, false,
			DME_PATH_TEX_ITEM "apple.png"
		);

		new (Stone) BlockItemProto(Stone->id,
			*block::AllBlockProto::Stone
		);

		new (Dirt) BlockItemProto(Dirt->id,
			*block::AllBlockProto::Dirt
		);

		new (GrassBlock) BlockItemProto(GrassBlock->id,
			*block::AllBlockProto::GrassBlock
		);

		new (Plank) BlockItemProto(Plank->id,
			*block::AllBlockProto::Plank
		);

		new (Log) BlockItemProto(Log->id,
			*block::AllBlockProto::Log
		);

		new (Leaves) BlockItemProto(Leaves->id,
			*block::AllBlockProto::Leaves
		);

		new (Grass) BlockItemProto(Grass->id,
			*block::AllBlockProto::Grass
		);
	}

}