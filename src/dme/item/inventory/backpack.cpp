#include <dme/item/inventory/backpack.h>
namespace dme::item {
	Backpack::Backpack() : Inventory(30), hotbarIndex(0) {}

	uint Backpack::getHotbarIndex() const {
		return hotbarIndex;
	}
	void Backpack::setHotbarIndex(uint hotbarIndex_) {
		hotbarIndex = hotbarIndex_;
	}

}