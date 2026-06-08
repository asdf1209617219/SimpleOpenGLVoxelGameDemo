#pragma once
#include <dme/core/utils/mem/hash_mem.h>
namespace dme {
	//hash_mem实现的指针稳定的Map，类型需支持哈希和等于比较
	//直接修改key的行为未定义
	template<typename K, typename V> requires (IsCanHash<K> && IsCanEqual<K>)
	class HashMap {
	public:
		using mem_type = core::_flat_hash_mem<true, K, V, true, true>;
		using key_type = mem_type::key_type;
		using val_type = mem_type::val_type;
		using slot_type = mem_type::slot_type;
		using pool_type = mem_type::pool_type;

		using value_type = mem_type::value_type;
		using pointer = value_type*;
		using reference = value_type&;

		using size_type = mem_type::size_type;
		using difference_type = mem_type::difference_type;

		using iterator = core::_obj_pool_iterator<pool_type>;
		using const_iterator = core::_obj_pool_const_iterator<pool_type>;
	private:
		mem_type mem;
	public:
		constexpr HashMap() noexcept : mem() {}
		constexpr explicit HashMap(size_t size) noexcept : mem(size) {}
		constexpr HashMap(const HashMap& a) noexcept(mem_type::noexcept_copy) : mem(a.mem) {}
		constexpr HashMap(HashMap&& a) noexcept : mem(std::move(a.mem)) {}
		constexpr HashMap(std::initializer_list<value_type> list) noexcept(mem_type::noexcept_copy) : mem(list) {}
		constexpr HashMap& operator =(const HashMap& a) noexcept(mem_type::noexcept_copy) {
			mem = a.mem;
			return *this;
		}
		constexpr HashMap& operator =(HashMap&& a) noexcept {
			mem = std::move(a.mem);
			return *this;
		}
		constexpr HashMap& operator =(std::initializer_list<value_type> list) noexcept(mem_type::noexcept_copy) {
			mem = list;
			return *this;
		}

		//返回起点的迭代器
		iterator begin() noexcept {
			return mem.pool.begin();
		}
		//返回起点的只读迭代器
		const_iterator begin() const noexcept {
			return mem.pool.begin();
		}
		//返回起点的只读迭代器
		const_iterator cbegin() const noexcept {
			return mem.pool.cbegin();
		}
		//返回终点的迭代器
		iterator end() noexcept {
			return mem.pool.end();
		}
		//返回终点的只读迭代器
		const_iterator end() const noexcept {
			return mem.pool.end();
		}
		//返回终点的只读迭代器
		const_iterator cend() const noexcept {
			return mem.pool.cend();
		}

		//元素数量
		constexpr size_t count() const noexcept {
			return mem.count;
		}
		//元素容量
		constexpr size_t size() const noexcept {
			return mem.size;
		}
		//元素数量是否为0
		constexpr bool empty() const noexcept {
			return mem.count == 0;
		}
		//元素容量是否为0
		constexpr bool isNull() const noexcept {
			return mem.size == 0;
		}

		//初始化默认容量
		constexpr void init() noexcept {
			if (mem.size) return;
			mem.init(mem_type::default_size);
		}

		//不存在则添加元素，已存在则替换val，返回新加元素或已有元素的指针
		template <typename KT = K, typename VT = V> requires(IsSameOrConvertible<KT, K> && IsSameOrConvertible<VT, V>)
		constexpr pointer add(KT&& key_, VT&& val_) noexcept(typename mem_type::template get_noexcept_slot<KT, VT>()) {
			return mem.add(std::forward<KT>(key_), std::forward<VT>(val_));
		}
		//不存在则添加元素，已存在则替换val，返回新加元素或已有元素的指针
		template <typename KT = K, typename VT = V> requires(IsSameOrConvertible<KT, K> && IsSameOrConvertible<VT, V>)
		constexpr pointer add(Pair<KT, VT>&& pair) noexcept(typename mem_type::template get_noexcept_slot<KT, VT>()) {
			return mem.add(std::forward<KT>(pair.key), std::forward<VT>(pair.val));
		}
		//如果不存在则添加元素，返回新加元素或已有元素的指针
		template <typename KT = K, typename VT = V> requires(IsSameOrConvertible<KT, K> && IsSameOrConvertible<VT, V>)
		constexpr pointer addIfAbsent(KT&& key_, VT&& val_) noexcept(typename mem_type::template get_noexcept_slot<KT, VT>()) {
			return mem.addIfAbsent(std::forward<KT>(key_), std::forward<VT>(val_));
		}
		//如果不存在则添加元素，返回新加元素或已有元素的指针
		template <typename KT = K, typename VT = V> requires(IsSameOrConvertible<KT, K> && IsSameOrConvertible<VT, V>)
		constexpr pointer addIfAbsent(Pair<KT, VT>&& pair) noexcept(typename mem_type::template get_noexcept_slot<KT, VT>()) {
			return mem.addIfAbsent(std::forward<KT>(pair.key), std::forward<VT>(pair.val));
		}

		//替换val，不存在则返回null
		template <typename KT = K, typename VT = V> requires(IsSameOrConvertible<KT, K> && IsSameOrConvertible<VT, V>)
		constexpr pointer replace(KT&& key_, VT&& val_) noexcept(typename mem_type::template get_noexcept_slot_val<VT>()) {
			return mem.replace(std::forward<KT>(key_), std::forward<VT>(val_));
		}
		//替换val，不存在则返回null
		template <typename KT = K, typename VT = V> requires(IsSameOrConvertible<KT, K> && IsSameOrConvertible<VT, V>)
		constexpr pointer replace(Pair<KT, VT>&& pair) noexcept(typename mem_type::template get_noexcept_slot_val<VT>()) {
			return mem.replace(std::forward<KT>(pair.key), std::forward<VT>(pair.val));
		}

		//移除元素
		constexpr bool remove(const key_type& key) noexcept {
			return mem.remove(key);
		}
		//查找
		constexpr pointer get(const key_type& key) noexcept {
			return mem.find(key);
		}
		//查找
		constexpr const pointer get(const key_type& key) const noexcept {
			return mem.find(key);
		}
		//是否存在
		constexpr bool contain(const key_type& key) const noexcept {
			return mem.find(key);
		}
		//清除所有元素
		constexpr void clear() noexcept {
			mem.clear();
		}
		//清除所有元素并回收内存
		constexpr void release() noexcept {
			mem.release();
		}

		//遍历元素
		constexpr void each(const std::function<void(reference)>& func) const {
			mem.each(func);
		}
		//遍历元素，传入函数的第二个参数如果修改为true，则跳过剩下的元素直接结束
		constexpr void each(const std::function<void(reference, bool&)>& func) const {
			mem.each(func);
		}

		constexpr bool operator ==(const HashMap& a) const noexcept {
			return mem == a.mem;
		}
		friend std::ostream& operator<<(std::ostream& os, const HashMap& a) {
			return os << a.mem;
		}
		friend constexpr Hash Hasher(const HashMap& a) noexcept {
			return Hasher(a.mem);
		}
	};
}