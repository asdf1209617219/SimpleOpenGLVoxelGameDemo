#include <dme/entity/entity.h>
#include <dme/block/chunk/cbpos.h>
#include <dme/world/world.h>
namespace dme::entity {

	Entity::Entity(Fix3 centerPos, world::World* world, bool checkCollision)
		: checkCollision(checkCollision), isAdmin(false), centerPos(std::move(centerPos)), node(), worldId(worldId), world(world), entityPoolPtr(null) {

		if (world) {
			worldId = world->getId();
			entityPoolPtr = world->addEntity(this);
		}
		else {
			worldId.setNull();
		}
	}

	void Entity::updateBVH() noexcept {
		if (node.isNull()) {
			if (checkCollision && world) {
				world->getBVH().add(node, getAbox(), this);
			}
		}
		else {
			if (checkCollision && world) {
				world->getBVH().updateAbox(node, getAbox());
			}
			else {
				world->getBVH().remove(node);
			}
		}
	}
	void Entity::updateBVH(const Float3& delta) noexcept {
		if (node.isNull()) {
			if (checkCollision && world) {
				world->getBVH().add(node, getAbox(), this);
			}
		}
		else {
			if (checkCollision && world) {
				world->getBVH().move(node, getAbox(), delta);
			}
			else {
				world->getBVH().remove(node);
			}
		}
	}

	Fix3 Entity::getCenterPos() const {
		return centerPos;
	}
	void Entity::setCenterPos(const Fix3& centerPos_) {
		centerPos = centerPos_;
	}
	void Entity::setCenterPos(const Fix3& centerPos_, sid worldId_) {
		centerPos = centerPos_;
		setWorldId(worldId_);
	}
	math::Abox3f Entity::getAbox() const noexcept {
		return math::Abox3f(centerPos, centerPos);
	}
	Int3 Entity::getChunkPos() const {
		return block::ToChunkPos(centerPos.i);
	}
	sid Entity::getWorldId() const {
		return worldId;
	}
	void Entity::setWorldId(sid worldId_) {
		if (world == null) {
			worldId = worldId_;
			world = world::World::GetWorld(worldId_);
			if (world) {
				entityPoolPtr = world->addEntity(this);
			}
			else {
				worldId.setNull();
			}
		}
		else if (worldId != worldId_) {
			world->removeEntity(entityPoolPtr); //从当前世界移除实体
			worldId = worldId_;
			world::World* world = world::World::GetWorld(worldId_);
			if (world) {
				entityPoolPtr = world->addEntity(this);
			}
			else {
				worldId.setNull();
			}
		}
	}
	world::World* Entity::getWorld() const {
		return world;
	}
	bool Entity::getCheckCollision() const {
		return checkCollision;
	}
	void Entity::setCheckCollision(bool checkCollision_) {
		checkCollision = checkCollision_;
	}
	bool Entity::getIsAdmin() const {
		return isAdmin;
	}
	void Entity::setIsAdmin(bool isAdmin_) {
		isAdmin = isAdmin_;
	}
	void Entity::triggerPhysics(const Float3& posDelta, const Float3& velocityDelta) {

	}

}