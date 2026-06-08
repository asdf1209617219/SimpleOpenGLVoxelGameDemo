#include <dme/block/chunk/chunk.h>
#include <dme/block/proto/all.h>
#include <dme/world/world.h>
#include <dme/entity/entity.h>
namespace dme::block {

	Chunk::Chunk() noexcept : posW(), world(null), blocks{}
	{
	}
	Chunk::Chunk(const Int4& id) noexcept : posW(id), world(null), blocks{}
	{
	}
	Chunk::Chunk(const Int3& pos) noexcept : pos(pos), worldId(Config::default_world), world(null), blocks{}
	{
	}
	Chunk::Chunk(const Int3& pos, sid worldId) noexcept : pos(pos), worldId(worldId), world(null), blocks{}
	{
	}

	Chunk* Chunk::nearChunk(Face3 face) const {
		return world->getChunk(face.nearPos(pos));
	}

	Block* Chunk::getBlocks() noexcept {
		return blocks;
	}
	const Block* Chunk::getBlocks() const noexcept {
		return blocks;
	}
	bool Chunk::existBlock(CBPos cbpos) const  noexcept {
		return blocks[cbpos].notNull();
	}
	Block Chunk::getBlock(CBPos cbpos) const  noexcept {
		return blocks[cbpos];
	}
	Block* Chunk::getBlockPtr(CBPos cbpos) noexcept {
		return blocks + cbpos;
	}
	const Block* Chunk::getBlockPtr(CBPos cbpos) const noexcept {
		return blocks + cbpos;
	}
	Block& Chunk::refBlock(CBPos cbpos)  noexcept {
		return blocks[cbpos];
	}
	const Block& Chunk::refBlock(CBPos cbpos) const  noexcept {
		return blocks[cbpos];
	}
	void Chunk::setBlock(CBPos cbpos, Block block)  noexcept {
		blocks[cbpos] = block;
	}
	void Chunk::removeBlock(CBPos cbpos)  noexcept {
		blocks[cbpos] = Block();
	}

	sid Chunk::getWorldId() const noexcept {
		return worldId;
	}
	world::World* Chunk::getWorld() const noexcept {
		//return world::World::getWorld(worldId);
		return world;
	}
	void Chunk::setWorld(world::World& world_) noexcept {
		world = &world_;
		worldId = world_.getId();
	}

	Int3 Chunk::getPos() const noexcept {
		return pos;
	}
	Int4 Chunk::getPosW() const noexcept {
		return posW;
	}
	void Chunk::setPosW(const Int4& id_) noexcept {
		posW = id_;
	}
	Int3 Chunk::getBlockPos(const Block& block) const noexcept {
		return block::ToBlockPos(pos, CBPos(tous(&block - blocks)));
	}

	void Chunk::addBlockRenderData(CBPos cbpos, Block& block, const BlockProto& proto) {
		switch (proto.getRenderType()) {
		case BlockRenderType::Cube:
			CubeMesh().add(world->getChunkMap(), *this, cbpos, block, proto);
			break;
		case BlockRenderType::Plant:
			PlantMesh().add(*this, cbpos, block, proto);
			break;
		}
	}
	void Chunk::updateBlockRenderData(CBPos cbpos, const Block& block, const BlockProto& proto) const {
		switch (proto.getRenderType()) {
		case BlockRenderType::Cube:
			CubeMesh().update(*this, cbpos, block, proto);
			break;
		case BlockRenderType::Plant:
			PlantMesh().update(*this, block, proto);
			break;
		}
	}
	void Chunk::removeBlockRenderData(CBPos cbpos, Block& block, const BlockProto& proto) {
		switch (proto.getRenderType()) {
		case BlockRenderType::Cube:
			CubeMesh().remove(world->getChunkMap(), *this, cbpos, block, proto);
			break;
		case BlockRenderType::Plant:
			PlantMesh().remove(*this, cbpos, block);
			break;
		}
	}
	void Chunk::loadChunkMesh() {
		CubeMesh().loadChunkMesh(*this);
	}
	void Chunk::unloadChunkMesh() {
		CubeMesh().unloadChunkMesh(*this);
	}

	bool Chunk::place(CBPos cbpos, entity::Entity& entity, const BlockProto& proto) {
		Block& blockRef = refBlock(cbpos);
		if (proto.place(*this, blockRef, entity)) {
			addBlockRenderData(cbpos, blockRef, proto);
			return true;
		}
		return false;
	}

	bool Chunk::destroy(CBPos cbpos, entity::Entity& entity) {
		Block& blockRef = refBlock(cbpos);
		if (blockRef.notNull()) {
			BlockProto* proto = blockRef.getProto();
			if (proto && proto->destroy(*this, blockRef, entity)) {
				//移除渲染数据
				removeBlockRenderData(cbpos, blockRef, *proto);
				blockRef.setNull();
				//更新与该方块紧贴的实体的物理
				auto callback = [](bool& isBreak, uid index, const util::BVH3f::Node& node) {
					entity::Entity* entity = reinterpret_cast<entity::Entity*>(node.data);
					entity->triggerPhysics(Float3(), Float3());
				};
				auto bpos = block::ToBlockPos(pos, cbpos);
				world->getBVH().queryCollision(math::Abox3f(Fix3(bpos), Fix3(bpos) + 1), callback);
				return true;
			}
		}
		return false;
	}

	/*
	bool Chunk::replace(CBPos cbpos, Block block) {
	Block& blockRef = refBlock(cbpos);
	if (block.notNull()) {
	BlockProto* proto = block.getProto();
	if (proto) {
	blockRef = block;

	updateBlockRenderData(cbpos, blockRef, *proto);
	return true;
	}
	}
	return false;
	}
	*/

	void Chunk::loadBlock(CBPos cbpos, Block block) {
		BlockProto* proto = AllBlockProto::GetProto(block.id);
		if (proto) {
			Block& blockRef = refBlock(cbpos);
			blockRef = block;
			addBlockRenderData(cbpos, blockRef, *proto);
		}
	}

	void Chunk::unloadBlock(CBPos cbpos) {
		Block& blockRef = refBlock(cbpos);
		BlockProto* proto = blockRef.getProto();
		if (proto) {
			removeBlockRenderData(cbpos, blockRef, *proto);
			blockRef.deleteData();
		}
	}

	void Chunk::DrawAll() {
		//TODO 多世界逻辑
		CubeMesh().draw();
		PlantMesh().draw();
	}
	graph::CubeMesh& Chunk::CubeMesh() {
		static graph::CubeMesh _mesh;
		return _mesh;
	}
	graph::PlantMesh& Chunk::PlantMesh() {
		static graph::PlantMesh _mesh;
		return _mesh;
	}

}