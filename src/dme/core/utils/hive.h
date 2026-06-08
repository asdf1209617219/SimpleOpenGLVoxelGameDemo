#pragma once
#include <dme/core/utils/mem/obj_pool_mem.h>
namespace dme {
	//支持遍历的对象池，遍历时增删对象会导致迭代器失效
	template<typename T>
	class Hive {
	public:
		using mem_type = core::_obj_pool_mem<T>;
		using size_type = mem_type::size_type;
		using difference_type = mem_type::difference_type;
		using value_type = mem_type::value_type;
		using comp_ptr = mem_type::comp_ptr;

		using iterator = mem_type::iterator;
		using const_iterator = mem_type::const_iterator;
	private:
		mem_type mem;

	public:
		constexpr Hive() noexcept : mem() {}
		constexpr explicit Hive(size_t size) noexcept : mem(size) {}
		constexpr Hive(const Hive& a) noexcept(mem_type::noexcept_copy) : mem(a.mem) {}
		constexpr Hive(Hive&& a) noexcept : mem(std::move(a.mem)) {}
		constexpr Hive(std::initializer_list<value_type> list) noexcept(mem_type::noexcept_copy) : mem(list) {}
		constexpr Hive& operator =(const Hive& a) noexcept(mem_type::noexcept_copy) {
			mem = a.mem;
			return *this;
		}
		constexpr Hive& operator =(Hive&& a) noexcept {
			mem = std::move(a.mem);
			return *this;
		}
		constexpr Hive& operator =(std::initializer_list<value_type> list) noexcept(mem_type::noexcept_copy) {
			mem = list;
			return *this;
		}

		iterator begin() noexcept {
			return mem.begin();
		}
		const_iterator begin() const noexcept {
			return mem.begin();
		}
		const_iterator cbegin() const noexcept {
			return mem.cbegin();
		}
		iterator end() noexcept {
			return mem.end();
		}
		const_iterator end() const noexcept {
			return mem.end();
		}
		const_iterator cend() const noexcept {
			return mem.cend();
		}

		//添加对象
		template<typename O = T> requires(IsSameOrConvertible<O, T>)
		constexpr comp_ptr add(O&& obj) noexcept(mem_type::noexcept_move) {
			return mem.add(std::forward<O>(obj));
		}
		constexpr void remove(comp_ptr ptr) noexcept {
			mem.remove(ptr);
		}

		//遍历元素
		constexpr void each(const std::function<void(value_type&)>& func) const {
			mem.each(func);
		}
		//遍历元素，传入函数的第二个参数如果修改为true，则跳过剩下的元素直接结束
		constexpr void each(const std::function<void(value_type&, bool&)>& func) const {
			mem.each(func);
		}
		
		friend constexpr Hash Hasher(const Hive& a) noexcept {
			return Hash::Combine(mem);
		}
	};
}

namespace std {
	template<typename T>
	struct hash<dme::Hive<T>> {
		size_t operator ()(const dme::Hive<T>& a) const noexcept {
			return Hasher(a);
		}
	};
}

