#pragma once
#include <dme/item/proto/item_proto.h>
#include <dme/item/proto/block_item_proto.h>
namespace dme::item {
	class AllItemProto {
	private:
		static MultiSizeArr prototypes;
	public:
		static ItemProto* GetProto(uid id);
		static void EachProto(const std::function<void(ItemProto& proto)>& func);

		//proto以及event的初始化，需在Game::init中调用该函数
		static void Init();

		//下面这些指针地址是有可能变动的，在游戏数据重新加载后

		static ItemProto* None; //用于空手或空的物品格子，id必为0
		static ItemProto* Apple;
		static BlockItemProto* Stone;
		static BlockItemProto* Dirt;
		static BlockItemProto* GrassBlock;
		static BlockItemProto* Plank;
		static BlockItemProto* Log;
		static BlockItemProto* Leaves;
		static BlockItemProto* Grass;
	};

}