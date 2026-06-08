#include <dme/graph/vertex/vertex_array.h>

namespace dme::graph {

	VertexArray::VertexArray()
		: vao(0), vbo(0), stride(0), vertexCount(0), vertex(null)
	{
	}
	VertexArray::VertexArray(const VertexType& types, void* vertex, uint vertexCount)
		: vao(0), vbo(0), stride(types.stride), vertexCount(vertexCount), vertex(vertex)
	{
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);

		//绑定顶点数组对象和顶点缓冲
		rebindBuffer();
		types.bindVertexAttr();
	}
	/*
	VertexArray::~VertexArray() {
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);

		std::cout << "VertexArray id:" << vao << " is deleted" << std::endl;
	}
	*/

	void VertexArray::draw() const {
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	}
	void VertexArray::release() {
		if (vao) {
			glDeleteVertexArrays(1, &vao);
			glDeleteBuffers(1, &vbo);
			vao = 0;
			vbo = 0;
		}
	}
	void VertexArray::rebindBuffer() const {
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, static_cast<ulong>(stride) * vertexCount, vertex, GL_STATIC_DRAW);
	}
	void VertexArray::changeCount(uint vertexCount_) {
		vertexCount = vertexCount_;
		rebindBuffer();
	}
	void VertexArray::changeVertex(void* vertex_, uint vertexCount_) {
		vertexCount = vertexCount_;
		vertex = vertex_;
		rebindBuffer();
	}

	void* VertexArray::getVertex() const {
		return vertex;
	}
	uint VertexArray::getVertexCount() const {
		return vertexCount;
	}

}