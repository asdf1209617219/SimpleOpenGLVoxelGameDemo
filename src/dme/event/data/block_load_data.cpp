#include <dme/event/data/block_load_data.h>
#include <dme/block/chunk/chunk.h>
namespace dme::event {
	BlockLoadData::BlockLoadData(block::Block block, block::CBPos pos, block::Chunk& chunk)
		: block(std::move(block)), pos(pos), chunk(&chunk) {
	}

	block::Block BlockLoadData::getBlock() const {
		return block;
	}
	block::CBPos BlockLoadData::getPos() const {
		return pos;
	}
	block::Chunk* BlockLoadData::getChunk() const {
		return chunk;
	}

}