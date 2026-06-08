#pragma once
#include <dme/math/fix.h>
#include <dme/math/geom/base.h>
namespace dme::math {
	//Ray3f与体素相交的结果
	struct Ray3fTarget {
		bool hasTarget; //是否有相交结果
		Face3 face; //与体素相交的面
		Fix3 point; //相交的点
		Int3 gridPos; //体素网格的位置

		constexpr Ray3fTarget() noexcept : hasTarget(false), point(), gridPos(), face() {};
		constexpr Ray3fTarget(const Fix3& point, const Int3& gridPos, Face3 face) noexcept :
			hasTarget(true), point(point), gridPos(gridPos), face(face) {};

		//获取gridPos朝向face的位置
		constexpr Int3 getFaceNearPos() const noexcept {
			return face.nearPos(gridPos);
		}
	};

	//2维射线
	struct Ray2 {
		//原点
		Float2 origin;
		//方向（必须归一化），更新后需手动调用update函数更新invDirection以及sign的值
		Float2 direction;
		//direction的倒数（外部调用不要进行修改）
		Float2 invDirection;
		//如果方向大于等于0则为false，否则为true（外部调用不要进行修改）
		Bool2 sign;

		constexpr Ray2() noexcept : origin(), direction(), invDirection(), sign() {}
		//注意方向必须归一化
		constexpr Ray2(const Float2& origin, const Float2& normalizedDirection) noexcept : origin(origin), direction(normalizedDirection),
			invDirection(1.0f / normalizedDirection.x, 1.0f / normalizedDirection.y),
			sign(normalizedDirection.x < 0, normalizedDirection.y < 0) {
		}

		//更新invDirection和sign
		void update() noexcept;
		//传入线段长度，返回从origin延长lineLength长度的线段终点
		Float2 interceptPoint(float lineLength) const noexcept;
	};
	//3维射线
	struct Ray3 {
		//原点
		Float3 origin;
		//方向（必须归一化），更新后需手动调用update函数更新invDirection以及sign的值
		Float3 direction;
		//direction的倒数（外部调用不要进行修改）
		Float3 invDirection;
		//如果方向大于等于0则为false，否则为true（外部调用不要进行修改）
		Bool3 sign;

		constexpr Ray3() noexcept : origin(), direction(), invDirection(), sign() {}
		//注意方向必须归一化
		constexpr Ray3(const Float3& origin, const Float3& normalizedDirection) noexcept : origin(origin), direction(normalizedDirection),
			invDirection(1.0f / normalizedDirection.x, 1.0f / normalizedDirection.y, 1.0f / normalizedDirection.z),
			sign(normalizedDirection.x < 0, normalizedDirection.y < 0, normalizedDirection.z < 0) {
		}

		//更新invDirection和sign
		void update() noexcept;
		//传入线段长度，返回从origin延长lineLength长度的线段终点
		Float3 interceptPoint(float lineLength) const noexcept;
	};
	//3维射线（原点为Fix3）
	struct Ray3f {
		//原点
		Fix3 origin;
		//方向（必须归一化），更新后需手动调用update函数更新invDirection以及sign的值
		Float3 direction;
		//direction的倒数（外部调用不要进行修改）
		Float3 invDirection;
		//如果方向大于等于0则为false，否则为true（外部调用不要进行修改）
		Bool3 sign;

		constexpr Ray3f() noexcept : origin(), direction(), invDirection(), sign() {}
		//注意方向必须归一化
		constexpr Ray3f(const Fix3& origin, const Float3& normalizedDirection) noexcept : origin(origin), direction(normalizedDirection),
			invDirection(1.0f / normalizedDirection.x, 1.0f / normalizedDirection.y, 1.0f / normalizedDirection.z),
			sign(normalizedDirection.x < 0, normalizedDirection.y < 0, normalizedDirection.z < 0) {
		}

		//更新invDirection和sign
		void update() noexcept;
		//传入线段长度，返回从origin延长lineLength长度的线段终点
		Fix3 interceptPoint(float lineLength) const noexcept;
		//使用fast-voxel-traversal算法遍历体素网格，如果射线与体素产生碰撞则调用callback，如果callback调用中将第一个参数改为true，则跳过剩下的检测
		void rayCastVoxel(float maxRange, const std::function<void(bool&, const Ray3fTarget&)>& callback) const;

		//使用fast-voxel-traversal算法遍历体素网格(direction必须归一化,direction后面的参数为direction的倒数)，如果射线与体素产生碰撞则调用callback，如果callback调用中将第一个参数改为true，则跳过剩下的检测
		static void RayCastVoxel(const Fix3& origin, const Float3& normalizedDirection, const Float3& invNormalizedDirection, float maxRange, const std::function<void(bool& isBreak, const Ray3fTarget& rayTarget)>& callback);
	};

}