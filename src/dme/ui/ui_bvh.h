#pragma once
#include <dme/core.h>
namespace dme::ui {
	class UI;
	//专门管理UI的包围盒
	class UIBVH {
	private:
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
			UI* data;

			constexpr Node() noexcept : box(), left(), right(), parent(), height(), data(null) {}
			constexpr Node(uid left, uid right, uid parent, uid height) noexcept : box(), left(left), right(right), parent(parent), height(height), data(null) {}
			constexpr Node(const Abox2& box, uid parent, uid height) noexcept : box(box), left(), right(), parent(parent), height(height), data(null) {}
			constexpr Node(const Abox2& box, uid height, UI* data) noexcept : box(box), left(), right(), parent(), height(height), data(data) {}

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

		//节点列表，根节点在最开始
		Node* nodes;
		//当前节点数量
		uint count;
		//节点池容量
		uint size;
		//标记第一个空闲节点，null表示没有空闲的
		uid freeList;
		//根节点下标
		uid root;

		//分配节点并将节点放入
		uid allocateNode(Node&& node = Node()) noexcept;
		//释放节点
		void freeNode(uid index) noexcept;

		bool _debug_check_node(uid index);

		Node& get(uid nodeIndex) noexcept;
		//插入叶节点
		void insertLeaf(uid leaf) noexcept;
		//移除叶节点
		void removeLeaf(uid leaf) noexcept;
		//平衡节点
		uid balance(uid index) noexcept;
	public:
		//初始化节点池
		UIBVH() noexcept;
		//释放节点池
		~UIBVH();

		//更新节点的包围盒或将UI的包围盒添加到bvh
		void updateAbox(UI& u) noexcept;
		//删除节点
		void remove(UI& u) noexcept;

		//触发事件时从rootUI开始进行碰撞查询（点），如果与叶节点产生碰撞则按照渲染顺序调用事件对应的处理方法，只返回位于渲染最顶层的UI
		UI* queryCollision(const Float2& point) const noexcept;
	};
}