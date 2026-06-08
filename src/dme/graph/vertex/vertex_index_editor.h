#pragma once
#include <dme/graph/vertex/vertex_type_base.h>
namespace dme::graph {
    class Mesh;
    class VertexArrayManage;
    class ShaderBase;
    template<IsVertex T>
    class Shader;

    template<IsVertex T>
    class VertexIndexEditor;

    class VertexIndexEditorBase {
    protected:
        void* _vertex;
        uint* _index;

        uint _vertexCount;
        uint _indexStart;
        uint _indexCount;

        uint _nowVertex;
        uint _nowIndex;

        //声明要使用的顶点数量，第二个参数返回顶点开始索引，剩余空间不够则返回false
        bool useVertex(uint count, uint& vertexIndex) noexcept {
            vertexIndex = _nowVertex;
            _nowVertex += count;
            if (_nowVertex > _vertexCount) {
                _nowVertex = vertexIndex;
                return false;
            }
            return true;
        }
        //声明要使用的索引数量，剩余空间不够则返回false
        bool useIndex(uint count, uint& indexIndex) noexcept {
            indexIndex = _nowIndex;
            _nowIndex += count;
            if (_nowIndex > _indexCount) {
                _nowIndex = indexIndex;
                return false;
            }
            return true;
        }

        void pushRectIndex(bool isAdd, uint vertexIndex) noexcept {
            if (isAdd) {
                uint nowIndex;
                if (useIndex(6, nowIndex)) {
                    uint i = vertexIndex + _indexStart;
                    _index[nowIndex + 0] = i + 0;
                    _index[nowIndex + 1] = i + 1;
                    _index[nowIndex + 2] = i + 2;
                    _index[nowIndex + 3] = i + 0;
                    _index[nowIndex + 4] = i + 2;
                    _index[nowIndex + 5] = i + 3;
                }
            }
        }
    public:
        constexpr VertexIndexEditorBase() noexcept : _vertex(null), _vertexCount(0), _index(null), _indexStart(0), _indexCount(0), _nowVertex(0), _nowIndex(0) {}
        constexpr VertexIndexEditorBase(void* vertex, uint vertexCount, uint* index, uint indexStart, uint indexCount) noexcept
            : _vertex(vertex), _vertexCount(vertexCount), _index(index), _indexStart(indexStart), _indexCount(indexCount), _nowVertex(0), _nowIndex(0) {}

        template<IsVertex T>
        constexpr const VertexIndexEditor<T>* cast() const noexcept {
            return reinterpret_cast<const VertexIndexEditor<T>*>(this);
        }

        template<IsVertex T>
        constexpr VertexIndexEditor<T>* cast() noexcept {
            return reinterpret_cast<VertexIndexEditor<T>*>(this);
        }

        uint vertexCount() const noexcept {
            return _vertexCount;
        }
        uint getIndex(uint indexIndex) const noexcept {
            if (indexIndex >= _indexCount) return 0;
            return _index[indexIndex] - _indexStart;
        }
        void setIndex(uint indexIndex, uint index) const noexcept {
            if (indexIndex >= _indexCount) return;
            _index[indexIndex] = index + _indexStart;
        }
        uint indexCount() const noexcept {
            return _indexCount;
        }
        uint nowVertex() const noexcept {
            return _nowVertex;
        }
        uint nowIndex() const noexcept {
            return _nowIndex;
        }
    };

    //用于直接编辑顶点数组和索引数组，索引只需要用0表示第一个顶点，不需要计算当前顶点在vbo中是第几个
    template<IsVertex T>
    class VertexIndexEditorTemplate : public VertexIndexEditorBase {
    public:
        using vertex_type = T;
        constexpr VertexIndexEditorTemplate() noexcept : VertexIndexEditorBase() {}
        constexpr VertexIndexEditorTemplate(T* vertex, uint vertexCount, uint* index, uint indexStart, uint indexCount) noexcept
            : VertexIndexEditorBase(vertex, vertexCount, index, indexStart, indexCount) {}

        T* vertexStart() const noexcept {
            return reinterpret_cast<T*>(_vertex);
        }
        T& getVertex(uint vertexIndex) const noexcept {
            if (vertexIndex >= _vertexCount) return vertexStart()[0];
            return vertexStart()[vertexIndex];
        }
        void setVertex(uint vertexIndex, T vertex) const noexcept {
            if (vertexIndex >= _vertexCount) return;
            new (vertexStart() + vertexIndex) T(std::move(vertex));
        }
        T* vertexPtr(uint vertexIndex) const noexcept {
            return vertexIndex < _vertexCount ? (vertexStart() + vertexIndex) : null;
        }

    };

    template<IsVertex T>
    class VertexIndexEditor : public VertexIndexEditorTemplate<T> {
    public:
        //using VertexIndexEditorTemplate<T>::VertexIndexEditorTemplate<T>;
    };


    //表示一个对象要在Mesh中渲染
    class MeshObjBase {
    private:
        friend Mesh;
        friend VertexArrayManage;

        MeshSegment meshSegment;
        //获取当前对象在网格中的顶点以及索引的位置和结束位置（相当于两条线段）
        const graph::MeshSegment& getMeshSegment() const noexcept {
            return meshSegment;
        }
        void setMeshSegment(const graph::MeshSegment& segment) noexcept {
            meshSegment = segment;
        }
        void setMeshSegmentEmpty() noexcept {
            meshSegment.setEmpty();
        }
    public:
        //获取渲染时调用的shader的准备函数，包括shader的use和各种参数设置（调用前自动调用shader.use）
        virtual void (*getShaderReadyFunc() const noexcept) (const graph::ShaderBase& shader) noexcept {
            return null;
        }
        //获取渲染时调用的shader
        virtual const graph::ShaderBase* getShader() const noexcept {
            return null;
        }
        //获取渲染时的顺序
        virtual sint getRenderGroup() const noexcept {
            return 0;
        }
        //修改添加时将要添加的顶点及索引个数，参数默认均为0，即不进行绘制
        virtual void willAddVertexIndexCount(graph::VertexIndexCount& vic) noexcept {

        }
        //用于添加或更新已经在mesh里的顶点及索引，添加时isAdd为true，更新时为false
        virtual void editMesh(graph::VertexIndexEditorBase& vi, bool isAdd) noexcept {

        }
    };

    template <typename T>
    concept IsMeshObj = std::derived_from<T, MeshObjBase>;

    template <typename T, typename V>
    concept _MeshObj_has_getShaderReadyFunc = requires(T * obj) {
        requires IsVertex<V>;
    { obj->getShaderReadyFuncImpl() } -> std::same_as<void (*) (const Shader<V>&) noexcept>;
    };

    template <typename T, typename V>
    concept _MeshObj_has_getShader = requires(T * obj) {
        requires IsVertex<V>;
    { obj->getShaderImpl() } -> std::same_as<const Shader<V>*>;
    };

    template <typename T, typename V>
    concept _MeshObj_has_editMesh = requires(T * obj, VertexIndexEditor<V> &vi, bool isAdd) {
        requires IsVertex<V>;
    { obj->editMeshImpl(vi, isAdd) } -> std::same_as<void>;
    };

    //具有顶点类型的MeshObjBase，子类需要有一个参数为VertexIndexEditor的editMesh方法
    template<typename Child, IsMeshObj Parent, IsVertex V>
    class MeshObj : public Parent {
    protected:
        using mesh_obj_type = MeshObj;
        using parent_type = Parent;
    public:
        using Parent::Parent;
        using vertex_type = V;


        virtual void (*getShaderReadyFunc() const noexcept) (const ShaderBase& shader) noexcept override {
            //if constexpr (_MeshObj_has_getShaderReadyFunc<const Child, V>) {
            return reinterpret_cast<void (*)(const ShaderBase&) noexcept>(
                reinterpret_cast<const Child*>(this)->getShaderReadyFuncImpl()
                );
            //}
            //else {
                //DME_STATIC_WARNING(false, "getShaderReadyFuncImpl is not implement");
            //    return null;
            //}
        }
        virtual const Shader<V>* getShader() const noexcept override {
            //if constexpr (_MeshObj_has_getShader<const Child, V>) {
            return reinterpret_cast<const Child*>(this)->getShaderImpl();
            //}
            //else {
                //DME_STATIC_WARNING(false, "getShaderImpl is not implement");
            //    return null;
            //}
        }
        virtual void editMesh(VertexIndexEditorBase& vi, bool isAdd) noexcept override {
            //if constexpr (_MeshObj_has_editMesh<Child, V>) {
            reinterpret_cast<Child*>(this)->editMeshImpl(*vi.cast<V>(), isAdd);
            //}
            //else {
                //DME_STATIC_WARNING(false, "editMeshImpl is not implement");
            //    return null;
            //}
        }
    };


}