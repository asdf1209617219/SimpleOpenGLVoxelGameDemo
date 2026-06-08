#pragma once
#include <dme/core/all.h>
#include <dme/math/all.h>

namespace dme::util {

	class BVHHelper {
	private:
	public:
		//栈默认大小
		static constexpr uint default_stack_size = 256;
		//包围盒默认扩大范围
		static constexpr float box_extend = 0.1f;
		//预测移动次数
		static constexpr float box_move_predict_count = 4.0f;

		//表面积启发算法
		static DME_INLINE float BoxCost(const math::Abox2& box) {
			return box.max.x - box.min.x + box.max.y - box.min.y;
		}
		//表面积启发算法
		static DME_INLINE float BoxCost(const math::Abox3& box) {
			float x = box.max.x - box.min.x;
			float y = box.max.y - box.min.y;
			return (box.max.z - box.min.z) * (x + y) + x * y;
		}
		//表面积启发算法
		static DME_INLINE float BoxCost(const math::Abox3f& box) {
			float x = box.max.subXToFloat(box.min);
			float y = box.max.subYToFloat(box.min);
			return box.max.subZToFloat(box.min) * (x + y) + x * y;
		}
		//表面积启发算法，两个box结合后的花费
		static DME_INLINE float BoxCombinedCost(const math::Abox2& box1, const math::Abox2& box2) {
			return math::Max(box1.max.x, box2.max.x) - math::Min(box1.min.x, box2.min.x)
				+ math::Max(box1.max.y, box2.max.y) - math::Min(box1.min.y, box2.min.y);
		}
		//表面积启发算法，两个box结合后的花费
		static DME_INLINE float BoxCombinedCost(const math::Abox3& box1, const math::Abox3& box2) {
			float x = math::Max(box1.max.x, box2.max.x) - math::Min(box1.min.x, box2.min.x);
			float y = math::Max(box1.max.y, box2.max.y) - math::Min(box1.min.y, box2.min.y);
			return (math::Max(box1.max.z, box2.max.z) - math::Min(box1.min.z, box2.min.z)) * (x + y) + x * y;
		}
		//表面积启发算法，两个box结合后的花费
		static DME_INLINE float BoxCombinedCost(const math::Abox3f& box1, const math::Abox3f& box2) {
			float x = math::Max(box1.max.getX(), box2.max.getX()).subToFloat(math::Min(box1.min.getX(), box2.min.getX()));
			float y = math::Max(box1.max.getY(), box2.max.getY()).subToFloat(math::Min(box1.min.getY(), box2.min.getY()));
			float z = math::Max(box1.max.getZ(), box2.max.getZ()).subToFloat(math::Min(box1.min.getZ(), box2.min.getZ()));
			return z * (x + y) + x * y;
		}
		//返回要选择哪一个子节点
		static DME_INLINE bool BoxSelect(const math::Abox2& leaf, const math::Abox2& child0, const math::Abox2& child1) {
			Float2 l = leaf.min + leaf.max;
			return std::abs(l.x - child0.min.x - child0.max.x) + std::abs(l.y - child0.min.y - child0.max.y)
			> std::abs(l.x - child1.min.x - child1.max.x) + std::abs(l.y - child1.min.y - child1.max.y);
		}
		//返回要选择哪一个子节点
		static DME_INLINE bool BoxSelect(const math::Abox3& leaf, const math::Abox3& child0, const math::Abox3& child1) {
			Float3 l = leaf.min + leaf.max;
			return std::abs(l.x - child0.min.x - child0.max.x) + std::abs(l.y - child0.min.y - child0.max.y) + std::abs(l.z - child0.min.z - child0.max.z)
			> std::abs(l.x - child1.min.x - child1.max.x) + std::abs(l.y - child1.min.y - child1.max.y) + std::abs(l.z - child1.min.z - child1.max.z);
		}
		//返回要选择哪一个子节点
		static DME_INLINE bool BoxSelect(const math::Abox3f& leaf, const math::Abox3f& child0, const math::Abox3f& child1) {
			Int3 i = leaf.min.i + leaf.max.i;
			Float3 f = leaf.min.f + leaf.max.f;
			return std::abs((i.x - child0.min.x - child0.max.x) + (f.x - child0.min.xf - child0.max.xf))
				+ std::abs((i.y - child0.min.y - child0.max.y) + (f.y - child0.min.yf - child0.max.yf))
				+ std::abs((i.z - child0.min.z - child0.max.z) + (f.z - child0.min.zf - child0.max.zf))
			> std::abs((i.x - child1.min.x - child1.max.x) + (f.x - child1.min.xf - child1.max.xf))
				+ std::abs((i.y - child1.min.y - child1.max.y) + (f.y - child1.min.yf - child1.max.yf))
				+ std::abs((i.z - child1.min.z - child1.max.z) + (f.z - child1.min.zf - child1.max.zf));
		}

		//获取扩大的包围盒
		static DME_INLINE math::Abox2 BoxFat(const math::Abox2& box) {
			return Abox2(box.min - box_extend, box.max + box_extend);
		}
		//获取扩大的包围盒
		static DME_INLINE math::Abox3 BoxFat(const math::Abox3& box) {
			return Abox3(box.min - box_extend, box.max + box_extend);
		}
		//获取扩大的包围盒
		static DME_INLINE math::Abox3f BoxFat(const math::Abox3f& box) {
			return Abox3f(box.min - box_extend, box.max + box_extend);
		}
		//向运动方向扩展包围盒
		static DME_INLINE void BoxMovePredict(math::Abox2& box, const Float2& delta) {
			if (delta.x < 0.0f) {
				box.min.x += delta.x * box_move_predict_count;
			}
			else {
				box.max.x += delta.x * box_move_predict_count;
			}
			if (delta.y < 0.0f) {
				box.min.y += delta.y * box_move_predict_count;
			}
			else {
				box.max.y += delta.y * box_move_predict_count;
			}
		}
		//向运动方向扩展包围盒
		static DME_INLINE void BoxMovePredict(math::Abox3& box, const Float3& delta) {
			if (delta.x < 0.0f) {
				box.min.x += delta.x * box_move_predict_count;
			}
			else {
				box.max.x += delta.x * box_move_predict_count;
			}
			if (delta.y < 0.0f) {
				box.min.y += delta.y * box_move_predict_count;
			}
			else {
				box.max.y += delta.y * box_move_predict_count;
			}
			if (delta.z < 0.0f) {
				box.min.z += delta.z * box_move_predict_count;
			}
			else {
				box.max.z += delta.z * box_move_predict_count;
			}
		}
		//向运动方向扩展包围盒
		static DME_INLINE void BoxMovePredict(math::Abox3f& box, const Float3& delta) {
			if (delta.x < 0.0f) {
				box.min.plusAssignmentX(delta.x * box_move_predict_count);
			}
			else {
				box.max.plusAssignmentX(delta.x * box_move_predict_count);
			}
			if (delta.y < 0.0f) {
				box.min.plusAssignmentY(delta.y * box_move_predict_count);
			}
			else {
				box.max.plusAssignmentY(delta.y * box_move_predict_count);
			}
			if (delta.z < 0.0f) {
				box.min.plusAssignmentZ(delta.z * box_move_predict_count);
			}
			else {
				box.max.plusAssignmentZ(delta.z * box_move_predict_count);
			}
		}
	};

	//Bounding Volume Hierarchies 层次包围盒（2维）
	class BVH2 {
	public:
		struct Node {
			Abox2 box; //包围盒
			union {
				struct {
					uid left; //左节点
					uid right; //右节点
				};
				uid children[2]; //子节点
			};
			union {
				uid parent; //父节点
				uid nextFree; //当空闲列表指向该节点的空间时，该属性表示下一个空闲节点
			};
			uid height; //节点高度，叶子节点为0，未初始化状态为null
			void* data;

			constexpr Node() noexcept : box(), left(), right(), parent(), height(), data(null) {}
			constexpr Node(uid left, uid right, uid parent, uid height) noexcept : box(), left(left), right(right), parent(parent), height(height), data(null) {}
			constexpr Node(const Abox2& box, uid parent, uid height) noexcept : box(box), left(), right(), parent(parent), height(height), data(null) {}
			constexpr Node(const Abox2& box, uid height, void* data) noexcept : box(box), left(), right(), parent(), height(height), data(data) {}

			DME_INLINE bool isLeaf() const noexcept {
				return left.isNull();
			}
			DME_INLINE bool notLeaf() const noexcept {
				return left.notNull();
			}
			DME_INLINE bool notNull() const noexcept {
				return height.notNull();
			}
		};

	private:
		//节点列表，根节点在最开始
		Node* nodes;
		//当前节点数量
		uint count;
		//节点池容量
		uint capacity;
		//标记第一个空闲节点，null表示没有空闲的
		uid freeList;
		//根节点下标
		uid root;

		//分配节点并将节点放入
		uid allocateNode(Node&& node = Node());
		//释放节点
		void freeNode(uid index);

		//插入叶节点
		void insertLeaf(uid leaf);
		//移除叶节点
		void removeLeaf(uid leaf);
		//平衡节点
		uid balance(uid index);
	public:
		//构造树将初始化节点池
		BVH2();
		//释放节点池
		~BVH2();

		//获取数据
		void* getData(uid index) const;
		//获取节点包围盒
		Abox2 getBox(uid index) const;
		//获取根节点包围盒
		Abox2 getRootBox() const;

		//添加节点和附加数据，包围盒会变得更大后加入节点
		//[[nodiscard]]
		void add(uid& index, const Abox2& box, void* data = null);
		//更新节点的附加数据
		void update(uid index, void* data = null);
		//更新节点的包围盒，适用于不方便预测下一步的移动这种场景
		void updateAbox(uid index, const Abox2& box);
		//删除节点
		void remove(uid& index);
		//对节点原本的包围盒进行移动（delta为移动距离），如果原本的包围盒移动出变大后的包围盒，则返回true
		bool move(uid index, const Abox2& box, const Float2& delta);

		//using Callback = void (*)(bool& isBreak, uid index, const Node& node);

		//碰撞查询（包围盒），目前包含紧贴的情况，如果与叶节点产生碰撞则调用callback，如果callback调用中将第一个参数改为true，则跳过剩下的检测
		void queryCollision(const Abox2& box, const std::function<void(bool&, uid, const Node&)>& callback) const;
		//碰撞查询（点），如果与叶节点产生碰撞则调用callback，如果callback调用中将第一个参数改为true，则跳过剩下的检测
		void queryCollision(const Float2& point, const std::function<void(bool&, uid, const Node&)>& callback) const;
		//射线检测，如果与叶节点产生碰撞则调用callback，如果callback调用中将第一个参数改为true，则跳过剩下的检测
		void rayCast(const math::Ray2& ray, float maxRange, const std::function<void(bool&, uid, const Node&)>& callback) const;
	};


	//Bounding Volume Hierarchies 层次包围盒（3维）
	class BVH3 {
	public:
		struct Node {
			Abox3 box; //包围盒
			union {
				struct {
					uid left; //左节点
					uid right; //右节点
				};
				uid children[2]; //子节点
			};
			union {
				uid parent; //父节点
				uid nextFree; //当空闲列表指向该节点的空间时，该属性表示下一个空闲节点
			};
			uid height; //节点高度，叶子节点为0，未初始化状态为null
			//union {
			void* data; //直接指向数据
			//};

			constexpr Node() noexcept : box(), left(), right(), parent(), height(), data(null) {}
			constexpr Node(uid left, uid right, uid parent, uid height) noexcept : box(), left(left), right(right), parent(parent), height(height), data(null) {}
			constexpr Node(const Abox3& box, uid parent, uid height) noexcept : box(box), left(), right(), parent(parent), height(height), data(null) {}
			constexpr Node(const Abox3& box, uid height, void* data) noexcept : box(box), left(), right(), parent(), height(height), data(data) {}

			DME_INLINE bool isLeaf() const noexcept {
				return left.isNull();
			}
			DME_INLINE bool notLeaf() const noexcept {
				return left.notNull();
			}
			DME_INLINE bool notNull() const noexcept {
				return height.notNull();
			}
		};
	private:
		//节点列表，根节点在最开始
		Node* nodes;
		//当前节点数量
		uint count;
		//节点池容量
		uint capacity;
		//标记第一个空闲节点，null表示没有空闲的
		uid freeList;
		//根节点下标
		uid root;

		//分配节点并将节点放入
		uid allocateNode(Node&& node = Node());
		//释放节点
		void freeNode(uid index);

		//插入叶节点
		void insertLeaf(uid leaf);
		//移除叶节点
		void removeLeaf(uid leaf);
		//平衡节点
		uid balance(uid index);
	public:
		//构造树将初始化节点池
		BVH3();
		//释放节点池
		~BVH3();

		//获取数据
		void* getData(uid index) const;

		//获取节点包围盒
		Abox3 getBox(uid index) const;
		//获取根节点包围盒
		Abox3 getRootBox() const;

		//添加节点和附加数据，包围盒会变得更大后加入节点
		//[[nodiscard]]
		void add(uid& index, const Abox3& box, void* data = null);
		//更新节点的附加数据
		void update(uid index, void* data = null);
		//更新节点的包围盒，适用于不方便预测下一步的移动这种场景
		void updateAbox(uid index, const Abox3& box);
		//删除节点
		void remove(uid& index);
		//对节点原本的包围盒进行移动（delta为移动距离），如果原本的包围盒移动出变大后的包围盒，则返回true
		bool move(uid index, const Abox3& box, const Float3& delta);

		//using Callback = void (*)(bool& isBreak, uid index, const Node& node);

		//碰撞查询（包围盒），目前包含紧贴的情况，如果与叶节点产生碰撞则调用callback，如果callback调用中将第一个参数改为true，则跳过剩下的检测
		void queryCollision(const Abox3& box, const std::function<void(bool&, uid, const Node&)>& callback) const;
		//碰撞查询（点），如果与叶节点产生碰撞则调用callback，如果callback调用中将第一个参数改为true，则跳过剩下的检测
		void queryCollision(const Float3& point, const std::function<void(bool&, uid, const Node&)>& callback) const;
		//射线检测，如果与叶节点产生碰撞则调用callback，如果callback调用中将第一个参数改为true，则跳过剩下的检测
		void rayCast(const math::Ray3& ray, float maxRange, const std::function<void(bool&, uid, const Node&)>& callback) const;
	};

	//Bounding Volume Hierarchies 层次包围盒（3维，且使用定点轴对齐包围盒）
	class BVH3f {
	public:
		struct Node {
			Abox3f box; //包围盒
			union {
				struct {
					uid left; //左节点
					uid right; //右节点
				};
				uid children[2]; //子节点
			};
			union {
				uid parent; //父节点
				uid nextFree; //当空闲列表指向该节点的空间时，该属性表示下一个空闲节点
			};
			uid height; //节点高度，叶子节点为0，未初始化状态为null
			//union {
			void* data; //直接指向数据
			//};

			constexpr Node() noexcept : box(), left(), right(), parent(), height(), data(null) {}
			constexpr Node(uid left, uid right, uid parent, uid height) noexcept : box(), left(left), right(right), parent(parent), height(height), data(null) {}
			constexpr Node(const Abox3f& box, uid parent, uid height) noexcept : box(box), left(), right(), parent(parent), height(height), data(null) {}
			constexpr Node(const Abox3f& box, uid height, void* data) noexcept : box(box), left(), right(), parent(), height(height), data(data) {}

			DME_INLINE bool isLeaf() const noexcept {
				return left.isNull();
			}
			DME_INLINE bool notLeaf() const noexcept {
				return left.notNull();
			}
			DME_INLINE bool notNull() const noexcept {
				return height.notNull();
			}
		};
	private:
		//节点列表，根节点在最开始
		Node* nodes;
		//当前节点数量
		uint count;
		//节点池容量
		uint capacity;
		//标记第一个空闲节点，null表示没有空闲的
		uid freeList;
		//根节点下标
		uid root;

		//分配节点并将节点放入
		uid allocateNode(Node&& node = Node());
		//释放节点
		void freeNode(uid index);

		//插入叶节点
		void insertLeaf(uid leaf);
		//移除叶节点
		void removeLeaf(uid leaf);
		//平衡节点
		uid balance(uid index);
	public:
		//构造树将初始化节点池
		BVH3f();
		//释放节点池
		~BVH3f();

		//获取数据
		void* getData(uid index) const;

		//获取节点包围盒
		Abox3f getBox(uid index) const;
		//获取根节点包围盒
		Abox3f getRootBox() const;

		//添加节点和附加数据，包围盒会变得更大后加入节点
		//[[nodiscard]]
		void add(uid& index, const Abox3f& box, void* data = null);
		//更新节点的附加数据
		void update(uid index, void* data = null);
		//通过删除再重新添加来更新节点的包围盒，适用于不方便预测下一步的移动这种场景
		void updateAbox(uid index, const Abox3f& box);
		//删除节点
		void remove(uid& index);
		//对节点原本的包围盒进行移动（delta为移动距离），如果原本的包围盒移动出变大后的包围盒，则返回true
		bool move(uid index, const Abox3f& box, const Float3& delta);

		//using Callback = void (*)(bool& isBreak, uid index, const Node& node);

		//碰撞查询（包围盒），目前包含紧贴的情况，如果与叶节点产生碰撞则调用callback，如果callback调用中将第一个参数改为true，则跳过剩下的检测
		void queryCollision(const Abox3f& box, const std::function<void(bool&, uid, const Node&)>& callback) const;
		//碰撞查询（点），如果与叶节点产生碰撞则调用callback，如果callback调用中将第一个参数改为true，则跳过剩下的检测
		void queryCollision(const Fix3& point, const std::function<void(bool&, uid, const Node&)>& callback) const;
		//射线检测，如果与叶节点产生碰撞则调用callback，如果callback调用中将第一个参数改为true，则跳过剩下的检测
		void rayCast(const math::Ray3f& ray, float maxRange, const std::function<void(bool&, uid, const Node&)>& callback) const;
	};
}