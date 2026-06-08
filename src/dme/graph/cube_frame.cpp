#include <dme/graph/cube_frame.h>
#include <dme/graph/shader.h>
#include <dme/graph/fix_camera.h>

namespace dme::graph {

    const Float3 CubeFrame::vertices[] = {
        Float3(0, 0, 0),
        Float3(1, 0, 0),
        Float3(0, 1, 0),
        Float3(1, 1, 0),
        Float3(0, 0, 1),
        Float3(1, 0, 1),
        Float3(0, 1, 1),
        Float3(1, 1, 1),

        Float3(0, 0, 0),
        Float3(0, 1, 0),
        Float3(0, 0, 1),
        Float3(0, 1, 1),
        Float3(1, 0, 0),
        Float3(1, 1, 0),
        Float3(1, 0, 1),
        Float3(1, 1, 1),

        Float3(0, 0, 0),
        Float3(0, 0, 1),
        Float3(1, 0, 0),
        Float3(1, 0, 1),
        Float3(0, 1, 0),
        Float3(0, 1, 1),
        Float3(1, 1, 0),
        Float3(1, 1, 1),
    };

    CubeFrame::CubeFrame() : vao(0), vbo(0), isDisplay(false) {
        init();
    }
    void CubeFrame::init() {
        if (!vao) {
            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &vbo);
            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Float3), (void*)0);
        }
    }
    void CubeFrame::release() {
        if (vao) {
            glDeleteVertexArrays(1, &vao);
            glDeleteBuffers(1, &vbo);
            vao = 0;
            vbo = 0;
        }
    }

    bool CubeFrame::getIsDisplay() const {
        return isDisplay;
    }
    void CubeFrame::display() {
        isDisplay = true;
    }
    void CubeFrame::hide() {
        isDisplay = false;
    }
    Int3 CubeFrame::getPos() const {
        return pos;
    }
    void CubeFrame::setPos(const Int3& pos_) {
        pos = pos_;
    }

    //目前不需要缩放
    static Matrix4 _model = Matrix4::Scale(Float3(1.01f));

    void CubeFrame::draw(const CameraF& camera) const {
        if (!isDisplay || !vao) {
            return;
        }
        //glDisable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        //glDepthFunc(GL_ALWAYS);

        //绘制方块边缘
        auto& shader = Shaders::CubeFrame();
        shader.use();
        shader.setInt3("pos", pos);
        shader.setInt3("cameraPosInt3", camera.getPosInt3());
        shader.setMatrix("projection", camera.getProjection());
        shader.setMatrix("view", camera.getView());
        //shader.setMatrix("model", _model);
        glBindVertexArray(vao);
        glDrawArrays(GL_LINES, 0, vertexCount);

        glBindVertexArray(0);
        glDepthFunc(GL_LESS);
        //glEnable(GL_DEPTH_TEST);
    }

}