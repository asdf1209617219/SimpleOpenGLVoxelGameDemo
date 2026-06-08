#include <dme/graph/mesh/plant_mesh.h>
#include <dme/block/chunk/chunk.h>
#include <dme/world/world.h>
#include <dme/block/block_data.h>
#include <dme/block/proto/all.h>
#include <dme/graph/texture/tex2d_array.h>
#include <dme/graph/shader.h>
#include <dme/graph/camera.h>
#include <dme/graph/fix_camera.h>
#include <dme/entity/player.h>

namespace dme::graph {

	using block::CubeFaceDisplay;
	using block::BlockProto;
	using block::AllBlockProto;
	using block::Block;
	using block::ChunkMap;
	using block::Chunk;
	using block::CBPos;
	using world::World;

	PlantMesh::PlantMesh()
		: vao(0), vbo(0),
		vertex(toui(100 * default_max)),
		vertexStart(toui(100)),
		vertexCount(toui(100)),
		vertexCapacity(toui(100)),
		chunkInfoMap(), vertexFreeList{}, vertexStartFreeList(),
		needRefreshBuffer(true)
	{
		init();
	}

	PlantMesh::PlantMesh(const block::ChunkMap& chunks)
		: vao(0), vbo(0),
		vertex(),
		vertexStart(),
		vertexCount(),
		vertexCapacity(),
		chunkInfoMap(), vertexFreeList{}, vertexStartFreeList(),
		needRefreshBuffer(true)
	{
		size_t chunkCount = chunks.size() < 100 ? 100 : chunks.size();
		vertex.resize(toui(chunkCount * default_max));
		vertexStart.resize(toui(chunkCount));
		vertexCount.resize(toui(chunkCount));
		vertexCapacity.resize(toui(chunkCount));

		init();
	}
	/*
	PlantMesh::~PlantMesh() {
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
	}
	*/

	void PlantMesh::init() {
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);

		//绑定顶点格式
		resizeBuffer();
		PlantVertex::GetVertexType().bindVertexAttr();
	}
	void PlantMesh::resizeBuffer() const {
		//绑定顶点数组对象
		glBindVertexArray(vao);
		//顶点缓冲
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertex.count() * sizeof(PlantVertex), vertex.data(), GL_DYNAMIC_DRAW);
	}

	bool PlantMesh::findSamePos(const uint start, const uint count, const block::CBPos pos, uint& vertexIndex) {
		uint posData = PlantVertex::posToData(pos);
		auto data = vertex.data() + start;
		for (uint i = 0; i < count; i++) {
			if (data[i].posEqual(posData)) {
				vertexIndex = start + i;
				return true;
			}
		}
		return false;
	}

	void PlantMesh::findAndMoveToFreeSpace(const uint chunkIndex, const uint oldCapacity, uint& start) {
		needRefreshBuffer = true;

		ushort newFaceCapacity = oldCapacity * 2;
		ushort oldapacityIndex = getCapacityIndex(oldCapacity);
		ushort newCapacityIndex = oldapacityIndex + 1;
		uint newVertexStart;
		if (vertexFreeList[newCapacityIndex].isNull()) {
			newVertexStart = static_cast<uint>(vertex.count());
			uint newSize = newVertexStart + newFaceCapacity;
			vertex.recount(newSize); //申请oldFaceCapacity*2的空间
		}
		else {
			newVertexStart = vertexFreeList[newCapacityIndex];
			vertexFreeList[newCapacityIndex] = vertex[newVertexStart].freeList;
		}
		vertexStart[chunkIndex] = newVertexStart;
		vertexCapacity[chunkIndex] = newFaceCapacity;
		//移动顶点
		auto data = vertex.data();
		Memmove<PlantVertex>(data + newVertexStart, data + start, oldCapacity);
		//加入空闲列表
		vertex[start].freeList = vertexFreeList[oldapacityIndex];
		vertexFreeList[oldapacityIndex] = start;
		start = newVertexStart;
	}

	PlantMesh::ChunkInfoMap::iterator PlantMesh::findOrApplyChunk(const Int3& chunkPos) {
		auto it = chunkInfoMap.find(chunkPos);
		if (it == chunkInfoMap.end()) {
			//为区块的每个面申请空间
			size_t oldVertexCapacity = vertex.size();
			uint chunkIndex;
			if (vertexStartFreeList.isNull()) {
				chunkIndex = static_cast<uint>(vertexStart.count());
				uint newSize = chunkIndex + 1;
				vertexStart.recount(newSize);
				vertexCount.recount(newSize);
				vertexCapacity.recount(newSize);
			}
			else {
				chunkIndex = vertexStartFreeList;
				vertexStartFreeList = vertexStart[chunkIndex];
			}

			uint vertexIndex;
			if (vertexFreeList[0].isNull()) { //默认每个面申请default_max的空间
				vertexIndex = static_cast<uint>(vertex.count());
				uint newSize = vertexIndex + default_max;
				vertex.recount(newSize);
			}
			else {
				vertexIndex = vertexFreeList[0];
				vertexFreeList[0] = vertex[vertexIndex].freeList;
			}
			vertexStart[chunkIndex] = vertexIndex;
			vertexCount[chunkIndex] = 0;
			vertexCapacity[chunkIndex] = default_max;

			if (oldVertexCapacity != vertex.size()) { //顶点数组发生扩容
				needRefreshBuffer = true;
			}

			chunkInfoMap.insert({ chunkPos , ChunkInfo(chunkIndex) });
			it = chunkInfoMap.find(chunkPos);
		}
		return it;
	}

	inline ushort PlantMesh::getCapacityIndex(ushort capacity) const {
		return BitWidth(capacity) - default_max_bit_width;
	}

	void PlantMesh::pushVertex(const ChunkInfoMap::iterator& it, uid& graph, const block::CBPos pos, const uint tex) {
		uint chunkIndex = it->second.chunkIndex;
		ushort count = vertexCount[chunkIndex];
		ushort capacity = vertexCapacity[chunkIndex];
		uint start = vertexStart[chunkIndex];
		//当前区块容量已达到上限
		if (count == capacity) {
			//该区块的容量扩充一倍
			findAndMoveToFreeSpace(chunkIndex, capacity, start);
		}
		start += count;
		vertex[start] = PlantVertex(tex, pos); //在最后面进行插入
		graph = count;
		vertexCount[chunkIndex]++;
	}

	void PlantMesh::updateVertex(const ChunkInfoMap::iterator& it, const uid& graph, const uint tex) {
		uint chunkIndex = it->second.chunkIndex;
		uint vertexIndex = vertexStart[chunkIndex] + graph;
		vertex[vertexIndex].tex = tex; //更新该位置上的数据
	}

	void PlantMesh::removeVertex(const ChunkInfoMap::iterator& it, block::Chunk& chunk, uid& graph) {
		uint chunkIndex = it->second.chunkIndex;
		vertexCount[chunkIndex]--; //减少该区块的顶点个数
		uint start = vertexStart[chunkIndex];
		uint removeVertexIndex = start + graph;
		uint lastVertexIndex = start + vertexCount[chunkIndex];
		if (lastVertexIndex != removeVertexIndex) {
			//将该区块面最后的顶点移动到删除的顶点上
			vertex[removeVertexIndex] = vertex[lastVertexIndex];
			//因为移动了顶点，需修改顶点对应block的graph
			chunk.refBlock(vertex[lastVertexIndex].pos()).setGraph(graph);
		}
		graph.setNull();
	}

	void PlantMesh::add(const block::Chunk& chunk, const block::CBPos pos, block::Block& block, const block::BlockProto& proto) {
		needRefreshBuffer = true;
		auto chunkIndexIt = findOrApplyChunk(chunk.getPos());
		//添加该方块的顶点
		pushVertex(chunkIndexIt, block.refGraph(), pos, proto.getTexArrIndex());
	}

	void PlantMesh::update(const block::Chunk& chunk, const block::Block& block, const block::BlockProto& proto) {
		needRefreshBuffer = true;
		auto chunkIndexIt = findOrApplyChunk(chunk.getPos());
		updateVertex(chunkIndexIt, block.refGraph(), proto.getTexArrIndex());
	}

	void PlantMesh::remove(Chunk& chunk, const block::CBPos pos, block::Block& block) {
		needRefreshBuffer = true;
		auto chunkIndexIt = findOrApplyChunk(chunk.getPos());
		//移除该方块原本的顶点
		removeVertex(chunkIndexIt, chunk, block.refGraph());
	}

	void PlantMesh::unloadChunkInfo(const block::Chunk& chunk) {
		auto it = chunkInfoMap.find(chunk.pos);
		if (it == chunkInfoMap.end()) return;
		uint chunkIndex = it->second.chunkIndex;
		uint start = vertexStart[chunkIndex];
		ushort capacityIndex = getCapacityIndex(vertexCapacity[chunkIndex]);
		vertex[start].freeList = vertexFreeList[capacityIndex];
		vertexFreeList[capacityIndex] = start;
		vertexStart[chunkIndex] = vertexStartFreeList;
		vertexStartFreeList = chunkIndex;
		chunkInfoMap.erase(it);
	}

	void PlantMesh::draw() {
		//if (needResizeBuffer) {
		//	resizeBuffer();
		//	needResizeBuffer = false;
		//}
		//else {
		//	glBindVertexArray(vao);
		//}
		//glMultiDrawArrays(GL_POINTS, faceStart.data(), faceCount.data(), faceStart.size());

		//TODO 如何通过一次调用绘制多个区块
		auto& shader = Shaders::PlantBlock();
		shader.use();

		if (needRefreshBuffer) {
			resizeBuffer(); //TODO 测试，先每帧都更新缓冲
		}

		shader.setTex2dArray("textureArray", graph::Tex2dArray::Cube(), 0);
		shader.setMatrix("view", Game::CameraF.getView());
		shader.setMatrix("projection", Game::CameraF.getProjection());

		auto cameraFPos = Game::GetControlling().getCameraFPos();
		shader.setInt3("cameraPosInt3", cameraFPos.i);
		shader.setFloat3("cameraPosFloat3", cameraFPos.f);

		for (auto& kv : chunkInfoMap) {
			uint index = kv.second.chunkIndex;
			uint count = vertexCount[index];
			if (count == 0) {
				continue;
			}
			uint start = vertexStart[index];
			shader.setInt3("chunkPos", kv.first);
			glDrawArrays(GL_POINTS, start, count);
		}
	}

}