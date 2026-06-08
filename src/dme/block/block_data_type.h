#pragma once
#include <dme/core.h>
namespace dme::block {
	//额外数据类型
	enum class BlockDataType : uint {
		None = 0,
		MultiBlock = 1, //多方块
		Array = 2, //数组
	};

	//约束

	template <typename T>
	concept IsBlockData = requires {
		{ T::GetBlockDataType() } -> std::same_as<BlockDataType>;
	};

}