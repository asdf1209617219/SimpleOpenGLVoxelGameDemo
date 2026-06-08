#include <dme/graph/mesh/cube_mesh.h>
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

	CubeMesh::CubeMesh()
		: vao(0), vbo(0),
		vertex(toui(100 * default_max * 6)),
		graphInfo(toui(100 * default_max)),
		faceStart(toui(100 * 6)),
		faceCount(toui(100 * 6)),
		faceCapacity(toui(100 * 6)),
		chunkInfoMap(), vertexFreeList{}, graphInfoFreeList(), faceStartFreeList(),
		needRefreshBuffer(true)
	{
		init();
	}

	CubeMesh::CubeMesh(const block::ChunkMap& chunks)
		: vao(0), vbo(0),
		vertex(),
		faceStart(),
		faceCount(),
		faceCapacity(),
		chunkInfoMap(), vertexFreeList{}, graphInfoFreeList(), faceStartFreeList(),
		needRefreshBuffer(true)
	{
		size_t chunkCount = chunks.size() < 100 ? 100 : chunks.size();
		vertex.resize(toui(chunkCount * default_max * 6));

		graphInfo.resize(toui(chunkCount * default_max));

		faceStart.resize(toui(chunkCount * 6));
		faceCount.resize(toui(chunkCount * 6));
		faceCapacity.resize(toui(chunkCount * 6));

		init();
	}
	CubeMesh::~CubeMesh() {
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
	}

	void CubeMesh::init() {
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);

		resizeBuffer();
		//绑定顶点格式
		CubeVertex::GetVertexType().bindVertexAttr();
	}
	void CubeMesh::resizeBuffer() const {
		//GL_STATIC_DRAW 数据不会或几乎不会改变
		//GL_DYNAMIC_DRAW 数据会被改变很多
		//GL_STREAM_DRAW 数据每次绘制时都会改变
		//绑定顶点数组对象
		glBindVertexArray(vao);
		//顶点缓冲
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertex.count() * sizeof(CubeVertex), vertex.data(), GL_DYNAMIC_DRAW);
	}

	bool CubeMesh::findSamePos(const uint thisFaceStart, const uint thisFaceCount, const block::CBPos pos, uint& vertexIndex) {
		uint posData = CubeVertex::posToData(pos);
		auto data = vertex.data() + thisFaceStart;
		for (uint i = 0; i < thisFaceCount; i++) {
			if (data[i].posEqual(posData)) {
				vertexIndex = thisFaceStart + i;
				return true;
			}
		}
		return false;
	}

	void CubeMesh::findAndMoveToFreeSpace(const uint chunkFaceStartIndex, const ushort oldFaceCapacity, uint& vertexStart) {
		needRefreshBuffer = true;

		ushort newFaceCapacity = oldFaceCapacity * 2;
		ushort oldapacityIndex = getCapacityIndex(oldFaceCapacity);
		ushort newCapacityIndex = oldapacityIndex + 1;
		uint newVertexStart;
		if (vertexFreeList[newCapacityIndex].isNull()) {
			newVertexStart = vertex.count();
			uint newVertexSize = newVertexStart + newFaceCapacity;
			vertex.recount(newVertexSize); //申请oldFaceCapacity*2的空间
		}
		else {
			newVertexStart = vertexFreeList[newCapacityIndex];
			vertexFreeList[newCapacityIndex] = vertex[newVertexStart].freeList;
		}
		faceStart[chunkFaceStartIndex] = newVertexStart;
		faceCapacity[chunkFaceStartIndex] = newFaceCapacity;
		//移动顶点
		auto data = vertex.data();
		Memmove<CubeVertex>(data + newVertexStart, data + vertexStart, oldFaceCapacity);
		//加入空闲列表
		vertex[vertexStart].freeList = vertexFreeList[oldapacityIndex];
		vertexFreeList[oldapacityIndex] = vertexStart;
		vertexStart = newVertexStart;
	}

	CubeMesh::ChunkInfoMap::pointer CubeMesh::findOrApplyChunk(const Int3& chunkPos) {
		auto r = chunkInfoMap.get(chunkPos);
		if (!r) {
			//为区块的每个面申请空间
			size_t oldVertexCapacity = vertex.size();
			uint chunkIndex;
			if (faceStartFreeList.isNull()) {
				chunkIndex = faceStart.count();
				uint newSize = chunkIndex + 6;
				faceStart.recount(newSize);
				faceCount.recount(newSize);
				faceCapacity.recount(newSize);
			}
			else {
				chunkIndex = faceStartFreeList;
				faceStartFreeList = faceStart[chunkIndex];
			}

			Face3::each([&](Face3 face3) {
				uint vertexIndex;
				if (vertexFreeList[0].isNull()) { //默认每个面申请default_max的空间
					vertexIndex = vertex.count();
					uint newVertexSize = vertexIndex + default_max;
					vertex.recount(newVertexSize);
				}
				else {
					vertexIndex = vertexFreeList[0];
					vertexFreeList[0] = vertex[vertexIndex].freeList;
				}
				uint index = chunkIndex + face3;
				faceStart[index] = vertexIndex;
				faceCount[index] = 0;
				faceCapacity[index] = default_max;
			});
			if (oldVertexCapacity != vertex.size()) { //顶点数组发生扩容
				needRefreshBuffer = true;
			}
			chunkInfoMap.add(chunkPos, ChunkInfo(chunkIndex));
			r = chunkInfoMap.get(chunkPos);
		}
		return r;
	}
	CubeMesh::ChunkInfo* CubeMesh::findChunk(const Int3& chunkPos) {
		auto r = chunkInfoMap.get(chunkPos);
		return r ? &r->val : null;
	}

	void CubeMesh::pushBlock(block::Block& block) {
		if (graphInfoFreeList.isNull()) {
			block.setGraph(graphInfo.count());
			graphInfo.addMake();
		}
		else {
			block.setGraph(graphInfoFreeList);
			graphInfoFreeList = graphInfo[graphInfoFreeList].freeList;
		}
	}
	void CubeMesh::removeBlock(block::Block& block) {
		uid graph = block.getGraph();
		graphInfo[graph].freeList = graphInfoFreeList;
		graphInfoFreeList = graph;
		block.setGraphNull();
	}
	void CubeMesh::pushVertex(const ChunkInfoMap::pointer it, GraphInfo& info, const block::CBPos pos, const Face3 face3, const uint tex, const FRotate rotate) {
		uint chunkFaceStartIndex = it->val.chunkIndex + face3;
		ushort count = faceCount[chunkFaceStartIndex]; //将要插入顶点的相对下标
		ushort capacity = faceCapacity[chunkFaceStartIndex];
		uint vertexStart = faceStart[chunkFaceStartIndex];
		//区块当前面容量已达到上限
		if (count == capacity) {
			//该面的大小扩充一倍
			findAndMoveToFreeSpace(chunkFaceStartIndex, capacity, vertexStart);
		}
		vertexStart += count;
		vertex[vertexStart] = CubeVertex(tex, pos, rotate, face3); //在最后面进行插入
		info.setIndexInFace(face3, count);
		faceCount[chunkFaceStartIndex]++;
	}

	void CubeMesh::updateVertex(const ChunkInfoMap::pointer ptr, const GraphInfo& info, const Face3 face3, const uint tex, const FRotate rotate) {
		uint chunkFaceStartIndex = ptr->val.chunkIndex + face3;
		uint vertexIndex = faceStart[chunkFaceStartIndex] + info.getIndexInFace(face3);
		//更新顶点
		vertex[vertexIndex].tex = tex;
		vertex[vertexIndex].setRotate(rotate);
	}

	void CubeMesh::removeVertex(const ChunkInfoMap::pointer ptr, const block::Chunk& chunk, GraphInfo& info, const Face3 face3) {
		uint chunkFaceStartIndex = ptr->val.chunkIndex + face3;
		uint start = faceStart[chunkFaceStartIndex];
		faceCount[chunkFaceStartIndex]--;
		uint lastVertexIndex = start + faceCount[chunkFaceStartIndex];
		ushort indexInFace = info.getIndexInFace(face3);
		info.deleteFace(face3);
		uint removeVertexIndex = start + indexInFace;
		if (lastVertexIndex != removeVertexIndex) {
			// 将该区块面最后的顶点移动到删除的顶点上
			vertex[removeVertexIndex] = vertex[lastVertexIndex];
			// 因为移动了顶点，需修改顶点对应block的graphInfo
			auto graph = chunk.refBlock(vertex[lastVertexIndex].pos()).getGraph();
			graphInfo[graph].setIndexInFace(face3, indexInFace);
		}
	}
	inline ushort CubeMesh::getCapacityIndex(ushort capacity) const {
		return BitWidth(capacity) - default_max_bit_width;
	}

	void CubeMesh::add(const block::ChunkMap& chunks, const block::Chunk& chunk, const block::CBPos pos, block::Block& block, const block::BlockProto& proto) {
		needRefreshBuffer = true;
		Int3 chunkPos = chunk.getPos();
		auto chunkIndexIt = findOrApplyChunk(chunkPos);
		pushBlock(block);
		auto& graph = graphInfo[block.getGraph()];
		Face3::each([&](Face3 face3) {
			CBPos posInChunk = CBPos(pos);
			if (posInChunk.moveToward(face3)) { //当前区块
				auto oldBlock = chunk.getBlock(posInChunk);
				BlockProto* oldProto = oldBlock.getProto();
				if (oldProto && oldProto->getRenderType() == block::BlockRenderType::Cube) { //必须同为正方体渲染方式 //TODO 透明方块的处理
					//移除两方块相邻的面
					removeVertex(chunkIndexIt, chunk, graphInfo[oldBlock.getGraph()], face3.getInvert());
					return;
				}
			}
			else { //相邻区块
				Int3 newChunkPos = face3.nearPos(chunkPos);
				auto chunksIt = chunks.get(newChunkPos);
				if (chunksIt && chunksIt->val) {
					auto oldBlock = chunksIt->val->getBlock(posInChunk);
					BlockProto* oldProto = oldBlock.getProto();
					if (oldProto && oldProto->getRenderType() == block::BlockRenderType::Cube) { //必须同为正方体渲染方式 //TODO 透明方块的处理
						//移除两方块相邻的面
						auto newChunkIndexIt = findOrApplyChunk(newChunkPos);
						removeVertex(newChunkIndexIt, *chunksIt->val, graphInfo[oldBlock.getGraph()], face3.getInvert());
						return;
					}
				}
			}
			//添加该方块的面
			auto indexRotate = block.getBase().cubeFaceToword().face3ToStartFace3Rotate(face3);
			pushVertex(chunkIndexIt, graph, pos, face3, proto.getTexArrIndex(indexRotate.face), indexRotate.rotate);
		});
	}

	void CubeMesh::update(const block::Chunk& chunk, const block::CBPos pos, const block::Block& block, const block::BlockProto& proto) {
		needRefreshBuffer = true;
		auto chunkIndexIt = findOrApplyChunk(chunk.getPos());
		auto& graph = graphInfo[block.getGraph()];
		Face3::each([&](Face3 face3) {
			if (graph.isDisplay(face3)) {
				auto indexRotate = block.getBase().cubeFaceToword().face3ToStartFace3Rotate(face3);
				updateVertex(chunkIndexIt, graph, face3, proto.getTexArrIndex(indexRotate.face), indexRotate.rotate);
			}
		});
	}

	void CubeMesh::remove(const ChunkMap& chunks, const Chunk& chunk, const CBPos pos, block::Block& block, const block::BlockProto& proto) {
		needRefreshBuffer = true;
		Int3 chunkPos = chunk.getPos();
		auto chunkIndexIt = findOrApplyChunk(chunkPos);
		auto& graph = graphInfo[block.getGraph()];
		Face3::each([&](Face3 face3) {
			CBPos posInChunk = CBPos(pos);
			if (posInChunk.moveToward(face3)) { //当前区块
				auto oldBlock = chunk.getBlock(posInChunk);
				BlockProto* oldProto = oldBlock.getProto();
				if (oldProto && oldProto->getRenderType() == block::BlockRenderType::Cube) { //必须同为正方体渲染方式 //TODO 透明方块的处理
					//要添加相反的面
					Face3 invertFace = face3.getInvert();
					auto indexRotate = oldBlock.getBase().cubeFaceToword().face3ToStartFace3Rotate(invertFace);
					pushVertex(chunkIndexIt, graphInfo[oldBlock.getGraph()], posInChunk, invertFace, oldProto->getTexArrIndex(indexRotate.face), indexRotate.rotate);
					return;
				}
			}
			else { //相邻区块
				Int3 newChunkPos = face3.nearPos(chunkPos);
				auto chunksIt = chunks.get(newChunkPos);
				if (chunksIt && chunksIt->val) {
					auto oldBlock = chunksIt->val->getBlock(posInChunk);
					BlockProto* oldProto = oldBlock.getProto();
					if (oldProto && oldProto->getRenderType() == block::BlockRenderType::Cube) { //必须同为正方体渲染方式 //TODO 透明方块的处理
						//要添加相反的面
						auto newChunkIndexIt = findOrApplyChunk(newChunkPos);
						Face3 invertFace = face3.getInvert();
						auto indexRotate = oldBlock.getBase().cubeFaceToword().face3ToStartFace3Rotate(invertFace);
						pushVertex(newChunkIndexIt, graphInfo[oldBlock.getGraph()], posInChunk, invertFace, oldProto->getTexArrIndex(indexRotate.face), indexRotate.rotate);
						return;
					}
				}
			}
			//移除该方块原本的面
			removeVertex(chunkIndexIt, chunk, graph, face3);
		});
		removeBlock(block);
	}

	bool CubeMesh::chunkMeshPushVertex(block::Chunk& chunk, const block::CBPos cbpos) {
		block::Block& block = chunk.refBlock(cbpos);
		BlockProto* blockProto = block.getProto();
		if (blockProto && blockProto->getRenderType() == block::BlockRenderType::Cube) {
			//TODO
			return true;
		}
		return false;
	}
	bool CubeMesh::chunkMeshRemoveVertex(block::Chunk& chunk, const block::CBPos cbpos) {
		block::Block& block = chunk.refBlock(cbpos);
		BlockProto* blockProto = block.getProto();
		if (blockProto && blockProto->getRenderType() == block::BlockRenderType::Cube) {
			uid graph = block.getGraph();
			graphInfo[graph].freeList = graphInfoFreeList;
			graphInfoFreeList = graph;
			block.setGraphNull();
			return true;
		}
		return false;
	}

	void CubeMesh::chunkMeshOtherChunkFace(const bool isPush, block::Chunk& chunk, block::Chunk& otherChunk, const block::CBPos cbpos, const Face3 face) {
		if (isPush) {

		}
		else {
			auto& block = otherChunk.refBlock(cbpos);
			BlockProto* proto = block.getProto();
			if (proto) {
				auto renderType = proto->getRenderType();
				if (renderType == block::BlockRenderType::Cube) { //必须同为正方体渲染方式 //TODO 透明方块的处理
					auto newChunkIndexIt = findOrApplyChunk(otherChunk.getPos());
					auto indexRotate = block.getBase().cubeFaceToword().face3ToStartFace3Rotate(face);
					pushVertex(newChunkIndexIt, graphInfo[block.getGraph()], cbpos, face, proto->getTexArrIndex(indexRotate.face), indexRotate.rotate);
				}
			}
		}
	}


	void CubeMesh::loadChunkMesh(block::Chunk& chunk) {

	}
	void CubeMesh::unloadChunkMesh(block::Chunk& chunk) {
		Int3 chunkPos = chunk.getPos();
		auto r = chunkInfoMap.get(chunkPos);
		if (!r) return;

		block::Chunk* otherChunks[Face3::Len] = {
			chunk.nearChunk(Face3::Right),
			chunk.nearChunk(Face3::Left),
			chunk.nearChunk(Face3::Front),
			chunk.nearChunk(Face3::Back),
			chunk.nearChunk(Face3::Top),
			chunk.nearChunk(Face3::Bottom),
		};
		chunkMeshEachBlockZ<false>(chunk, otherChunks, block::CBPos());

		uint chunkIndex = r->val.chunkIndex;
		Face3::each([&](Face3 face3) {
			uint index = chunkIndex + face3;
			uint start = faceStart[index];
			ushort capacityIndex = getCapacityIndex(faceCapacity[index]);
			vertex[start].freeList = vertexFreeList[capacityIndex];
			vertexFreeList[capacityIndex] = start;
		});
		faceStart[chunkIndex] = faceStartFreeList;
		faceStartFreeList = chunkIndex;
		chunkInfoMap.remove(chunk.pos);
	}

	void CubeMesh::draw() {
		//if (needResizeBuffer) {
		//	resizeBuffer();
		//	needResizeBuffer = false;
		//}
		//else {
		//	glBindVertexArray(vao);
		//}
		//glMultiDrawArrays(GL_POINTS, faceStart.data(), faceCount.data(), faceStart.count());


		//TODO 如何通过一次调用绘制多个区块 glMultiDrawArrays
		auto& shader = Shaders::CubeBlock();
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
		//shader.setInt3("playerChunkPos", Game::Player.getChunkPos());

		shader.setFloat("material.specular", 0.5f);
		shader.setFloat("material.shininess", 32.0f);

		Float3 lightDir = Game::GetControlling().getWorld()->getLightDirection();
		shader.setFloat3("dirLight.direction", lightDir);
		shader.setFloat3("dirLight.ambient", Float3(0.2f));
		shader.setFloat3("dirLight.diffuse", Float3(0.8f));
		shader.setFloat3("dirLight.specular", Float3(1.0f));

		shader.setFloat3("pointLights[0].position", cameraFPos.f); //点光源测试，暂时使用摄像机位置
		shader.setFloat3("pointLights[0].ambient", Float3(0.2f));
		shader.setFloat3("pointLights[0].diffuse", Float3(0.8f));
		shader.setFloat3("pointLights[0].specular", Float3(1.0f));
		//点光源衰减公式，d为距离: 衰减比率 = 1.0 / (constant + linear * d + quadratic * d * d)
		shader.setFloat("pointLights[0].constant", 1.0f); //衰减公式常数项
		shader.setFloat("pointLights[0].linear", 0.09f); //衰减公式一次项
		shader.setFloat("pointLights[0].quadratic", 0.032f); //衰减公式二次项

		for (auto& kv : chunkInfoMap) {
			uint index = kv.val.chunkIndex;
			shader.setInt3("chunkPos", kv.key);
			for (uchar i = Face3c::Min; i < Face3c::Len; i++, index++) {
				uint count = faceCount[index];
				if (count == 0) {
					continue;
				}
				uint start = faceStart[index];
				glDrawArrays(GL_POINTS, start, count);
			}
		}
	}

}