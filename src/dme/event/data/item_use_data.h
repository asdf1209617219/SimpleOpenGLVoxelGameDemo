#pragma once
#include <dme/item/item_stack.h>
#include <dme/entity/entity.h>
namespace dme::event {

	class ItemUseData {
	private:
	protected:
		item::ItemFunc func;
		item::ItemStack* stack;
		entity::Entity* entity;
		math::Ray3fTarget target;
	public:
		ItemUseData(item::ItemStack& stack, item::ItemFunc func, entity::Entity& entity, math::Ray3fTarget target);

		item::ItemFunc getFunc() const noexcept;
		item::ItemStack& getStack() const noexcept;
		entity::Entity& getEntity() const noexcept;
		bool hasTarget() const noexcept;
		math::Ray3fTarget& getTarget() noexcept;
		const math::Ray3fTarget& getTarget() const noexcept;
	};

}