#include <dme/event/data/block_update_data.h>
namespace dme::event {
	BlockUpdateData::BlockUpdateData(const block::Block& block, Face3 fromFace, BlockUpdateType type)
		: block(block), fromFace(fromFace), type(type) {
	}
	const block::Block& BlockUpdateData::getBlock() const {
		return block;
	}
	Face3 BlockUpdateData::getFromFace() const {
		return fromFace;
	}
	BlockUpdateType BlockUpdateData::getType() const {
		return type;
	}

}