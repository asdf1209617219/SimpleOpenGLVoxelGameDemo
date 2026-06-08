#include <dme/ui/ui_bvh_bak.h>
#include <dme/ui/ui.h>
namespace dme::ui {
	using util::BVHHelper;

	UIBVHBak::UIBVHBak(UI& root) noexcept : count(1), size(128), freeList() {
		nodes = Malloc<Node>(size);
		Memset<Node>(nodes, uchar_max, size); //全部初始化为-1
		new (nodes) Node(root.box, std::addressof(root));
		root.bvhIndex = 0;
	}
	UIBVHBak::~UIBVHBak() {
		if (nodes) {
			Free(nodes);
			nodes = null;
		}
	}

	uid UIBVHBak::allocateNode(const Node& node) noexcept {
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
	void UIBVHBak::freeNode(uid index) noexcept {
		DME_ASSERT(index.notNull() && index < size, "subscript exceeds the capacity");
		nodes[index].nextFree = freeList;
		freeList = index;
		count--;
	}

	void UIBVHBak::insertLeaf(uid leaf, uid parentTreeLeaf) noexcept {
		//查找此节点的最佳兄弟节点
		const auto& leafBox = nodes[leaf].box;
		uid index = nodes[parentTreeLeaf].left; //子树根节点
		if (index.isNull()) { //第一次添加子UI
			nodes[parentTreeLeaf].left = leaf;
			nodes[parentTreeLeaf].box.extend(leafBox);
			nodes[leaf].parent = parentTreeLeaf;
			//修复父树的包围盒
			index = nodes[parentTreeLeaf].parent;
			while (index.notNull()) {
				Node& node = nodes[index];
				if (node.isLeaf()) {
					node.box.combine(nodes[node.left].box, node.data->box);
				}
				else {
					node.box.combine(nodes[node.left].box, nodes[node.right].box);
				}
				index = node.parent;
			};
			return;
		}
		uint child = 0; //选择的子节点索引
		while (nodes[index].notLeaf()) { //不是叶子节点的必有左右子节点
			uid children[2] = { nodes[index].children[0], nodes[index].children[1] };
			child = BVHHelper::BoxSelect(leafBox, nodes[children[0]].box, nodes[children[1]].box);
			index = children[child];
		}

		//新建父节点
		uid oldParent = nodes[index].parent;
		uid newParent = allocateNode(Node(Abox2::GetCombine(leafBox, nodes[index].box), oldParent, nodes[index].height + 1));
		if (oldParent == parentTreeLeaf) { //index是子树根节点
			nodes[oldParent].left = newParent; //newParent作为新的子树根节点
		}
		else {
			nodes[oldParent].children[child] = newParent;//将index替换为newParent
		}
		//分别将index和leaf加入newParent的左右节点
		nodes[newParent].left = index;
		nodes[newParent].right = leaf;
		nodes[index].parent = newParent;
		nodes[leaf].parent = newParent;

		//从底部向上修复节点的高度和包围盒的大小
		index = newParent;
		do {
			index = balance(index, parentTreeLeaf);

			uid left = nodes[index].left;
			uid right = nodes[index].right;

			nodes[index].height = 1 + math::Max(nodes[left].height, nodes[right].height);
			nodes[index].box.combine(nodes[left].box, nodes[right].box);

			index = nodes[index].parent;
		} while (index != parentTreeLeaf);
		//修复父树的包围盒
		do {
			Node& node = nodes[index];
			if (node.isLeaf()) {
				node.box.combine(nodes[node.left].box, node.data->box);
			}
			else {
				node.box.combine(nodes[node.left].box, nodes[node.right].box);
			}
			index = node.parent;
		} while (index.notNull());
	}

	void UIBVHBak::disconnectLeaf(uid leaf, uid parentTreeLeaf) noexcept {
		if (leaf == 0) {
			return;
		}
		uid parent = nodes[leaf].parent;
		nodes[leaf].parent.setNull();
		if (nodes[parent].isLeaf()) { //leaf是子树根节点，也就是parent == parentTreeLeaf
			nodes[parent].left.setNull();
			nodes[parent].box = nodes[parent].data->box;
		}
		else {
			uid grandParent = nodes[parent].parent;
			uid sibling = nodes[parent].left == leaf ? nodes[parent].right : nodes[parent].left;
			if (nodes[grandParent].isLeaf()) { //子树只有两个叶节点，parent是子树根节点
				nodes[grandParent].left = sibling;
				nodes[sibling].parent = grandParent;
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
				do {
					grandParent = balance(grandParent, parentTreeLeaf);

					uid left = nodes[grandParent].left;
					uid right = nodes[grandParent].right;

					nodes[grandParent].box.combine(nodes[left].box, nodes[right].box);
					nodes[grandParent].height = 1 + math::Max(nodes[left].height, nodes[right].height);

					grandParent = nodes[grandParent].parent;
				} while (grandParent != parentTreeLeaf);
			}
			Node& node = nodes[parentTreeLeaf];
			node.box.combine(nodes[node.left].box, node.data->box);
		}
		//修复父树的包围盒
		parentTreeLeaf = nodes[parentTreeLeaf].parent;
		while (parentTreeLeaf.notNull()) {
			Node& node = nodes[parentTreeLeaf];
			if (node.isLeaf()) {
				node.box.combine(nodes[node.left].box, node.data->box);
			}
			else {
				node.box.combine(nodes[node.left].box, nodes[node.right].box);
			}
			parentTreeLeaf = node.parent;
		};
	}

	uid UIBVHBak::balance(uid iA, uid parentTreeLeaf) noexcept {
		Node& A = nodes[iA];
		if (A.height < 2) {
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
			if (C.parent == parentTreeLeaf) {
				nodes[parentTreeLeaf].left = iC;
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
			if (B.parent == parentTreeLeaf) {
				nodes[parentTreeLeaf].left = iB;
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

	Abox2 UIBVHBak::getBox(uid index) const noexcept {
		DME_ASSERT(index.notNull() && index < size, "subscript exceeds the size");
		return nodes[index].box;
	}

	void UIBVHBak::updateAbox(UI& u) noexcept {
		if (!u.parent) {
			return;
		}
		uid parentTreeLeaf = u.parent->bvhIndex;
		if (parentTreeLeaf.isNull()) {
			return;
		}
		if (u.bvhIndex.isNull()) {
			uid index = allocateNode(
				Node(
					u.box,
					std::addressof(u)
				)
			);
			u.bvhIndex = index;
			insertLeaf(index, parentTreeLeaf);
		}
		else { //说明需要重新连接
			uid index = u.bvhIndex;
			Node& node = nodes[index];
			if (node.parent.notNull()) { //已经在bvh中
				disconnectLeaf(index, parentTreeLeaf);
			}
			node.box = u.box;
			insertLeaf(index, parentTreeLeaf);
		}
	}

	void UIBVHBak::updateNodeAboxImpl(uid index) noexcept {
		Node& node = nodes[index];
		if (node.right.isNull()) {
			if (node.left.notNull()) {
				updateNodeAboxImpl(node.left);
				node.box.combine(nodes[node.left].box, node.data->box);
			}
			else {
				node.box = node.data->box;
			}
		}
		else {
			updateNodeAboxImpl(node.left);
			updateNodeAboxImpl(node.right);
			node.box.combine(nodes[node.left].box, nodes[node.right].box);
		}
	}
	void UIBVHBak::updateThisAndAllChildrenAbox(UI& u) noexcept {
		if (u.bvhIndex.isNull()) {
			return;
		}
		if (!u.parent) {
			return;
		}
		uid parentTreeLeaf = u.parent->bvhIndex;
		if (parentTreeLeaf.isNull()) {
			return;
		}
		disconnectLeaf(u.bvhIndex, parentTreeLeaf);
		updateNodeAboxImpl(u.bvhIndex);
		insertLeaf(u.bvhIndex, parentTreeLeaf);
	}
	void UIBVHBak::disconnect(UI& u) noexcept {
		if (u.bvhIndex.isNull()) {
			return;
		}
		if (!u.parent) {
			return;
		}
		uid parentTreeLeaf = u.parent->bvhIndex;
		if (parentTreeLeaf.isNull()) {
			return;
		}
		disconnectLeaf(u.bvhIndex, parentTreeLeaf);
	}

	void UIBVHBak::removeImpl(UI& u) noexcept {
		if (u.bvhIndex.isNull()) {
			return;
		}
		freeNode(u.bvhIndex);
		u.bvhIndex.setNull();
		uint c = u.children.count();
		for (uint i = 0; i < c; i++) {
			removeImpl(*u.children[i]);
		}
	}
	void UIBVHBak::remove(UI& u) noexcept {
		if (u.bvhIndex.isNull()) {
			return;
		}
		if (!u.parent) {
			return;
		}
		uid parentTreeLeaf = u.parent->bvhIndex;
		if (parentTreeLeaf.isNull()) {
			return;
		}
		disconnectLeaf(u.bvhIndex, parentTreeLeaf);
		removeImpl(u);
	}

	//auto _collisionResult = DynArr<Pair<UIMeshKey, UI*>>(128);
	UI* UIBVHBak::queryCollision(const Float2& point) const noexcept {
		UI* r = null;
		UIMeshKey k = UIMeshKey::MinKey();

		GrowableStack<uid, BVHHelper::default_stack_size> stack;
		stack.push(0);
		while (stack.count() > 0) {
			uid nodeId = stack.pop();
			const Node& node = nodes[nodeId];
			if (node.box.containsNotEqualMaxSide(point)) { //左闭右开
				if (node.data) {
					UI& u = *node.data;
					if (u.box.containsNotEqualMaxSide(point) && u.pointInUI(point)) {
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