#pragma once
#include <dme/core.h>

namespace dme::graph {
    enum class CameraType {
        Perspective = 0,
        Orthographic = 1,
    };

    //摄像头的封装
    class Camera {
    private:
        CameraType type;
        Matrix4 projection;
        Matrix4 view;

        float fieldOfViewVertical;
        float fieldOfViewVerticalRadians;
        float nearPlaneDistance;
        float farPlaneDistance;

        Float3 position;
        Float3 front;
        Float3 up;

        void setProjection(const Matrix4& p) noexcept;
        void setView(const Matrix4& v) noexcept;

        Camera(float fieldOfViewVertical, float nearPlaneDistance, float farPlaneDistance) noexcept;
        Camera(float fieldOfViewVertical, float nearPlaneDistance, float farPlaneDistance, const Float3& pos, const Float3& front, const Float3& up) noexcept;
    public:
        CameraType getType() const noexcept;
        const Matrix4& getProjection() const noexcept;
        void updateAspectRatio(float aspect) noexcept;

        const Matrix4& getView() const noexcept;
        void updatePosition(const Float3& pos) noexcept;
        void updateView(const Float3& pos, const Float3& front, const Float3& up) noexcept;

        const Float3& getPosition() const noexcept;
        const Float3& getFront() const noexcept;
        const Float3& getUp() const noexcept;

        static Camera CreatePerspectiveCamera(float fieldOfViewVertical, float nearPlaneDistance, float farPlaneDistance) noexcept;
        static Camera CreatePerspectiveCamera(float fieldOfViewVertical, float nearPlaneDistance, float farPlaneDistance, const Float3& pos, const Float3& front, const Float3& up) noexcept;
        //static Camera createOrthographicCamera(float width, float height, float nearPlaneDistance, float farPlaneDistance);
    };

}