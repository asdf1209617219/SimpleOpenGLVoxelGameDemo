#include <dme/util/bvh.h>
namespace dme::util {
	BVH2::BVH2() : count(0), capacity(128), freeList(), root(0) {
		nodes = Malloc<Node>(capacity);
		Memset<Node>(nodes, uchar_max, capacity); //相当于除了box属性全部初始化为-1
	}

	BVH2::~BVH2() {
		if (nodes) {
			Free(nodes);
			nodes = null;
		}
	}

	uid BVH2::allocateNode(Node&& node) {
		uid index;
		if (freeList.isNull()) {
			if (count == capacity) { //扩容
				capacity = capacity << 1;
				Node* newNodes = Malloc<Node>(capacity);
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
	void BVH2::freeNode(uid index) {
		DME_ASSERT(index.notNull() && index < capacity, "subscript exceeds the capacity");
		nodes[index].nextFree = freeList;
		freeList = index;
		count--;
	}

	void BVH2::insertLeaf(uid leaf) {
		if (leaf == root) {
			return;
		}

		//查找此节点的最佳兄弟节点
		const auto& leafBox = nodes[leaf].box;
		uid index = root;
		uint child = 0; //选择的子节点索引
		while (nodes[index].notLeaf()) { //不是叶子节点的必有左右子节点
			/*
			uid left = nodes[index].left;
			uid right = nodes[index].right;

			float currCost = BVHHelper::boxCost(nodes[index].box);
			float combinedCost = BVHHelper::boxCombinedCost(nodes[index].box, leafBox);
			//为此节点和新叶节点创建新父节点的成本
			float cost = combinedCost * 2.0f;
			//将叶节点下降的最低成本
			float inheritanceCost = (combinedCost - currCost) * 2.0f;
			//下降至左节点的成本
			float leftCost = BVHHelper::boxCombinedCost(nodes[left].box, leafBox) + inheritanceCost;
			if (nodes[left].notLeaf()) {
				leftCost -= BVHHelper::boxCost(nodes[left].box);
			}
			//下降至右节点的成本
			float rightCost = BVHHelper::boxCombinedCost(nodes[right].box, leafBox) + inheritanceCost;
			if (nodes[right].notLeaf()) {
				rightCost -= BVHHelper::boxCost(nodes[right].box);
			}
			//根据最低成本下降
			if (cost < leftCost && cost < rightCost) {
				break;
			}
			//下降
			index = leftCost < rightCost ? left : right;
			*/

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
			//将index替换为newParent
			/*
			if (nodes[oldParent].left == index) {
				nodes[oldParent].left = newParent;
			}
			else {
				nodes[oldParent].right = newParent;
			}
			*/
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
	void BVH2::removeLeaf(uid leaf) {
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
	uid BVH2::balance(uid iA) {
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


	void* BVH2::getData(uid index) const {
		DME_ASSERT(index.notNull() && index < capacity, "subscript exceeds the capacity");
		return nodes[index].data;
	}
	Abox2 BVH2::getBox(uid index) const {
		DME_ASSERT(index.notNull() && index < capacity, "subscript exceeds the capacity");
		return nodes[index].box;
	}
	Abox2 BVH2::getRootBox() const {
		return nodes[root].box;
	}

	void BVH2::add(uid& index, const Abox2& box, void* data) {
		DME_ASSERT(index.isNull(), "index cannot be null");
		uid index_ = allocateNode(
			Node(
				//添加的包围盒要比实际的更大
				BVHHelper::BoxFat(box),
				0,
				data
			)
		);

		insertLeaf(index_);
		index = index_;
	}

	void BVH2::update(uid index, void* data) {
		DME_ASSERT(index.notNull() && index < capacity, "subscript exceeds the capacity");
		nodes[index].data = data;
	}
	void BVH2::updateAbox(uid index, const math::Abox2& box) {
		DME_ASSERT(index.notNull() && index < capacity, "subscript exceeds the capacity");
		DME_ASSERT(nodes[index].isLeaf(), "node is not a leaf node");

		removeLeaf(index);
		nodes[index].box = box;
		insertLeaf(index);
	}
	void BVH2::remove(uid& index) {
		DME_ASSERT(index.notNull() && index < capacity, "subscript exceeds the capacity");
		DME_ASSERT(nodes[index].isLeaf(), "node is not a leaf node");

		removeLeaf(index);
		freeNode(index);
		index.setNull();
	}
	bool BVH2::move(uid index, const math::Abox2& box, const Float2& delta) {
		DME_ASSERT(index.notNull() && index < capacity, "subscript exceeds the capacity");
		DME_ASSERT(nodes[index].isLeaf(), "node is not a leaf node");

		Abox2 fatBox = BVHHelper::BoxFat(box);
		//预测之后的移动，将包围盒进行扩展
		BVHHelper::BoxMovePredict(fatBox, delta);

		const Abox2& treeBox = nodes[index].box;
		if (treeBox.contains(box)) {
			//treeBox仍然包含该对象，但它可能太大
			//可能这个物体移动得很快，但后来停止了
			//hugeBox比fatBox大
			Abox2 hugeBox = Abox2(
				fatBox.min - BVHHelper::box_move_predict_count * BVHHelper::box_extend,
				fatBox.max + BVHHelper::box_move_predict_count * BVHHelper::box_extend
			);

			//treeBox不太大，不需要更新树
			if (hugeBox.contains(treeBox)) {
				return false;
			}
			//否则treeBox太大，需要缩小
		}

		removeLeaf(index);
		nodes[index].box = fatBox;
		insertLeaf(index);

		return true;
	}

	void BVH2::queryCollision(const Abox2& box, const std::function<void(bool&, uid, const Node&)>& callback) const {
		if (callback == null || count == 0) return;

		GrowableStack<uid, BVHHelper::default_stack_size> stack;
		stack.push(root);

		bool isBreak = false;
		while (stack.count() > 0) {
			uid nodeId = stack.pop();
			//if (nodeId.isNull()) {
			//	continue;
			//}

			const Node& node = nodes[nodeId];

			if (node.box.adjacentOrIntersect(box)) {
			//if (node.box.intersect(box)) {
				if (node.isLeaf()) {
					callback(isBreak, nodeId, node);
					if (isBreak) {
						return;
					}
				}
				else {
					stack.push(node.left);
					stack.push(node.right);
				}
			}
		}
	}
	void BVH2::queryCollision(const Float2& point, const std::function<void(bool&, uid, const Node&)>& callback) const {
		if (callback == null || count == 0) return;

		GrowableStack<uid, BVHHelper::default_stack_size> stack;
		stack.push(root);

		bool isBreak = false;
		while (stack.count() > 0) {
			uid nodeId = stack.pop();
			//if (nodeId.isNull()) {
			//	continue;
			//}

			const Node& node = nodes[nodeId];

			if (node.box.contains(point)) {
				if (node.isLeaf()) {
					callback(isBreak, nodeId, node);
					if (isBreak) {
						return;
					}
				}
				else {
					stack.push(node.left);
					stack.push(node.right);
				}
			}
		}
	}

	void BVH2::rayCast(const math::Ray2& ray, float maxRange, const std::function<void(bool&, uid, const Node&)>& callback) const {
		if (callback == null || count == 0) return;

		GrowableStack<uid, BVHHelper::default_stack_size> stack;
		stack.push(root);

		bool isBreak = false;
		while (stack.count() > 0) {
			uid nodeId = stack.pop();
			//if (nodeId.isNull()) {
			//	continue;
			//}

			const Node& node = nodes[nodeId];

			if (node.box.intersect(ray, maxRange)) {
				if (node.isLeaf()) {
					callback(isBreak, nodeId, node);
					if (isBreak) {
						return;
					}
				}
				else {
					stack.push(node.left);
					stack.push(node.right);
				}
			}
		}
	}



	BVH3::BVH3() : count(0), capacity(128), freeList(), root(0) {
		nodes = Malloc<Node>(capacity);
		Memset<Node>(nodes, uchar_max, capacity); //相当于除了box属性全部初始化为-1
	}

	BVH3::~BVH3() {
		if (nodes) {
			Free(nodes);
			nodes = null;
		}
	}

	uid BVH3::allocateNode(Node&& node) {
		uid index;
		if (freeList.isNull()) {
			if (count == capacity) { //扩容
				capacity = capacity << 1;
				Node* newNodes = Malloc<Node>(capacity);
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
	void BVH3::freeNode(uid index) {
		DME_ASSERT(index.notNull() && index < capacity, "subscript exceeds the capacity");
		nodes[index].nextFree = freeList;
		freeList = index;
		count--;
	}

	void BVH3::insertLeaf(uid leaf) {
		if (leaf == root) {
			return;
		}

		//查找此节点的最佳兄弟节点
		const auto& leafBox = nodes[leaf].box;
		uid index = root;
		uint child = 0; //选择的子节点索引
		while (nodes[index].notLeaf()) { //不是叶子节点的必有左右子节点
			/*
			uid left = nodes[index].left;
			uid right = nodes[index].right;

			float currCost = BVHHelper::boxCost(nodes[index].box);
			float combinedCost = BVHHelper::boxCombinedCost(nodes[index].box, leafBox);
			//为此节点和新叶节点创建新父节点的成本
			float cost = combinedCost * 2.0f;
			//将叶节点下降的最低成本
			float inheritanceCost = (combinedCost - currCost) * 2.0f;
			//下降至左节点的成本
			float leftCost = BVHHelper::boxCombinedCost(nodes[left].box, leafBox) + inheritanceCost;
			if (nodes[left].notLeaf()) {
				leftCost -= BVHHelper::boxCost(nodes[left].box);
			}
			//下降至右节点的成本
			float rightCost = BVHHelper::boxCombinedCost(nodes[right].box, leafBox) + inheritanceCost;
			if (nodes[right].notLeaf()) {
				rightCost -= BVHHelper::boxCost(nodes[right].box);
			}
			//根据最低成本下降
			if (cost < leftCost && cost < rightCost) {
				break;
			}
			//下降
			index = leftCost < rightCost ? left : right;
			*/

			uid children[2] = { nodes[index].children[0], nodes[index].children[1] };
			child = BVHHelper::BoxSelect(leafBox, nodes[children[0]].box, nodes[children[1]].box);
			index = children[child];
		}

		//新建父节点
		uid oldParent = nodes[index].parent;
		uid newParent = allocateNode(Node(Abox3::GetCombine(leafBox, nodes[index].box), oldParent, nodes[index].height + 1));
		if (oldParent.isNull()) { //index是根节点
			root = newParent;
		}
		else {
			//将index替换为newParent
			/*
			if (nodes[oldParent].left == index) {
				nodes[oldParent].left = newParent;
			}
			else {
				nodes[oldParent].right = newParent;
			}
			*/
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
	void BVH3::removeLeaf(uid leaf) {
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
	uid BVH3::balance(uid iA) {
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


	void* BVH3::getData(uid index) const {
		DME_ASSERT(index.notNull() && index < capacity, "subscript exceeds the capacity");
		return nodes[index].data;
	}
	Abox3 BVH3::getBox(uid index) const {
		DME_ASSERT(index.notNull() && index < capacity, "subscript exceeds the capacity");
		return nodes[index].box;
	}
	Abox3 BVH3::getRootBox() const {
		return nodes[root].box;
	}

	void BVH3::add(uid& index, const Abox3& box, void* data) {
		DME_ASSERT(index.isNull(), "index cannot be null");
		uid index_ = allocateNode(
			Node(
				//添加的包围盒要比实际的更大
				BVHHelper::BoxFat(box),
				0,
				data
			)
		);
		insertLeaf(index_);
		index = index_;
	}

	void BVH3::update(uid index, void* data) {
		DME_ASSERT(index.notNull() && index < capacity, "subscript exceeds the capacity");
		nodes[index].data = data;
	}
	void BVH3::updateAbox(uid index, const math::Abox3& box) {
		DME_ASSERT(index.notNull() && index < capacity, "subscript exceeds the capacity");
		DME_ASSERT(nodes[index].isLeaf(), "node is not a leaf node");

		removeLeaf(index);
		nodes[index].box = box;
		insertLeaf(index);
	}
	void BVH3::remove(uid& index) {
		DME_ASSERT(index.notNull() && index < capacity, "subscript exceeds the capacity");
		DME_ASSERT(nodes[index].isLeaf(), "node is not a leaf node");

		removeLeaf(index);
		freeNode(index);
		index.setNull();
	}
	bool BVH3::move(uid index, const math::Abox3& box, const Float3& delta) {
		DME_ASSERT(index.notNull() && index < capacity, "subscript exceeds the capacity");
		DME_ASSERT(nodes[index].isLeaf(), "node is not a leaf node");

		Abox3 fatBox = BVHHelper::BoxFat(box);
		//预测之后的移动，将包围盒进行扩展
		BVHHelper::BoxMovePredict(fatBox, delta);

		const Abox3& treeBox = nodes[index].box;
		if (treeBox.contains(box)) {
			//treeBox仍然包含该对象，但它可能太大
			//可能这个物体移动得很快，但后来停止了
			//hugeBox比fatBox大
			Abox3 hugeBox = Abox3(
				fatBox.min - BVHHelper::box_move_predict_count * BVHHelper::box_extend,
				fatBox.max + BVHHelper::box_move_predict_count * BVHHelper::box_extend
			);

			//treeBox不太大，不需要更新树
			if (hugeBox.contains(treeBox)) {
				return false;
			}
			//否则treeBox太大，需要缩小
		}

		removeLeaf(index);
		nodes[index].box = fatBox;
		insertLeaf(index);

		return true;
	}

	void BVH3::queryCollision(const Abox3& box, const std::function<void(bool&, uid, const Node&)>& callback) const {
		if (callback == null || count == 0) return;

		GrowableStack<uid, BVHHelper::default_stack_size> stack;
		stack.push(root);

		bool isBreak = false;
		while (stack.count() > 0) {
			uid nodeId = stack.pop();
			//if (nodeId.isNull()) {
			//	continue;
			//}

			const Node& node = nodes[nodeId];

			if (node.box.adjacentOrIntersect(box)) {
			//if (node.box.intersect(box)) {
				if (node.isLeaf()) {
					callback(isBreak, nodeId, node);
					if (isBreak) {
						return;
					}
				}
				else {
					stack.push(node.left);
					stack.push(node.right);
				}
			}
		}
	}
	void BVH3::queryCollision(const Float3& point, const std::function<void(bool&, uid, const Node&)>& callback) const {
		if (callback == null || count == 0) return;

		GrowableStack<uid, BVHHelper::default_stack_size> stack;
		stack.push(root);

		bool isBreak = false;
		while (stack.count() > 0) {
			uid nodeId = stack.pop();
			//if (nodeId.isNull()) {
			//	continue;
			//}

			const Node& node = nodes[nodeId];

			if (node.box.contains(point)) {
				if (node.isLeaf()) {
					callback(isBreak, nodeId, node);
					if (isBreak) {
						return;
					}
				}
				else {
					stack.push(node.left);
					stack.push(node.right);
				}
			}
		}
	}

	void BVH3::rayCast(const math::Ray3& ray, float maxRange, const std::function<void(bool&, uid, const Node&)>& callback) const {
		if (callback == null || count == 0) return;

		GrowableStack<uid, BVHHelper::default_stack_size> stack;
		stack.push(root);

		bool isBreak = false;
		while (stack.count() > 0) {
			uid nodeId = stack.pop();
			//if (nodeId.isNull()) {
			//	continue;
			//}

			const Node& node = nodes[nodeId];

			if (node.box.intersect(ray, maxRange)) {
				if (node.isLeaf()) {
					callback(isBreak, nodeId, node);
					if (isBreak) {
						return;
					}
				}
				else {
					stack.push(node.left);
					stack.push(node.right);
				}
			}
		}
	}


	BVH3f::BVH3f() : count(0), capacity(128), freeList(), root(0) {
		nodes = Malloc<Node>(capacity);
		Memset<Node>(nodes, uchar_max, capacity); //相当于除了box属性全部初始化为-1
	}

	BVH3f::~BVH3f() {
		if (nodes) {
			Free(nodes);
			nodes = null;
		}
	}

	uid BVH3f::allocateNode(Node&& node) {
		uid index;
		if (freeList.isNull()) {
			if (count == capacity) { //扩容
				capacity = capacity << 1;
				Node* newNodes = Malloc<Node>(capacity);
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
	void BVH3f::freeNode(uid index) {
		DME_ASSERT(index.notNull() && index < capacity, "subscript exceeds the capacity");
		nodes[index].nextFree = freeList;
		freeList = index;
		count--;
	}

	void BVH3f::insertLeaf(uid leaf) {
		if (leaf == root) {
			return;
		}

		//查找此节点的最佳兄弟节点
		const auto& leafBox = nodes[leaf].box;
		uid index = root;
		uint child = 0; //选择的子节点索引
		while (nodes[index].notLeaf()) { //不是叶子节点的必有左右子节点
			/*
			uid left = nodes[index].left;
			uid right = nodes[index].right;

			float currCost = BVHHelper::boxCost(nodes[index].box);
			float combinedCost = BVHHelper::boxCombinedCost(nodes[index].box, leafBox);
			//为此节点和新叶节点创建新父节点的成本
			float cost = combinedCost * 2.0f;
			//将叶节点下降的最低成本
			float inheritanceCost = (combinedCost - currCost) * 2.0f;
			//下降至左节点的成本
			float leftCost = BVHHelper::boxCombinedCost(nodes[left].box, leafBox) + inheritanceCost;
			if (nodes[left].notLeaf()) {
				leftCost -= BVHHelper::boxCost(nodes[left].box);
			}
			//下降至右节点的成本
			float rightCost = BVHHelper::boxCombinedCost(nodes[right].box, leafBox) + inheritanceCost;
			if (nodes[right].notLeaf()) {
				rightCost -= BVHHelper::boxCost(nodes[right].box);
			}
			//根据最低成本下降
			if (cost < leftCost && cost < rightCost) {
				break;
			}
			//下降
			index = leftCost < rightCost ? left : right;
			*/

			uid children[2] = { nodes[index].children[0], nodes[index].children[1] };
			child = BVHHelper::BoxSelect(leafBox, nodes[children[0]].box, nodes[children[1]].box);
			index = children[child];
		}

		//新建父节点
		uid oldParent = nodes[index].parent;
		uid newParent = allocateNode(Node(Abox3f::GetCombine(leafBox, nodes[index].box), oldParent, nodes[index].height + 1));
		if (oldParent.isNull()) { //index是根节点
			root = newParent;
		}
		else {
			//将index替换为newParent
			/*
			if (nodes[oldParent].left == index) {
				nodes[oldParent].left = newParent;
			}
			else {
				nodes[oldParent].right = newParent;
			}
			*/
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
	void BVH3f::removeLeaf(uid leaf) {
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
	uid BVH3f::balance(uid iA) {
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


	void* BVH3f::getData(uid index) const {
		DME_ASSERT(index.notNull() && index < capacity, "subscript exceeds the capacity");
		return nodes[index].data;
	}
	Abox3f BVH3f::getBox(uid index) const {
		DME_ASSERT(index.notNull() && index < capacity, "subscript exceeds the capacity");
		return nodes[index].box;
	}
	Abox3f BVH3f::getRootBox() const {
		return nodes[root].box;
	}

	void BVH3f::add(uid& index, const Abox3f& box, void* data) {
		DME_ASSERT(index.isNull(), "index cannot be null");
		uid index_ = allocateNode(
			Node(
				//添加的包围盒要比实际的更大
				BVHHelper::BoxFat(box),
				0,
				data
			)
		);
		insertLeaf(index_);
		index = index_;
	}

	void BVH3f::update(uid index, void* data) {
		DME_ASSERT(index.notNull() && index < capacity, "subscript exceeds the capacity");
		nodes[index].data = data;
	}
	void BVH3f::updateAbox(uid index, const math::Abox3f& box) {
		DME_ASSERT(index.notNull() && index < capacity, "subscript exceeds the capacity");
		DME_ASSERT(nodes[index].isLeaf(), "node is not a leaf node");

		if (nodes[index].box != box) {
			removeLeaf(index);
			nodes[index].box = box;
			insertLeaf(index);
		}
	}
	void BVH3f::remove(uid& index) {
		DME_ASSERT(index.notNull() && index < capacity, "subscript exceeds the capacity");
		DME_ASSERT(nodes[index].isLeaf(), "node is not a leaf node");

		removeLeaf(index);
		freeNode(index);
		index.setNull();
	}
	bool BVH3f::move(uid index, const math::Abox3f& box, const Float3& delta) {
		DME_ASSERT(index.notNull() && index < capacity, "subscript exceeds the capacity");
		DME_ASSERT(nodes[index].isLeaf(), "node is not a leaf node");

		Abox3f fatBox = BVHHelper::BoxFat(box);
		//预测之后的移动，将包围盒进行扩展
		BVHHelper::BoxMovePredict(fatBox, delta);

		const Abox3f& treeBox = nodes[index].box;
		if (treeBox.contains(box)) {
			//treeBox仍然包含该对象，但它可能太大
			//可能这个物体移动得很快，但后来停止了
			//hugeBox比fatBox大
			Abox3f hugeBox = Abox3f(
				fatBox.min - BVHHelper::box_move_predict_count * BVHHelper::box_extend,
				fatBox.max + BVHHelper::box_move_predict_count * BVHHelper::box_extend
			);

			//treeBox不太大，不需要更新树
			if (hugeBox.contains(treeBox)) {
				return false;
			}
			//否则treeBox太大，需要缩小
		}

		removeLeaf(index);
		nodes[index].box = fatBox;
		insertLeaf(index);

		return true;
	}

	void BVH3f::queryCollision(const Abox3f& box, const std::function<void(bool&, uid, const Node&)>& callback) const {
		if (callback == null || count == 0) return;

		GrowableStack<uid, BVHHelper::default_stack_size> stack;
		stack.push(root);

		bool isBreak = false;
		while (stack.count() > 0) {
			uid nodeId = stack.pop();
			//if (nodeId.isNull()) {
			//	continue;
			//}

			const Node& node = nodes[nodeId];

			if (node.box.adjacentOrIntersect(box)) {
			//if (node.box.intersect(box)) {
				if (node.isLeaf()) {
					callback(isBreak, nodeId, node);
					if (isBreak) {
						return;
					}
				}
				else {
					stack.push(node.left);
					stack.push(node.right);
				}
			}
		}
	}
	void BVH3f::queryCollision(const Fix3& point, const std::function<void(bool&, uid, const Node&)>& callback) const {
		if (callback == null || count == 0) return;

		GrowableStack<uid, BVHHelper::default_stack_size> stack;
		stack.push(root);

		bool isBreak = false;
		while (stack.count() > 0) {
			uid nodeId = stack.pop();
			//if (nodeId.isNull()) {
			//	continue;
			//}

			const Node& node = nodes[nodeId];

			if (node.box.contains(point)) {
				if (node.isLeaf()) {
					callback(isBreak, nodeId, node);
					if (isBreak) {
						return;
					}
				}
				else {
					stack.push(node.left);
					stack.push(node.right);
				}
			}
		}
	}

	void BVH3f::rayCast(const math::Ray3f& ray, float maxRange, const std::function<void(bool&, uid, const Node&)>& callback) const {
		if (callback == null || count == 0) return;

		GrowableStack<uid, BVHHelper::default_stack_size> stack;
		stack.push(root);

		bool isBreak = false;
		while (stack.count() > 0) {
			uid nodeId = stack.pop();
			//if (nodeId.isNull()) {
			//	continue;
			//}

			const Node& node = nodes[nodeId];

			if (node.box.intersect(ray, maxRange)) {
				if (node.isLeaf()) {
					callback(isBreak, nodeId, node);
					if (isBreak) {
						return;
					}
				}
				else {
					stack.push(node.left);
					stack.push(node.right);
				}
			}
		}
	}

}