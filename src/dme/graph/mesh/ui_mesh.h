#pragma once
#include <dme/graph/shader.h>
#include <dme/graph/mesh/mesh_draw_mode.h>
#include <dme/graph/vertex/vertex_type.h>
#include <dme/ui/ui_mesh_key.h>

namespace dme {
	namespace ui {
		class UI;
	}
	namespace graph {
		template<IsVertex T>
		class MeshEditor;
		class MeshEditorBase;

		//UI的mesh接口
		class IUIMesh : private ObjPool<IUIMesh> {
			friend MeshEditorBase;
			friend ObjPool<IUIMesh>;
		private:
			uint vao = 0;
			uint vbo = 0;
			uint ibo = 0;

			MeshDrawMode drawMode = MeshDrawMode::triangles;
			bool needUpdateBuffer = false; //在draw前判断是否需要更新缓冲
			ushort stride = 0; //整个顶点对象的大小

			uchar* data = null; //顶点数组起点
			uint* index = null; //索引数组起点
			uint vertexCount = 0; //顶点数组当前数量
			uint indexCount = 0; //索引数组当前数量
			uint vertexSize = 0; //顶点容量
			uint indexSize = 0; //索引容量

			uchar* drawInfo = null; //第一部分为每个draw命令绘制的索引数量组成的数组，第二部分为每个draw命令的起始偏移量
			uint drawCount = 0; //MultiDraw总共要进行多少次绘制（即drawInfo数组的数量）
			uint drawSize = 0;

			ArrFreeSpan freeVertex; //记录顶点的空闲空间
			ArrFreeSpan freeIndex; //记录索引的空闲空间

			//重新设置缓冲大小以及开始位置
			void resizeBuffer() const;
			//扩容
			void extendVertexBuffer(uint newVertexCount);
			//扩容
			void extendIndexBuffer(uint newIndexCount);
			//更新缓冲到GPU
			void updateBuffer() const;

			//申请对应数量的顶点和索引
			//如果顶点的个数固定（同类型UI使用的顶点个数相同，比如都是单矩形），则回收UI空间时会将尾部的UI的顶点移动到被删除UI的位置
			void alloc(ui::UI& u, MeshEditorBase& editor, VertexIndexCount uiNewCount);

			void addDrawInfo(uint count, size_t offset) noexcept;
			void removeDrawInfo(size_t offset) noexcept;
			void removeDrawInfo(uint* indexCounts, size_t* indexOffsets, uint index) noexcept;
			void splitDrawInfo(uint* indexCounts, size_t* indexOffsets, uint i, uint start, uint end, uint count) noexcept;

			void insertDrawInfo(uint* indexCounts, size_t* indexOffsets, uint index, uint count, size_t offset) noexcept;
			bool binarySearchDrawInfo(uint* indexCounts, size_t* indexOffsets, uint start, uint& result) const noexcept;
			uint binarySearchDrawInfo(uint* indexCounts, size_t* indexOffsets, size_t offset) const noexcept;

			void removeFixed(ui::UI& u, VertexIndexCount useVertexIndexCount) noexcept;
			void removeNotFixedIndex(ui::UI& u) noexcept;
			void removeNotFixedVertex(ui::UI& u) noexcept;
		protected:
			IUIMesh(const VertexType& vertexType, uint vertexSize, uint indexSize, const bool isFixed, MeshDrawMode drawMode = MeshDrawMode::triangles);

			template<IsVertex V>
			V* vertex() noexcept {
				return reinterpret_cast<V*>(data);
			}
			template<IsVertex V>
			const V* vertex() const noexcept {
				return reinterpret_cast<const V*>(data);
			}
		public:
			static IUIMesh* Create(const VertexType& vertexType, uint vertexSize, uint indexSize, const bool isFixed, MeshDrawMode drawMode = MeshDrawMode::triangles) {
				return ObjPool<IUIMesh>::Add(vertexType, vertexSize, indexSize, isFixed, drawMode);
			}
			//更新或添加UI的顶点，添加前ui会加入到uiArray的最后
			void update(ui::UI& u, const bool isAdd, const bool isFixedCount, VertexIndexCount useVertexIndexCount);
			//移除UI的顶点
			void remove(ui::UI& u, const bool isFixedCount, VertexIndexCount useVertexIndexCount) noexcept;
			//绘制顶点数组
			void draw(const bool isFixedCount);
			//回收该对象的内存，删除数据
			void release();

			//调用ui具体的编辑顶点和索引方法
			virtual void editMesh(ui::UI& u, MeshEditorBase editor, const bool isAdd);
		};


		//mesh绘制和更新时使用的函数
		struct UIMeshFunc {
			uint id;
			uint vertexId;
			//渲染时调用的shader的准备函数，包括shader的use和各种参数设置，进入函数前会自动调用shader.use
			void (*shaderReadyFunc) (const ShaderBase&);
			//渲染时通过调用该函数获取shader
			const ShaderBase& (*getShaderFunc) ();
			//创建UIMesh（指针必须稳定）
			IUIMesh* (*createMesh) ();
			//数量为0则表示数量不固定，否则表示为单个UI使用的顶点和索引数量
			const VertexIndexCount vertexIndexCount;
			//是否启用深度测试和面剔除
			bool is3D;

			constexpr UIMeshFunc() noexcept : id(0), vertexId(0), shaderReadyFunc(null), getShaderFunc(null), createMesh(null), is3D(false) {}
			constexpr UIMeshFunc(
				uint id,
				uint vertexId,
				void (*shaderReadyFunc) (const ShaderBase&),
				const ShaderBase& (*getShaderFunc) (),
				IUIMesh* (*createMesh) (),
				VertexIndexCount vertexIndexCount,
				bool is3D
			) noexcept : id(id), vertexId(vertexId), shaderReadyFunc(shaderReadyFunc), getShaderFunc(getShaderFunc),
				createMesh(createMesh), vertexIndexCount(vertexIndexCount), is3D(is3D) {}
		};

		//注册不同mesh类型的UI
		class UIMeshRegister {
		private:
			inline static DynArr<UIMeshFunc> _arr = DynArr<UIMeshFunc>(16);
		public:
			//注册UIMesh，类名下必须有函数ShaderReadyFunc，属性Shader，函数CreateMesh
			static UIMeshFunc RegisterUIMesh(
				void (*getShaderReadyFunc) (const ShaderBase&),
				const ShaderBase& (*getShaderFunc) (),
				IUIMesh* (*createMesh) (),
				VertexIndexCount vertexIndexCount,
				bool is3D
			) noexcept {
				auto& shader = getShaderFunc();
				UIMeshFunc func = UIMeshFunc(
					_arr.count(),
					shader.getType().id,
					getShaderReadyFunc,
					getShaderFunc,
					createMesh,
					vertexIndexCount,
					is3D
				);
				_arr.add(func);
				return func;
			}
			static const UIMeshFunc& GetMeshFunc(uint id) noexcept {
				return _arr[id];
			}
		};

		/*
		template<typename U>
		concept _HasUIMeshFunc = requires() {
			requires UsingVertexType<std::remove_cvref_t<decltype(U::Shader)>>;
			requires std::is_same_v<decltype(U::Shader), const Shader<typename std::remove_cvref_t<decltype(U::Shader)>::vertex_type>&>;
			{ U::ShaderReadyFunc(std::declval<decltype(U::Shader)>()) } -> std::same_as<void>;
			{ U::CreateMesh() } -> std::same_as<IUIMesh*>;
			requires std::is_same_v<decltype(U::UseVertexIndexCount), const VertexIndexCount>;
		};
		*/

		template<typename U>
		concept _HasUIMeshFunc = requires() {
			requires UsingVertexType<std::remove_cvref_t<decltype(U::GetShader())>>;
			requires std::is_same_v<decltype(U::GetShader()), const Shader<typename std::remove_cvref_t<decltype(U::GetShader())>::vertex_type>&>;
			{ U::ShaderReadyFunc(std::declval<decltype(U::GetShader())>()) } -> std::same_as<void>;
			{ U::CreateMesh() } -> std::same_as<IUIMesh*>;
			requires std::is_same_v<decltype(U::UseVertexIndexCount), const VertexIndexCount>;
		};

		template <typename T>
		concept _has_is_3d = requires {
			//检查是否存在静态常量Is3D
			requires std::is_same_v<decltype(T::Is3D), const bool>;
		};

		//UI通过该模板类获取meshId，UI需要实现约束里的函数
		template<typename U> requires(_HasUIMeshFunc<U>)
		class UIMeshBase {
		private:
			inline static void _shaderReadyFunc(const ShaderBase& shader) {
				U::ShaderReadyFunc(static_cast<decltype(U::GetShader())>(shader));
			}
			inline static const ShaderBase& _getShaderFunc() {
				return static_cast<const ShaderBase&>(U::GetShader());
			}

			/*
			static inline auto& _meshFunc = DME_SINIT_FUNC(UIMeshRegister::RegisterUIMesh,
				_shaderReadyFunc,
				U::Shader,
				U::CreateMesh,
				U::UseVertexIndexCount
			);
			*/

		public:
			static const UIMeshFunc& GetMeshFunc() noexcept {
				bool is3D;
				if constexpr (_has_is_3d<U>) {
					is3D = U::Is3D;
				}
				else {
					is3D = false;
				}

				static UIMeshFunc _func = UIMeshRegister::RegisterUIMesh(_shaderReadyFunc, _getShaderFunc, U::CreateMesh, U::UseVertexIndexCount, is3D);
				return _func;
			}
			static uint GetMeshType() noexcept {
				return GetMeshFunc().id;
			}
		};

		//管理全部UI的顶点
		class UIMeshGroup {
		public:
			struct NodeVal {
				DynArr<ui::UI*> uiArray;
				UIMeshFunc func;
				bool isFixedCount;
				IUIMesh* mesh;

				NodeVal(uint meshType) noexcept : uiArray(), func(UIMeshRegister::GetMeshFunc(meshType)) {
					isFixedCount = func.vertexIndexCount.vertexCount != 0 && func.vertexIndexCount.indexCount != 0;
					mesh = func.createMesh();
				}
			};
		private:
			TreeMap<ui::UIMeshKey, NodeVal> map;
		public:
			//更新UI的顶点
			void update(ui::UI& u);
			//移除UI的顶点，然后UIMesh会将uiArray最后一个替换到被删除UI的位置，传入参数时last的uiArrayIndex已经被替换，如果被删除的ui就是最后一个则last为null
			//会将UI的uiArray设为null
			void remove(ui::UI& u);
			//更改同一组ui的zIndex
			void setGroupZIndex(ui::UI& u, const sint zIndex);
			//更改ui的zIndex
			void setZIndex(ui::UI& u, const sint zIndex);
			//绘制UI
			void draw();
			//调用所有mesh的release方法
			void release();
		};
	}
}