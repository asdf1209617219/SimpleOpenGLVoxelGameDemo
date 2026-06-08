#include <dme/block/block.h>
#include <dme/block/proto/all.h>
#include <dme/block/chunk/chunk.h>
#include <dme/world/world.h>
namespace dme::block {
	BlockProto* Block::getProto() const {
		if (id.isNull()) return null;
		return AllBlockProto::GetProto(getId());
	}
}