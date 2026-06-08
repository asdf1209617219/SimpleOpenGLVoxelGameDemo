#pragma once
#include <dme/math/vector.h>
#include <dme/math/fix.h>
namespace dme {
	namespace math {
		struct Ray2;
		struct Ray3;
		struct Ray3f;

		//轴对齐包围盒2D(Axis-Aligned Bounding Box)
		struct Abox2 {
			union {
				struct {
					Float2 min;
					Float2 max;
				};
				Float2 vertices[2];
				float v[2 * Float2::axis];
			};

			constexpr Abox2() noexcept : min(), max() {}
			constexpr Abox2(const Float2& min, const Float2& max) noexcept : min(min), max(max) {}
			//通过center和half创建
			static constexpr Abox2 CreateCH(const Float2& center, const Float2& half) noexcept {
				return Abox2(center - half, center + half);
			}

			//获取当前物体的边长
			Float2 sideLength() const noexcept;
			//获取当前物体的中心点
			Float2 center() const noexcept;
			//获取当前物体的范围（长宽高的一半）
			Float2 extents() const noexcept;
			//根据另一个物体进行扩展
			void extend(const Abox2& other) noexcept;
			//将当前物体修改为合并两个物体的结果
			void combine(const Abox2& other1, const Abox2& other2) noexcept;
			//获取合并两个物体的结果
			static Abox2 GetCombine(const Abox2& other1, const Abox2& other2) noexcept;
			//当前物体是否包含另一个物体
			bool contains(const Abox2& other) const noexcept;
			//当前物体是否包含点，左闭右闭
			bool contains(const Float2& point) const noexcept;
			//当前物体是否包含点，左闭右开
			bool containsNotEqualMaxSide(const Float2& point) const noexcept;
			//当前物体是否包含点，左开右闭
			bool containsNotEqualMinSide(const Float2& point) const noexcept;
			//当前物体是否包含点，左开右开
			bool containsNotEqualSide(const Float2& point) const noexcept;

			//判断是否相邻或相交，包含两个物体刚好贴在一起的情况
			bool adjacentOrIntersect(const Abox2& other) const noexcept;

			//判断是否相交，不包含两个物体刚好贴在一起的情况
			bool intersect(const Abox2& other) const noexcept;
			//测试射线是否相交
			bool intersect(const Ray2& ray) const noexcept;
			//测试射线是否相交，限制有效的相交间隔
			bool intersect(const Ray2& ray, float intervalMin, float intervalMax) const noexcept;
			//测试射线是否相交，限制有效的相交间隔
			bool intersect(const Ray2& ray, float intervalMax) const noexcept;

			constexpr bool operator ==(const Abox2& a) const noexcept {
				return min == a.min && max == a.max;
			}
			friend std::ostream& operator<<(std::ostream& os, const Abox2& a) {
				return os << "min: " << a.min << ", max: " << a.max;
			}
			friend constexpr Hash Hasher(const Abox2& a) noexcept {
				return Hash::Combine(a.min, a.max);
			}
		};

		//轴对齐包围盒2D(Axis-Aligned Bounding Box)
		struct Abox2i {
			union {
				struct {
					Int2 min;
					Int2 max;
				};
				Int2 vertices[2];
				sint v[2 * Int2::axis];
			};

			constexpr Abox2i() noexcept : min(), max() {}
			constexpr Abox2i(const Int2& min, const Int2& max) noexcept : min(min), max(max) {}
			//通过center和half创建
			static constexpr Abox2i CreateCH(const Int2& center, const Int2& half) noexcept {
				return Abox2i(center - half, center + half);
			}

			/*
			//通过光栅化左上填充规则创建(top-left rule)
			static constexpr Abox2i CreateRasterization(const Abox2& box) noexcept {
				Int2 min = box.min.floori();
				Int2 max = box.max.floori();
				if (min.x + 0.5f < box.min.x) min.x++;
				if (min.y + 0.5f <= box.min.y) min.y++;
				if (max.x + 0.5f < box.max.x) max.x++;
				if (max.y + 0.5f <= box.max.y) max.y++;
				return Abox2i(min, max);
			}
			*/

			//获取当前物体的边长
			Int2 sideLength() const noexcept;
			//获取当前物体的中心点（舍去小数部分）
			Int2 center() const noexcept;
			//获取当前物体的范围（长宽高的一半，舍去小数部分）
			Int2 extents() const noexcept;
			//根据另一个物体进行扩展
			void extend(const Abox2i& other) noexcept;
			//将当前物体修改为合并两个物体的结果
			void combine(const Abox2i& other1, const Abox2i& other2) noexcept;
			//获取合并两个物体的结果
			static Abox2i GetCombine(const Abox2i& other1, const Abox2i& other2) noexcept;
			//当前物体是否包含另一个物体
			bool contains(const Abox2i& other) const noexcept;
			//当前物体是否包含点，左闭右闭
			bool contains(const Int2& point) const noexcept;
			//当前物体是否包含点，左闭右开
			bool containsNotEqualMaxSide(const Int2& point) const noexcept;
			//当前物体是否包含点，左开右闭
			bool containsNotEqualMinSide(const Int2& point) const noexcept;
			//当前物体是否包含点，左开右开
			bool containsNotEqualSide(const Int2& point) const noexcept;

			//判断是否相邻或相交，包含两个物体刚好贴在一起的情况
			bool adjacentOrIntersect(const Abox2i& other) const noexcept;

			//判断是否相交，不包含两个物体刚好贴在一起的情况
			bool intersect(const Abox2i& other) const noexcept;

			constexpr bool operator ==(const Abox2i& a) const noexcept {
				return min == a.min && max == a.max;
			}
			friend std::ostream& operator<<(std::ostream& os, const Abox2i& a) {
				return os << "min: " << a.min << ", max: " << a.max;
			}
			friend constexpr Hash Hasher(const Abox2i& a) noexcept {
				return Hash::Combine(a.min, a.max);
			}
		};

		//轴对齐包围盒3D(Axis-Aligned Bounding Box)
		struct Abox3 {
			union {
				struct {
					Float3 min;
					Float3 max;
				};
				Float3 vertices[2];
				float v[2 * Float3::axis];
			};

			constexpr Abox3() noexcept : min(), max() {}
			constexpr Abox3(const Float3& min, const Float3& max) noexcept : min(min), max(max) {}
			//通过center和half创建
			static constexpr Abox3 CreateCH(const Float3& center, const Float3& half) noexcept {
				return Abox3(center - half, center + half);
			}

			//获取当前物体的边长
			Float3 sideLength() const noexcept;
			//获取当前物体的中心点
			Float3 center() const noexcept;
			//获取当前物体的范围（长宽高的一半）
			Float3 extents() const noexcept;
			//根据另一个物体进行扩展
			void extend(const Abox3& other) noexcept;
			//将当前物体修改为合并两个物体的结果
			void combine(const Abox3& other1, const Abox3& other2) noexcept;
			//获取合并两个物体的结果
			static Abox3 GetCombine(const Abox3& other1, const Abox3& other2) noexcept;
			//当前物体是否包含另一个物体
			bool contains(const Abox3& other) const noexcept;
			//当前物体是否包含点
			bool contains(const Float3& point) const noexcept;

			//判断是否相邻或相交，包含两个物体刚好贴在一起的情况
			bool adjacentOrIntersect(const Abox3& other) const noexcept;

			//判断是否相交，不包含两个物体刚好贴在一起的情况
			bool intersect(const Abox3& other) const noexcept;
			//测试射线是否相交
			bool intersect(const Ray3& ray) const noexcept;
			//测试射线是否相交，限制有效的相交间隔
			bool intersect(const Ray3& ray, float intervalMin, float intervalMax) const noexcept;
			//测试射线是否相交，限制有效的相交间隔
			bool intersect(const Ray3& ray, float intervalMax) const noexcept;

			constexpr bool operator ==(const Abox3& a) const noexcept {
				return min == a.min && max == a.max;
			}
			friend std::ostream& operator<<(std::ostream& os, const Abox3& a) {
				return os << "min: " << a.min << ", max: " << a.max;
			}
			friend constexpr Hash Hasher(const Abox3& a) noexcept {
				return Hash::Combine(a.min, a.max);
			}
		};

		//定点轴对齐包围盒3D(Fixed-Axis-Aligned Bounding Box)
		struct Abox3f {
			union {
				struct {
					Fix3 min;
					Fix3 max;
				};
				Fix3 vertices[2];
			};

			constexpr Abox3f() noexcept : min(), max() {}
			constexpr Abox3f(const Fix3& min, const Fix3& max) noexcept : min(min), max(max) {}
			//通过center和half创建
			static constexpr Abox3f CreateCH(const Fix3& center, const Float3& half) noexcept {
				return Abox3f(center - half, center + half);
			}
			//通过center和half创建
			static constexpr Abox3f CreateCH(const Fix3& center, const Fix3& half) noexcept {
				return Abox3f(center - half, center + half);
			}

			//获取当前物体的边长
			Fix3 sideLength() const noexcept;
			//获取当前物体的中心点
			Fix3 center() const noexcept;
			//获取当前物体的范围（长宽高的一半）
			Fix3 extents() const noexcept;
			//根据另一个物体进行扩展
			void extend(const Abox3f& other) noexcept;
			//将当前物体修改为合并两个物体的结果
			void combine(const Abox3f& other1, const Abox3f& other2) noexcept;
			//获取合并两个物体的结果
			static Abox3f GetCombine(const Abox3f& other1, const Abox3f& other2) noexcept;
			//当前物体是否包含另一个物体
			bool contains(const Abox3f& other) const noexcept;
			//当前物体是否包含点
			bool contains(const Fix3& point) const noexcept;

			//判断是否相邻或相交，包含两个物体刚好贴在一起的情况
			bool adjacentOrIntersect(const Abox3f& other) const noexcept;

			//判断是否相交，不包含两个物体刚好贴在一起的情况
			bool intersect(const Abox3f& other) const noexcept;
			//测试射线是否相交
			bool intersect(const Ray3f& ray) const noexcept;
			//测试射线是否相交，限制有效的相交间隔
			bool intersect(const Ray3f& ray, float intervalMin, float intervalMax) const noexcept;
			//测试射线是否相交，限制有效的相交间隔
			bool intersect(const Ray3f& ray, float intervalMax) const noexcept;

			constexpr bool operator ==(const Abox3f& a) const noexcept {
				return min == a.min && max == a.max;
			}
			friend std::ostream& operator<<(std::ostream& os, const Abox3f& a) {
				return os << "min: " << a.min << ", max: " << a.max;
			}
			friend constexpr Hash Hasher(const Abox3f& a) noexcept {
				return Hash::Combine(a.min, a.max);
			}
		};
	}
	using math::Abox2;
	using math::Abox2i;
	using math::Abox3;
	using math::Abox3f;
}