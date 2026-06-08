#include <dme/graph/vertex/vertex_array_manage.h>
#include <dme/block/chunk/cbpos.h>

namespace dme::graph {

	void VertexArrayManage::findFreeMap(MeshSegment& segment, uint& newVertexCount, uint& newIndexCount, uint addVertexCount, uint addIndexCount) {
		if (freeMap.size() > 0) {
			//查找空闲空间
			bool findVertex = false;
			bool findIndex = false;
			uint findVertexStartKey = 0, findVertexEndKey = 0, findIndexStartKey = 0, findIndexEndKey = 0;
			for (auto& kv : freeMap) {
				if (kv.first < kv.second) {
					if (!HighBitIsOne(kv.first)) {
						if (!findVertex) {
							uint count = kv.second - kv.first;
							if (addVertexCount <= count) { //找到顶点空闲空间
								findVertex = true;
								findVertexStartKey = kv.first;
								findVertexEndKey = kv.second;
								segment.vertexStart = findVertexStartKey;
								segment.vertexEnd = findVertexStartKey + count;
								if (findIndex) {
									break;
								}
							}
						}
					}
					else {
						if (!findIndex) {
							uint count = kv.second - kv.first;
							if (addIndexCount <= count) { //找到索引空闲空间
								findIndex = true;
								findIndexStartKey = kv.first;
								findIndexEndKey = kv.second;
								segment.indexStart = HighBitSetZero(findIndexStartKey);
								segment.indexEnd = segment.indexStart + count;
								if (findVertex) {
									break;
								}
							}
						}
					}
				}
			}
			if (findVertex) {
				newVertexCount = vertexIndex.vertexCount;
				freeMap.erase(findVertexStartKey);
				freeMap.erase(findVertexEndKey);
				findVertexStartKey = segment.vertexEnd;
				if (findVertexEndKey != findVertexStartKey) { //如果相同说明该空间已经被占完
					freeMap.insert({ findVertexStartKey , findVertexEndKey });
					freeMap.insert({ findVertexEndKey , findVertexStartKey });
				}
			}
			if (findIndex) {
				newIndexCount = vertexIndex.indexCount;
				freeMap.erase(findIndexStartKey);
				freeMap.erase(findIndexEndKey);
				findIndexStartKey = HighBitSetOne(segment.indexEnd);
				if (findIndexEndKey != findIndexStartKey) {
					freeMap.insert({ findIndexStartKey , findIndexEndKey });
					freeMap.insert({ findIndexEndKey , findIndexStartKey });
				}
			}
		}
	}
	void VertexArrayManage::expandBuffer(uint oldVertexCount, uint newVertexCount, uint oldIndexCount, uint newIndexCount) {
		//扩容
		if (newVertexCount > vertexMaxCount) {
			do {
				vertexMaxCount *= 2;
			} while (newVertexCount > vertexMaxCount);
			uchar* newVertex = new uchar[stride * vertexMaxCount]{ 0 };

			//void* newVertex = std::malloc(static_cast<size_t>(stride * vertexMaxCount));
			/*
			while (newVertex == null) {
				std::this_thread::sleep_for(std::chrono::milliseconds(100)); //0.1秒
				newVertex = std::malloc(static_cast<size_t>(stride * vertexMaxCount));
				std::cout << "newVertex is null!\n";
			}
			*/

			uint oldVertexSize = stride * oldVertexCount;
			std::memcpy(newVertex, vertexIndex.vertex, oldVertexSize);
			//std::memmove(newVertex, vertexIndex.vertex, oldVertexSize);
			//std::free(vertexIndex.vertex);
			delete[] vertexIndex.vertex;
			vertexIndex.vertex = newVertex;
			vertexIndex.vertexCount = newVertexCount;
		}
		if (newIndexCount > indexMaxCount) {
			do {
				indexMaxCount *= 2;
			} while (newIndexCount > indexMaxCount);
			uint* newIndex = new uint[indexMaxCount];
			/*
			while (newIndex == null) {
				std::this_thread::sleep_for(std::chrono::milliseconds(100)); //0.1秒
				newIndex = new uint[indexMaxCount];
				std::cout << "newIndex is null!\n";
			}
			*/

			uint oldIndexSize = int_size * oldIndexCount;
			std::memcpy(newIndex, vertexIndex.index, oldIndexSize);
			//std::memmove(newIndex, vertexIndex.index, oldIndexSize);
			delete[] vertexIndex.index;
			vertexIndex.index = newIndex;
			vertexIndex.indexCount = newIndexCount;
		}
	}
	void VertexArrayManage::resizeBuffer() const {
		//GL_STATIC_DRAW 数据不会或几乎不会改变
		//GL_DYNAMIC_DRAW 数据会被改变很多
		//GL_STREAM_DRAW 数据每次绘制时都会改变
		//绑定顶点数组对象
		glBindVertexArray(vao);
		//顶点缓冲
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, static_cast<ulong>(vertexIndex.vertexCount) * stride, vertexIndex.vertex, GL_DYNAMIC_DRAW);
		//索引缓冲
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<ulong>(vertexIndex.indexCount) * int_size, vertexIndex.index, GL_DYNAMIC_DRAW);
	}
	void VertexArrayManage::draw() {
		if (vertexIndex.vertexCount == 0) {
			return;
		}
		if (needResizeBuffer) {
			resizeBuffer();
			needResizeBuffer = false;

			//TODO test
			/*
			std::cout << "vertex test:\n";
			for (uint i = 0; i < vertexIndex.vertexCount; i++) {
				VertexPositionTexture3d* vertex = reinterpret_cast<VertexPositionTexture3d*>(vertexIndex.vertex);
				VertexPositionTexture3d v = vertex[i];
				auto pos = v.position;
				auto tex = v.texCoord;
				std::cout << i << " pos x:" << pos.x << ", y:" << pos.y << ", z:" << pos.z << "; \n";
				std::cout << i << " tex x:" << tex.x << ", y:" << tex.y << ", z:" << tex.z << "; \n";
				if (i % 4 == 3) {
					std::cout << "\n";
				}
			}
			std::cout << "index test:\n";
			for (uint i = 0; i < vertexIndex.indexCount; i++) {
				uint* index = vertexIndex.index;
				std::cout << i << " :" << index[i] << " ;\n";
				if (i % 6 == 5) {
					std::cout << "\n";
				}
			}
			*/
		}
		else {
			glBindVertexArray(vao);
		}
		glDrawElements(GL_TRIANGLES, vertexIndex.indexCount, GL_UNSIGNED_INT, 0);
	}
	void VertexArrayManage::startAdd(MeshObjBase& inMesh) {
		VertexIndexCount vic = VertexIndexCount();
		inMesh.willAddVertexIndexCount(vic);
		if (vic.empty()) {
			return;
		}
		needResizeBuffer = true;

		uint oldVertexCount = vertexIndex.vertexCount;
		uint oldIndexCount = vertexIndex.indexCount;
		uint newVertexCount = oldVertexCount + vic.vertexCount;
		uint newIndexCount = oldIndexCount + vic.indexCount;
		MeshSegment indexInMesh = MeshSegment(oldVertexCount, newVertexCount, oldIndexCount, newIndexCount);
		findFreeMap(indexInMesh, newVertexCount, newIndexCount, vic.vertexCount, vic.indexCount);
		inMesh.setMeshSegment(indexInMesh);
		vertexIndex.vertexCount = newVertexCount;
		vertexIndex.indexCount = newIndexCount;
		expandBuffer(oldVertexCount, newVertexCount, oldIndexCount, newIndexCount);

		VertexIndexEditorBase vi = VertexIndexEditorBase(
			reinterpret_cast<uchar*>(vertexIndex.vertex) + stride * indexInMesh.vertexStart,
			vic.vertexCount,
			vertexIndex.index + indexInMesh.indexStart,
			indexInMesh.vertexStart,
			vic.indexCount
		);
		inMesh.editMesh(vi, true);
	}

	void VertexArrayManage::startUpdate(MeshObjBase& inMesh) {
		needResizeBuffer = true;
		const MeshSegment& indexInMesh = inMesh.getMeshSegment();

		VertexIndexEditorBase vi = VertexIndexEditorBase(
			reinterpret_cast<uchar*>(vertexIndex.vertex) + stride * indexInMesh.vertexStart,
			indexInMesh.vertexEnd - indexInMesh.vertexStart,
			vertexIndex.index + indexInMesh.indexStart,
			indexInMesh.vertexStart,
			indexInMesh.indexEnd - indexInMesh.indexStart
		);
		inMesh.editMesh(vi, false);
	}

	void VertexArrayManage::remove(MeshObjBase& inMesh) {
		if (vertexIndex.vertexCount == 0 || inMesh.meshSegment.empty()) {
			return;
		}
		needResizeBuffer = true;

		const MeshSegment& index = inMesh.getMeshSegment();
		bool needFindFreeVertex = true;
		bool needFindFreeIndex = true;
		if (index.vertexEnd == vertexIndex.vertexCount) { //如果正好在尾部删除，则没必要合并空闲空间
			vertexIndex.vertexCount -= index.vertexEnd - index.vertexStart; //直接减去该方块占用的数量
			needFindFreeVertex = false;
		}
		if (index.indexEnd == vertexIndex.indexCount) { //如果正好在尾部删除，则没必要合并空闲空间
			vertexIndex.indexCount -= index.indexEnd - index.indexStart; //直接减去该方块占用的数量
			needFindFreeIndex = false;
		}
		uint indexStartKey = HighBitSetOne(index.indexStart);
		uint indexEndKey = HighBitSetOne(index.indexEnd);
		if (freeMap.size() > 0) {
			//开始合并空闲空间，如果空间相邻的话
			if (needFindFreeVertex) {
				auto result = freeMap.find(index.vertexStart);
				if (result != freeMap.end()) {
					//正常来说找到的肯定是空闲空间的尾部，值对应的是开始
					//index.vertexStart = result->second;
					freeMap.erase(result);
					freeMap.erase(index.vertexStart);
				}
				result = freeMap.find(index.vertexEnd);
				if (result != freeMap.end()) {
					//index.vertexEnd = result->second;
					freeMap.erase(result);
					freeMap.erase(index.vertexEnd);
				}
			}
			if (needFindFreeIndex) {
				auto result = freeMap.find(indexStartKey);
				if (result != freeMap.end()) {
					//index.indexStart = UintHighestBitSetZero(result->second);
					indexStartKey = result->second;
					freeMap.erase(result);
					freeMap.erase(indexStartKey);
				}
				result = freeMap.find(indexEndKey);
				if (result != freeMap.end()) {
					//index.indexEnd = UintHighestBitSetZero(result->second);
					indexEndKey = result->second;
					freeMap.erase(result);
					freeMap.erase(indexEndKey);
				}
			}
		}
		uchar* vertexPtr = (uchar*)vertexIndex.vertex + stride * index.vertexStart;
		uint* indexPtr = vertexIndex.index + index.indexStart;
		uint removeVertexSize = stride * (index.vertexEnd - index.vertexStart);
		uint removeIndexSize = int_size * (index.indexEnd - index.indexStart);

		std::memset(vertexPtr, 0, removeVertexSize); //清空这两段内存
		//std::memset(indexPtr, 0, removeIndexSize);

		if (needFindFreeVertex) {
			freeMap.insert({ index.vertexStart , index.vertexEnd });
			freeMap.insert({ index.vertexEnd , index.vertexStart });
		}
		if (needFindFreeIndex) {
			freeMap.insert({ indexStartKey , indexEndKey });
			freeMap.insert({ indexEndKey , indexStartKey });
		}
		inMesh.setMeshSegment(MeshSegment(index.vertexStart, index.vertexStart, index.indexStart, index.indexStart));
	}

	void VertexArrayManage::release() {
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ibo);
		vertexIndex.release();
	}

	VertexArrayManage::VertexArrayManage(const VertexType& type)
		//	: ChunkVertexArray(type, 20000, 100000) {}
		: VertexArrayManage(type, 384, 576) {
	}

	VertexArrayManage::VertexArrayManage(const VertexType& type, uint vertexMaxCount_, uint indexMaxCount_)
		: vao(0), vbo(0), ibo(0), stride(type.stride), needResizeBuffer(false), vertexIndex(), vertexMaxCount(vertexMaxCount_), indexMaxCount(indexMaxCount_), freeMap() 
	{
		//vertexIndex.vertex = std::malloc(static_cast<size_t>(vertexMaxCount * stride));
		vertexIndex.vertex = new uchar[stride * vertexMaxCount];
		vertexIndex.index = new uint[indexMaxCount];
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ibo);
		resizeBuffer();
		type.bindVertexAttr();
	}
}