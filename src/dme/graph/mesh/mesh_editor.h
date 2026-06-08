#pragma once
#include <dme/graph/mesh/ui_mesh.h>
namespace dme {
	namespace ui {
		class UI;
	}
	namespace graph {
		class MeshIndexEditor {
		private:
			uint* index;
			uint start;
		public:
			MeshIndexEditor(uint* index, uint start) noexcept : index(index), start(start) {}

			MeshIndexEditor& operator=(uint i) noexcept {
				*index = start + i;
				return *this;
			}
			uint val() noexcept {
				return *index - start;
			}
			operator uint() noexcept {
				return *index - start;
			}
		};

		//以类似于操作数组的方式编辑mesh里的顶点和索引，当目前申请的空间不够时顶点和索引会移动到新申请的位置，同时UI的MeshSegment会被更新
		//如果UI是顶点和索引数量固定的类型，则使用数量之外的顶点会报错
		class MeshEditorBase {
			friend class IUIMesh;
		protected:

			IUIMesh* mesh;
			ui::UI* u;
			const bool isFixedCount;
			uint start;

			void _resize(VertexIndexCount vertexIndexCount) {
				if (isFixedCount) {
					return;
				}
				mesh->alloc(*u, *this, vertexIndexCount);
			}
			template<IsVertex V>
			V* _vertexPtr() noexcept {
				return mesh->vertex<V>();
			}
			uint* _indexPtr() noexcept {
				return mesh->index;
			}
			static const stdstr& _uiName(ui::UI* u) noexcept;
			static const MeshSegment& _meshSegment(ui::UI* u) noexcept;

			MeshEditorBase(IUIMesh* mesh, ui::UI* u, const bool isFixedCount) noexcept;
		public:

			template<IsVertex T>
			const MeshEditor<T>& cast() const noexcept {
				return reinterpret_cast<const MeshEditor<T>&>(*this);
			}
			template<IsVertex T>
			MeshEditor<T>& cast() noexcept {
				return reinterpret_cast<MeshEditor<T>&>(*this);
			}
		};

		template<IsVertex T>
		class MeshEditor : private MeshEditorBase {
		private:
			const stdstr& uiName() const noexcept {
				return _uiName(u);
			}
		public:
			using vertex_type = T;

			//返回ui的meshSegment
			const MeshSegment& meshSegment() const noexcept {
				return _meshSegment(u);
			}
			//返回顶点的起始索引值，需要meshSegment不为空时使用
			//uint vertexStart() const noexcept {
			//	return start;
			//}

			//设置顶点个数和索引个数的最大个数
			void resize(VertexIndexCount vertexIndexCount) {
				_resize(vertexIndexCount);
			}
			//设置顶点个数和索引个数的最大个数
			void resize(uint vertexCount, uint indexCount) {
				_resize(VertexIndexCount(vertexCount, indexCount));
			}

			T& vertex(uint i) noexcept {
				auto& segment = meshSegment();
				DME_ASSERT(i < segment.vertexCount(), "vertex subscript exceeds the count, ui name: " + uiName());
				return _vertexPtr<T>()[segment.vertexStart + i];
			}

			void setVertex(std::initializer_list<T> list) noexcept {
				auto& segment = meshSegment();
				uint count = toui(list.size());
				DME_ASSERT(count <= segment.vertexCount(), "vertex subscript exceeds the count, ui name: " + uiName());
				auto ptr = _vertexPtr<T>() + segment.vertexStart;
				auto begin = list.begin();
				for (uint i = 0; i < count; ++i) {
					ptr[i] = begin[i];
				}
			}

			MeshIndexEditor index(uint i) noexcept {
				auto& segment = meshSegment();
				DME_ASSERT(i < segment.indexCount(), "index subscript exceeds the count, ui name: " + uiName());
				return MeshIndexEditor(_indexPtr() + (segment.indexStart + i), start);
			}

			void setIndex(std::initializer_list<uint> list) noexcept {
				auto& segment = meshSegment();
				uint count = toui(list.size());
				DME_ASSERT(count <= segment.indexCount(), "index subscript exceeds the count, ui name: " + uiName());
				auto ptr = _indexPtr() + segment.indexStart;
				auto begin =  list.begin();
				for (uint i = 0; i < count; ++i) {
					ptr[i] = begin[i] + start;
				}
			}

			void setRectIndices(uint rectCount) noexcept {
				auto& segment = meshSegment();
				DME_ASSERT(rectCount * 6 <= segment.indexCount(), "index subscript exceeds the count, ui name: " + uiName());
				uint* index = _indexPtr() + segment.indexStart;
				uint vi = start;
				uint ii = 0;
				for (uint i = 0; i < rectCount; ++i) {
					index[ii + 0] = vi + 0;
					index[ii + 1] = vi + 1;
					index[ii + 2] = vi + 2;
					index[ii + 3] = vi + 0;
					index[ii + 4] = vi + 2;
					index[ii + 5] = vi + 3;
					vi += 4;
					ii += 6;
				}
			}

			void testPrintIndex(uint indexCount) noexcept {
				auto& segment = meshSegment();
				uint* index = _indexPtr() + segment.indexStart;
				for (uint i = 0; i < indexCount; ++i) {
					Log::Info("index[", i, "]: ", index[i]);
				}
			}
		};
	}
}