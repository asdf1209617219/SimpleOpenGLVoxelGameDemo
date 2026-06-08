#include <dme/math/geom/box.h>
#include <dme/math/geom/ray.h>
namespace dme::math {

	Float2 Abox2::sideLength() const noexcept {
		return max - min;
	}
	Float2 Abox2::center() const noexcept {
		return (max + min) * 0.5f;
	}
	Float2 Abox2::extents() const noexcept {
		return (max - min) * 0.5f;
	}
	void Abox2::extend(const Abox2& other) noexcept {
		min = math::Min(min, other.min);
		max = math::Max(max, other.max);
	}
	void Abox2::combine(const Abox2& other1, const Abox2& other2) noexcept {
		min = math::Min(other1.min, other2.min);
		max = math::Max(other1.max, other2.max);
	}
	Abox2 Abox2::GetCombine(const Abox2& other1, const Abox2& other2) noexcept {
		return Abox2(math::Min(other1.min, other2.min), math::Max(other1.max, other2.max));
	}
	bool Abox2::contains(const Abox2& other) const noexcept {
		return min.x <= other.min.x && min.y <= other.min.y
			&& other.max.x <= max.x && other.max.y <= max.y;
	}
	bool Abox2::contains(const Float2& point) const noexcept {
		return min.x <= point.x && min.y <= point.y
			&& max.x >= point.x && max.y >= point.y;
	}
	bool Abox2::containsNotEqualMaxSide(const Float2& point) const noexcept {
		return min.x <= point.x && min.y <= point.y
			&& max.x > point.x && max.y > point.y;
	}
	bool Abox2::containsNotEqualMinSide(const Float2& point) const noexcept {
		return min.x < point.x && min.y < point.y
			&& max.x >= point.x && max.y >= point.y;
	}
	bool Abox2::containsNotEqualSide(const Float2& point) const noexcept {
		return min.x < point.x && min.y < point.y
			&& max.x > point.x && max.y > point.y;
	}

	bool Abox2::adjacentOrIntersect(const Abox2& other) const noexcept {
		return min.x <= other.max.x && max.x >= other.min.x
			&& min.y <= other.max.y && max.y >= other.min.y;
	}
	bool Abox2::intersect(const Abox2& other) const noexcept {
		return min.x < other.max.x && max.x > other.min.x
			&& min.y < other.max.y && max.y > other.min.y;
	}
	bool Abox2::intersect(const Ray2& ray) const noexcept {
		float tmin, tmax, tmin2, tmax2;
		tmin = (vertices[ray.sign.x].x - ray.origin.x) * ray.invDirection.x;
		tmax = (vertices[!ray.sign.x].x - ray.origin.x) * ray.invDirection.x;
		tmin2 = (vertices[ray.sign.y].y - ray.origin.y) * ray.invDirection.y;
		tmax2 = (vertices[!ray.sign.y].y - ray.origin.y) * ray.invDirection.y;
		if (tmin > tmax2 || tmin2 > tmax)
			return false;
		if (tmin2 > tmin)
			tmin = tmin2;
		if (tmax2 < tmax)
			tmax = tmax2;
		return tmax > 0.0f;
	}
	bool Abox2::intersect(const Ray2& ray, float intervalMin, float intervalMax) const noexcept {
		float tmin, tmax, tmin2, tmax2;
		tmin = (vertices[ray.sign.x].x - ray.origin.x) * ray.invDirection.x;
		tmax = (vertices[!ray.sign.x].x - ray.origin.x) * ray.invDirection.x;
		tmin2 = (vertices[ray.sign.y].y - ray.origin.y) * ray.invDirection.y;
		tmax2 = (vertices[!ray.sign.y].y - ray.origin.y) * ray.invDirection.y;
		if (tmin > tmax2 || tmin2 > tmax)
			return false;
		if (tmin2 > tmin)
			tmin = tmin2;
		if (tmax2 < tmax)
			tmax = tmax2;
		return tmin < intervalMax && tmax > intervalMin;
	}
	bool Abox2::intersect(const Ray2& ray, float intervalMax) const noexcept {
		float tmin, tmax, tmin2, tmax2;
		tmin = (vertices[ray.sign.x].x - ray.origin.x) * ray.invDirection.x;
		tmax = (vertices[!ray.sign.x].x - ray.origin.x) * ray.invDirection.x;
		tmin2 = (vertices[ray.sign.y].y - ray.origin.y) * ray.invDirection.y;
		tmax2 = (vertices[!ray.sign.y].y - ray.origin.y) * ray.invDirection.y;
		if (tmin > tmax2 || tmin2 > tmax)
			return false;
		if (tmin2 > tmin)
			tmin = tmin2;
		if (tmax2 < tmax)
			tmax = tmax2;
		return tmin < intervalMax && tmax > 0.0f;
	}



	Int2 Abox2i::sideLength() const noexcept {
		return max - min;
	}
	Int2 Abox2i::center() const noexcept {
		return (max + min) / 2;
	}
	Int2 Abox2i::extents() const noexcept {
		return (max - min) / 2;
	}
	void Abox2i::extend(const Abox2i& other) noexcept {
		min = math::Min(min, other.min);
		max = math::Max(max, other.max);
	}
	void Abox2i::combine(const Abox2i& other1, const Abox2i& other2) noexcept {
		min = math::Min(other1.min, other2.min);
		max = math::Max(other1.max, other2.max);
	}
	Abox2i Abox2i::GetCombine(const Abox2i& other1, const Abox2i& other2) noexcept {
		return Abox2i(math::Min(other1.min, other2.min), math::Max(other1.max, other2.max));
	}
	bool Abox2i::contains(const Abox2i& other) const noexcept {
		return min.x <= other.min.x && min.y <= other.min.y
			&& other.max.x <= max.x && other.max.y <= max.y;
	}
	bool Abox2i::contains(const Int2& point) const noexcept {
		return min.x <= point.x && min.y <= point.y
			&& max.x >= point.x && max.y >= point.y;
	}
	bool Abox2i::containsNotEqualMaxSide(const Int2& point) const noexcept {
		return min.x <= point.x && min.y <= point.y
			&& max.x > point.x && max.y > point.y;
	}
	bool Abox2i::containsNotEqualMinSide(const Int2& point) const noexcept {
		return min.x < point.x && min.y < point.y
			&& max.x >= point.x && max.y >= point.y;
	}
	bool Abox2i::containsNotEqualSide(const Int2& point) const noexcept {
		return min.x < point.x && min.y < point.y
			&& max.x > point.x && max.y > point.y;
	}

	bool Abox2i::adjacentOrIntersect(const Abox2i& other) const noexcept {
		return min.x <= other.max.x && max.x >= other.min.x
			&& min.y <= other.max.y && max.y >= other.min.y;
	}
	bool Abox2i::intersect(const Abox2i& other) const noexcept {
		return min.x < other.max.x && max.x > other.min.x
			&& min.y < other.max.y && max.y > other.min.y;
	}



	Float3 Abox3::sideLength() const noexcept {
		return max - min;
	}
	Float3 Abox3::center() const noexcept {
		return (max + min) * 0.5f;
	}
	Float3 Abox3::extents() const noexcept {
		return (max - min) * 0.5f;
	}
	void Abox3::extend(const Abox3& other) noexcept {
		min = math::Min(min, other.min);
		max = math::Max(max, other.max);
	}
	void Abox3::combine(const Abox3& other1, const Abox3& other2) noexcept {
		min = math::Min(other1.min, other2.min);
		max = math::Max(other1.max, other2.max);
	}
	Abox3 Abox3::GetCombine(const Abox3& other1, const Abox3& other2) noexcept {
		return Abox3(math::Min(other1.min, other2.min), math::Max(other1.max, other2.max));
	}
	bool Abox3::contains(const Abox3& other) const noexcept {
		return min.x <= other.min.x && min.y <= other.min.y && min.z <= other.min.z
			&& other.max.x <= max.x && other.max.y <= max.y && other.max.z <= max.z;
	}
	bool Abox3::contains(const Float3& point) const noexcept {
		return min.x <= point.x && min.y <= point.y && min.z <= point.z
			&& max.x >= point.x && max.y >= point.y && max.z >= point.z;
	}

	bool Abox3::adjacentOrIntersect(const Abox3& other) const noexcept {
		return min.x <= other.max.x && max.x >= other.min.x
			&& min.y <= other.max.y && max.y >= other.min.y
			&& min.z <= other.max.z && max.z >= other.min.z;
	}
	bool Abox3::intersect(const Abox3& other) const noexcept {
		return min.x < other.max.x && max.x > other.min.x
			&& min.y < other.max.y && max.y > other.min.y
			&& min.z < other.max.z && max.z > other.min.z;
	}
	bool Abox3::intersect(const Ray3& ray) const noexcept {
		float tmin, tmax, tmin2, tmax2;
		tmin = (vertices[ray.sign.x].x - ray.origin.x) * ray.invDirection.x;
		tmax = (vertices[!ray.sign.x].x - ray.origin.x) * ray.invDirection.x;
		tmin2 = (vertices[ray.sign.y].y - ray.origin.y) * ray.invDirection.y;
		tmax2 = (vertices[!ray.sign.y].y - ray.origin.y) * ray.invDirection.y;
		if (tmin > tmax2 || tmin2 > tmax)
			return false;
		if (tmin2 > tmin)
			tmin = tmin2;
		if (tmax2 < tmax)
			tmax = tmax2;
		tmin2 = (vertices[ray.sign.z].z - ray.origin.z) * ray.invDirection.z;
		tmax2 = (vertices[!ray.sign.z].z - ray.origin.z) * ray.invDirection.z;
		if (tmin > tmax2 || tmin2 > tmax)
			return false;
		if (tmin2 > tmin)
			tmin = tmin2;
		if (tmax2 < tmax)
			tmax = tmax2;
		return tmax > 0.0f;
	}
	bool Abox3::intersect(const Ray3& ray, float intervalMin, float intervalMax) const noexcept {
		float tmin, tmax, tmin2, tmax2;
		tmin = (vertices[ray.sign.x].x - ray.origin.x) * ray.invDirection.x;
		tmax = (vertices[!ray.sign.x].x - ray.origin.x) * ray.invDirection.x;
		tmin2 = (vertices[ray.sign.y].y - ray.origin.y) * ray.invDirection.y;
		tmax2 = (vertices[!ray.sign.y].y - ray.origin.y) * ray.invDirection.y;
		if (tmin > tmax2 || tmin2 > tmax)
			return false;
		if (tmin2 > tmin)
			tmin = tmin2;
		if (tmax2 < tmax)
			tmax = tmax2;
		tmin2 = (vertices[ray.sign.z].z - ray.origin.z) * ray.invDirection.z;
		tmax2 = (vertices[!ray.sign.z].z - ray.origin.z) * ray.invDirection.z;
		if (tmin > tmax2 || tmin2 > tmax)
			return false;
		if (tmin2 > tmin)
			tmin = tmin2;
		if (tmax2 < tmax)
			tmax = tmax2;
		return tmin < intervalMax && tmax > intervalMin;
	}
	bool Abox3::intersect(const Ray3& ray, float intervalMax) const noexcept {
		float tmin, tmax, tmin2, tmax2;
		tmin = (vertices[ray.sign.x].x - ray.origin.x) * ray.invDirection.x;
		tmax = (vertices[!ray.sign.x].x - ray.origin.x) * ray.invDirection.x;
		tmin2 = (vertices[ray.sign.y].y - ray.origin.y) * ray.invDirection.y;
		tmax2 = (vertices[!ray.sign.y].y - ray.origin.y) * ray.invDirection.y;
		if (tmin > tmax2 || tmin2 > tmax)
			return false;
		if (tmin2 > tmin)
			tmin = tmin2;
		if (tmax2 < tmax)
			tmax = tmax2;
		tmin2 = (vertices[ray.sign.z].z - ray.origin.z) * ray.invDirection.z;
		tmax2 = (vertices[!ray.sign.z].z - ray.origin.z) * ray.invDirection.z;
		if (tmin > tmax2 || tmin2 > tmax)
			return false;
		if (tmin2 > tmin)
			tmin = tmin2;
		if (tmax2 < tmax)
			tmax = tmax2;
		return tmin < intervalMax && tmax > 0.0f;
	}


	Fix3 Abox3f::sideLength() const noexcept {
		return max - min;
	}
	Fix3 Abox3f::center() const noexcept {
		return min.center(max);
	}
	Fix3 Abox3f::extents() const noexcept {
		Fix3 r = Fix3(max.f - min.f, max.i - min.i);
		r *= 0.5f;
		return r;
	}
	void Abox3f::extend(const Abox3f& other) noexcept {
		min = math::Min(min, other.min);
		max = math::Max(max, other.max);
	}
	void Abox3f::combine(const Abox3f& other1, const Abox3f& other2) noexcept {
		min = math::Min(other1.min, other2.min);
		max = math::Max(other1.max, other2.max);
	}
	Abox3f Abox3f::GetCombine(const Abox3f& other1, const Abox3f& other2) noexcept {
		return Abox3f(math::Min(other1.min, other2.min), math::Max(other1.max, other2.max));
	}
	bool Abox3f::contains(const Abox3f& other) const noexcept {
		return min.lessEqualX(other.min) && min.lessEqualY(other.min) && min.lessEqualZ(other.min)
			&& max.greaterEqualX(other.max) && max.greaterEqualY(other.max) && max.greaterEqualZ(other.max);
	}
	bool Abox3f::contains(const Fix3& point) const noexcept {
		return min.lessEqualX(point) && min.lessEqualY(point) && min.lessEqualZ(point)
			&& max.greaterEqualX(point) && max.greaterEqualY(point) && max.greaterEqualZ(point);
	}

	bool Abox3f::adjacentOrIntersect(const Abox3f& other) const noexcept {
		return min.lessEqualX(other.max) && max.greaterEqualX(other.min)
			&& min.lessEqualY(other.max) && max.greaterEqualY(other.min)
			&& min.lessEqualZ(other.max) && max.greaterEqualZ(other.min);
	}
	bool Abox3f::intersect(const Abox3f& other) const noexcept {
		return min.lessX(other.max) && max.greaterX(other.min)
			&& min.lessY(other.max) && max.greaterY(other.min)
			&& min.lessZ(other.max) && max.greaterZ(other.min);
	}

	bool Abox3f::intersect(const Ray3f& ray) const noexcept {
		float tmin, tmax, tmin2, tmax2;
		tmin = (vertices[ray.sign.x].subXToFloat(ray.origin)) * ray.invDirection.x;
		tmax = (vertices[!ray.sign.x].subXToFloat(ray.origin)) * ray.invDirection.x;
		tmin2 = (vertices[ray.sign.y].subYToFloat(ray.origin)) * ray.invDirection.y;
		tmax2 = (vertices[!ray.sign.y].subYToFloat(ray.origin)) * ray.invDirection.y;
		if (tmin > tmax2 || tmin2 > tmax)
			return false;
		if (tmin2 > tmin)
			tmin = tmin2;
		if (tmax2 < tmax)
			tmax = tmax2;
		tmin2 = (vertices[ray.sign.z].subZToFloat(ray.origin)) * ray.invDirection.z;
		tmax2 = (vertices[!ray.sign.z].subZToFloat(ray.origin)) * ray.invDirection.z;
		if (tmin > tmax2 || tmin2 > tmax)
			return false;
		if (tmin2 > tmin)
			tmin = tmin2;
		if (tmax2 < tmax)
			tmax = tmax2;
		return tmax > 0.0f;
	}
	bool Abox3f::intersect(const Ray3f& ray, float intervalMin, float intervalMax) const noexcept {
		float tmin, tmax, tmin2, tmax2;
		tmin = (vertices[ray.sign.x].subXToFloat(ray.origin)) * ray.invDirection.x;
		tmax = (vertices[!ray.sign.x].subXToFloat(ray.origin)) * ray.invDirection.x;
		tmin2 = (vertices[ray.sign.y].subYToFloat(ray.origin)) * ray.invDirection.y;
		tmax2 = (vertices[!ray.sign.y].subYToFloat(ray.origin)) * ray.invDirection.y;
		if (tmin > tmax2 || tmin2 > tmax)
			return false;
		if (tmin2 > tmin)
			tmin = tmin2;
		if (tmax2 < tmax)
			tmax = tmax2;
		tmin2 = (vertices[ray.sign.z].subZToFloat(ray.origin)) * ray.invDirection.z;
		tmax2 = (vertices[!ray.sign.z].subZToFloat(ray.origin)) * ray.invDirection.z;
		if (tmin > tmax2 || tmin2 > tmax)
			return false;
		if (tmin2 > tmin)
			tmin = tmin2;
		if (tmax2 < tmax)
			tmax = tmax2;
		return tmin < intervalMax && tmax > intervalMin;
	}
	bool Abox3f::intersect(const Ray3f& ray, float intervalMax) const noexcept {
		float tmin, tmax, tmin2, tmax2;
		tmin = (vertices[ray.sign.x].subXToFloat(ray.origin)) * ray.invDirection.x;
		tmax = (vertices[!ray.sign.x].subXToFloat(ray.origin)) * ray.invDirection.x;
		tmin2 = (vertices[ray.sign.y].subYToFloat(ray.origin)) * ray.invDirection.y;
		tmax2 = (vertices[!ray.sign.y].subYToFloat(ray.origin)) * ray.invDirection.y;
		if (tmin > tmax2 || tmin2 > tmax)
			return false;
		if (tmin2 > tmin)
			tmin = tmin2;
		if (tmax2 < tmax)
			tmax = tmax2;
		tmin2 = (vertices[ray.sign.z].subZToFloat(ray.origin)) * ray.invDirection.z;
		tmax2 = (vertices[!ray.sign.z].subZToFloat(ray.origin)) * ray.invDirection.z;
		if (tmin > tmax2 || tmin2 > tmax)
			return false;
		if (tmin2 > tmin)
			tmin = tmin2;
		if (tmax2 < tmax)
			tmax = tmax2;
		return tmin < intervalMax && tmax > 0.0f;
	}

}