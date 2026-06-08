#include <dme/math/geom/ray.h>
namespace dme::math {
	void Ray2::update() noexcept {
		invDirection = direction.inversion();
		sign = Bool2(direction.x < 0, direction.y < 0);
	}
	Float2 Ray2::interceptPoint(float lineLength) const noexcept {
		//direction没有归一化则需要return origin + direction * (lineLength / direction.length())
		return origin + direction * lineLength;
	}

	void Ray3::update() noexcept {
		invDirection = direction.inversion();
		sign = Bool3(direction.x < 0, direction.y < 0, direction.z < 0);
	}
	Float3 Ray3::interceptPoint(float lineLength) const noexcept {
		//direction没有归一化则需要return origin + direction * (lineLength / direction.length())
		return origin + direction * lineLength;
	}

	void Ray3f::update() noexcept {
		invDirection = direction.inversion();
		sign = Bool3(direction.x < 0, direction.y < 0, direction.z < 0);
	}
	Fix3 Ray3f::interceptPoint(float lineLength) const noexcept {
		//direction没有归一化则需要return origin + direction * (lineLength / direction.length())
		return origin + direction * lineLength;
	}
	void Ray3f::rayCastVoxel(float maxRange, const std::function<void(bool&, const Ray3fTarget&)>& callback) const {
		RayCastVoxel(origin, direction, invDirection, maxRange, callback);
	}
	void Ray3f::RayCastVoxel(const Fix3& origin, const Float3& dir, const Float3& invDir, float maxRange, const std::function<void(bool&, const Ray3fTarget&)>& callback) {
		Ray3fTarget target;
		target.gridPos = origin.i;
		Char3 step;
		Char3 dirNegative; //将网格坐标转换为实际坐标时需要加的数
		Float3 tMax;
		Float3 tDelta;
		Face3 faceX; Face3 faceY; Face3 faceZ;
		if (dir.x == 0) {
			step.x = 0;
			dirNegative.x = 0;

			tMax.x = float_pinf;
			tDelta.x = float_pinf;
		}
		else {
			if (dir.x > 0) {
				step.x = 1;
				dirNegative.x = 0;
				faceX = Face3c::Left;

				tMax.x = (1 - origin.f.x) * invDir.x;
				tDelta.x = invDir.x;
			}
			else {
				step.x = -1;
				dirNegative.x = 1;
				faceX = Face3c::Right;

				tMax.x = (-origin.f.x) * invDir.x;
				tDelta.x = -invDir.x;
			}
		}

		if (dir.y == 0) {
			step.y = 0;
			dirNegative.y = 0;

			tMax.y = float_pinf;
			tDelta.y = float_pinf;
		}
		else {
			if (dir.y > 0) {
				step.y = 1;
				dirNegative.y = 0;
				faceY = Face3c::Back;

				tMax.y = (1 - origin.f.y) * invDir.y;
				tDelta.y = invDir.y;
			}
			else {
				step.y = -1;
				dirNegative.y = 1;
				faceY = Face3c::Front;

				tMax.y = (-origin.f.y) * invDir.y;
				tDelta.y = -invDir.y;
			}
		}

		if (dir.z == 0) {
			step.z = 0;
			dirNegative.z = 0;

			tMax.z = float_pinf;
			tDelta.z = float_pinf;
		}
		else {
			if (dir.z > 0) {
				step.z = 1;
				dirNegative.z = 0;
				faceZ = Face3c::Bottom;

				tMax.z = (1 - origin.f.z) * invDir.z;
				tDelta.z = invDir.z;
			}
			else {
				step.z = -1;
				dirNegative.z = 1;
				faceZ = Face3c::Top;

				tMax.z = (-origin.f.z) * invDir.z;
				tDelta.z = -invDir.z;
			}
		}

		Int3 endPos = (origin + (dir * maxRange)).i;
		bool isBreak = false;
		float temp = 0;
		goto last;
		while (target.gridPos != endPos) {
			if (tMax.z < tMax.y) {
				if (tMax.x < tMax.z) {
					goto x_is_min;
				}
				else {
					goto z_is_min;
				}
			}
			else {
				if (tMax.x < tMax.y) {
					goto x_is_min;
				}
				else {
					goto y_is_min;
				}
			}
		x_is_min:
			target.gridPos.x += step.x;
			tMax.x += tDelta.x;
			target.face = faceX;

			target.point.x = target.gridPos.x + dirNegative.x;
			target.point.xf = 0;

			temp = ((target.point.x - origin.x) - origin.xf) * invDir.x;
			target.point.y = origin.y;
			target.point.yf = origin.yf + temp * dir.y;
			target.point.updateY();

			target.point.z = origin.z;
			target.point.zf = origin.zf + temp * dir.z;
			target.point.updateZ();
			goto last;
		y_is_min:
			target.gridPos.y += step.y;
			tMax.y += tDelta.y;
			target.face = faceY;

			target.point.y = target.gridPos.y + dirNegative.y;
			target.point.yf = 0;

			temp = ((target.point.y - origin.y) - origin.yf) * invDir.y;
			target.point.x = origin.x;
			target.point.xf = origin.xf + temp * dir.x;
			target.point.updateX();

			target.point.z = origin.z;
			target.point.zf = origin.zf + temp * dir.z;
			target.point.updateZ();
			goto last;
		z_is_min:
			target.gridPos.z += step.z;
			tMax.z += tDelta.z;
			target.face = faceZ;

			target.point.z = target.gridPos.z + dirNegative.z;
			target.point.zf = 0;

			temp = ((target.point.z - origin.z) - origin.zf) * invDir.z;
			target.point.x = origin.x;
			target.point.xf = origin.xf + temp * dir.x;
			target.point.updateX();

			target.point.y = origin.y;
			target.point.yf = origin.yf + temp * dir.y;
			target.point.updateY();
			goto last;
		last:
			callback(isBreak, target);
			if (isBreak) {
				return;
			}
		}
	}

}