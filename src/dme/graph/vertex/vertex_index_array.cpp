#include <dme/graph/vertex/vertex_index_array.h>
#include <dme/graph/vertex/vertex_type.h>

namespace dme::graph {

	VertexIndexArray::VertexIndexArray(const VertexType& types, void* vertex, uint vertexCount, uint* index, uint indexCount)
		: vao(0), vbo(0), ibo(0), stride(types.stride), vertexCount(vertexCount), vertex(vertex), index(index), indexCount(indexCount) {

		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ibo);

		//绑定顶点数组对象和顶点缓冲索引缓冲
		rebindBuffer();
		types.bindVertexAttr();
	}
	VertexIndexArray::~VertexIndexArray() {
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ibo);

		std::cout << "VertexIndexArray id:" << vao << " is deleted" << std::endl;
	}


	void VertexIndexArray::draw() const {
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
		if (false) { //TODO test
			float* v = reinterpret_cast<float*>(vertex);
			uint fc = stride / 4;
			for (uint i = 0; i < vertexCount; i++) {
				std::cout << "vertex: ";
				for (uint j = 0; j < fc; j++) {
					float f = v[i * fc + j];
					std::cout << f << ", ";
				}
				std::cout << " ;\n";
			}
			std::cout << "index: ";
			for (uint i = 0; i < indexCount; i++) {
				std::cout << index[i] << ", ";
			}
			std::cout << " ;\n";
		}
	}
	void VertexIndexArray::rebindBuffer() const {
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, static_cast<ulong>(stride * vertexCount), vertex, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<ulong>(int_size * indexCount), index, GL_STATIC_DRAW);
	}
	void VertexIndexArray::changeCount(uint vertexCount_, uint indexCount_) {
		vertexCount = vertexCount_;
		indexCount = indexCount_;
		rebindBuffer();
	}
	void VertexIndexArray::changeCount(void* vertex_, uint vertexCount_, uint* index_, uint indexCount_) {
		vertexCount = vertexCount_;
		vertex = vertex_;
		index = index_;
		indexCount = indexCount_;
		rebindBuffer();
	}

	void* VertexIndexArray::getVertex() const {
		return vertex;
	}
	uint VertexIndexArray::getVertexCount() const {
		return vertexCount;
	}
	uint* VertexIndexArray::getIndex() const {
		return index;
	}
	uint VertexIndexArray::getIndexCount() const {
		return indexCount;
	}

}