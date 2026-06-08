#pragma once
#include <dme/block/cube_face.h>
#include <dme/block/block.h>
namespace dme::event {

	enum class BlockUpdateType {
		Place = 0,
		Destroy,
	};

	class BlockUpdateData {
	private:
	protected:
		block::Block block;
		Face3 fromFace;
		BlockUpdateType type;
	public:
		BlockUpdateData(const block::Block& block, Face3 fromFace, BlockUpdateType type);

		const block::Block& getBlock() const;
		Face3 getFromFace() const;
		BlockUpdateType getType() const;
	};

}