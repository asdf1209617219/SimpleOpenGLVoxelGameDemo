#pragma once
#include <dme/core/utils.h>
namespace dme {

	template <typename T, uint N>
	//可增长的栈，如果栈空间不足，则切换到堆上
	class GrowableStack {
	private:
		uint _count;
		uint _capacity;
		T* _stack;
		T _array[N];
	public:
		GrowableStack() : _count(0), _capacity(N), _stack(_array), _array{} {}

		~GrowableStack() {
			if (_stack && _stack != _array) {
				Free(_stack);
				_stack = null;
			}
		}

		void push(const T& obj) {
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

		T pop() {
			DME_ASSERT(_count != 0, "called on empty container");
			_count--;
			return _stack[_count];
		}

		uint count() {
			return _count;
		}
	};

}