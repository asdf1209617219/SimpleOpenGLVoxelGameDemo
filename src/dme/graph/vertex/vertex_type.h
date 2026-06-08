#pragma once
#include <dme/graph/vertex/vertex_index_editor.h>
#include <dme/block/cube_face.h>
#include <dme/block/chunk/cbpos.h>
namespace dme::graph {

    struct VertexPos
        : VertexBase<VertexPos, Float3>
    {
        Float3 pos;

        constexpr VertexPos() noexcept : pos() {}
        constexpr VertexPos(const Float3& pos) noexcept : pos(pos) {}
        constexpr VertexPos(float posX, float posY, float posZ) noexcept : pos(posX, posY, posZ) {}
    };
    template <>
    class VertexIndexEditor<VertexPos> : public VertexIndexEditorTemplate<VertexPos> {
    public:
        //添加一个矩形
        void pushRect(bool isAdd, const Float2& center, const Float2& half, float posZ) noexcept {
            uint nowVertex;
            if (useVertex(4, nowVertex)) {
                pushRectIndex(isAdd, nowVertex);
                Float2 min = center - half;
                Float2 max = center + half;
                auto ptr = vertexStart() + nowVertex;
                new (ptr) vertex_type(min.x, min.y, posZ);
                ptr++;
                new (ptr) vertex_type(min.x, max.y, posZ);
                ptr++;
                new (ptr) vertex_type(max.x, max.y, posZ);
                ptr++;
                new (ptr) vertex_type(max.x, min.y, posZ);
            }
        }
    };

    struct VertexPos2
        : VertexBase<VertexPos2, Float2>
    {
        Float2 pos;

        constexpr VertexPos2() noexcept : pos() {}
        constexpr VertexPos2(const Float2& pos) noexcept : pos(pos) {}
        constexpr VertexPos2(float posX, float posY) noexcept : pos(posX, posY) {}
    };
    template <>
    class VertexIndexEditor<VertexPos2> : public VertexIndexEditorTemplate<VertexPos2> {
    public:
        //添加一个矩形
        void pushRect(bool isAdd, const Float2& center, const Float2& half) noexcept {
            uint nowVertex;
            if (useVertex(4, nowVertex)) {
                pushRectIndex(isAdd, nowVertex);
                Float2 min = center - half;
                Float2 max = center + half;
                auto ptr = vertexStart() + nowVertex;
                new (ptr) vertex_type(min.x, min.y);
                ptr++;
                new (ptr) vertex_type(min.x, max.y);
                ptr++;
                new (ptr) vertex_type(max.x, max.y);
                ptr++;
                new (ptr) vertex_type(max.x, min.y);
            }
        }
    };

    struct VertexPos2Rgb
        : VertexBase<VertexPos2Rgb, Float2, Float3>
    {
        Float2 pos;
        Float3 rgb;

        constexpr VertexPos2Rgb() noexcept : pos(), rgb() {}
        constexpr VertexPos2Rgb(const Float2& pos, const Float3& rgb) noexcept : pos(pos), rgb(rgb) {}
    };
    template <>
    class VertexIndexEditor<VertexPos2Rgb> : public VertexIndexEditorTemplate<VertexPos2Rgb> {
    public:
        //添加一个矩形
        void pushRect(bool isAdd, const Float2& center, const Float2& half) noexcept {
            uint nowVertex;
            if (useVertex(4, nowVertex)) {
                pushRectIndex(isAdd, nowVertex);
                Float2 min = center - half;
                Float2 max = center + half;
                auto ptr = vertexStart() + nowVertex;
                new (ptr) vertex_type(Float2(min.x, min.y), Float3());
                ptr++;
                new (ptr) vertex_type(Float2(min.x, max.y), Float3());
                ptr++;
                new (ptr) vertex_type(Float2(max.x, max.y), Float3());
                ptr++;
                new (ptr) vertex_type(Float2(max.x, min.y), Float3());
            }
        }
    };

    struct VertexPos2Tex
        : VertexBase<VertexPos2Tex, Float2, Float2>
    {
        Float2 pos;
        Float2 tex; //纹理坐标

        constexpr VertexPos2Tex() noexcept : pos(), tex() {}
        constexpr VertexPos2Tex(const Float2& pos, const Float2& tex) noexcept : pos(pos), tex(tex) {}
        constexpr VertexPos2Tex(float posX, float posY, float texX, float texY) noexcept : pos(posX, posY), tex(texX, texY) {}
    };
    template <>
    class VertexIndexEditor<VertexPos2Tex> : public VertexIndexEditorTemplate<VertexPos2Tex> {
    public:
        //添加一个矩形
        void pushRect(bool isAdd, const Float2& center, const Float2& half, const Float2& texRepeatTime = Float2(1, 1) ) noexcept {
            uint nowVertex;
            if (useVertex(4, nowVertex)) {
                pushRectIndex(isAdd, nowVertex);
                Float2 min = center - (half * texRepeatTime);
                Float2 max = center + (half * texRepeatTime);
                auto ptr = vertexStart() + nowVertex;
                new (ptr) vertex_type(Float2(min.x, min.y), Float2(0, 0));
                ptr++;
                new (ptr) vertex_type(Float2(min.x, max.y), Float2(0, texRepeatTime.y));
                ptr++;
                new (ptr) vertex_type(Float2(max.x, max.y), Float2(texRepeatTime.x, texRepeatTime.y));
                ptr++;
                new (ptr) vertex_type(Float2(max.x, min.y), Float2(texRepeatTime.x, 0));
            }
        }
    };

    struct VertexPosTex
        : VertexBase<VertexPosTex, Float3, Float2>
    {
        Float3 pos;
        Float2 tex; //纹理坐标

        constexpr VertexPosTex() noexcept : pos(), tex() {}
        constexpr VertexPosTex(const Float3& pos, const Float2& tex) noexcept : pos(pos), tex(tex) {}
        constexpr VertexPosTex(float posX, float posY, float posZ, float texX, float texY) noexcept : pos(posX, posY, posZ), tex(texX, texY) {}
    };
    template <>
    class VertexIndexEditor<VertexPosTex> : public VertexIndexEditorTemplate<VertexPosTex> {
    public:
        //添加一个矩形
        void pushRect(bool isAdd, const Float2& center, const Float2& half, float posZ, const Float2& texRepeatTime = Float2(1, 1) ) noexcept {
            uint nowVertex;
            if (useVertex(4, nowVertex)) {
                pushRectIndex(isAdd, nowVertex);
                Float2 min = center - (half * texRepeatTime);
                Float2 max = center + (half * texRepeatTime);
                auto ptr = vertexStart() + nowVertex;
                new (ptr) vertex_type(Float3(min.x, min.y, posZ), Float2(0, 0));
                ptr++;
                new (ptr) vertex_type(Float3(min.x, max.y, posZ), Float2(0, texRepeatTime.y));
                ptr++;
                new (ptr) vertex_type(Float3(max.x, max.y, posZ), Float2(texRepeatTime.x, texRepeatTime.y));
                ptr++;
                new (ptr) vertex_type(Float3(max.x, min.y, posZ), Float2(texRepeatTime.x, 0));
            }
        }
    };

    struct VertexPosRgbTex
        : VertexBase<VertexPosRgbTex, Float3, Float3, Float2>
    {
        Float3 pos;
        Float3 rgb;
        Float2 tex; //纹理坐标

        constexpr VertexPosRgbTex() noexcept : pos(), rgb(), tex() {}
        constexpr VertexPosRgbTex(const Float3& pos, const Float3& rgb, const Float2& tex) noexcept : pos(pos), rgb(rgb), tex(tex) {}
        constexpr VertexPosRgbTex(float posX, float posY, float posZ, float r, float g, float b, float texX, float texY) noexcept
            : pos(posX, posY, posZ), rgb(r, g, b), tex(texX, texY) {}
    };
    template <>
    class VertexIndexEditor<VertexPosRgbTex> : public VertexIndexEditorTemplate<VertexPosRgbTex> {
    public:
        //添加一个矩形
        void pushRect(bool isAdd, const Float2& center, const Float2& half, float posZ, const Float2& texRepeatTime = Float2(1, 1)) noexcept {
            uint nowVertex;
            if (useVertex(4, nowVertex)) {
                pushRectIndex(isAdd, nowVertex);
                Float2 min = center - (half * texRepeatTime);
                Float2 max = center + (half * texRepeatTime);
                auto ptr = vertexStart() + nowVertex;
                new (ptr) vertex_type(Float3(min.x, min.y, posZ), Float3(), Float2(0, 0));
                ptr++;
                new (ptr) vertex_type(Float3(min.x, max.y, posZ), Float3(), Float2(0, texRepeatTime.y));
                ptr++;
                new (ptr) vertex_type(Float3(max.x, max.y, posZ), Float3(), Float2(texRepeatTime.x, texRepeatTime.y));
                ptr++;
                new (ptr) vertex_type(Float3(max.x, min.y, posZ), Float3(), Float2(texRepeatTime.x, 0));
            }
        }
    };

    struct VertexPosNormalRgbaTex
        : VertexBase<VertexPosNormalRgbaTex, Float3, Float3, Float4, Float2>
    {
        Float3 pos;
        Float3 normal;
        Float4 rgba;
        Float2 tex; //纹理坐标

        constexpr VertexPosNormalRgbaTex() noexcept : pos(), rgba(), tex() {}
        constexpr VertexPosNormalRgbaTex(const Float3& pos, const Float3& normal, const Float4& rgba, const Float2& tex) noexcept
            : pos(pos), normal(normal), rgba(rgba), tex(tex) {}
        constexpr VertexPosNormalRgbaTex(float posX, float posY, float posZ, float normalX, float normalY, float normalZ, float r, float g, float b, float a, float texX, float texY) noexcept
            : pos(posX, posY, posZ), normal(normalX, normalY, normalZ), rgba(r, g, b, a), tex(texX, texY) {}
    };
    template <>
    class VertexIndexEditor<VertexPosNormalRgbaTex> : public VertexIndexEditorTemplate<VertexPosNormalRgbaTex> {
    public:
        //添加一个矩形
        void pushRect(bool isAdd, const Float2& center, const Float2& half, float posZ, const Float2& texRepeatTime = Float2(1, 1) ) noexcept {
            uint nowVertex;
            if (useVertex(4, nowVertex)) {
                pushRectIndex(isAdd, nowVertex);
                Float2 min = center - (half * texRepeatTime);
                Float2 max = center + (half * texRepeatTime);
                auto ptr = vertexStart() + nowVertex;
                new (ptr) vertex_type(Float3(min.x, min.y, posZ), Float3(0, 0, 1), Float4(), Float2(0, 0));
                ptr++;
                new (ptr) vertex_type(Float3(min.x, max.y, posZ), Float3(0, 0, 1), Float4(), Float2(0, texRepeatTime.y));
                ptr++;
                new (ptr) vertex_type(Float3(max.x, max.y, posZ), Float3(0, 0, 1), Float4(), Float2(texRepeatTime.x, texRepeatTime.y));
                ptr++;
                new (ptr) vertex_type(Float3(max.x, min.y, posZ), Float3(0, 0, 1), Float4(), Float2(texRepeatTime.x, 0));
            }
        }
    };

    struct VertexPosTex3
        : VertexBase<VertexPosTex3, Float3, Float3>
    {
        Float3 pos;
        Float3 tex; //纹理坐标，z值为纹理数组下标

        constexpr VertexPosTex3() noexcept : pos(), tex() {}
        constexpr VertexPosTex3(const Float3& pos, const Float3& tex) noexcept : pos(pos), tex(tex) {}
        constexpr VertexPosTex3(float posX, float posY, float posZ, float texX, float texY, float texZ) noexcept
            : pos(posX, posY, posZ), tex(texX, texY, texZ) {}
    };
    template <>
    class VertexIndexEditor<VertexPosTex3> : public VertexIndexEditorTemplate<VertexPosTex3> {
    public:
        //添加一个矩形
        void pushRect(bool isAdd, const Float2& center, const Float2& half, float posZ, float texZ, const Float2& texRepeatTime = Float2(1, 1)) noexcept {
            uint nowVertex;
            if (useVertex(4, nowVertex)) {
                pushRectIndex(isAdd, nowVertex);
                Float2 min = center - (half * texRepeatTime);
                Float2 max = center + (half * texRepeatTime);
                auto ptr = vertexStart() + nowVertex;
                new (ptr) vertex_type(Float3(min.x, min.y, posZ), Float3(0, 0, texZ));
                ptr++;
                new (ptr) vertex_type(Float3(min.x, max.y, posZ), Float3(0, texRepeatTime.y, texZ));
                ptr++;
                new (ptr) vertex_type(Float3(max.x, max.y, posZ), Float3(texRepeatTime.x, texRepeatTime.y, texZ));
                ptr++;
                new (ptr) vertex_type(Float3(max.x, min.y, posZ), Float3(texRepeatTime.x, 0, texZ));
            }
        }
    };

    struct VertexPosTexId
        : VertexBase<VertexPosTexId, Float3, Float2, uint>
    {
        Float3 pos;
        Float2 tex;
        uint id;

        constexpr VertexPosTexId() noexcept : pos(), tex(), id(0) {}
        constexpr VertexPosTexId(const Float3& pos, const Float2& tex, uint id) noexcept : pos(pos), tex(tex), id(id) {}
        constexpr VertexPosTexId(float posX, float posY, float posZ, float texX, float texY, uint id) noexcept
            : pos(posX, posY, posZ), tex(texX, texY), id(id) {}
    };
    template <>
    class VertexIndexEditor<VertexPosTexId> : public VertexIndexEditorTemplate<VertexPosTexId> {
    public:
        //添加一个矩形
        void pushRect(bool isAdd, const Float2& center, const Float2& half, float posZ, uint id, const Float2& texRepeatTime = Float2(1, 1)) noexcept {
            uint nowVertex;
            if (useVertex(4, nowVertex)) {
                pushRectIndex(isAdd, nowVertex);
                Float2 min = center - (half * texRepeatTime);
                Float2 max = center + (half * texRepeatTime);
                auto ptr = vertexStart() + nowVertex;
                new (ptr) vertex_type(Float3(min.x, min.y, posZ), Float2(0, 0), id);
                ptr++;
                new (ptr) vertex_type(Float3(min.x, max.y, posZ), Float2(0, texRepeatTime.y), id);
                ptr++;
                new (ptr) vertex_type(Float3(max.x, max.y, posZ), Float2(texRepeatTime.x, texRepeatTime.y), id);
                ptr++;
                new (ptr) vertex_type(Float3(max.x, min.y, posZ), Float2(texRepeatTime.x, 0), id);
            }
        }
    };

    struct CubeVertex
        : VertexBase<CubeVertex, uint, uint>
    {
        union {
            uint tex;
            uid freeList;
        };
        //8bit对应宽高（默认宽高为1最大16），12bit对应区块内位置，2bit对应block::FaceRotate，3bit对应Face3
        //000000000wwwwhhhhzzzzyyyyxxxxrrfff
        uint data;

        constexpr CubeVertex() noexcept : tex(0), data(0) {};
        constexpr CubeVertex(const CubeVertex& a) noexcept = default;
        constexpr CubeVertex(CubeVertex&& a) noexcept = default;
        constexpr CubeVertex& operator =(const CubeVertex& a) noexcept = default;
        constexpr CubeVertex& operator =(CubeVertex&& a) noexcept = default;

        constexpr CubeVertex(uint tex, block::CBPos pos, FRotate rotate, Face3 face3) noexcept
            : tex(tex),
            data((pos << pos_bit_offset) | (static_cast<uint>(rotate) << rotate_bit_offset) | static_cast<uint>(face3)) {}


        static constexpr uint pos_bit_offset = 5;
        static constexpr uint rotate_bit_offset = 3;
        static constexpr uint compare_pos_mask = block::cbpos_max << pos_bit_offset;
        static constexpr uint rotate_mask = 0b11;
        static constexpr uint rotate_set_mask = rotate_mask << rotate_bit_offset;
        static constexpr uint rotate_set_mask_inverse = ~rotate_set_mask;

        constexpr block::CBPos pos() const noexcept {
            return block::CBPos((data >> pos_bit_offset) & block::cbpos_max);
        }
        constexpr FRotate rotate() const noexcept {
            return FRotate((data >> rotate_bit_offset) & rotate_mask);
        }
        constexpr void setRotate(FRotate rotate) noexcept {
            data = data & rotate_set_mask_inverse | (static_cast<uint>(rotate) << rotate_bit_offset);
        }

        //位置是否相等
        constexpr bool posEqual(const CubeVertex& a) const noexcept {
            return (data & compare_pos_mask) == (a.data & compare_pos_mask);
        }
        constexpr bool posEqual(uint data_) const noexcept {
            return (data & compare_pos_mask) == (data_ & compare_pos_mask);
        }
        //返回位置是否小于、等于、大于a，小于返回-1，等于返回0，大于返回1
        constexpr schar comparePos(const CubeVertex& a) const noexcept {
            uint d = data & compare_pos_mask;
            uint ad = a.data & compare_pos_mask;
            if (d < ad) {
                return -1;
            }
            if (d == ad) {
                return 0;
            }
            return 1;
        }

        static constexpr uint posToData(block::CBPos pos) noexcept {
            return pos.v << pos_bit_offset;
        }
    };

    struct PlantVertex
        : VertexBase<PlantVertex, uint, uint>
    {
        union {
            uint tex;
            uid freeList;
        };
        uint data;

        constexpr PlantVertex() noexcept : tex(0), data(0) {};
        constexpr PlantVertex(uint tex, block::CBPos pos) noexcept
            : tex(tex), data(pos.v) {}

        constexpr block::CBPos pos() const noexcept {
            return block::CBPos(data);
        }

        //位置是否相等
        constexpr bool posEqual(const PlantVertex& a) const noexcept {
            return data == a.data;
        }
        constexpr bool posEqual(uint data_) const noexcept {
            return data == data_;
        }
        //返回位置是否小于、等于、大于a，小于返回-1，等于返回0，大于返回1
        constexpr schar comparePos(const PlantVertex& a) const noexcept {
            if (data < a.data) {
                return -1;
            }
            if (data == a.data) {
                return 0;
            }
            return 1;
        }

        static constexpr uint posToData(block::CBPos pos) noexcept {
            return pos.v;
        }
    };

    struct RectUIVertex
        : VertexBase<RectUIVertex, Float2, Int3>
    {
        Float2 pos;
        Int3 tex; //纹理坐标，z值为纹理数组下标

        constexpr RectUIVertex() noexcept : pos(), tex() {}
        constexpr RectUIVertex(const Float2& pos, const Int3& tex) noexcept : pos(pos), tex(tex) {}
        constexpr RectUIVertex(float posX, float posY, sint texX, sint texY, sint texZ) noexcept
            : pos(posX, posY), tex(texX, texY, texZ) {}
    };

    struct UI3Vertex
        : VertexBase<UI3Vertex, Float3, Float2, Float3, Int3>
    {
        Float3 pos; //3d顶点坐标
        Float2 ui; //ui坐标
        Float3 normal; //法线
        Int3 tex; //纹理坐标，z值为纹理数组下标

        constexpr UI3Vertex() noexcept {}
        constexpr UI3Vertex(const Float3& pos, const Float2& ui, const Float3& normal, const Int3& tex) noexcept
            : pos(pos), ui(ui), normal(normal), tex(tex) {}
        constexpr UI3Vertex(float posX, float posY, float posZ, float uiX, float uiY, float normalX, float normalY, float normalZ, sint texX, sint texY, sint texZ) noexcept
            : pos(posX, posY, posZ), ui(uiX, uiY), normal(normalX, normalY, normalZ), tex(texX, texY, texZ) {}
    };

    struct VertexBlockItemIcon
        : VertexBase<VertexBlockItemIcon, Float3, Float2, Float3, Float3>
    {
        Float3 pos; //3d顶点坐标
        Float2 ui; //ui坐标
        Float3 normal; //法线
        Float3 tex; //纹理坐标，z值为纹理数组下标

        constexpr VertexBlockItemIcon() noexcept {}
        constexpr VertexBlockItemIcon(const Float3& pos, const Float2& ui, const Float3& normal, const Float3& tex) noexcept
            : pos(pos), ui(ui), normal(normal), tex(tex) {}
        constexpr VertexBlockItemIcon(float posX, float posY, float posZ, float uiX, float uiY, float normalX, float normalY, float normalZ, float texX, float texY, float texZ) noexcept
            : pos(posX, posY, posZ), ui(uiX, uiY), normal(normalX, normalY, normalZ), tex(texX, texY, texZ) {}
    };
}