#pragma once
#include <dme/core.h>
namespace dme::block {
	//方块形状，注意这里只能使用0~127的值，超过这部分会导致BlockBase出现问题
	enum class BlockShape : uchar {
		Cube = 0, //正方体，默认形状
		Half = 1, //在一个方块的范围内，由多个长度为0.5的正方体组成
		Ramp = 2, //斜坡，可以有一个或两个斜面，可以有内角
	};

}
