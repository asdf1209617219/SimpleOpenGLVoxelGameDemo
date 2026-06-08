#pragma once
#include <dme/block/block.h>
namespace dme::event {
	class BlockLoadData {
	private:
	protected:
		block::Block block;
		block::CBPos pos;
		block::Chunk* chunk;
	public:
		BlockLoadData(block::Block block, block::CBPos pos, block::Chunk& chunk);

		block::Block getBlock() const;
		block::CBPos getPos() const;
		block::Chunk* getChunk() const;
	};

}