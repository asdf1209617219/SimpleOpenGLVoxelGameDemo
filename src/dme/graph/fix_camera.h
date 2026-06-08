#pragma once
#include <dme/graph/camera.h>

namespace dme::graph {
    //Fix版本的摄像头
    class CameraF {
    private:
        CameraType type;
        Matrix4 projection;
        Matrix4 view;

        float fieldOfViewVertical;
        float fieldOfViewVerticalRadians;
        float nearPlaneDistance;
        float farPlaneDistance;

        Fix3 position;
        Float3 front;
        Float3 up;

        void setProjection(const Matrix4& p) noexcept;
        void setView(const Matrix4& v) noexcept;

        CameraF(float fieldOfViewVertical, float nearPlaneDistance, float farPlaneDistance) noexcept;
        CameraF(float fieldOfViewVertical, float nearPlaneDistance, float farPlaneDistance, const Fix3& pos, const Float3& front, const Float3& up) noexcept;
    public:
        CameraType getType() const noexcept;
        const Matrix4& getProjection() const noexcept;
        void updateAspectRatio(float aspect) noexcept;

        const Matrix4& getView() const noexcept;
        void updatePosition(const Fix3& pos) noexcept;
        void updateView(const Fix3& pos, const Float3& front, const Float3& up) noexcept;

        const Fix3& getPos() const noexcept;
        const Int3& getPosInt3() const noexcept;
        const Float3& getFront() const noexcept;
        const Float3& getUp() const noexcept;

        static CameraF CreatePerspectiveCamera(float fieldOfViewVertical, float nearPlaneDistance, float farPlaneDistance) noexcept;
        static CameraF CreatePerspectiveCamera(float fieldOfViewVertical, float nearPlaneDistance, float farPlaneDistance, const Fix3& pos, const Float3& front, const Float3& up) noexcept;
        //static FixCamera createOrthographicCamera(float width, float height, float nearPlaneDistance, float farPlaneDistance);
    };

}