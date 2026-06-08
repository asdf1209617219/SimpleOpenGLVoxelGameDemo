#pragma once
#include <dme/core/utils/mem/hash_mem.h>
namespace dme {
	//flat_hash实现的不具有指针稳定性的Set，类型需支持哈希和等于比较
	//直接修改key的行为未定义
	template<typename T> requires (IsCanHash<T> && IsCanEqual<T>)
	class FlatSet {
	public:
		using mem_type = core::_flat_hash_mem<false, T, T, false, false>;
		using key_type = mem_type::key_type;
		using val_type = mem_type::val_type;
		using slot_type = mem_type::slot_type;

		using value_type = mem_type::value_type;
		using pointer = slot_type*;
		using reference = slot_type&;

		using size_type = mem_type::size_type;
		using difference_type = mem_type::difference_type;

		using iterator = core::_flat_hash_mem_iterator<FlatSet>;
		using const_iterator = core::_flat_hash_mem_const_iterator<FlatSet>;
	private:
		mem_type mem;
	public:
		constexpr FlatSet() noexcept : mem() {}
		constexpr explicit FlatSet(size_t size) noexcept : mem(size) {}
		constexpr FlatSet(const FlatSet& a) noexcept(mem_type::noexcept_copy) : mem(a.mem) {}
		constexpr FlatSet(FlatSet&& a) noexcept : mem(std::move(a.mem)) {}
		constexpr FlatSet(std::initializer_list<value_type> list) noexcept(mem_type::noexcept_copy) : mem(list) {}
		constexpr FlatSet& operator =(const FlatSet& a) noexcept(mem_type::noexcept_copy) {
			mem = a.mem;
			return *this;
		}
		constexpr FlatSet& operator =(FlatSet&& a) noexcept {
			mem = std::move(a.mem);
			return *this;
		}
		constexpr FlatSet& operator =(std::initializer_list<value_type> list) noexcept(mem_type::noexcept_copy) {
			mem = list;
			return *this;
		}

		//返回起点的迭代器
		iterator begin() noexcept {
			return iterator(&mem);
		}
		//返回起点的只读迭代器
		const_iterator begin() const noexcept {
			return const_iterator(&mem);
		}
		//返回起点的只读迭代器
		const_iterator cbegin() const noexcept {
			return const_iterator(&mem);
		}
		//返回终点的迭代器
		iterator end() noexcept {
			return iterator(&mem, mem.size);
		}
		//返回终点的只读迭代器
		const_iterator end() const noexcept {
			return const_iterator(&mem, mem.size);
		}
		//返回终点的只读迭代器
		const_iterator cend() const noexcept {
			return const_iterator(&mem, mem.size);
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

		//不存在则添加元素，已存在则替换，返回新加元素或已有元素的指针
		template <typename KT = T> requires(IsSameOrConvertible<KT, T>)
		constexpr pointer add(KT&& key) noexcept(typename mem_type::template get_noexcept_slot<KT>()) {
			return mem.add(std::forward<KT>(key));
		}
		//如果不存在则添加元素，返回新加元素或已有元素的指针
		template <typename KT = T> requires(IsSameOrConvertible<KT, T>)
		constexpr pointer addIfAbsent(KT&& key) noexcept(typename mem_type::template get_noexcept_slot<KT>()) {
			return mem.addIfAbsent(std::forward<KT>(key));
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

		constexpr bool operator ==(const FlatSet& a) const noexcept {
			return mem == a.mem;
		}
		friend std::ostream& operator<<(std::ostream& os, const FlatSet& a) {
			return os << a.mem;
		}
		friend constexpr Hash Hasher(const FlatSet& a) noexcept {
			return Hasher(a.mem);
		}
	};
}