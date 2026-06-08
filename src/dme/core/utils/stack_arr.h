#pragma once
#include <dme/core/utils.h>
namespace dme {
	template <typename T, uint N>
	//可增长的栈上数组，如果栈上空间不足，则切换到堆上
	class StackArr {
	private:
		uint _count;
		uint _capacity;
		T* _stack;
		T _array[N];
	public:
		constexpr StackArr() noexcept : _count(0), _capacity(N), _stack(_array), _array{} {}

		~StackArr() {
			if (_stack && _stack != _array) {
				Free(_stack);
				_stack = null;
			}
		}

		constexpr void add(const T& obj) noexcept(nothrow_copy_construct<T>) {
			if (_count == _capacity) {
				T* old = _stack;
				_capacity = _capacity << 1;
				_stack = Malloc<T>(_capacity);
				Memcpy<T>(_stack, old, _count);
				if (old != _array) {
					Free(old);
				}
			}

			_stack[_count] = obj;
			_count++;
		}

		constexpr void remove() noexcept {
			DME_ASSERT(_count != 0, "called on empty container");
			_count--;
			return _stack[_count];
		}

		constexpr uint count() noexcept {
			return _count;
		}

		constexpr T& operator [](uint index) const noexcept {
			DME_ASSERT(index < _capacity, "subscript exceeds the capacity");
			return _stack[index];
		}
	};

}