#include <dme/item/item_stack.h>
#include <dme/item/proto/all.h>

namespace dme::item {

	ItemStack::ItemStack() noexcept : proto(AllItemProto::None), count(0), maxCount(0) {}
	ItemStack::ItemStack(const ItemProto& proto_, uint count) noexcept : proto(&proto_), count(count) {
		maxCount = proto_.getStackMaxCount();
	}

	bool ItemStack::isEmpty() const noexcept {
		return count == 0 || proto->isNone();
	}
	const ItemProto& ItemStack::getProto() const noexcept {
		return *proto;
	}
	void ItemStack::setProto(const ItemProto& proto_) noexcept {
		proto = &proto_;
		maxCount = proto_.getStackMaxCount();
	}
	uid ItemStack::getProtoId() const noexcept {
		return proto->getId();
	}

	uint ItemStack::getCount() const noexcept {
		return count;
	}
	void ItemStack::setCount(uint count_) noexcept {
		count = count_;
	}
	uint ItemStack::addCount() noexcept {
		uint sum = count + 1;
		if (sum > maxCount) {
			count = maxCount;
			return sum - maxCount;
		}
		count = sum;
		return 0;
	}
	uint ItemStack::addCount(uint count_) noexcept {
		uint sum = count + count_;
		if (sum > maxCount) {
			count = maxCount;
			return sum - maxCount;
		}
		count = sum;
		return 0;
	}
	uint ItemStack::subCount() noexcept {
		if (count == 0) {
			return 1;
		}
		count--;
		return 0;
	}
	uint ItemStack::subCount(uint count_) noexcept {
		if (count_ > count) {
			count = 0;
			return count_ - count;
		}
		count -= count_;
		return 0;
	}

}