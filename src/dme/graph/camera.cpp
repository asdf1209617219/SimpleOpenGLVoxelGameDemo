#include <dme/graph/camera.h>
#include <dme/input/input.h>

namespace dme::graph {
	using input::Input;
	using input::InputAction;

	Camera::Camera(float fieldOfViewVertical, float nearPlaneDistance, float farPlaneDistance) noexcept
		: type(CameraType::Perspective),
		fieldOfViewVertical(fieldOfViewVertical),
		fieldOfViewVerticalRadians(math::radians(fieldOfViewVertical)),
		projection(Matrix4::Perspective(math::radians(fieldOfViewVertical), Game::GetWinAspectRatio(), nearPlaneDistance, farPlaneDistance)),
		nearPlaneDistance(nearPlaneDistance),
		farPlaneDistance(farPlaneDistance),

		position(),
		front(0, 1, 0),
		up(0, 0, 1),
		view(Matrix4::LookFront(Float3(), Float3(0, 1, 0), Float3(0, 0, 1))) {
	}
	Camera::Camera(float fieldOfViewVertical, float nearPlaneDistance, float farPlaneDistance, const Float3& pos, const Float3& front, const Float3& up) noexcept
		: type(CameraType::Perspective),
		fieldOfViewVertical(fieldOfViewVertical),
		fieldOfViewVerticalRadians(math::radians(fieldOfViewVertical)),
		projection(Matrix4::Perspective(math::radians(fieldOfViewVertical), Game::GetWinAspectRatio(), nearPlaneDistance, farPlaneDistance)),
		nearPlaneDistance(nearPlaneDistance),
		farPlaneDistance(farPlaneDistance),

		position(pos),
		front(front),
		up(up),
		view(Matrix4::LookFront(pos, front, up)) {
	}

	CameraType Camera::getType() const noexcept {
		return type;
	}
	const Matrix4& Camera::getProjection() const noexcept {
		return projection;
	}
	void Camera::setProjection(const Matrix4& p) noexcept {
		projection = p;
	}
	void Camera::updateAspectRatio(float aspect) noexcept {
		projection[0][0] = 1.0f / (aspect * std::tan(fieldOfViewVerticalRadians / 2.0f));
	}

	const Matrix4& Camera::getView() const noexcept {
		return view;
	}
	void Camera::setView(const Matrix4& v) noexcept {
		view = v;
	}
	void Camera::updatePosition(const Float3& pos) noexcept {
		position = pos;
		view = Matrix4::LookFront(pos, front, up);
	}
	void Camera::updateView(const Float3& pos, const Float3& f, const Float3& u) noexcept {
		position = pos;
		front = f;
		up = u;
		view = Matrix4::LookFront(pos, f, u);
	}


	const Float3& Camera::getPosition() const noexcept {
		return position;
	}
	const Float3& Camera::getFront() const noexcept {
		return front;
	}
	const Float3& Camera::getUp() const noexcept {
		return up;
	}

	Camera Camera::CreatePerspectiveCamera(float fieldOfViewVertical, float nearPlaneDistance, float farPlaneDistance) noexcept {
		return Camera(fieldOfViewVertical, nearPlaneDistance, farPlaneDistance);
	}
	Camera Camera::CreatePerspectiveCamera(float fieldOfViewVertical, float nearPlaneDistance, float farPlaneDistance, const Float3& pos, const Float3& front, const Float3& up) noexcept {
		return Camera(fieldOfViewVertical, nearPlaneDistance, farPlaneDistance, pos, front, up);
	}

}