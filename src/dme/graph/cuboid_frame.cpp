#include <dme/graph/cuboid_frame.h>
#include <dme/graph/shader.h>

namespace dme::graph {

    const uint CuboidFrame::indices[] = {
        //right
        25,1,29,25,29,28,  29,5,31,29,31,30,  31,7,27,31,27,26,  27,3,25,27,25,24,
        24,28,13,24,13,9,  28,30,15,28,15,13, 30,26,11,30,11,15, 26,24,9,26,9,11,
        //left
        19,2,23,19,23,22,  23,6,21,23,21,20,  21,4,17,21,17,16,  17,0,19,17,19,18,
        18,22,14,18,14,10, 22,20,12,22,12,14, 20,16,8,20,8,12,   16,18,10,16,10,8,
        //back
        43,3,47,43,47,46,  47,7,45,47,45,44,  45,6,41,45,41,40,  41,2,43,41,43,42,
        42,46,15,42,15,11, 46,44,14,46,14,15, 44,40,10,44,10,14, 40,42,11,40,11,10,
        //front
        33,0,37,33,37,36,  37,4,39,37,39,38,  39,5,35,39,35,34,  35,1,33,35,33,32,
        32,36,12,32,12,8,  36,38,13,36,13,12, 38,34,9,38,9,13,   34,32,8,34,8,9,
        //top
        57,4,61,57,61,60,  61,6,63,61,63,62,  63,7,59,63,59,58,  59,5,57,59,57,56,
        56,60,14,56,14,12, 60,62,15,60,15,14, 62,58,13,62,13,15, 58,56,12,58,12,13,
        //bottom
        53,2,49,53,49,48,  49,0,51,49,51,50,  51,1,55,51,55,54,  55,3,53,55,53,52,
        52,48,8,52,8,10,   48,50,9,48,9,8,    50,54,11,50,11,9,  54,52,10,54,10,11,
    };

    void CuboidFrame::changeVertices(const Float3& size) {
        //outside
        vertices[0] = Float3(0, 0, 0); // bottom-front-left      0
        vertices[1] = Float3(size.x, 0, 0); // bottom-front-right     1
        vertices[2] = Float3(0, size.y, 0); // bottom-back-left       2
        vertices[3] = Float3(size.x, size.y, 0); // bottom-back-right      3
        vertices[4] = Float3(0, 0, size.z); // top-front-left         4
        vertices[5] = Float3(size.x, 0, size.z); // top-front-right        5
        vertices[6] = Float3(0, size.y, size.z); // top-back-left          6
        vertices[7] = Float3(size.x, size.y, size.z); // top-back-right         7
        //inside
        vertices[8] = Float3(aa, aa, aa); // bottom-front-left   8
        vertices[9] = Float3(size.x - aa, aa, aa); // bottom-front-right  9
        vertices[10] = Float3(aa, size.y - aa, aa); // bottom-back-left    10
        vertices[11] = Float3(size.x - aa, size.y - aa, aa); // bottom-back-right   11
        vertices[12] = Float3(aa, aa, size.z - aa); // top-front-left      12
        vertices[13] = Float3(size.x - aa, aa, size.z - aa); // top-front-right     13
        vertices[14] = Float3(aa, size.y - aa, size.z - aa); // top-back-left       14
        vertices[15] = Float3(size.x - aa, size.y - aa, size.z - aa); // top-back-right      15
        //left-inside
        vertices[16] = Float3(0, aa, aa); // bottom-front         16
        vertices[17] = Float3(0, 0, aa); // bottom-front-out     17
        vertices[18] = Float3(0, size.y - aa, aa); // bottom-back          18
        vertices[19] = Float3(0, size.y - aa, 0); // bottom-back-out      19
        vertices[20] = Float3(0, aa, size.z - aa); // top-front            20
        vertices[21] = Float3(0, aa, size.z); // top-front-out        21
        vertices[22] = Float3(0, size.y - aa, size.z - aa); // top-back             22
        vertices[23] = Float3(0, size.y, size.z - aa); // top-back-out         23
        //right-inside
        vertices[24] = Float3(size.x, aa, aa); // bottom-front         24
        vertices[25] = Float3(size.x, aa, 0); // bottom-front-out     25
        vertices[26] = Float3(size.x, size.y - aa, aa); // bottom-back          26
        vertices[27] = Float3(size.x, size.y, aa); // bottom-back-out      27
        vertices[28] = Float3(size.x, aa, size.z - aa); // top-front            28
        vertices[29] = Float3(size.x, 0, size.z - aa); // top-front-out        29
        vertices[30] = Float3(size.x, size.y - aa, size.z - aa); // top-back             30
        vertices[31] = Float3(size.x, size.y - aa, size.z); // top-back-out         31
        //front-inside
        vertices[32] = Float3(aa, 0, aa); // bottom-left          32
        vertices[33] = Float3(aa, 0, 0); // bottom-left-out      33
        vertices[34] = Float3(size.x - aa, 0, aa); // bottom-right         34
        vertices[35] = Float3(size.x, 0, aa); // bottom-right-out     35
        vertices[36] = Float3(aa, 0, size.z - aa); // top-left             36
        vertices[37] = Float3(0, 0, size.z - aa); // top-left-out         37
        vertices[38] = Float3(size.x - aa, 0, size.z - aa); // top-right            38
        vertices[39] = Float3(size.x - aa, 0, size.z); // top-right-out        39
        //back-inside
        vertices[40] = Float3(aa, size.y, aa); // bottom-left          40
        vertices[41] = Float3(0, size.y, aa); // bottom-left-out      41
        vertices[42] = Float3(size.x - aa, size.y, aa); // bottom-right         42
        vertices[43] = Float3(size.x - aa, size.y, 0); // bottom-right-out     43
        vertices[44] = Float3(aa, size.y, size.z - aa); // top-left             44
        vertices[45] = Float3(aa, size.y, size.z); // top-left-out         45
        vertices[46] = Float3(size.x - aa, size.y, size.z - aa); // top-right            46
        vertices[47] = Float3(size.x, size.y, size.z - aa); // top-right-out        47
        //bottom-inside
        vertices[48] = Float3(aa, aa, 0); // front-left           48
        vertices[49] = Float3(0, aa, 0); // front-left-out       49
        vertices[50] = Float3(size.x - aa, aa, 0); // front-right          50
        vertices[51] = Float3(size.x - aa, 0, 0); // front-right-out      51
        vertices[52] = Float3(aa, size.y - aa, 0); // back-left            52
        vertices[53] = Float3(aa, size.y, 0); // back-left-out        53
        vertices[54] = Float3(size.x - aa, size.y - aa, 0); // back-right           54
        vertices[55] = Float3(size.x, size.y - aa, 0); // back-right-out       55
        //top-inside
        vertices[56] = Float3(aa, aa, size.z); // front-left           56
        vertices[57] = Float3(aa, 0, size.z); // front-left-out       57
        vertices[58] = Float3(size.x - aa, aa, size.z); // front-right          58
        vertices[59] = Float3(size.x, aa, size.z); // front-right-out      59
        vertices[60] = Float3(aa, size.y - aa, size.z); // back-left            60
        vertices[61] = Float3(0, size.y - aa, size.z); // back-left-out        61
        vertices[62] = Float3(size.x - aa, size.y - aa, size.z); // back-right           62
        vertices[63] = Float3(size.x - aa, size.y, size.z); // back-right-out       63

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Float3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    }

    CuboidFrame::CuboidFrame() : vao(0), vbo(0), ibo(0), isDisplay(false), vertices(), size() {
        init();
    }
    void CuboidFrame::init() {
        vertices.resize(vertexCount);

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ibo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Float3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * float_size, (void*)0);
    }
    bool CuboidFrame::getIsDisplay() const {
        return isDisplay;
    }
    void CuboidFrame::display() {
        isDisplay = true;
    }
    void CuboidFrame::hide() {
        isDisplay = false;
    }
    Float3 CuboidFrame::getPos() const {
        return pos;
    }
    void CuboidFrame::setPos(const Float3& pos_) {
        pos = pos_;
    }
    Float3 CuboidFrame::getColor() const {
        return color;
    }
    void CuboidFrame::setColor(const Float3& color_) {
        color = color_;
    }

    void CuboidFrame::draw(const Matrix4& projection, const Matrix4& view) const {
        if (!isDisplay) {
            return;
        }
        glDisable(GL_DEPTH_TEST);

        //绘制放大的方块
        auto& shader = Shaders::CubeFrame();
        shader.use();
        shader.setFloat3("pos", pos);
        shader.setFloat3("color", color);
        shader.setMatrix("projection", projection);
        shader.setMatrix("view", view);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
        glEnable(GL_DEPTH_TEST);
    }
    CuboidFrame::~CuboidFrame() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ibo);
    }

}