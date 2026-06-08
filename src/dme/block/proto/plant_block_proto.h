#pragma once
#include <dme/block/proto/block_proto.h>
namespace dme::block {
	class PlantBlockProto : public BlockProto {
	private:
	protected:
	public:
		PlantBlockProto(uid id, strid name, strid tex);
		//目前植物类方块的图标与普通物品图标相同
		virtual ui::ItemIcon& createIcon(const item::ItemStack& itemStack) const override;
	};

}
