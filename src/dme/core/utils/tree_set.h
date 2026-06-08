#pragma once
#include <dme/core/utils/mem/tree_pool_mem.h>
namespace dme {
	//平衡二叉树实现的指针稳定的Set，类型需支持小于比较
	//直接修改key的行为未定义
	template<typename T, typename LessFunc = std::less<T>> requires IsInvocableRet<LessFunc, bool, const T&, const T&>
	class TreeSet {
	public:
		using mem_type = core::_tree_pool_mem<false, T, T, LessFunc>;
		using node_type = mem_type::node_type;
		using key_type = mem_type::key_type;

		using value_type = mem_type::value_type;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		using reference = value_type&;
		using const_reference = const value_type&;

		using iterator = core::_tree_iterator<TreeSet>;
		using const_iterator = core::_tree_const_iterator<TreeSet>;
	private:
		mem_type mem;
	public:

		template<typename F = LessFunc> requires std::same_as<std::remove_cvref_t<F>, LessFunc>
		constexpr explicit TreeSet(F&& f = F()) noexcept : mem(std::forward<F>(f)) {}

		template<typename F = LessFunc> requires std::same_as<std::remove_cvref_t<F>, LessFunc>
		constexpr explicit TreeSet(size_t size, F&& f = F()) noexcept : mem(size, std::forward<F>(f)) {}

		constexpr TreeSet(const TreeSet& a) noexcept(mem_type::noexcept_copy) : mem(a.mem) {}
		constexpr TreeSet(TreeSet&& a) noexcept : mem(std::move(a.mem)) {}

		template<typename F = LessFunc> requires std::same_as<std::remove_cvref_t<F>, LessFunc>
		constexpr TreeSet(std::initializer_list<value_type> list, F&& f = F()) noexcept(mem_type::noexcept_copy) : mem(list, std::forward<F>(f)) {}

		constexpr TreeSet& operator =(const TreeSet& a) noexcept(mem_type::noexcept_copy) {
			mem = a.mem;
			return *this;
		}
		constexpr TreeSet& operator =(TreeSet&& a) noexcept {
			mem = std::move(a.mem);
			return *this;
		}
		constexpr TreeSet& operator =(std::initializer_list<value_type> list) noexcept(mem_type::noexcept_copy) {
			mem = list;
			return *this;
		}
		//返回起点的迭代器
		iterator begin() noexcept {
			return iterator(&mem, mem.firstNode());
		}
		//返回起点的只读迭代器
		const_iterator begin() const noexcept {
			return const_iterator(&mem, mem.firstNode());
		}
		//返回起点的只读迭代器
		const_iterator cbegin() const noexcept {
			return const_iterator(&mem, mem.firstNode());
		}
		//返回终点的迭代器
		iterator end() noexcept {
			return iterator(&mem, mem_type::end_node);
		}
		//返回终点的只读迭代器
		const_iterator end() const noexcept {
			return const_iterator(&mem, mem_type::end_node);
		}
		//返回终点的只读迭代器
		const_iterator cend() const noexcept {
			return const_iterator(&mem, mem_type::end_node);
		}

		//元素数量
		constexpr size_t count() const noexcept {
			return mem.count();
		}
		//元素容量
		constexpr size_t size() const noexcept {
			return mem.getInfoTotalSize();
		}
		//元素数量是否为0
		constexpr bool empty() const noexcept {
			return mem.count() == 0;
		}
		//元素容量是否为0
		constexpr bool isNull() const noexcept {
			return mem.infos.isNull();
		}

		//引用第一个元素
		constexpr value_type& first() noexcept {
			DME_ASSERT(mem.count() != 0, "called on empty container");
			return *mem.firstNode()->getDataPtr();
		}
		//引用第一个元素
		constexpr const value_type& first() const noexcept {
			DME_ASSERT(mem.count() != 0, "called on empty container");
			return *mem.firstNode()->getDataPtr();
		}
		//引用最后一个元素
		constexpr value_type& last() noexcept {
			DME_ASSERT(mem.count() != 0, "called on empty container");
			return *mem.lastNode()->getDataPtr();
		}
		//引用最后一个元素
		constexpr const value_type& last() const noexcept {
			DME_ASSERT(mem.count() != 0, "called on empty container");
			return *mem.lastNode()->getDataPtr();
		}
		//初始化默认容量
		constexpr void init() noexcept {
			if (!mem.infos.isNull()) return;
			mem.init(mem_type::default_size);
		}

		//不存在则添加元素，已存在则替换，返回新加元素或已有元素的指针
		template <typename KT = T> requires(IsSameOrConvertible<KT, T>)
			constexpr value_type* add(KT&& key) noexcept(noexcept(mem.add(std::forward<KT>(key)))) {
			return mem.add(std::forward<KT>(key));
		}

		//不存在则添加元素，已存在则替换
		constexpr void add(const TreeSet& other) noexcept(noexcept(mem.addMem(other.mem))) {
			mem.addMem(other.mem);
		}
		//不存在则添加元素，已存在则替换
		constexpr void add(TreeSet&& other) noexcept(noexcept(mem.addMem(std::move(other.mem)))) {
			mem.addMem(std::move(other.mem));
		}

		//如果不存在则添加元素，返回新加元素或已有元素的指针
		template <typename KT = T> requires(IsSameOrConvertible<KT, T>)
			constexpr value_type* addIfAbsent(KT&& key) noexcept(noexcept(mem.addIfAbsent(std::forward<KT>(key)))) {
			return mem.addIfAbsent(std::forward<KT>(key));
		}

		//添加所有非重复元素
		constexpr void addIfAbsent(const TreeSet& other) noexcept(noexcept(mem.addMemIfAbsent(other.mem))) {
			mem.addMemIfAbsent(other.mem);
		}
		//添加所有非重复元素
		constexpr void addIfAbsent(TreeSet&& other) noexcept(noexcept(mem.addMemIfAbsent(std::move(other.mem)))) {
			mem.addMemIfAbsent(std::move(other.mem));
		}

		//通过传入的函数修改key对应的值，如果没找到key则返回null
		template<typename Func> requires(IsInvocableRet<Func, void, value_type&>)
			constexpr value_type* modify(const key_type& key, const Func& f) noexcept(noexcept(mem.modify(key, f))) {
			return mem.modify(key, f);
		}

		//移除元素
		constexpr bool remove(const key_type& key) noexcept {
			return mem.removeByKey(key);
		}
		//移除add返回的指针上的元素
		constexpr bool remove(value_type* ptr) noexcept {
			return mem.removeByValuePtr(ptr);
		}

		//查找
		constexpr value_type* get(const key_type& key) const noexcept {
			return mem.find(key);
		}
		//查找add返回的指针上的元素
		constexpr value_type* get(const value_type* ptr) const noexcept {
			return mem.find(ptr);
		}

		//查找范围内的值（左闭右闭），返回值从小到大排序
		constexpr DynArr<value_type*> getRange(const key_type& key0, const key_type& key1) const noexcept {
			if (mem.func()(key1, key0)) {
				return mem.findInKeyRange(key1, key0);
			}
			else {
				return mem.findInKeyRange(key0, key1);
			}
		}
		//查找范围内的值（左闭右闭），返回值其中最小值
		constexpr value_type* getRangeMin(const key_type& key0, const key_type& key1) const noexcept {
			if (mem.func()(key1, key0)) {
				return mem.findInKeyRangeMin(key1, key0);
			}
			else {
				return mem.findInKeyRangeMin(key0, key1);
			}
		}
		//查找范围内的值（左闭右闭），返回值其中最大值
		constexpr value_type* getRangeMax(const key_type& key0, const key_type& key1) const noexcept {
			if (mem.func()(key1, key0)) {
				return mem.findInKeyRangeMax(key1, key0);
			}
			else {
				return mem.findInKeyRangeMax(key0, key1);
			}
		}

		//是否存在
		constexpr bool contain(const key_type& key) const noexcept {
			return mem.contain(key);
		}
		//是否add返回的指针上的元素
		constexpr bool contain(const value_type* ptr) const noexcept {
			return mem.contain(ptr);
		}

		//通过三路比较查找符合的元素，并返回匹配的最小的元素（查找时通过func返回小于或大于加速遍历）
		template<typename UserType, typename CompareFunc> requires IsInvocableRet<CompareFunc, std::strong_ordering, const UserType&, const key_type&>
		constexpr value_type* matchMinUserType(const UserType& key, CompareFunc&& func) const noexcept {
			return mem.matchByUserType(key, true, func);
		}
		//通过三路比较查找func返回等于的元素，并返回匹配的最大的元素（查找时通过func返回小于或大于加速遍历）
		template<typename UserType, typename CompareFunc> requires IsInvocableRet<CompareFunc, std::strong_ordering, const UserType&, const key_type&>
		constexpr value_type* matchMaxUserType(const UserType& key, CompareFunc&& func) const noexcept {
			return mem.matchByUserType(key, false, func);
		}

		//清除所有元素
		constexpr void clear() noexcept {
			mem.destoryAllInfo();
		}
		//清除所有元素并回收内存
		constexpr void release() noexcept {
			mem.release();
		}

		//遍历元素
		constexpr void each(const std::function<void(value_type&)>& func) const {
			mem.each(func);
		}
		//遍历元素，传入函数的第二个参数如果修改为true，则跳过剩下的元素直接结束
		constexpr void each(const std::function<void(value_type&, bool&)>& func) const {
			mem.each(func);
		}

		//获取key对应的处迭代器
		constexpr iterator it(const key_type& key) noexcept {
			return iterator(&mem, mem.findNodeByKey(key));
		}
		//获取add返回的指针对应的处迭代器
		constexpr iterator it(const value_type* ptr) noexcept {
			return iterator(&mem, mem.findNodeByValuePtr(ptr));
		}
		//获取key对应的只读迭代器
		constexpr const_iterator it(const key_type& key) const noexcept {
			return const_iterator(&mem, mem.findNodeByKey(key));
		}
		//获取add返回的指针对应的只读迭代器
		constexpr const_iterator it(const value_type* ptr) const noexcept {
			return const_iterator(&mem, mem.findNodeByValuePtr(ptr));
		}
		//获取key对应的只读迭代器
		constexpr const_iterator cit(const key_type& key) const noexcept {
			return const_iterator(&mem, mem.findNodeByKey(key));
		}
		//获取add返回的指针对应的只读迭代器
		constexpr const_iterator cit(const value_type* ptr) const noexcept {
			return const_iterator(&mem, mem.findNodeByValuePtr(ptr));
		}

		constexpr bool operator ==(const TreeSet& a) const noexcept {
			return mem == a.mem;
		}
		friend std::ostream& operator<<(std::ostream& os, const TreeSet& a) {
			return os << a.mem;
		}
		friend constexpr Hash Hasher(const TreeSet& a) noexcept {
			return Hasher(a.mem);
		}
	};

}

namespace std {
	template<typename T>
	struct hash<dme::TreeSet<T>> {
		size_t operator ()(const dme::TreeSet<T>& a) const noexcept {
			return Hasher(a);
		}
	};
}