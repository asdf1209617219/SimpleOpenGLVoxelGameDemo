#pragma once
#include <dme/core/utils/mem/arr_mem.h>
#include <dme/core/utils/mem/iterator.h>
#include <dme/core/utils/pair.h>
namespace dme {
	namespace core {	
		//动态数组，扩容和增删时元素位置会变动，不具有指针稳定性
		//数组索引以类型I传递
		template<typename T, IsUnsigned I>
		class _dyn_arr {
		public:
			using mem_type = _arr_mem<T, I>;

			using value_type = T;
			using pointer = value_type*;
			using const_pointer = const value_type*;
			using reference = value_type&;
			using const_reference = const value_type&;

			using size_type = I;
			using difference_type = std::make_signed_t<I>;

			using iterator = _array_iterator<_dyn_arr>;
			using const_iterator = _array_const_iterator<_dyn_arr>;
		private:
			mem_type mem;

			//在数组最后分配1个元素的空间，分配后用户需要手动调用定位new进行构造
			constexpr T* alloc() noexcept(mem_type::noexcept_move) {
				mem.extend(mem.count + 1);
				T* ptr = mem.data + mem.count;
				mem.count++;
				return ptr;
			}
			//在数组最后分配allocCount个元素的空间，分配后用户需要手动调用定位new进行构造
			//返回申请的空间的起点
			constexpr T* alloc(I allocCount) noexcept(mem_type::noexcept_move) {
				DME_ASSERT(allocCount != 0, "allocate count is zero");
				mem.extend(mem.count + allocCount);
				T* ptr = mem.data + mem.count;
				mem.count += allocCount;
				return ptr;
			}
			//在传入索引处分配1个元素的空间，已有的元素会向后移动，分配后用户需要手动调用定位new进行构造
			//注意，如果index超出count，则[count, index)的这些元素均未初始化
			constexpr T* allocAt(I index) noexcept(mem_type::noexcept_move) {
				mem.extend(index + 1);
				if (index < mem.count) {
					mem.moveToRight(index, mem.count - index, 1);
					mem.count++;
				}
				else {
					mem.count = index + 1;
				}
				return mem.data + index;
			}
			//在indexStart处分配allocCount个元素的空间，已有的元素会向后移动，分配后用户需要手动调用定位new进行构造
			//注意，如果indexStart超出count，则[count, indexStart)的这些元素均未初始化
			//返回申请的空间的起点
			constexpr T* allocAt(I indexStart, I allocCount) noexcept(mem_type::noexcept_move) {
				DME_ASSERT(allocCount != 0, "allocate count is zero");
				I indexEnd = indexStart + allocCount;
				mem.extend(indexEnd);
				if (indexStart < mem.count) {
					mem.moveToRight(indexStart, mem.count - indexStart, allocCount);
					mem.count += allocCount;
				}
				else {
					mem.count = indexEnd;
				}
				return mem.data + indexStart;
			}
		public:
			constexpr _dyn_arr() noexcept : mem() {}
			constexpr explicit _dyn_arr(I size) noexcept : mem(size) {}
			constexpr _dyn_arr(const _dyn_arr& a) noexcept(mem_type::noexcept_copy) : mem(a.mem) {}
			constexpr _dyn_arr(_dyn_arr&& a) noexcept : mem(std::move(a.mem)) {}
			constexpr _dyn_arr(std::initializer_list<T> list) noexcept(mem_type::noexcept_copy) : mem(list) {}
			constexpr _dyn_arr& operator =(const _dyn_arr& a) noexcept(mem_type::noexcept_copy) {
				mem = a.mem;
				return *this;
			}
			constexpr _dyn_arr& operator =(_dyn_arr&& a) noexcept {
				mem = std::move(a.mem);
				return *this;
			}
			constexpr _dyn_arr& operator =(std::initializer_list<T> list) noexcept(mem_type::noexcept_copy) {
				mem = list;
				return *this;
			}
			//返回数组起点的迭代器
			iterator begin() noexcept {
				return iterator(this);
			}
			//返回数组起点的只读迭代器
			const_iterator begin() const noexcept {
				return const_iterator(this);
			}
			//返回数组起点的只读迭代器
			const_iterator cbegin() const noexcept {
				return const_iterator(this);
			}
			//返回数组终点的迭代器
			iterator end() noexcept {
				return iterator(this, mem.count);
			}
			//返回数组终点的只读迭代器
			const_iterator end() const noexcept {
				return const_iterator(this, mem.count);
			}
			//返回数组终点的只读迭代器
			const_iterator cend() const noexcept {
				return const_iterator(this, mem.count);
			}

			//数组指针
			constexpr T* data() const noexcept {
				return mem.data;
			}
			//数组中元素数量
			constexpr I count() const noexcept {
				return mem.count;
			}
			//数组元素容量
			constexpr I size() const noexcept {
				return mem.size;
			}
			//数组元素数量是否为0
			constexpr bool empty() const noexcept {
				return mem.count == 0;
			}
			//数组容量是否为0
			constexpr bool isNull() const noexcept {
				return mem.size == 0;
			}
			//引用数组第一个元素
			constexpr T& first() noexcept {
				DME_ASSERT(mem.count != 0, "called on empty container");
				return mem.data[0];
			}
			//引用数组第一个元素
			constexpr const T& first() const noexcept {
				DME_ASSERT(mem.count != 0, "called on empty container");
				return mem.data[0];
			}
			//引用数组最后一个元素
			constexpr T& last() noexcept {
				DME_ASSERT(mem.count != 0, "called on empty container");
				return mem.data[mem.count - 1];
			}
			//引用数组最后一个元素
			constexpr const T& last() const noexcept {
				DME_ASSERT(mem.count != 0, "called on empty container");
				return mem.data[mem.count - 1];
			}
			//数组最后的索引，如果数组没有元素也会返回0
			constexpr I lastIndex() const noexcept {
				return mem.count ? mem.count - 1 : mem.count;
			}

			//初始化默认容量
			constexpr void init() noexcept {
				if (mem.size) return;
				mem.init();
			}

			//构造到数组最后
			template <typename... Args> requires(std::is_constructible_v<T, Args...>)
			constexpr void addMake(Args&&... args) noexcept(mem_type::noexcept_move && noexcept(T(std::forward<Args>(args)...))) {
				new (alloc()) T(std::forward<Args>(args)...);
			}
			//添加到数组最后
			template <typename P = T> requires(IsSameOrConvertible<P, T>)
			constexpr void add(P&& obj) noexcept(mem_type::noexcept_move && noexcept(T(std::forward<P>(obj)))) {
				new (alloc()) T(std::forward<P>(obj));
			}
			//添加到数组最后
			constexpr void add(const _dyn_arr& a) noexcept(mem_type::noexcept_copy) {
				if (a.mem.count == 0) return;
				mem.extend(mem.count + a.mem.count);
				mem.copyToTail(a.mem.data, a.mem.count);
				mem.count += a.mem.count;
			}
			//添加到数组最后
			constexpr void add(_dyn_arr&& a) noexcept(mem_type::noexcept_move) {
				if (a.mem.count == 0) return;
				mem.extend(mem.count + a.mem.count);
				mem.moveToTail(a.mem.data, a.mem.count);
				mem.count += a.mem.count;
				a.mem.count = 0;
			}
			//添加到数组最后
			constexpr void add(std::initializer_list<T> list) noexcept(mem_type::noexcept_copy) {
				I s = static_cast<I>(list.size());
				if (s == 0) return;
				mem.extend(mem.count + s);
				mem.copyToTail(list.begin(), s);
				mem.count += s;
			}
			//添加多次到数组最后
			constexpr void add(I addCount, const T& obj) noexcept(mem_type::noexcept_copy) {
				T* d = alloc(addCount);
				for (I i = 0; i < addCount; ++i) {
					new (d + i) T(obj);
				}
			}

			//构造到数组index处，如果位置超出当前数量，则会用未初始化对象填充
			template <typename... Args> requires(std::is_constructible_v<T, Args...>)
			constexpr void insertMake(I index, Args&&... args) noexcept(mem_type::noexcept_move && noexcept(T(std::forward<Args>(args)...))) {
				new (allocAt(index)) T(std::forward<Args>(args)...);
			}
			//插入到数组index处，如果位置超出当前数量，则会用未初始化对象填充
			template <typename P = T> requires(IsSameOrConvertible<P, T>)
			constexpr void insert(I index, P&& obj) noexcept(mem_type::noexcept_move && noexcept(T(std::forward<P>(obj)))) {
				new (allocAt(index)) T(std::forward<P>(obj));
			}
			//插入到数组indexStart处，如果位置超出当前数量，则会用未初始化对象填充
			constexpr void insert(I indexStart, const _dyn_arr& a) noexcept(mem_type::noexcept_copy) {
				if (a.mem.count == 0) return;
				allocAt(indexStart, a.mem.count);
				mem.copyToIndex(a.mem.data, a.mem.count, indexStart);
			}
			//插入到数组indexStart处，如果位置超出当前数量，则会用未初始化对象填充
			constexpr void insert(I indexStart, _dyn_arr&& a) noexcept(mem_type::noexcept_move) {
				if (a.mem.count == 0) return;
				allocAt(indexStart, a.mem.count);
				mem.moveToIndex(a.mem.data, a.mem.count, indexStart);
				a.mem.count = 0;
			}
			//插入到数组indexStart处，如果位置超出当前数量，则会用未初始化对象填充
			constexpr void insert(I indexStart, std::initializer_list<T> list) noexcept(mem_type::noexcept_copy) {
				I s = static_cast<I>(list.size());
				if (s == 0) return;
				allocAt(indexStart, s);
				mem.copyToIndex(list.begin(), s, indexStart);
			}
			//插入到数组indexStart处insertCount个元素，如果位置超出当前数量，则会用未初始化对象填充
			constexpr void insert(I indexStart, I insertCount, const T& obj) noexcept(mem_type::noexcept_copy) {
				T* d = allocAt(indexStart, insertCount);
				for (I i = 0; i < insertCount; ++i) {
					new (d + i) T(obj);
				}
			}

			//顺序插入，数组必须是从小到大排序的，返回插入的下标并移动元素
			template <typename P = T> requires(IsSameOrConvertible<P, T>)
			constexpr I insertOrdered(P&& obj) noexcept(mem_type::noexcept_move && noexcept(T(std::forward<P>(obj)))) {
				I i1 = 0;
				I i2 = mem.count;
				while (i1 < i2) { //二分查找
					I i = i1 + ((i2 - i1) / 2);
					if (mem.data[i] < obj) {
						i1 = i + 1;
					}
					else {
						i2 = i;
					}
				}
				new (allocAt(i1)) T(std::forward<P>(obj));
				return i1;
			}
			//二分查找匹配的元素，使用三路比较，未找到则返回的key为false。数组必须是从小到大排序的
			template<typename UserType, typename CompareFunc> requires IsInvocableRet<CompareFunc, std::strong_ordering, const UserType&, const T&>
			constexpr Pair<bool, I> binarySearchUserType(const UserType& userType, CompareFunc&& func) const noexcept{
				I i1 = 0;
				I i2 = mem.count;
				I i = 0;
				while (i1 < i2) { //二分查找
					i = i1 + ((i2 - i1) / 2);
					auto r = func(userType, mem.data[i]);
					if (r == std::strong_ordering::less) {
						i2 = i;
					}
					else if (r == std::strong_ordering::greater) {
						i1 = i + 1;
					}
					else {
						return { true, i };
					}
				}
				return { false, i };
			}

			//移除并替换构造某个下标的元素
			template <typename... Args> requires(std::is_constructible_v<T, Args...>)
			constexpr void replaceMake(I index, Args&&... args) noexcept(noexcept(new (mem.data + index) T(std::forward<Args>(args)...))) {
				DME_ASSERT(index < mem.count, "subscript exceeds the count");
				mem.data[index].~T();
				new (mem.data + index) T(std::forward<Args>(args)...);
			}
			//移除并替换某个下标的元素
			template <typename P = T> requires(IsSameOrConvertible<P, T>)
			constexpr void replace(I index, P&& obj) noexcept(noexcept(new (mem.data + index) T(std::forward<P>(obj)))) {
				DME_ASSERT(index < mem.count, "subscript exceeds the count");
				mem.data[index].~T();
				new (mem.data + index) T(std::forward<P>(obj));
			}
			//移除并替换某个下标的元素
			template <typename P = T> requires(IsSameOrConvertible<P, T>)
			constexpr void replaceAssign(I index, P&& obj) noexcept(noexcept(mem.data[index] = T(std::forward<P>(obj)))) {
				DME_ASSERT(index < mem.count, "subscript exceeds the count");
				mem.data[index] = T(std::forward<P>(obj));
			}
			//移除并替换从indexStart开始的a.count()个元素
			constexpr void replace(I indexStart, const _dyn_arr& a) noexcept(mem_type::noexcept_copy_assign) {
				if (a.mem.count == 0) return;
				mem.copyToIndexReplace(a.mem.data, a.mem.count, indexStart);
			}
			//移除并替换从indexStart开始的a.count()个元素
			constexpr void replace(I indexStart, _dyn_arr&& a) noexcept(mem_type::noexcept_move_assign) {
				if (a.mem.count == 0) return;
				mem.moveToIndexReplace(a.mem.data, a.mem.count, indexStart);
				a.mem.count = 0;
			}
			//移除并替换从indexStart开始的list.size()个元素
			constexpr void replace(I indexStart, std::initializer_list<T> list) noexcept(mem_type::noexcept_copy) {
				I s = static_cast<I>(list.size());
				if (s == 0) return;
				mem.copyToIndexReplace(list.begin(), s, indexStart);
			}
			//移除并替换从indexStart开始的replaceCount个元素
			constexpr void replace(I indexStart, I replaceCount, const T& obj) noexcept(mem_type::noexcept_copy) {
				I endIndex = indexStart + replaceCount;
				DME_ASSERT(endIndex <= mem.count, "end subscript exceeds the count");
				for (I i = indexStart; i < endIndex; ++i) {
					mem.data[i] = T(obj);
				}
			}

			//移除最后的元素
			constexpr void remove() noexcept {
				if (mem.count == 0) return;
				mem.count--;
				mem.destory(mem.count);
			}
			//移除某个下标的元素，后面的元素往前移动
			constexpr void remove(I index) noexcept(mem_type::noexcept_move) {
				DME_ASSERT(index < mem.count, "subscript exceeds the count");
				mem.destory(index);
				mem.count--;
				if (index < mem.count) {
					mem.moveToLeft(index + 1, mem.count - index, 1);
				}
			}
			//移除第一个下标后的removeCount个元素（包含第一个下标），后面的元素往前移动
			constexpr void remove(I indexStart, I removeCount) noexcept(mem_type::noexcept_move) {
				if (removeCount == 0) return;
				I endIndex = indexStart + removeCount;
				DME_ASSERT(endIndex <= mem.count, "end subscript exceeds the count");
				mem.destory(indexStart, removeCount);
				if (endIndex < mem.count) {
					mem.moveToLeft(endIndex, mem.count - endIndex, removeCount);
				}
				mem.count -= removeCount;
			}

			//交换两个下标的元素
			constexpr void swap(I index1, I index2) noexcept(mem_type::noexcept_move_assign) {
				DME_ASSERT(index1 < mem.count && index2 < mem.count, "subscript exceeds the count");
				if (index1 != index2) {
					mem.swap(index1, index2);
				}
			}
			//交换两个数组
			constexpr void swap(_dyn_arr& a) noexcept {
				mem.swap(a.mem);
			}
			//移除某个下标的元素，将尾部的元素交换到该位置，其他元素位置不变，返回删除前尾部元素的下标
			constexpr I removeSwapTail(I index) noexcept(mem_type::noexcept_move_assign) {
				DME_ASSERT(index < mem.count, "subscript exceeds the count");
				mem.count--;
				if (index < mem.count) {
					mem.data[index] = std::move(mem.data[mem.count]);
				}
				mem.destory(mem.count);
				return mem.count;
			}

			//遍历元素
			constexpr void each(const std::function<void(T&)>& func) const {
				for (I i = 0; i < mem.count; ++i) {
					func(mem.data[i]);
				}
			}
			//遍历元素，传入函数的第二个参数如果修改为true，则跳过剩下的元素直接结束
			constexpr void each(const std::function<void(T&, bool&)>& func) const {
				bool isBreak = false;
				for (I i = 0; i < mem.count; ++i) {
					func(mem.data[i], isBreak);
					if (isBreak) return;
				}
			}

			//清除所有元素
			constexpr void clear() noexcept {
				mem.destoryAll();
				mem.count = 0;
			}
			//删除所有元素并回收数组的内存
			constexpr void release() noexcept {
				mem.release();
			}


			//重新设置数组数量，如果数量比目前的大，则多出的对象内存是未初始化状态（但是不会减少原本的数量，只会扩大）
			constexpr void recount(I newCount) noexcept(mem_type::noexcept_move) {
				if (mem.count == newCount) return;
				if (mem.count > newCount) {
					mem.destory(newCount, mem.count - newCount);
				}
				else { //判断是否需要扩容
					mem.extend(newCount);
				}
				mem.count = newCount;
			}
			//重新设置数组容量（但是不会减少原本的容量，只会扩大）
			constexpr void resize(I newSize) noexcept(mem_type::noexcept_move) {
				if (mem.size == newSize) return;
				if (mem.count > newSize) {
					mem.destory(newSize, mem.count - newSize);
					mem.count = newSize;
				}else { //判断是否需要扩容
					mem.extend(newSize);
				}
			}
			//获取索引处迭代器
			constexpr iterator it(I index) const noexcept {
				return iterator(this, index);
			}
			//获取索引处只读迭代器
			constexpr const_iterator cit(I index) const noexcept {
				return const_iterator(this, index);
			}

			constexpr T& operator [](I index) noexcept {
				DME_ASSERT(index < mem.size, "subscript exceeds the size");
				return mem.data[index];
			}
			constexpr const T& operator [](I index) const noexcept {
				DME_ASSERT(index < mem.size, "subscript exceeds the size");
				return mem.data[index];
			}

			constexpr bool operator ==(const _dyn_arr& a) const noexcept {
				return mem == a.mem;
			}
			friend std::ostream& operator<<(std::ostream& os, const _dyn_arr& a) {
				return os << a.mem;
			}
			friend constexpr Hash Hasher(const _dyn_arr& a) noexcept {
				return Hasher(a.mem);
			}
		};

	}
	//一种简单的动态大小数组，扩容和增删时元素位置会变动
	//数组索引以uint传递
	template<typename T>
	using DynArr = core::_dyn_arr<T, uint>;
	//一种简单的动态大小数组，扩容和增删时元素位置会变动
	//数组索引以ulong传递
	template<typename T>
	using DynArrL = core::_dyn_arr<T, ulong>;
}

namespace std {
	template<typename T, typename I>
	struct hash<dme::core::_dyn_arr<T, I>> {
		size_t operator ()(const dme::core::_dyn_arr<T, I>& a) const noexcept {
			return Hasher(a);
		}
	};
}