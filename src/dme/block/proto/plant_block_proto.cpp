#include <dme/block/proto/plant_block_proto.h>
#include <dme/item/item_stack.h>
#include <dme/ui/item/item_icon.h>
namespace dme::block {

	PlantBlockProto::PlantBlockProto(uid id, strid name, strid tex)
		: BlockProto(id, name, tex, flags_transparent, BlockRenderType::Plant, BlockCollisionType::None) {}

	ui::ItemIcon& PlantBlockProto::createIcon(const item::ItemStack& itemStack) const {
		return ui::ItemIcon::Create(itemStack);
	}
}