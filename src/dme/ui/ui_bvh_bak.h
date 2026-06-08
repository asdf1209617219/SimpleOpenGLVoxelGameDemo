#pragma once
#include <dme/core.h>
namespace dme::ui {
	class UI;
	//专门管理UI的包围盒
	class UIBVHBak {
	private:
		/*
		* 示例图：0为普通节点，1为UI节点
		*                   (subTreeRoot) 0 -------- 1 (rootUI)
		*                               /   \
		*                              0	 0
		*                             /  \  /  \
		*     (subTreeRoot) 0 ------- 1  1 1    1 --------- 1 (subTreeRoot UI)
		*                  / \
		*                 1   1
		* UI的左子节点为subTreeRoot，平衡都在子树内进行，UI在第一次添加子UI时才会申请子树。
		* subTreeRoot的parent为父UI的bvhIndex，UI节点永远为叶节点且heigth为0。
		*
		*/

		struct Node {
			Abox2 box; //包围盒（物理像素）
			union {
				struct {
					uid left; //左节点，当节点为UI时，left为subTreeRoot
					uid right; //右节点
				};
				uid children[2]; //子节点
			};
			union {
				uid parent; //父节点
				uid nextFree; //当空闲列表指向该节点的空间时，该属性表示下一个空闲节点
			};
			uid height; //节点高度，未初始化状态为null，保存的是当前子树中的高度，如果为叶节点则为子树高度
			UI* data; //有data时，节点是子树根节点的同时也是父树的叶节点，通过上下文判断是子树还是父树

			constexpr Node() noexcept : box(), left(), right(), parent(), height(), data(null) {}
			constexpr Node(const Abox2& box, UI* data = null) noexcept : box(box), left(), right(), parent(), height(0), data(data) {}
			constexpr Node(const Abox2& box, uid parent, uid height) noexcept : box(box), left(), right(), parent(parent), height(height), data(null) {}
			constexpr Node(const Abox2& box, uid left, uid right, uid height) noexcept : box(box), left(left), right(right), parent(), height(height), data(null) {}

			DME_INLINE bool isLeaf() const noexcept {
				return right.isNull();
			}
			DME_INLINE bool notLeaf() const noexcept {
				return right.notNull();
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

		//分配节点并将节点放入
		uid allocateNode(const Node& node = Node()) noexcept;
		//释放节点
		void freeNode(uid index) noexcept;

		//插入叶节点作为子树
		void insertLeaf(uid leaf, uid parentTreeLeaf) noexcept;
		//断开叶节点，暂时不删除
		void disconnectLeaf(uid leaf, uid parentTreeLeaf) noexcept;
		//平衡节点
		uid balance(uid index, uid parentTreeLeaf) noexcept;

		//删除断开的UI节点，包括UI的子节点
		void removeImpl(UI& u) noexcept;
		void updateNodeAboxImpl(uid index) noexcept;
	public:
		//初始化节点池，必有根节点，如果一个UI没有碰撞盒，则它的子节点也不会有
		UIBVHBak(UI& root) noexcept;
		//释放节点池
		~UIBVHBak();

		//获取节点包围盒
		Abox2 getBox(uid index) const noexcept;

		//更新节点的包围盒或将UI的包围盒添加到父节点
		void updateAbox(UI& u) noexcept;
		//更新节点以及所有子节点的包围盒
		void updateThisAndAllChildrenAbox(UI& u) noexcept;
		//断开节点，暂时不删除
		void disconnect(UI& u) noexcept;
		//删除节点，包括UI的子节点
		void remove(UI& u) noexcept;

		//触发事件时从rootUI开始进行碰撞查询（点），如果与叶节点产生碰撞则按照渲染顺序调用事件对应的处理方法，只返回位于渲染最顶层的UI
		UI* queryCollision(const Float2& point) const noexcept;
	};
}