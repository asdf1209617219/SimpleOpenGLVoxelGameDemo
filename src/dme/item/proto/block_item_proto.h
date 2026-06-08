#pragma once
#include <dme/item/proto/item_proto.h>

namespace dme {
	namespace block {
		class BlockProto;
	}
	namespace item {
		class BlockItemProto : public ItemProto {
		private:
		protected:
			const block::BlockProto* blockProto; //对应的方块原型
		public:
			BlockItemProto(uid id, const block::BlockProto& blockProto, uint stackMaxCount = ItemProto::defaultStackMaxCount);
			const block::BlockProto& getBlockProto() const noexcept;
			virtual void use(const event::ItemUseData& data) const override;
			virtual ui::ItemIcon& createIcon(const ItemStack& itemStack) const override;
		};

	}
}