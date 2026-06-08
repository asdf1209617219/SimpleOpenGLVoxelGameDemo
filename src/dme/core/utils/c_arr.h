#pragma once
#include <dme/core/utils.h>
namespace dme {
	namespace core {
		//固定大小无法扩容的数组，数组索引以类型I传递
		template<typename T, IsUnsigned I>
		class _c_arr {
		private:
			T* _arr; //数组起点
			I _size; //容量与数量相等
		public:
			constexpr _c_arr() noexcept : _arr(null), _size(0) {}
			explicit constexpr _c_arr(I size) noexcept : _arr(null), _size(size) {
				if (_size) {
					_arr = new T[_size]{};
				}
			}
			constexpr _c_arr(const _c_arr& a) noexcept : _arr(null), _size(a._size) {
				if (_size) {
					_arr = new T[_size];
					std::copy_n(a._arr, _size, _arr);
				}
			}
			constexpr _c_arr(_c_arr&& a) noexcept : _arr(a._arr), _size(a._size) {
				a._arr = null;
				a._size = 0;
			}
			constexpr _c_arr& operator =(const _c_arr& a) noexcept {
				if (this != &a) {
					delete[] _arr;
					_size = a._size;
					if (_size) {
						_arr = new T[_size];
						std::copy_n(a._arr, _size, _arr);
					}
				}
				return *this;
			}
			constexpr _c_arr& operator =(_c_arr&& a) noexcept {
				if (this != &a) {
					_arr = a._arr;
					_size = a._size;

					a._arr = null;
					a._size = 0;
				}
				return *this;
			}
			~_c_arr() {
				delArr();
			}
			//迭代器，每次遍历对象指针都移动到下一个对象的位置，直到ptr为null
			class iterator {
				friend _c_arr;
			private:
				I size;
				I index;
			public:
				T* ptr;
			private:
				explicit constexpr iterator(const _c_arr* arr) noexcept : size(arr->_size), index(0), ptr(arr->_arr) {}
			public:
				//ptr为null则遍历完成
				constexpr void next() noexcept {
					index++;
					if (index < size) {
						ptr++;
					}
					else {
						ptr = null;
						index--;
					}
				}
			};

			//获取迭代器
			constexpr iterator iterate() const noexcept {
				return iterator(this);
			}
			//将数组清空，容量设置为0
			constexpr void delArr() noexcept {
				if (_arr) {
					delete[] _arr;
					_arr = null;
					_size = 0;
				}
			}
			//删除之前数组的同时创建新数组
			constexpr void newArr(I size) noexcept {
				if (_arr) {
					delete[] _arr;
				}
				_arr = new T[size]{};
				_size = size;
			}
			//设置数组，注意数据合法性
			constexpr void set(T* arr, I size) noexcept {
				_arr = arr;
				_size = size;
			}
			//数组指针
			constexpr T* data() const noexcept {
				return _arr;
			}
			//数组中元素数量
			constexpr I count() const noexcept {
				return _size;
			}
			//数组元素容量
			constexpr I size() const noexcept {
				return _size;
			}
			//数组元素数量是否为0
			constexpr bool empty() const noexcept {
				return _size == 0;
			}
			//数组容量是否为0
			constexpr bool isNull() const noexcept {
				return _size == 0;
			}
			//引用数组第一个元素
			constexpr T& first() const noexcept {
				DME_ASSERT(_size != 0, "called on empty container");
				return _arr[0];
			}
			//引用数组最后一个元素
			constexpr T& last() const noexcept {
				DME_ASSERT(_size != 0, "called on empty container");
				return _arr[lastIndex()];
			}
			//数组最后的索引
			constexpr I lastIndex() const noexcept {
				return _size - 1;
			}
			//引用某个元素
			constexpr T& operator [](I index) noexcept {
				DME_ASSERT(index < _size, "subscript exceeds the count");
				return _arr[index];
			}
			//引用某个元素
			constexpr const T& operator [](I index) const noexcept {
				DME_ASSERT(index < _size, "subscript exceeds the count");
				return _arr[index];
			}

			constexpr bool operator ==(const _c_arr& a) const noexcept {
				return _arr == a._arr && _size == a._size;
			}
			friend constexpr Hash Hasher(const _c_arr& a) noexcept {
				return Hash::Combine(a._arr, a._size);
			}
		};

	}

	//固定大小无法扩容的数组
	//数组索引以uint传递
	template<typename T>
	using CArr = core::_c_arr<T, uint>;

	//固定大小无法扩容的数组
	//数组索引以ulong传递
	template<typename T>
	using CArrL = core::_c_arr<T, ulong>;

}

namespace std {
	template<typename T, typename I>
	struct hash<dme::core::_c_arr<T, I>> {
		size_t operator ()(const dme::core::_c_arr<T, I>& a) const noexcept {
			return Hasher(a);
		}
	};
}