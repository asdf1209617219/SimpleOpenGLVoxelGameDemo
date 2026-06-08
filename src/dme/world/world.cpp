#include <dme/world/world.h>
#include <dme/world/world_generator.h>
#include <dme/event/data/block_place_data.h>
#include <dme/event/data/block_destroy_data.h>
#include <dme/block/proto/block_proto.h>

namespace dme::world {

	using block::Chunk;

	World::World(sid id, strid name, math::Seed& seed, WorldGenerator& worldGenerator) noexcept
		: id(id), name(name), chunkMap(), seed(&seed), generator(&worldGenerator), bvh(), entityPool(32) {
	}

	sid World::getId() const noexcept {
		return id;
	}
	Int3 World::getSpawnPoint() {
		return generator->getSpawnPoint(*this);
	}
	strid World::getName() const noexcept {
		return name;
	}
	const block::ChunkMap& World::getChunkMap() const noexcept {
		return chunkMap;
	}
	math::Seed& World::getSeed() const noexcept {
		return *seed;
	}
	util::BVH3f& World::getBVH() noexcept {
		return bvh;
	}
	const util::BVH3f& World::getBVH() const noexcept {
		return bvh;
	}
	eng::WorldTime World::getWorldTime() const noexcept {
		return time;
	}
	Float3 World::getLightDirection() const noexcept {
		float rate = time.getDayRate() * math::pi2;
		//目前定为太阳从(1,0,0)升起，转到(-1,0,0)落下
		float sin = std::sin(rate);
		float cos = std::cos(rate);
		return Float3(-cos, 0, -sin);
	}

	void World::deleteAllWorld() {
		auto& all = _allWorld();
		for (auto& worldKV : all) {
			for (auto& chunkKV : worldKV.val->chunkMap) {
				Chunk::Remove(chunkKV.val);
			}
			worldKV.val->chunkMap.clear();
		}
		all.clear();
	}
	void World::_addToAllWorld(World* world) {
		_allWorld().add(world->id, world);
	}

	block::Chunk* World::getOrGenChunk(const Int3& chunkPos) {
		auto result = chunkMap.get(chunkPos);
		if (result) {
			return result->val;
		}
		//申请空的chunk空间并直接放置到world中，防止生成方块过程中再次触发place事件导致循环调用
		auto chunk = Chunk::Create();
		chunk->posW = Int4(chunkPos, id);
		chunk->world = this;
		chunkMap.add(chunkPos, chunk);

		generator->editChunk(*chunk, chunkPos, *this);
		return chunk;
	}

	static std::atomic<uint> _arr_count = 0;

	static DynArr<Pair<Int3, Chunk*>> _chunk_arr = DynArr<Pair<Int3, Chunk*>>(32);
	block::Chunk* World::getOrGenChunkAsync(const Int3& chunkPos) {
		auto result = chunkMap.addIfAbsent(chunkPos, null);
		if (result) {
			return result->val;
		}
		//TODO 异步生成
		return null;
	}

	Chunk* World::getChunk(const Int3& chunkPos) const {
		auto result = chunkMap.get(chunkPos);
		return result ? result->val : null;
	}
	block::Chunk* World::loadChunk(const Int3& chunkPos) {
		return getOrGenChunk(chunkPos);
	}
	Chunk* World::loadChunkAsync(const Int3& chunkPos) {
		return getOrGenChunk(chunkPos); //TODO
	}
	bool World::unloadChunk(const Int3& chunkPos) {
		auto result = chunkMap.get(chunkPos);
		if (result) {
			auto chunk = result->val;
			block::Chunk::Remove(chunk);
			chunkMap.remove(chunkPos);
			return true;
		}
		return false;
	}

	bool World::placeBlock(const Int3& pos, entity::Entity& entity, const block::BlockProto& blockProto) {
		Int3 chunkPos = block::ToChunkPos(pos);
		Chunk* chunk = getOrGenChunk(chunkPos);
		return chunk->place(block::ToCBPos(pos), entity, blockProto);
	}
	bool World::destroyBlock(const Int3& pos, entity::Entity& entity) {
		Int3 chunkPos = block::ToChunkPos(pos);
		Chunk* chunk = getOrGenChunk(chunkPos);
		return chunk->destroy(block::ToCBPos(pos), entity);
	}

	block::Block* World::getBlock(const Int3& pos) {
		Int3 chunkPos = block::ToChunkPos(pos);
		Chunk* chunk = getOrGenChunk(chunkPos);
		return chunk ? chunk->getBlockPtr(block::ToCBPos(pos)) : null;
	}

	Hive<entity::Entity*>::comp_ptr World::addEntity(entity::Entity* ptr) noexcept {
		return entityPool.add(ptr);
	}
	void World::removeEntity(Hive<entity::Entity*>::comp_ptr ptr) noexcept {
		if (ptr.isNull()) {
			return;
		}
		entityPool.remove(ptr);
	}
	void World::update() {
		time.update();
		for (auto& obj : entityPool) {
			obj->update();
		}
	}

	void World::test(const Int3& pos) {
		generator->test(pos);
	}

	//
	// static
	//

	static StartWorldGen& _startWorldGen() {
		static StartWorldGen _gen = StartWorldGen(Game::Seed);
		return _gen;
	}
	static StartWorldGen2& _startWorldGen2() {
		static StartWorldGen2 _gen = StartWorldGen2(Game::Seed);
		return _gen;
	}

	//当前世界ID，新增世界且没传入世界ID则+1
	static sid& _currentWorldId() {
		static sid _id = Config::default_world;
		return _id;
	}

	FlatMap<sid, World*>& World::_allWorld() {
		static FlatMap<sid, World*> _allWorld = { {Config::default_world,
			ObjPool<World>::Add(Config::default_world, "Main World", Game::Seed, _startWorldGen2())} };
		return _allWorld;
	}
	World& World::StartWorld() {
		World& _startWorld = *_allWorld().get(Config::default_world)->val;
		return _startWorld;
	}

	World* World::CreateAutoId(strid name, math::Seed& seed, WorldGenerator& worldGenerator) {
		auto r = _allWorld().get(_currentWorldId());
		while (r || _currentWorldId().isNull()) {
			_currentWorldId().v++;
			r = _allWorld().get(_currentWorldId());
		}
		auto ptr = ObjPool<World>::Add(_currentWorldId(), name, seed, worldGenerator);
		World::_addToAllWorld(ptr);
		return ptr;
	}
	World* World::Create(sid worldId, strid name, math::Seed& seed, WorldGenerator& worldGenerator) {
		if (worldId.isNull() || _allWorld().get(worldId)) {
			return null;
		}
		auto ptr = ObjPool<World>::Add(worldId, name, seed, worldGenerator);
		World::_addToAllWorld(ptr);
		return ptr;
	}
	World* World::GetWorld(sid worldId) {
		auto result = _allWorld().get(worldId);
		return result ? result->val : null;
	}

	void World::UpdateAll() {
		auto& all = _allWorld();
		for (auto& kv : all) {
			kv.val->update();
		}
	}

}