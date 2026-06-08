#pragma once
#include <dme/core/utils/mem/hash_mem.h>
namespace dme {
	//hash_mem实现的指针稳定的Set，类型需支持哈希和等于比较
	//直接修改key的行为未定义
	template<typename T> requires (IsCanHash<T> && IsCanEqual<T>)
	class HashSet {
	public:
		using mem_type = core::_flat_hash_mem<false, T, T, true, true>;
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
		constexpr HashSet() noexcept : mem() {}
		constexpr explicit HashSet(size_t size) noexcept : mem(size) {}
		constexpr HashSet(const HashSet& a) noexcept(mem_type::noexcept_copy) : mem(a.mem) {}
		constexpr HashSet(HashSet&& a) noexcept : mem(std::move(a.mem)) {}
		constexpr HashSet(std::initializer_list<value_type> list) noexcept(mem_type::noexcept_copy) : mem(list) {}
		constexpr HashSet& operator =(const HashSet& a) noexcept(mem_type::noexcept_copy) {
			mem = a.mem;
			return *this;
		}
		constexpr HashSet& operator =(HashSet&& a) noexcept {
			mem = std::move(a.mem);
			return *this;
		}
		constexpr HashSet& operator =(std::initializer_list<value_type> list) noexcept(mem_type::noexcept_copy) {
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

		//传入自定义key和hash，使用自定义比较函数来查找key是否存在，函数传入UserKeyType、hash、slot中的key，返回比较结果
		template<typename UserKeyType, typename EqualFunc> requires IsInvocableRet<EqualFunc, bool, const UserKeyType&, const key_type&>
		constexpr pointer getByUserKeyTypeAndHash(const UserKeyType& key, Hash hash, EqualFunc&& func) const noexcept {
			return mem.findByUserKeyTypeAndHash(key, hash, std::forward<EqualFunc>(func));
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

		constexpr bool operator ==(const HashSet& a) const noexcept {
			return mem == a.mem;
		}
		friend std::ostream& operator<<(std::ostream& os, const HashSet& a) {
			return os << a.mem;
		}
		friend constexpr Hash Hasher(const HashSet& a) noexcept {
			return Hasher(a.mem);
		}
	};
}