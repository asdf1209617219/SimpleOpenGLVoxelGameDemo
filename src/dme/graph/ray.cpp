#include <dme/graph/ray.h>
#include <dme/graph/shader.h>

namespace dme::graph {

    Ray::Ray() : vao(0), vbo(0), isDisplay(false) {
        init();
    }
    void Ray::init() {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(start) * 2, &start, GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * float_size, (void*)0);
    }
    bool Ray::getIsDisplay() const {
        return isDisplay;
    }
    void Ray::display() {
        isDisplay = true;
    }
    void Ray::hide() {
        isDisplay = false;
    }
    void Ray::updateStart(const Float3& start_) {
        start = start_;
    }
    void Ray::updateEnd(const Float3& end_) {
        end = end_;
    }

    void Ray::draw(const Matrix4& projection, const Matrix4& view) const {
        if (!isDisplay) {
            return;
        }
        glDisable(GL_DEPTH_TEST);

        auto& shader = Shaders::Ray();
        shader.use();
        shader.setMatrix("projection", projection);
        shader.setMatrix("view", view);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(start) * 2, &start, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_LINES, 0, 2);

        glBindVertexArray(0);
        glEnable(GL_DEPTH_TEST);
    }
    Ray::~Ray() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
    }

}