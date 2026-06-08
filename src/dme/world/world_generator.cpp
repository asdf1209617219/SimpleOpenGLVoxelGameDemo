#include <dme/world/world_generator.h>
#include <dme/world/world.h>
#include <dme/event/event.h>
#include <dme/block/proto/all.h>
#include <dme/entity/player.h>
namespace dme::world {
	block::Chunk* WorldGenerator::getOrGenChunk(World& world, const Int3& chunkPos) {
		return world.getOrGenChunk(chunkPos);
	}

	StartWorldGen::StartWorldGen(math::Seed& seed) : perlin(seed), heightMap() {}

	constexpr double _factor = 1.0 / block::chunk_length;
	constexpr sint _base_height = block::chunk_length;
	constexpr sint _min_height = 0;
	constexpr sint _max_height = _base_height * 2;

	StartWorldGen::HeightData& StartWorldGen::getOrGenHeightData(const Int3& chunkPos, const Int3& chunkStartPos) {
		Int2 xy = chunkPos.vxy;
		auto r = heightMap.get(xy);
		if (!r) {
			HeightData arr{};
			for (uint x = 0; x < block::chunk_length; x++) {
				for (uint y = 0; y < block::chunk_length; y++) {
					uint index = x + y * block::chunk_length;
					//范围在0~32
					//sint noise = toi(
					//	perlin.gen((x + chunkStartPos.x) * _factor, (y + chunkStartPos.y) * _factor)
					//	* block::chunk_length
					//);
					sint noise = toi(
						perlin.octave2((x + chunkStartPos.x) * _factor, (y + chunkStartPos.y) * _factor, 4)
						* block::chunk_length
					);
					arr[index] = _base_height + noise;
					//arr[index] = _base_height + block::chunk_length;
				}
			}
			r = heightMap.add(xy, arr);
		}
		return *r->val;
	}
	Int3 StartWorldGen::getSpawnPoint(World& world) {
		return Int3(0, 0, getOrGenHeightData(Int3(), Int3())[0] + 1); //加上方块的高度
	}
	void StartWorldGen::editChunk(block::Chunk& chunk, const Int3& chunkPos, const World& world) {
		//生成高度信息
		Int3 chunkStart = block::ToBlockPos(chunkPos);
		//生成方块
		if (chunkStart.z < _min_height || chunkStart.z > _max_height) {
			return;
		}
		auto& data = getOrGenHeightData(chunkPos, chunkStart);
		for (uint x = 0; x < block::chunk_length; x++) {
			for (uint y = 0; y < block::chunk_length; y++) {
				uint index = x + y * block::chunk_length;
				uint height = data[index];
				uint startZ = chunkStart.z;
				for (uint z = 0; z < block::chunk_length; z++) {
					if (startZ > height) {
						break;
					}
					block::CBPos pos = block::CBPos(x, y, z);
					uint diff = height - startZ;
					uid id;
					if (diff == 0) {
						id = block::AllBlockProto::GrassBlock->getId();
					}
					else if (diff < 5) {
						id = block::AllBlockProto::Dirt->getId();
					}
					else {
						id = block::AllBlockProto::Stone->getId();
					}
					chunk.loadBlock(pos, block::Block::createCube(id));

					startZ++;
				}
			}
		}
	}


	constexpr sint _chunk_count2 = block::chunk_length;
	constexpr sint _min_height2 = 0;
	constexpr sint _max_height2 = block::chunk_length * _chunk_count2 - 1;

	StartWorldGen2::StartWorldGen2(math::Seed& seed) : perlin(seed) {
		
	}

	//每次会一次性生成高度在0~255范围内的方块
	StartWorldGen2::DepthData* StartWorldGen2::getOrGenDepthData(const Int3& chunkPos) noexcept {
		Int3 chunkStartPos = block::ToBlockPos(chunkPos);
		if (chunkStartPos.z < _min_height2 || chunkStartPos.z > _max_height2) {
			return null;
		}
		auto r = depthMap.get(chunkPos);
		if (!r) {
			Int3 startPos = Int3(chunkPos.vxy, _chunk_count2 - 1); //从上往下生成
			DepthData* depthDataPtr[_chunk_count2] = {};
			for (sint c = _chunk_count2 - 1; c >= 0; c--) {
				r = depthMap.add(startPos, DepthData{});
				depthDataPtr[c] = r->val.getRaw();
				startPos.moveDown();

				DepthData& d = *r->val;
				for (sint x = 0; x < block::chunk_length; x++) {
					for (sint y = 0; y < block::chunk_length; y++) {
						sint xy = x + y * block::chunk_length;
						for (sint z = block::chunk_side_max; z >= 0; z--) {
							sint index = xy + z * block::chunk_area;
							Int3 blockPos = Int3(chunkStartPos.x + x, chunkStartPos.y + y, z + c * block::chunk_length);
							double noise = perlin.octave3(blockPos.x * 0.005, blockPos.y * 0.005, blockPos.z * 0.02, 4);
							double gradient = tod(blockPos.z) / (_max_height2 * 2);
							double density = noise - gradient;
							if (density > 0) {
								//计算当前处于地表多少层下，地表为1
								if (z == block::chunk_side_max) {
									if (c == _chunk_count2 - 1) {
										d[index] = 1;
									}
									else {
										d[index] = (*depthDataPtr[c + 1])[xy] + 1;
									}
								}
								else {
									sint i = xy + (z + 1) * block::chunk_area;
									d[index] = d[i] + 1;
								}
							}
							else {
								d[index] = 0; //表示空洞
							}
						}
					}
				}
			}
			r = depthMap.get(chunkPos);
		}
		return r->val.getRaw();
	}

	Int3 StartWorldGen2::getSpawnPoint(World& world) {
		if (!hasSpawnPoint) {
			auto c = getOrGenChunk(world, Int3());
			for (sint z = _max_height2; z >= 0; z--) {
				auto block = world.getBlock(Int3(0, 0, z));
				if (block && block->notNull()) {
					spawnPoint = Int3(0, 0, z + 1);
					hasSpawnPoint = true;
					break;
				}
			}
			if (!hasSpawnPoint) {
				c->loadBlock(block::CBPos(), block::Block::createCube(block::AllBlockProto::Stone->getId()));
				spawnPoint = Int3(0, 0, 1);
				hasSpawnPoint = true;
			}
		}
		return spawnPoint;
	}
	void StartWorldGen2::editChunk(block::Chunk& chunk, const Int3& chunkPos, const World& world) {
		auto dataptr = getOrGenDepthData(chunkPos);
		if (!dataptr) {
			return;
		}
		auto& data = *dataptr;
		for (uint x = 0; x < block::chunk_length; x++) {
			for (uint y = 0; y < block::chunk_length; y++) {
				for (uint z = 0; z < block::chunk_length; z++) {
					uint index = x + y * block::chunk_length + z * block::chunk_area;
					uchar depth = data[index];
					uid id;
					if (depth == 0) {
						continue;
					}else if (depth == 1) {
						id = block::AllBlockProto::GrassBlock->getId();
					}
					else if (depth < 5) {
						id = block::AllBlockProto::Dirt->getId();
					}
					else {
						id = block::AllBlockProto::Stone->getId();
					}
					chunk.loadBlock(block::CBPos(x, y, z), block::Block::createCube(id));
				}
			}
		}
	}

	void StartWorldGen2::test(const Int3& pos) {
		Int3 chunkPos = block::ToChunkPos(pos);
		auto r = depthMap.get(chunkPos);
		if (!r) {
			Log::Info("depthMap not found.");
			return;
		}
		auto& d = *r->val;
		block::CBPos cbpos = block::ToCBPos(pos);
		double noise = perlin.octave3(pos.x * 0.005, pos.y * 0.005, pos.z * 0.02, 4);
		double gradient = tod(pos.z) / (_max_height2 * 2);
		double density = noise - gradient;
		Log::Info("cbpos: ", cbpos, ", depth: ", tous(d[cbpos]), ", noise: ", noise, ", gradient: ", gradient, ", density: ", density);
	}
}