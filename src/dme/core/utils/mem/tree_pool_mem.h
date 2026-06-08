#pragma once
#include <dme/core/utils/dyn_arr.h>
#include <dme/core/utils/pair.h>
#include <dme/core/utils/growable_stack.h>
#include <dme/core/utils/mem/_compressed_pair.h>
namespace dme::core {
	//树的节点
	template<typename T>
	struct _tree_node {
		using value_type = T;
		static constexpr bool SOO = sizeof(value_type) <= ptr_size;
		using data_type = std::conditional_t<SOO, value_type, value_type*>;

		_tree_node* parent; //父节点
		union {
			struct {
				_tree_node* left; //左节点
				_tree_node* right; //右节点
			};
			_tree_node* children[2]; //子节点
		};
		data_type data; //如果value_type大小小于等于size_t，则直接保存为值
		sint height; //设置为0则表示该节点是被删除状态，叶子节点为1

		constexpr sint getLeftHeight() const noexcept {
			if (left) {
				return left->height;
			}
			return 0;
		}
		constexpr sint getRightHeight() const noexcept {
			if (right) {
				return right->height;
			}
			return 0;
		}
		constexpr void updateHeight() noexcept {
			sint hl = getLeftHeight();
			sint hr = getRightHeight();
			height = (hl > hr ? hl : hr) + 1;
		}
		constexpr value_type* getDataPtr() noexcept {
			if constexpr (SOO) {
				return std::addressof(data);
			}
			else {
				return data;
			}
		}
		constexpr const value_type* getDataPtr() const noexcept {
			if constexpr (SOO) {
				return std::addressof(data);
			}
			else {
				return data;
			}
		}
		//释放节点的数据
		constexpr void destoryData() noexcept {
			if constexpr (SOO) {
				data.~value_type();
			}
			else {
				data->~value_type();
			}
		}
		//设置为叶子节点
		constexpr void setIsLeaf() noexcept {
			parent = null;
			left = null;
			right = null;
			height = 1;
		}
		//设置为叶子节点，并设置父节点的子节点
		constexpr void setIsLeaf(_tree_node* parent_, uint childrenIndex) noexcept {
			parent = parent_;
			left = null;
			right = null;
			height = 1;
			parent_->children[childrenIndex] = this;
		}
		constexpr bool isLeaf() const noexcept {
			return height == 1;
		}
		constexpr void setDeleted() noexcept {
			height = 0;
			left = null;
			right = null;
		}
		constexpr bool isDeleted() const noexcept {
			return height == 0;
		}
	};

	//树内存块信息
	template<bool SOO, typename value_type>
	struct _tree_pool_mem_info {};

	template<typename value_type>
	struct _tree_pool_mem_info<false, value_type> {
		//内存块开始地址
		uchar* begin;
		//内存块结束地址+1
		uchar* end;
		union {
			//value对象开始地址
			uchar* valueBegin;
			value_type* valueBeginPtr;
		};
		//下一个node对象的位置
		uchar* node;
		//下一个value对象的位置，>=end则内存块用尽
		uchar* value;

		constexpr _tree_pool_mem_info(uchar* begin_, uchar* end_, uchar* valueBegin_)
			noexcept : begin(begin_), end(end_), valueBegin(valueBegin_), node(begin_), value(valueBegin_) {
		}
		constexpr bool isFull() const noexcept {
			return value >= end;
		}
	};

	template<typename value_type>
	struct _tree_pool_mem_info<true, value_type> {
		//内存块开始地址
		uchar* begin;
		//内存块结束地址+1
		uchar* end;
		//下一个node对象的位置
		uchar* node;

		constexpr _tree_pool_mem_info(uchar* begin_, uchar* end_)
			noexcept : begin(begin_), end(end_), node(begin_) {
		}
		constexpr bool isFull() const noexcept {
			return node >= end;
		}
	};


	//拷贝并交换，利用RAII保证强异常安全性
	template<typename M>
	struct _tree_pool_mem_copy_and_swap {
		using mem_type = M;
		using value_type = mem_type::value_type;
		static constexpr bool SOO = mem_type::SOO;
		using info_type = mem_type::info_type;
		using node_type = mem_type::node_type;
		static constexpr size_t node_size = mem_type::node_size;
		static constexpr size_t value_size = mem_type::value_size;
		static constexpr size_t data_offset = mem_type::data_offset;
		static constexpr size_t val_offset = mem_type::val_offset;

		uchar* data = null;
		size_t success_count = 0;

		//拷贝成功则清空to_.infos并将新的info添加进去，拷贝过程中触发异常则原对象无影响
		constexpr void copyLValAndSwap(size_t count, size_t size, const mem_type& from_, mem_type& to_, const bool needClear) noexcept(mem_type::noexcept_copy) {
			info_type info = mem_type::MallocInfo(size);
			data = info.begin;

			node_type* nowNode = reinterpret_cast<node_type*>(data);
			nowNode->left = from_.root; //用left记录下一个需要拷贝的原node
			node_type* usedNode = nowNode;
			node_type* fromNode;
			for (; nowNode < reinterpret_cast<node_type*>(data) + count; ++nowNode) {
				++usedNode;
				fromNode = nowNode->left;

				value_type* pv;
				if constexpr (SOO) {
					pv = nowNode->getDataPtr();
				}
				else {
					pv = reinterpret_cast<value_type*>(info.valueBegin + value_size * success_count);
					nowNode->data = pv;
				}
				if constexpr (is_relocatable<value_type>) {
					MemcpyByte(pv, fromNode->getDataPtr(), value_size);
				}
				else {
					new (pv) value_type(*fromNode->getDataPtr());
				}

				nowNode->height = fromNode->height;
				if (fromNode->left) {
					nowNode->left = usedNode;
					usedNode->left = fromNode->left;
					usedNode->parent = nowNode;
					++usedNode;
				}
				if (fromNode->right) {
					nowNode->right = usedNode;
					usedNode->left = fromNode->right;
					usedNode->parent = nowNode;
					++usedNode;
				}
				++success_count;
			}
			//swap
			info.node = info.begin + node_size * count;
			if constexpr (!SOO) {
				info.value = info.valueBegin + value_size * count;
			}
			if (needClear) {
				to_.destoryAllInfo();
			}
			to_.root = reinterpret_cast<node_type*>(data);
			to_.freelist = null;
			to_.count() = count;
			if (!to_.infosSize) {
				to_.infos = Malloc<info_type>(2);
				to_.infosCount = 1;
				to_.infosSize = 2;
				to_.nowInfo = 0;
				to_.infos[0] = info;
			}
			else {
				to_.infosCount = 1;
				to_.nowInfo = 0;
				to_.infos[0] = info;
			}
			data = null;
		}

		//拷贝成功则清空to_.infos并将新的info添加进去，拷贝过程中触发异常则原对象无影响
		//函数结束后需要对node挨个调用更新insertNodeUpdateTree方法
		constexpr void copyRValAndSwap(size_t count, const value_type* from_, mem_type& to_, const bool needClear) noexcept(mem_type::noexcept_move) {
			info_type info = mem_type::MallocInfo(count);
			data = info.begin;

			if constexpr (!SOO && is_relocatable<value_type>) { //非SOO情况下value才是连续内存
				MemcpyByte(info.valueBegin, from_, count * value_size);
				for (size_t i = 0; i < count; ++i) {
					node_type* nowNode = reinterpret_cast<node_type*>(data) + i;
					value_type* pv = reinterpret_cast<value_type*>(info.valueBegin + value_size * i);
					nowNode->data = pv;
				}
			}
			else {
				node_type* nowNode = reinterpret_cast<node_type*>(data);
				const value_type* nowValue = from_;
				while (nowNode < reinterpret_cast<node_type*>(data) + count) {
					value_type* pv;
					if constexpr (SOO) {
						pv = nowNode->getDataPtr();
					}
					else {
						pv = reinterpret_cast<value_type*>(info.valueBegin + value_size * success_count);
						nowNode->data = pv;
					}
					if constexpr (is_relocatable<value_type>) {
						if constexpr (value_size <= Config::memory_alignment && std::is_trivially_copyable_v<value_type>) {
							new (pv) value_type(std::move(*nowValue));
						}
						else {
							MemcpyByte(pv, nowValue, value_size);
						}
					}
					else {
						new (pv) value_type(std::move(*nowValue));
						nowValue->~value_type();
					}
					++nowNode;
					++nowValue;
					++success_count;
				}
			}

			//swap
			info.node = info.begin + node_size * count;
			if constexpr (!SOO) {
				info.value = info.valueBegin + value_size * count;
			}
			if (needClear) {
				to_.destoryAllInfo();
			}
			//to_.root = reinterpret_cast<node_type*>(data); //通过insertNodeUpdateTree更新root
			to_.freelist = null;
			to_.count() = count;
			if (!to_.infosSize) {
				to_.infos = Malloc<info_type>(2);
				to_.infosCount = 1;
				to_.infosSize = 2;
				to_.nowInfo = 0;
				to_.infos[0] = info;
			}
			else {
				to_.infosCount = 1;
				to_.nowInfo = 0;
				to_.infos[0] = info;
			}
			data = null;
		}

		constexpr ~_tree_pool_mem_copy_and_swap() {
			if (data) [[unlikely]] { //拷贝时触发异常的分支
				//析构success_count个元素，如果元素的类型具有trivially destructible特性，则跳过析构
				if constexpr (!std::is_trivially_destructible_v<value_type>) {
					for (node_type* pn = reinterpret_cast<node_type*>(data);
						pn < reinterpret_cast<node_type*>(data) + success_count;
						++pn)
					{
						pn->getDataPtr()->~value_type();
					}
				}
				Free(data);
				data = null;
			}
		}
	};


	//树内存池，具有指针稳定性，同时保存node和value对象，删除和保存必须一对一对进行，但是两种对象分为两段内存并且同种对象连续
	template<bool IsPair, typename K, typename V, typename LessFunc>
	struct _tree_pool_mem {
		using key_type = K;
		using value_type = std::conditional_t<IsPair, Pair<K, V>, K>;
		static constexpr bool SOO = sizeof(value_type) <= ptr_size; //是否将指针优化为值
		using info_type = _tree_pool_mem_info<SOO, value_type>;
		using node_type = _tree_node<value_type>;
		using pair_type = Pair<K, V>;
		static constexpr size_t data_offset = offsetof(node_type, data); //data属性在node里的偏移量
		static constexpr size_t val_offset = offsetof(pair_type, val); //val属性在Pair<K, V>里的偏移量
		static constexpr size_t node_size = sizeof(node_type);
		static constexpr size_t value_size = sizeof(value_type);
		static constexpr size_t default_size = 8 * Config::default_init_size;
		static constexpr node_type* end_node = null;

		//默认构造函数
		static constexpr bool noexcept_default = is_relocatable<value_type> || nothrow_default_construct<value_type>;
		//拷贝
		static constexpr bool noexcept_copy = is_relocatable<value_type> || nothrow_copy_construct<value_type>;
		//所有权转移
		static constexpr bool noexcept_move = is_relocatable<value_type> || nothrow_move_construct<value_type> || nothrow_copy_construct<value_type>;
		//析构（理论上不允许析构时抛出异常）
		static constexpr bool noexcept_destory = nothrow_destruct<value_type>;

		//按地址顺序保存内存块信息
		info_type* infos;
		//info数量
		uchar infosCount;
		//infos容量
		uchar infosSize;
		//当前使用info
		uchar nowInfo;
		//自由链表表头（指向空闲node对象内存，其中parent属性指向下一个freelist）
		node_type* freelist;
		//根节点
		node_type* root;
		//元素总数，压缩LessFunc和count
		_compressed_pair<LessFunc, size_t> compress;

		constexpr size_t& count() noexcept {
			return compress.val;
		}
		constexpr const size_t& count() const noexcept {
			return compress.val;
		}
		constexpr LessFunc& func() noexcept {
			return compress.key();
		}
		constexpr const LessFunc& func() const noexcept {
			return compress.key();
		}

		static constexpr const key_type& GetValueKey(const value_type& value) noexcept {
			if constexpr (IsPair) {
				return value.key;
			}
			else {
				return value;
			}
		}
		static constexpr const key_type& GetNodeKey(const node_type* node) noexcept {
			if constexpr (IsPair) {
				return node->getDataPtr()->key;
			}
			else {
				return *node->getDataPtr();
			}
		}
		static constexpr const key_type* GetNodeKeyPtr(const node_type* node) noexcept {
			if constexpr (IsPair) {
				return std::addressof(node->getDataPtr()->key);
			}
			else {
				return node->getDataPtr();
			}
		}
		//node的内存会初始化为0
		static constexpr info_type MallocInfo(const size_t size) noexcept {
			if constexpr (SOO) {
				size_t nsize = node_size * size; //node对象占用字节
				uchar* data = Malloc<uchar>(nsize);
				MemsetByte(data, 0, nsize);//初始化为0
				return info_type(data, data + nsize);
			}
			else {
				size_t nsize = node_size * size; //node对象占用字节
				if (nsize % Config::memory_alignment != 0) { //内存对齐
					nsize += Config::memory_alignment - (nsize % Config::memory_alignment);
				}
				size_t vsize = value_size * size; //value对象占用字节
				uchar* data = Malloc<uchar>(nsize + vsize);
				MemsetByte(data, 0, nsize);//初始化为0
				return info_type(data, data + nsize + vsize, data + nsize);
			}
		}

		constexpr void insertInfo(info_type info) noexcept {
			uchar i1 = 0;
			uchar i2 = infosCount;
			while (i1 < i2) { //二分查找
				uchar i = i1 + ((i2 - i1) / 2);
				if (infos[i].begin < info.begin) {
					i1 = i + 1;
				}
				else {
					i2 = i;
				}
			}
			if (infosCount == infosSize) {
				infosSize *= 2;
				info_type* newInfos = Malloc<info_type>(infosSize);
				if (i1 != 0) {
					Memcpy<info_type>(newInfos, infos, i1);
				}
				if (i1 < infosCount) {
					uchar c = infosCount - i1;
					Memcpy<info_type>(newInfos + i1 + 1, infos + i1, c);
				}
				Free(infos);
				infos = newInfos;
			}
			else {
				if (i1 < infosCount) {
					uchar c = infosCount - i1;
					Memmove<info_type>(infos + i1 + 1, infos + i1, c);
				}
			}
			infosCount++;
			nowInfo = i1;
			infos[i1] = info;
		}

		constexpr size_t getInfoSize(const info_type& info) const noexcept {
			if constexpr (SOO) {
				return (info.end - info.begin) / node_size;
			}
			else {
				return (info.end - info.valueBegin) / value_size;
			}
		}
		constexpr size_t getInfoTotalSize() const noexcept {
			size_t total = 0;
			for (uchar i = 0; i < infosCount; ++i) {
				total += getInfoSize(infos[i]);
			}
			return total;
		}

		//TODO 目前认为LessFunc是noexcept的
		template<typename F>
		constexpr explicit _tree_pool_mem(F&& f) noexcept : infos(null), infosCount(0), infosSize(0), nowInfo(0), 
			freelist(null), root(null), compress(std::forward<F>(f), 0) {}

		template<typename F>
		constexpr explicit _tree_pool_mem(size_t size, F&& f) noexcept : infos(null), infosCount(0), infosSize(0), nowInfo(0), 
			freelist(null), root(null), compress(std::forward<F>(f), 0) 
		{
			init(size);
		}
		constexpr _tree_pool_mem(const _tree_pool_mem& a) noexcept(noexcept_copy) : infos(null), infosCount(0), infosSize(0), nowInfo(0), 
			freelist(null), root(null), compress(a.func(), 0) 
		{
			copyInit(a);
		}
		constexpr _tree_pool_mem(_tree_pool_mem&& a) noexcept : infos(a.infos), infosCount(a.infosCount), infosSize(a.infosSize), nowInfo(a.nowInfo),
			freelist(a.freelist), root(a.root), compress(std::move(a.compress)) 
		{
			a.infos = null;
			a.infosCount = 0;
			a.infosSize = 0;
			a.nowInfo = 0;
			a.freelist = null;
			a.root = null;
			a.count() = 0;
		}
		template<typename F>
		constexpr _tree_pool_mem(std::initializer_list<value_type> list_, F&& f) noexcept(noexcept_copy) : infos(null), infosCount(0), infosSize(0), nowInfo(0),
			freelist(null), root(null), compress(std::forward<F>(f), 0) 
		{
			moveInit(list_.begin(), list_.size());
		}
		constexpr _tree_pool_mem& operator =(const _tree_pool_mem& a) noexcept(noexcept_copy) {
			if (this != std::addressof(a)) {
				copyAndCoverThis(a);
			}
			return *this;
		}
		constexpr _tree_pool_mem& operator =(_tree_pool_mem&& a) noexcept {
			if (this != std::addressof(a)) {
				destoryAllInfo();
				Free(infos);
				infos = a.infos;
				infosCount = a.infosCount;
				infosSize = a.infosSize;
				nowInfo = a.nowInfo;
				freelist = a.freelist;
				root = a.root;
				compress = std::move(a.compress);

				a.infos = null;
				a.infosCount = 0;
				a.infosSize = 0;
				a.nowInfo = 0;
				a.freelist = null;
				a.root = null;
				a.count() = 0;
			}
			return *this;
		}
		constexpr _tree_pool_mem& operator =(std::initializer_list<value_type> list_) noexcept(noexcept_copy) {
			moveAndCoverThis(list_.begin(), list_.size());
			return *this;
		}

		~_tree_pool_mem() {
			release();
		}
		//清除所有元素并回收内存
		constexpr void release() noexcept {
			if (infos) {
				destoryAllInfo();
				Free(infos);
				infos = null;
				infosSize = 0;
			}
		}

		constexpr void init(size_t size) noexcept {
			if (!size) return;
			infos = Malloc<info_type>(2);
			infosCount = 1;
			infosSize = 2;
			nowInfo = 0;
			infos[0] = MallocInfo(size);
		}

		//拷贝左值初始化（强异常安全性）
		constexpr void copyInit(const _tree_pool_mem& a) noexcept(noexcept_copy) {
			if (!a.count()) return;
			auto m = _tree_pool_mem_copy_and_swap<_tree_pool_mem>();
			m.copyLValAndSwap(a.count(), a.getInfoTotalSize(), a, *this, false);
		}
		//拷贝右值初始化（强异常安全性）
		constexpr void moveInit(const value_type* from_, const size_t count_) noexcept(noexcept_move) {
			if (!count_) return;
			auto m = _tree_pool_mem_copy_and_swap<_tree_pool_mem>();
			m.copyRValAndSwap(count_, from_, *this, false);
			auto& info = infos[nowInfo];
			for (node_type* node = reinterpret_cast<node_type*>(info.begin);
				node < reinterpret_cast<node_type*>(info.node);
				++node)
			{
				insertNodeUpdateTree(node);
			}
		}
		//拷贝左值并覆盖当前内存，自动析构原内存，修改count和size（强异常安全性）
		constexpr void copyAndCoverThis(const _tree_pool_mem& a) noexcept(noexcept_copy) {
			func() = a.func();
			if (!a.count()) {
				destoryAllInfo();
				return;
			}
			auto m = _tree_pool_mem_copy_and_swap<_tree_pool_mem>();
			m.copyLValAndSwap(a.count(), a.getInfoTotalSize(), a, *this, true);
		}
		//拷贝右值并覆盖当前内存，自动析构原内存，修改count和size（强异常安全性）
		constexpr void moveAndCoverThis(const value_type* from_, const size_t count_) noexcept(noexcept_move) {
			if (!count_) {
				destoryAllInfo();
				return;
			}
			auto m = _tree_pool_mem_copy_and_swap<_tree_pool_mem>();
			m.copyRValAndSwap(count_, from_, *this, true);
			auto& info = infos[nowInfo];
			for(node_type* node = reinterpret_cast<node_type*>(info.begin);
				node < reinterpret_cast<node_type*>(info.node);
				++node)
			{
				insertNodeUpdateTree(node);
			}
		}


		//如果空间不足则申请新的内存块
		constexpr void extend() noexcept {
			if (infos == null) {
				init(default_size);
			}
			else if (infos[nowInfo].isFull()) {
				size_t size = getInfoSize(infos[nowInfo]) * 2;
				insertInfo(MallocInfo(size));
			}
		}

		//申请node和value的空间，返回node指针，如果非SOO则设置value指针
		constexpr node_type* alloc() noexcept {
			node_type* r;
			if (freelist) {
				r = freelist;
				freelist = r->parent;
				r->parent = null;
			}
			else {
				extend();
				auto& info = infos[nowInfo];
				r = reinterpret_cast<node_type*>(info.node);
				info.node += node_size;
				if constexpr (!SOO) {
					r->data = reinterpret_cast<value_type*>(info.value);
					info.value += value_size;
				}
			}
			count()++;
			return r;
		}

		//第一个node
		constexpr node_type* firstNode() const noexcept {
			node_type* node = root;
			if (node) {
				while (node->left) {
					node = node->left;
				}
			}
			return node;
		}
		//最后一个node
		constexpr node_type* lastNode() const noexcept {
			node_type* node = root;
			if (node) {
				while (node->right) {
					node = node->right;
				}
			}
			return node;
		}
		//上一个node
		constexpr node_type* nodePrevious(node_type* node) const noexcept {
			if (node != end_node) {
				if (node->left) {
					node = node->left;
					while (node->right) {
						node = node->right;
					}
				}
				else {
					while (1) {
						node_type* pre = node;
						node = node->parent;
						if (!node) break; //一直回溯到root，说明已经遍历完整棵树，返回null
						if (node->right == pre) break;
					}
				}
			}
			else {
				node = lastNode();
			}
			return node;
		}
		//下一个node
		constexpr node_type* nodeNext(node_type* node) const noexcept {
			if (node != end_node) {
				if (node->right) {
					node = node->right;
					while (node->left) {
						node = node->left;
					}
				}
				else {
					while (true) {
						node_type* pre = node;
						node = node->parent;
						if (!node) break; //一直回溯到root，说明已经遍历完整棵树，返回null
						if (node->left == pre) break;
					}
				}
			}
			return node;
		}

		//替换子节点为新节点
		constexpr void replaceChildenNode(node_type* oldChild, node_type* newChild, node_type* parent) noexcept {
			if (parent) {
				if (parent->left == oldChild) {
					parent->left = newChild;
				}
				else {
					parent->right = newChild;
				}
			}
			else {
				root = newChild;
			}
		}
		//左旋
		constexpr node_type* nodeRotateLeft(node_type* node) noexcept {
			node_type* right = node->right;
			node_type* parent = node->parent;
			node->right = right->left;
			if (right->left) right->left->parent = node;
			right->left = node;
			right->parent = parent;
			replaceChildenNode(node, right, parent);
			node->parent = right;
			return right;
		}
		//右旋
		constexpr node_type* nodeRotateRight(node_type* node) noexcept {
			node_type* left = node->left;
			node_type* parent = node->parent;
			node->left = left->right;
			if (left->right) left->right->parent = node;
			left->right = node;
			left->parent = parent;
			replaceChildenNode(node, left, parent);
			node->parent = left;
			return left;
		}

		//LL型或LR型
		constexpr node_type* leftFix(node_type* node) noexcept {
			node_type* left = node->left;
			if (left->getLeftHeight() < left->getRightHeight()) { //LR型
				left = nodeRotateLeft(left);
				left->left->updateHeight();
				left->updateHeight();
			}
			node = nodeRotateRight(node);
			node->right->updateHeight();
			node->updateHeight();
			return node;
		}
		//RR型或RL型
		constexpr node_type* rightFix(node_type* node) noexcept {
			node_type* right = node->right;
			if (right->getLeftHeight() > right->getRightHeight()) { //RL型
				right = nodeRotateRight(right);
				right->right->updateHeight();
				right->updateHeight();
			}
			node = nodeRotateLeft(node);
			node->left->updateHeight();
			node->updateHeight();
			return node;
		}
		//添加节点后的更新工作，设置节点要插入树的哪个位置，更新调整树
		constexpr void insertNodeUpdateTree(node_type* node) noexcept {
			if (!root) {
				root = node;
				node->setIsLeaf();
				return;
			}
			const key_type& key = GetNodeKey(node);
			node_type* parent = root;
			uint selectChild;
			while(true) {
				const key_type& k = GetNodeKey(parent);
				if (func()(key , k)) {
					if (!parent->left) {
						selectChild = 0;
						break;
					}
					parent = parent->left;
				}
				else {
					if (!func()(k, key)) { //重复插入
						destructNode(node);
						return;
					}

					if (!parent->right) {
						selectChild = 1;
						break;
					}
					parent = parent->right;
				}
			};
			node->setIsLeaf(parent, selectChild);
			do {
				sint hl = parent->getLeftHeight();
				sint hr = parent->getRightHeight();
				sint h = (hl > hr ? hl : hr) + 1;
				sint diff = hl - hr;
				if (parent->height == h) break;
				parent->height = h;
				if (diff < -1) {
					parent = rightFix(parent);
				}
				else if (diff > 1) {
					parent = leftFix(parent);
				}
				parent = parent->parent;
			} while (parent);
		}

		//将node删除，count减一
		constexpr void destructNode(node_type* node) noexcept {
			node->destoryData();
			node->parent = freelist; //记录原freelist
			node->setDeleted();
			freelist = node;
			count()--;
		}

		//将节点移出树并更新树，但还未真正删除节点
		constexpr void disconnectNodeUpdateTree(node_type* node) noexcept {
			node_type* needRemoveNode = node;
			//开始更新树
			node_type* child;
			node_type* parent;
			if (node->left && node->right) {
				node = node->right;
				while (node->left) { //找到node的下一个节点
					node = node->left;
				}
				child = node->right;
				parent = node->parent;
				//将child替换到node原来的位置
				if (child) {
					child->parent = parent;
				}
				if (parent->left == node) {
					parent->left = child;
				}
				else {
					parent->right = child;
				}
				//通过parent记录需要重新平衡的节点
				if (node->parent == needRemoveNode) {
					parent = node;
				}
				//node替换needRemoveNode
				node->parent = needRemoveNode->parent;
				node->left = needRemoveNode->left;
				node->right = needRemoveNode->right;
				node->height = needRemoveNode->height;
				replaceChildenNode(needRemoveNode, node, needRemoveNode->parent);
				needRemoveNode->left->parent = node;
				if (needRemoveNode->right) {
					needRemoveNode->right->parent = node;
				}
			}
			else { //只有两个以下节点的树
				if (node->left) {
					child = node->left;
				}
				else {
					child = node->right;
				}
				parent = node->parent; //通过parent记录需要重新平衡的节点
				replaceChildenNode(node, child, parent);
				if (child) {
					child->parent = parent;
				}
			}
			needRemoveNode->parent = null;
			//开始平衡
			while (parent) {
				sint hl = parent->getLeftHeight();
				sint hr = parent->getRightHeight();
				sint h = (hl > hr ? hl : hr) + 1;
				sint diff = hl - hr;
				if (parent->height != h) {
					parent->height = h;
				}
				else if (diff > -2 && diff < 2) {
					break;
				}
				if (diff < -1) {
					parent = rightFix(parent);
				}
				else if (diff > 1) {
					parent = leftFix(parent);
				}
				parent = parent->parent;
			}
		}

		//释放node还有对应的value的空间，加入freelist，更新树
		constexpr void removeNodeUpdateTree(node_type* node) noexcept {
			disconnectNodeUpdateTree(node);
			destructNode(node);
		}

		//通过key的值查找node
		constexpr node_type* findNodeByKey(const key_type& key) const noexcept {
			node_type* node = root;
			while (node) {
				const key_type& k = GetNodeKey(node);
				if (func()(key, k)) {
					node = node->left;
				}
				else {
					if (!func()(k, key)) {
						return node;
					}
					node = node->right;
				}
			}
			return node;
		}


		template<typename UserType, typename CompareFunc>
		constexpr node_type* matchNodeByUserType(const UserType& key, const bool isMin, const CompareFunc& f) const noexcept {
			node_type* node = root;
			while (node) {
				const key_type& k = GetNodeKey(node);
				auto cmp = f(key, k);
				if (cmp == std::strong_ordering::less) {
					node = node->left;
				}
				else if (cmp == std::strong_ordering::greater) {
					node = node->right;
				}
				else {
					if(isMin) {
						node_type* nowNode = node->left;
						while (nowNode) {
							const key_type& k2 = GetNodeKey(nowNode);
							cmp = f(key, k2);
							if (cmp == std::strong_ordering::less) {
								nowNode = nowNode->right;
							}
							else {
								if (cmp == std::strong_ordering::equal) {
									node = nowNode;
								}
								nowNode = nowNode->left;
							}
						}
					}
					else {
						node_type* nowNode = node->right;
						while (nowNode) {
							const key_type& k2 = GetNodeKey(nowNode);
							cmp = f(key, k2);
							if (cmp == std::strong_ordering::greater) {
								nowNode = nowNode->left;
							}
							else {
								if (cmp == std::strong_ordering::equal) {
									node = nowNode;
								}
								nowNode = nowNode->right;
							}
						}
					}
					break;
				}
			}
			return node;
		}

		//验证add返回的指针是否在infos范围内且能找到对应node
		constexpr node_type* findNodeByValuePtr(const value_type* ptr) const noexcept {
			if (!ptr) return null;
			using I = DynArr<info_type>::size_type;
			auto p = reinterpret_cast<const uchar*>(ptr);
			I i1 = 0;
			I i2 = infosCount;
			I i;
			while (i1 < i2) { //二分查找
				i = i1 + ((i2 - i1) / 2);
				if (p < infos[i].begin) {
					i2 = i;
				}
				else if (p >= infos[i].end) {
					i1 = i + 1;
				}
				else {
					auto& info = infos[i];
					size_t index;
					if constexpr (SOO) {
						index = (p - data_offset - info.begin);
						if (index % node_size != 0) {
							return null;
						}
						index /= node_size;
					}
					else {
						index = (p - info.valueBegin);
						if (index % value_size != 0) {
							return null;
						}
						index /= value_size;
					}
					node_type* node = reinterpret_cast<node_type*>(info.begin) + index;
					if (node->isDeleted()) { //已被删除
						return null;
					}
					return node;
				}
			}
			return null;
		}

		//如果不是已存在的值则移动到node，更新树，如果存在则替换
		template<typename VT = value_type>
		constexpr value_type* add(VT&& v) noexcept(noexcept(value_type(std::forward<VT>(v))))
		{
			node_type* node = findNodeByKey(GetValueKey(v));
			if (node) { //已存在
				value_type* ptr = node->getDataPtr();
				ptr->~value_type();
				new (ptr) value_type(std::forward<VT>(v));
				return ptr;
			}
			else {
				node = alloc();
				value_type* ptr = node->getDataPtr();
				new (ptr) value_type(std::forward<VT>(v));
				insertNodeUpdateTree(node);
				return ptr;
			}
		}

		//如果不是已存在的值则移动到node，更新树，如果存在则替换val
		template<typename KT = K, typename VT = V> requires(IsPair)
		constexpr value_type* add(KT&& key, VT&& val) noexcept(noexcept(value_type(std::forward<KT>(key), std::forward<VT>(val))))
		{
			node_type* node = findNodeByKey(key);
			if (node) { //已存在
				value_type* ptr = node->getDataPtr();
				ptr->val.~V();
				new (std::addressof(ptr->val)) V(std::forward<VT>(val));
				return ptr;
			}
			else {
				node = alloc();
				value_type* ptr = node->getDataPtr();
				new (ptr) value_type(std::forward<KT>(key), std::forward<VT>(val));
				insertNodeUpdateTree(node);
				return ptr;
			}
		}

		template<typename M>
		constexpr void addMem(M&& other) noexcept(std::is_const_v<M> ? noexcept_copy : noexcept_move) {
			GrowableStack<node_type*, 128> stack;
			if (other.root) {
				stack.push(other.root);
				while (stack.count()) {
					node_type* node = stack.pop();
					if constexpr (std::is_const_v<M>) {
						const value_type* ptr = node->getDataPtr();
						add(*ptr);
					}
					else {
						value_type* ptr = node->getDataPtr();
						add(std::move(*ptr));
					}
					if (node->left) {
						stack.push(node->left);
					}
					if (node->right) {
						stack.push(node->right);
					}
				}
				if constexpr (!std::is_const_v<M>) {
					other.destoryAllInfo();
				}
			}
		}



		//如果不是已存在的值则移动到node，更新树
		template<typename VT = value_type>
		constexpr value_type* addIfAbsent(VT&& v) noexcept(noexcept(value_type(std::forward<VT>(v))))
		{
			node_type* node = findNodeByKey(GetValueKey(v));
			if (node) return node->getDataPtr(); //已存在
			node = alloc();
			value_type* ptr = node->getDataPtr();
			new (ptr) value_type(std::forward<VT>(v));
			insertNodeUpdateTree(node);
			return ptr;
		}

		//如果不是已存在的值则移动到node，更新树
		template<typename KT = K, typename VT = V> requires(IsPair)
		constexpr value_type* addIfAbsent(KT&& key, VT&& val) noexcept(noexcept(value_type(std::forward<KT>(key), std::forward<VT>(val))))
		{
			node_type* node = findNodeByKey(key);
			if (node) return node->getDataPtr(); //已存在
			node = alloc();
			value_type* ptr = node->getDataPtr();
			new (ptr) value_type(std::forward<KT>(key), std::forward<VT>(val));
			insertNodeUpdateTree(node);
			return ptr;
		}

		template<typename M>
		constexpr void addMemIfAbsent(M&& other) noexcept(std::is_const_v<M> ? noexcept_copy : noexcept_move) {
			GrowableStack<node_type*, 128> stack;
			if (other.root) {
				stack.push(other.root);
				while (stack.count()) {
					node_type* node = stack.pop();
					if constexpr (std::is_const_v<M>) {
						const value_type* ptr = node->getDataPtr();
						addIfAbsent(*ptr);
					}
					else {
						value_type* ptr = node->getDataPtr();
						addIfAbsent(std::move(*ptr));
					}
					if (node->left) {
						stack.push(node->left);
					}
					if (node->right) {
						stack.push(node->right);
					}
				}
				if constexpr (!std::is_const_v<M>) {
					other.destoryAllInfo();
				}
			}
		}

		//删除
		constexpr bool removeByKey(const key_type& key) noexcept {
			node_type* node = findNodeByKey(key);
			if (!node) return false;
			removeNodeUpdateTree(node);
			return true;
		}
		//删除
		constexpr bool removeByValuePtr(value_type* ptr) noexcept {
			node_type* node = findNodeByValuePtr(ptr);
			if (!node) return false;
			removeNodeUpdateTree(node);
			return true;
		}

		//通过key的值替换val，如果没找到key则返回null，只有IsPair时可以使用该函数
		template<class VT = V> requires(IsPair)
		constexpr value_type* replace(const key_type& key, VT&& val) noexcept(noexcept(V(std::forward<VT>(val)))) {
			node_type* node = findNodeByKey(key);
			if (!node) return null;
			value_type* ptr = node->getDataPtr();
			ptr->val.~V();
			new (std::addressof(ptr->val)) V(std::forward<VT>(val));
			return ptr;
		}

		//查找key的值替换为新key并将节点重新加入树，val不变，如果没找到key则返回null，只有IsPair时可以使用该函数
		template<class KT1 = K, class KT2 = K> requires(IsPair)
		constexpr value_type* replaceKey(const KT1& oldKey, KT2&& newKey) noexcept(noexcept(K(std::forward<KT2>(newKey)))) {
			node_type* node = findNodeByKey(oldKey);
			if (!node) return null;
			disconnectNodeUpdateTree(node);
			value_type* ptr = node->getDataPtr();
			ptr->key.~K();
			new (std::addressof(ptr->key)) K(std::forward<KT2>(newKey));
			insertNodeUpdateTree(node);
			return ptr;
		}

		//通过key的值查找
		constexpr value_type* find(const key_type& key) const noexcept {
			node_type* node = findNodeByKey(key);
			if(!node) return null;
			return node->getDataPtr();
		}
		//通过add返回的指针查找
		constexpr value_type* find(const value_type* ptr) const noexcept {
			node_type* node = findNodeByValuePtr(ptr);
			if (!node) return null;
			return node->getDataPtr();
		}

		//通过传入的函数修改key对应的值，如果没找到key则返回null
		template<typename Func>
		constexpr value_type* modify(const key_type& key, const Func& f) noexcept {
			node_type* node = findNodeByKey(key);
			if (!node) return null;
			disconnectNodeUpdateTree(node);
			auto ptr = node->getDataPtr();
			f(*ptr);
			insertNodeUpdateTree(node);
			return ptr;
		}

		//查找2个key之间的值，返回值从小到大排序
		constexpr DynArr<value_type*> findInKeyRange(const key_type& keyMin, const key_type& keyMax) const noexcept {
			DynArr<value_type*> r;
			node_type* highestNode = root;
			while (highestNode) {
				const key_type& k = GetNodeKey(highestNode);
				if (func()(k, keyMin)) {
					highestNode = highestNode->right;
				}
				else if (func()(keyMax, k)) {
					highestNode = highestNode->left;
				}
				else {
					goto found_node; //找到范围内最高的节点
				}
			}
			return r;
		found_node:
			node_type* startNode = highestNode;
			node_type* nowNode = startNode->left;
			while (nowNode) {
				const key_type& k = GetNodeKey(nowNode);
				if (func()(k, keyMin)) {
					nowNode = nowNode->right;
				}
				else {
					startNode = nowNode;
					nowNode = nowNode->left;
				}
			}
			node_type* endNode = highestNode;
			nowNode = endNode->right;
			while (nowNode) {
				const key_type& k = GetNodeKey(nowNode);
				if (func()(keyMax, k)) {
					nowNode = nowNode->left;
				}
				else {
					endNode = nowNode;
					nowNode = nowNode->right;
				}
			}
			nowNode = startNode;
			r.add(nowNode->getDataPtr());
			while (nowNode != endNode) {
				if (nowNode->right) {
					nowNode = nowNode->right;
					while (nowNode->left) {
						nowNode = nowNode->left;
					}
				}
				else {
					while (true) {
						startNode = nowNode;
						nowNode = nowNode->parent;
						if (nowNode->left == startNode) break;
					}
				}
				r.add(nowNode->getDataPtr());
			}
			return r;
		}
		//查找2个key之间的值，返回值其中最小值 TODO 目前不是左闭右闭
		constexpr value_type* findInKeyRangeMin(const key_type& keyMin, const key_type& keyMax) const noexcept {
			node_type* node = root;
			while (node) {
				const key_type& k = GetNodeKey(node);
				if (func()(k, keyMin)) {
					node = node->right;
				}
				else if (func()(keyMax, k)) {
					node = node->left;
				}
				else {
					goto found_node; //找到范围内最高的节点
				}
			}
			return null;
		found_node:
			node_type* nowNode = node->left;
			while (nowNode) {
				const key_type& k = GetNodeKey(nowNode);
				if (func()(k, keyMin)) {
					nowNode = nowNode->right;
				}
				else{
					node = nowNode;
					nowNode = nowNode->left;
				}
			}
			return node->getDataPtr();
		}
		//查找2个key之间的值，返回值其中最大值
		constexpr value_type* findInKeyRangeMax(const key_type& keyMin, const key_type& keyMax) const noexcept {
			node_type* node = root;
			while (node) {
				const key_type& k = GetNodeKey(node);
				if (func()(k, keyMin)) {
					node = node->right;
				}
				else if (func()(keyMax, k)) {
					node = node->left;
				}
				else {
					goto found_node; //找到范围内最高的节点
				}
			}
			return null;
		found_node:
			node_type* nowNode = node->right;
			while (nowNode) {
				const key_type& k = GetNodeKey(nowNode);
				if (func()(keyMax, k)) {
					nowNode = nowNode->left;
				}
				else {
					node = nowNode;
					nowNode = nowNode->right;
				}
			}
			return node->getDataPtr();
		}

		//是否存在
		constexpr bool contain(const key_type& key) const noexcept {
			return findNodeByKey(key);
		}
		//是否存在
		constexpr bool contain(const value_type* ptr) const noexcept {
			return findNodeByValuePtr(ptr);
		}

		template<typename UserType, typename CompareFunc>
		constexpr value_type* matchByUserType(const UserType& key, const bool isMin, const CompareFunc& f) const noexcept {
			node_type* node = matchNodeByUserType(key, isMin, f);
			if (!node) return null;
			return node->getDataPtr();
		}

		//遍历元素
		constexpr void each(const std::function<void(value_type&)>& f) const {
			node_type* n = firstNode();
			while (n) {
				f(*n->getDataPtr());
				n = nodeNext(n);
			}
		}

		//遍历元素，传入函数的第二个参数如果修改为true，则跳过剩下的元素直接结束
		constexpr void each(const std::function<void(value_type&, bool&)>& f) const {
			bool isBreak = false;
			node_type* n = firstNode();
			while (n) {
				f(*n->getDataPtr(), isBreak);
				if (isBreak) return;
				n = nodeNext(n);
			}
		}

		constexpr void destoryNode(node_type* node) noexcept {
			if (node) {
				node->destoryData();
				destoryNode(node->left);
				destoryNode(node->right);
			}
		}

		//实现info对应内存的析构逻辑，并将infosCount设为0（但infos本身的内存不回收）
		constexpr void destoryAllInfo() noexcept {
			if (infosCount) {
				if constexpr (!std::is_trivially_destructible_v<value_type>) {
					destoryNode(root);
				}
				for (uchar i = 0; i < infosCount; i++) {
					Free(infos[i].begin);
				}
				infosCount = 0;
				nowInfo = 0;
				root = null;
				freelist = null;
				count() = 0;
			}
		}

		//注意这里只对K进行比较
		constexpr bool operator ==(const _tree_pool_mem& a) const noexcept {
			if (this == &a) {
				return true;
			}
			if (count() != a.count()) {
				return false;
			}
			node_type* n1 = firstNode();
			node_type* n2 = a.firstNode();
			while (n1 && n2) {
				const key_type& k1 = GetNodeKey(n1);
				const key_type& k2 = GetNodeKey(n2);
				if (func()(k1, k2) || func()(k2, k1)) {
					return false;
				}
				n1 = nodeNext(n1);
				n2 = a.nodeNext(n2);
			}
			return true;
		}
		friend std::ostream& operator<<(std::ostream& os, const _tree_pool_mem& a) {
			os << "count: " << a.count() << ", size: " << a.getInfoTotalSize() << ", data: [";
			node_type* n = a.firstNode();
			size_t i = 0;
			while (n) {
				os << "\n\t[" << i << "]:\t";
				StreamSmartPrint(os, *n->getDataPtr());
				os << ",";
				n = a.nodeNext(n);
				++i;
			}
			os << "\n]";
			return os;
		}

		friend constexpr Hash Hasher(const _tree_pool_mem& a) noexcept {
			node_type* node = a.firstNode();
			Hash hash = Hasher(a.count());
			while (node) {
				hash.combine(GetNodeKey(node));
				node = a.nodeNext(node);
			}
			return hash;
		}
	};
}