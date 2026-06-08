#include <dme/graph/fix_camera.h>
#include <dme/input/input.h>

namespace dme::graph {
	using input::Input;
	using input::InputAction;

	CameraF::CameraF(float fieldOfViewVertical, float nearPlaneDistance, float farPlaneDistance) noexcept
		: type(CameraType::Perspective),
		fieldOfViewVertical(fieldOfViewVertical),
		fieldOfViewVerticalRadians(math::radians(fieldOfViewVertical)),
		projection(Matrix4::Perspective(math::radians(fieldOfViewVertical), Game::GetWinAspectRatio(), nearPlaneDistance, farPlaneDistance)),
		nearPlaneDistance(nearPlaneDistance),
		farPlaneDistance(farPlaneDistance),

		position(),
		front(Float3c::Forward),
		up(Float3c::Up),
		view(Matrix4::LookFront(Float3(), Float3c::Forward, Float3c::Up)) {
	}
	CameraF::CameraF(float fieldOfViewVertical, float nearPlaneDistance, float farPlaneDistance, const Fix3& pos, const Float3& front, const Float3& up) noexcept
		: type(CameraType::Perspective),
		fieldOfViewVertical(fieldOfViewVertical),
		fieldOfViewVerticalRadians(math::radians(fieldOfViewVertical)),
		projection(Matrix4::Perspective(math::radians(fieldOfViewVertical), Game::GetWinAspectRatio(), nearPlaneDistance, farPlaneDistance)),
		nearPlaneDistance(nearPlaneDistance),
		farPlaneDistance(farPlaneDistance),

		position(pos),
		front(front),
		up(up),
		view(Matrix4::LookFront(pos.f, front, up)) {
	}

	CameraType CameraF::getType() const noexcept {
		return type;
	}
	const Matrix4& CameraF::getProjection() const noexcept {
		return projection;
	}
	void CameraF::setProjection(const Matrix4& p) noexcept {
		projection = p;
	}
	void CameraF::updateAspectRatio(float aspect) noexcept {
		projection[0][0] = 1.0f / (aspect * std::tan(fieldOfViewVerticalRadians / 2.0f));
	}

	const Matrix4& CameraF::getView() const noexcept {
		return view;
	}
	void CameraF::setView(const Matrix4& v) noexcept {
		view = v;
	}
	void CameraF::updatePosition(const Fix3& pos) noexcept {
		position = pos;
		//view = Matrix4::LookFront(pos.f, front, up);
		view.lookFrontUpdatePos(pos.f);
	}
	void CameraF::updateView(const Fix3& pos, const Float3& f, const Float3& u) noexcept {
		position = pos;
		front = f;
		up = u;
		view = Matrix4::LookFront(pos.f, f, u);
	}


	const Fix3& CameraF::getPos() const noexcept {
		return position;
	}
	const Int3& CameraF::getPosInt3() const noexcept {
		return position.i;
	}
	const Float3& CameraF::getFront() const noexcept {
		return front;
	}
	const Float3& CameraF::getUp() const noexcept {
		return up;
	}

	CameraF CameraF::CreatePerspectiveCamera(float fieldOfViewVertical, float nearPlaneDistance, float farPlaneDistance) noexcept {
		return CameraF(fieldOfViewVertical, nearPlaneDistance, farPlaneDistance);
	}
	CameraF CameraF::CreatePerspectiveCamera(float fieldOfViewVertical, float nearPlaneDistance, float farPlaneDistance, const Fix3& pos, const Float3& front, const Float3& up) noexcept {
		return CameraF(fieldOfViewVertical, nearPlaneDistance, farPlaneDistance, pos, front, up);
	}

}