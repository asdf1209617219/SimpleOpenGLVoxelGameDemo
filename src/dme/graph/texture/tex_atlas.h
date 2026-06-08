#pragma once
#include <dme/graph/texture/tex2d_int.h>
namespace dme::graph {
	struct _TexAtlasLessFunc {
		constexpr bool operator()(const Pair<sint, Int3>& key1, const Pair<sint, Int3>& key2) const noexcept {
			if (key1.key < key2.key) {
				return true;
			}
			if (key1.key > key2.key) {
				return false;
			}
			if (key1.val.x < key2.val.x) {
				return true;
			}
			if (key1.val.x > key2.val.x) {
				return false;
			}
			if (key1.val.y < key2.val.y) {
				return true;
			}
			if (key1.val.y > key2.val.y) {
				return false;
			}
			if (key1.val.z < key2.val.z) {
				return true;
			}
			return false;
		}
	};

	//纹理图集数组，为RGBA4通道
	class TexAtlas : Tex {
		friend class Game;
	private:
		using FreeRectY = TreeSet<Pair<sint, Int3>, _TexAtlasLessFunc>;
		using FreeRectX = TreeMap<sint, FreeRectY>;

		FlatMap<strid, TexIndex> texIndexMap; //通过图片路径查询纹理索引
		DynArr<TexPos> texPosArr; //通过索引查询纹理坐标，同时将位置数据保存到纹理中
		TreeMap<TexData, TexIndex> texDataToIndex; //合并前纹理数据根据宽高排序
		FreeRectX freeRect; //空闲的矩形根据宽高和坐标排序

		DynArr<TexData> dataArr; //纹理数组每层的合并后的纹理数据
		sint depth; //图集的层数
		sint size; //图集的宽高（正方形）

		//扩容
		FreeRectX::value_type* extend();
	public:
		TexAtlas() noexcept;

		//禁止复制
		TexAtlas(const TexAtlas&) = delete;
		TexAtlas& operator=(const TexAtlas&) = delete;

		static constexpr sint default_max_size = 2048;

		//图集的宽高
		sint getSize() const noexcept;

		//加载一个新纹理，如果已经存在则返回原有的索引，否则添加到图集中并返回新索引（添加后需要调用merge函数将新纹理合并到图集中）
		TexIndex addIfAbsent(strid path);

		//获取纹理在纹理图集中的坐标
		TexPos getPos(strid path) const noexcept;
		//获取纹理在纹理图集中的坐标
		TexPos getPos(TexIndex index) const noexcept;
		//获取纹理在纹理图集中的索引
		TexIndex getIndex(strid path) const noexcept;

		//将所有纹理合并成一个图集
		void merge();

		//将图集数据导出到文件，路径为相对于游戏可执行文件的路径，不要包含文件扩展名，返回是否成功
		bool exportToFile(strid path) const;

		void bind(sint location = 0) const;

		//通用2d纹理图集
		static TexAtlas& Default();

		//字体纹理图集
		//static TexAtlas& Font();
	};
}
