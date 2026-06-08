#pragma once
#include <dme/item/inventory/inventory.h>
namespace dme::item {
	class Backpack : public Inventory {
	private:
	protected:
		uint hotbarIndex;
	public:
		Backpack();

		uint getHotbarIndex() const;
		void setHotbarIndex(uint hotbarIndex);
	};

}