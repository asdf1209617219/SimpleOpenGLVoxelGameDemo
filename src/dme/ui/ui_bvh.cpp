#include <dme/ui/ui_bvh.h>
#include <dme/ui/ui.h>
namespace dme::ui {
	using util::BVHHelper;

	UIBVH::UIBVH() noexcept : count(0), size(128), freeList(), root(0) {
		nodes = Malloc<Node>(size);
		Memset<Node>(nodes, uchar_max, size); //全部初始化为-1
	}
	UIBVH::~UIBVH() {
		if (nodes) {
			Free(nodes);
			nodes = null;
		}
	}

	uid UIBVH::allocateNode(Node&& node) noexcept {
		uid index;
		if (freeList.isNull()) {
			if (count == size) { //扩容
				size *= 2;
				Node* newNodes = Malloc<Node>(size);
				Memcpy<Node>(newNodes, nodes, count);
				Memset<Node>(newNodes + count, uchar_max, count);
				Free(nodes);
				nodes = newNodes;
			}
			index = count;
		}
		else {
			index = freeList;
			freeList = nodes[index].nextFree;
		}
		count++;
		nodes[index] = node;
		return index;
	}
	void UIBVH::freeNode(uid index) noexcept {
		DME_ASSERT(index.notNull() && index < size, "subscript exceeds the capacity");
		nodes[index].nextFree = freeList;
		freeList = index;
		count--;
	}
	
	bool UIBVH::_debug_check_node(uid index) {
		if (index.isNull()) return false;
		auto& node = nodes[index];

		uid parent = node.parent;
		uid left = node.left;
		uid right = node.right;
		Log::Info("node info: index:", index, ", parent: ", parent, ", left: ", left, ", right: ", right);
		if (left.notNull()) {
			auto& child = nodes[left];
			if (child.left == index || child.right == index) {
				Log::Info("boom");
				return true;
			}
		}
		if (right.notNull()) {
			auto& child = nodes[right];
			if (child.left == index || child.right == index) {
				Log::Info("boom");
				return true;
			}
		}
		return false;
	}
	UIBVH::Node& UIBVH::get(uid nodeIndex) noexcept {
		Node& node = nodes[nodeIndex];
		// TODO 测试循环问题
		uid parent = node.parent;
		uid left = node.left;
		uid right = node.right;
		if (left.notNull()) {
			auto& child = nodes[left];
			if (child.left == nodeIndex || child.right == nodeIndex) {
				Log::Info("boom");
			}
		}
		if (right.notNull()) {
			auto& child = nodes[right];
			if (child.left == nodeIndex || child.right == nodeIndex) {
				Log::Info("boom");
			}
		}
		return node;
	}

	void UIBVH::insertLeaf(uid leaf) noexcept {
		if (leaf == root) {
			return;
		}

		//查找此节点的最佳兄弟节点
		const auto& leafBox = nodes[leaf].box;
		uid index = root;
		uint child = 0; //选择的子节点索引
		while (nodes[index].notLeaf()) { //不是叶子节点的必有左右子节点
			uid children[2] = { nodes[index].children[0], nodes[index].children[1] };
			child = BVHHelper::BoxSelect(leafBox, nodes[children[0]].box, nodes[children[1]].box);
			index = children[child];
		}

		//新建父节点
		uid oldParent = nodes[index].parent;
		uid newParent = allocateNode(Node(Abox2::GetCombine(leafBox, nodes[index].box), oldParent, nodes[index].height + 1));
		if (oldParent.isNull()) { //index是根节点
			root = newParent;
		}
		else {
			nodes[oldParent].children[child] = newParent;
		}
		//分别将index和leaf加入newParent的左右节点
		nodes[newParent].left = index;
		nodes[newParent].right = leaf;
		nodes[index].parent = newParent;
		nodes[leaf].parent = newParent;

		//从底部向上修复节点的高度和包围盒的大小
		index = newParent;

		while (index.notNull()) {
			index = balance(index);

			uid left = nodes[index].left;
			uid right = nodes[index].right;

			//DME_ASSERT(left != 0);
			//DME_ASSERT(right != 0);

			nodes[index].height = 1 + math::Max(nodes[left].height, nodes[right].height);
			nodes[index].box.combine(nodes[left].box, nodes[right].box);

			index = nodes[index].parent;
		}
	}
	
	void UIBVH::removeLeaf(uid leaf) noexcept {
		if (leaf == root) {
			return;
		}
		uid parent = nodes[leaf].parent;
		uid grandParent = nodes[parent].parent;
		uid sibling = nodes[parent].left == leaf ? nodes[parent].right : nodes[parent].left;

		if (grandParent.isNull()) {
			root = sibling;
			nodes[sibling].parent.setNull();
			freeNode(parent);
		}
		else {
			// 销毁父节点并将兄弟节点连接到祖父节点
			if (nodes[grandParent].left == parent) {
				nodes[grandParent].left = sibling;
			}
			else {
				nodes[grandParent].right = sibling;
			}
			nodes[sibling].parent = grandParent;
			freeNode(parent);

			// 调整祖级节点包围盒
			while (grandParent.notNull()) {
				grandParent = balance(grandParent);

				uid left = nodes[grandParent].left;
				uid right = nodes[grandParent].right;

				nodes[grandParent].box.combine(nodes[left].box, nodes[right].box);
				nodes[grandParent].height = 1 + math::Max(nodes[left].height, nodes[right].height);

				grandParent = nodes[grandParent].parent;
			}
		}
	}

	uid UIBVH::balance(uid iA) noexcept {
		Node& A = nodes[iA];
		if (A.isLeaf() || A.height < 2) {
			return iA;
		}
		uid iB = A.left;
		uid iC = A.right;
		Node& B = nodes[iB];
		Node& C = nodes[iC];
		sint balance = C.height - B.height;

		//将节点C向上旋转
		if (balance > 1) {
			uid iF = C.left;
			uid iG = C.right;
			Node& F = nodes[iF];
			Node& G = nodes[iG];

			// 交换A和C
			C.left = iA;
			C.parent = A.parent;
			A.parent = iC;

			//设置A的旧父节点的子节点指向C
			if (C.parent.isNull()) {
				root = iC;
			}
			else {
				if (nodes[C.parent].left == iA) {
					nodes[C.parent].left = iC;
				}
				else {
					nodes[C.parent].right = iC;
				}
			}

			// 旋转
			if (F.height > G.height) {
				C.right = iF;
				A.right = iG;
				G.parent = iA;
				A.box.combine(B.box, G.box);
				C.box.combine(A.box, F.box);

				A.height = 1 + math::Max(B.height, G.height);
				C.height = 1 + math::Max(A.height, F.height);
			}
			else {
				C.right = iG;
				A.right = iF;
				F.parent = iA;
				A.box.combine(B.box, F.box);
				C.box.combine(A.box, G.box);

				A.height = 1 + math::Max(B.height, F.height);
				C.height = 1 + math::Max(A.height, G.height);
			}

			return iC;
		}

		//将节点B向上旋转
		if (balance < -1) {
			uid iD = B.left;
			uid iE = B.right;
			Node& D = nodes[iD];
			Node& E = nodes[iE];

			// 交换A和B
			B.left = iA;
			B.parent = A.parent;
			A.parent = iB;

			//设置A的旧父节点的子节点指向B
			if (B.parent.isNull()) {
				root = iB;
			}
			else {
				if (nodes[B.parent].left == iA) {
					nodes[B.parent].left = iB;
				}
				else {
					nodes[B.parent].right = iB;
				}
			}

			// 旋转
			if (D.height > E.height) {
				B.right = iD;
				A.left = iE;
				E.parent = iA;
				A.box.combine(C.box, E.box);
				B.box.combine(A.box, D.box);

				A.height = 1 + math::Max(C.height, E.height);
				B.height = 1 + math::Max(A.height, D.height);
			}
			else {
				B.right = iE;
				A.left = iD;
				D.parent = iA;
				A.box.combine(C.box, D.box);
				B.box.combine(A.box, E.box);

				A.height = 1 + math::Max(C.height, D.height);
				B.height = 1 + math::Max(A.height, E.height);
			}

			return iB;
		}

		return iA;
	}

	void UIBVH::updateAbox(UI& u) noexcept {
		if (u.bvhIndex.isNull()) {
			uid index = allocateNode(
				Node(
					u.box,
					0,
					&u
				)
			);
			insertLeaf(index);
			u.bvhIndex = index;
		}
		else { //需要更新
			uid index = u.bvhIndex;
			removeLeaf(index);
			auto& node = nodes[index];
			node.box = u.box;
			node.height = 0;
			node.parent.setNull();
			node.left.setNull();
			node.right.setNull();
			insertLeaf(index);
		}
	}

	void UIBVH::remove(UI& u) noexcept {
		if (u.bvhIndex.isNull()) {
			return;
		}
		removeLeaf(u.bvhIndex);
		freeNode(u.bvhIndex);
	}

	//auto _collisionResult = DynArr<Pair<UIMeshKey, UI*>>(128);

	UI* UIBVH::queryCollision(const Float2& point) const noexcept {
		UI* r = null;
		UIMeshKey k = UIMeshKey::MinKey();

		GrowableStack<uid, BVHHelper::default_stack_size> stack;
		stack.push(root);
		while (stack.count() > 0) {
			uid nodeId = stack.pop();
			const Node& node = nodes[nodeId];
			
			if (node.box.contains(point)) {
				if (node.data) {
					UI& u = *node.data;
					//if (u.box.contains(point) && u.pointInUI(point)) {
					if (u.pointInUI(point)) {
						if (k <= u.getUIMeshKey()) {
							r = std::addressof(u);
							k = r->getUIMeshKey();
						}
					}
					if (node.left.notNull()) {
						stack.push(node.left);
					}
				}
				else {
					stack.push(node.left);
					stack.push(node.right);
				}
			}
		}

		return r;

		/*
		for (uint i = _collisionResult.count(); i > 0;) {
			i--;
			_collisionResult[i]; // TODO 触发具体事件
		}
		_collisionResult.clear();
		*/
	}
}