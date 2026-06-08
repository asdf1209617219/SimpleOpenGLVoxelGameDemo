#include <dme/event/data/item_use_data.h>
namespace dme::event {

	ItemUseData::ItemUseData(item::ItemStack& stack, item::ItemFunc func, entity::Entity& entity, math::Ray3fTarget target)
		: stack(&stack), func(func), entity(&entity), target(target) {}

	item::ItemStack& ItemUseData::getStack() const noexcept {
		return *stack;
	}
	item::ItemFunc ItemUseData::getFunc() const noexcept {
		return func;
	}
	entity::Entity& ItemUseData::getEntity() const noexcept {
		return *entity;
	}
	bool ItemUseData::hasTarget() const noexcept {
		return target.hasTarget;
	}
	math::Ray3fTarget& ItemUseData::getTarget() noexcept {
		return target;
	}
	const math::Ray3fTarget& ItemUseData::getTarget() const noexcept {
		return target;
	}

}