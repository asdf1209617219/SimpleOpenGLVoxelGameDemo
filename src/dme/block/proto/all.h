#pragma once
#include <dme/block/proto/block_proto.h>
#include <dme/block/proto/plant_block_proto.h>
namespace dme::block {

	class AllBlockProto {
	private:
		static MultiSizeArr prototypes;
	public:
		static BlockProto* GetProto(uid id);
		static void EachProto(const std::function<void(BlockProto& proto)>& func);

		//proto以及event的初始化，需在Game::init中调用该函数
		static void Init();

		//下面这些指针地址是有可能变动的，在游戏数据重新加载后

		static BlockProto* Stone;
		static BlockProto* Dirt;
		static BlockProto* GrassBlock;
		static BlockProto* Plank;
		static BlockProto* Log;
		static BlockProto* Leaves;
		static PlantBlockProto* Grass;
	};

}