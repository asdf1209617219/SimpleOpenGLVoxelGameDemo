#pragma once
#include <dme/core/utils.h>
namespace dme::core {
	//拷贝并交换，利用RAII保证强异常安全性
	template<typename T, IsUnsigned I>
	struct _arr_mem_copy_and_swap {
		T* data;
		I count;
		I success_count;

		constexpr _arr_mem_copy_and_swap(I count_, I size_) noexcept : data(Malloc<T>(size_)), count(count_), success_count(0) {}
		//oldCount_为to_原先的元素个数，拷贝成功则替换to_，拷贝过程中触发异常则原对象无影响
		constexpr void copyAndSwap(const T* from_, T*& to_, const I oldCount_) {
			for (; success_count < count; ++success_count) {
				new (data + success_count) T(from_[success_count]);
			}
			//swap
			T* r = data;
			data = to_;
			to_ = r;
			success_count = oldCount_;
		}

		constexpr ~_arr_mem_copy_and_swap() {
			if (data) [[likely]] { //自动析构原来的数据，或者触发异常后处理临时数据
				//析构success_count个元素，如果元素的类型具有trivially destructible特性，则跳过析构
				if constexpr (!std::is_trivially_destructible_v<T>) {
					for (I i = 0; i < success_count; ++i) {
						data[i].~T();
					}
				}
				Free(data);
				data = null;
			}
		}
	};
	//拷贝初始化，利用RAII保证强异常安全性
	template<typename T, IsUnsigned I>
	struct _arr_mem_copy_init {
		T* data;
		I count;
		I success_count;

		constexpr _arr_mem_copy_init(I count_, I size_) noexcept : data(Malloc<T>(size_)), count(count_), success_count(0) {}
		//拷贝成功则返回指针，拷贝过程中触发异常则原对象无影响
		[[nodiscard]] constexpr T* copyInit(const T* from_) {
			for (; success_count < count; ++success_count) {
				new (data + success_count) T(from_[success_count]);
			}
			T* r = data;
			data = null;
			return r;
		}

		constexpr ~_arr_mem_copy_init() {
			if (data) [[unlikely]] { //拷贝初始化失败的分支
				//析构success_count个元素，如果元素的类型具有trivially destructible特性，则跳过析构
				if constexpr (!std::is_trivially_destructible_v<T>) {
					for (I i = 0; i < success_count; ++i) {
						data[i].~T();
					}
				}
				Free(data);
				data = null;
			}
		}
	};

	//以数组形式管理内存，提供基本的扩容、拷贝、移动等功能，I只建议使用uint、ulong
	template<typename T, IsUnsigned I>
	struct _arr_mem {
	private:
	public:
		T* data; //内存起点
		I count; //当前数量
		I size; //容量

		//默认构造函数
		static constexpr bool noexcept_default = is_relocatable<T> || nothrow_default_construct<T>;
		//拷贝
		static constexpr bool noexcept_copy = is_relocatable<T> || nothrow_copy_construct<T>;
		//移动
		static constexpr bool noexcept_move = is_relocatable<T> || nothrow_move_construct<T> || nothrow_copy_construct<T>;
		//拷贝赋值
		static constexpr bool noexcept_copy_assign = is_relocatable<T> || nothrow_copy_assign<T>;
		//移动赋值
		static constexpr bool noexcept_move_assign = is_relocatable<T> || nothrow_move_assign<T> || nothrow_copy_assign<T>;
		//析构（理论上不允许析构时抛出异常）
		static constexpr bool noexcept_destory = nothrow_destruct<T>;
		static constexpr I default_size = Config::default_init_size;

		constexpr _arr_mem() noexcept : data(null), count(0), size(0) {}
		constexpr explicit _arr_mem(const I size_) noexcept : data(null), count(0), size(size_) {
			if (size) {
				init(size);
			}
		}
		constexpr _arr_mem(const _arr_mem& a) noexcept(noexcept_copy) : _arr_mem() {
			copyInit(a.data, a.count, a.size);
		}
		constexpr _arr_mem(_arr_mem&& a) noexcept : data(a.data), count(a.count), size(a.size) {
			a.setNull();
		}
		constexpr _arr_mem(std::initializer_list<T> list_) noexcept(noexcept_copy) : _arr_mem() {
			I size_ = static_cast<I>(list_.size());
			copyInit(list_.begin(), size_, size_);
		}

		constexpr _arr_mem& operator =(const _arr_mem& a) noexcept(noexcept_copy) {
			if (this != std::addressof(a)) {
				copyAndCoverThis(a.data, a.count, a.size);
			}
			return *this;
		}
		constexpr _arr_mem& operator =(_arr_mem&& a) noexcept {
			if (this != std::addressof(a)) {
				if (data) {
					destoryAll();
					Free(data);
				}
				data = a.data;
				count = a.count;
				size = a.size;

				a.setNull();
			}
			return *this;
		}
		constexpr _arr_mem& operator =(std::initializer_list<T> list) noexcept(noexcept_copy) {
			I size_ = static_cast<I>(list.size());
			copyAndCoverThis(list.begin(), size_, size_);
			return *this;
		}

		constexpr ~_arr_mem() {
			release();
		}
		//清除所有元素并回收内存
		constexpr void release() noexcept {
			if (data) {
				destoryAll();
				Free(data);
				setNull();
			}
		}

		//初始化容量，参数不能为0
		constexpr void init(I size_ = default_size) noexcept {
			size = size_;
			data = Malloc<T>(size);
		}

		//拷贝左值初始化（强异常安全性）
		constexpr void copyInit(const T* from_, const I count_, const I size_) noexcept(noexcept_copy) {
			if (!size_) {
				return;
			}
			if constexpr (is_relocatable<T>) {
				data = Malloc<T>(size_);
				if (count_) {
					Memcpy<T>(data, from_, count_);
				}
			}
			else if constexpr (nothrow_copy_construct<T>) {
				data = Malloc<T>(size_);
				for (I i = 0; i < count_; ++i) {
					new (data + i) T(from_[i]);
				}
			}
			else {
				auto m = _arr_mem_copy_init<T, I>(count_, size_);
				data = m.copyInit(from_);
			}
			count = count_;
			size = size_;
		}

		//拷贝左值并覆盖当前内存，自动析构原内存，修改count和size（强异常安全性）
		constexpr void copyAndCoverThis(const T* from_, const I count_, const I size_) noexcept(noexcept_copy) {
			if (!count_) {
				destoryAll();
				count = count_;
				return;
			}
			if constexpr (is_relocatable<T>) {
				exceedThenRealloc(size_);
				Memcpy<T>(data, from_, count_);
			}
			else if constexpr (nothrow_copy_construct<T>) {
				destoryAll();
				exceedThenRealloc(size_);
				for (I i = 0; i < count_; ++i) {
					new (data + i) T(from_[i]);
				}
			}
			else {
				I s = getApplySize(size_);
				auto m = _arr_mem_copy_and_swap<T, I>(count_, s);
				m.copyAndSwap(from_, data, count);
				size = s;
			}
			count = count_;
		}

		//拷贝左值并添加到尾部，不保证强异常安全性，不修改count和size
		constexpr void copyToTail(const T* from_, const I count_) noexcept(noexcept_copy) {
			T* d = data + count;
			if constexpr (is_relocatable<T>) {
				Memcpy<T>(d, from_, count_);
			}
			else {
				for (I i = 0; i < count_; ++i) {
					new (d + i) T(from_[i]);
				}
			}
		}
		//拷贝右值并添加到尾部，不保证强异常安全性，不修改count和size
		constexpr void moveToTail(T* from_, const I count_) noexcept(noexcept_move) {
			T* d = data + count;
			if constexpr (is_relocatable<T>) {
				Memcpy<T>(d, from_, count_);
			}
			else {
				for (I i = 0; i < count_; ++i) {
					if constexpr (nothrow_move_construct<T>) {
						new (d + i) T(std::move(from_[i]));
					}
					else {
						new (d + i) T(from_[i]);
					}
					from_[i].~T();
				}
			}
		}

		//拷贝左值并添加到index_，不保证强异常安全性，不修改count和size
		constexpr void copyToIndex(const T* from_, const I count_, const I index_) noexcept(noexcept_copy) {
			T* d = data + index_;
			if constexpr (is_relocatable<T>) {
				Memcpy<T>(d, from_, count_);
			}
			else {
				for (I i = 0; i < count_; ++i) {
					new (d + i) T(from_[i]);
				}
			}
		}
		//拷贝右值并添加到index_，不保证强异常安全性，不修改count和size
		constexpr void moveToIndex(T* from_, const I count_, const I index_) noexcept(noexcept_move) {
			T* d = data + index_;
			if constexpr (is_relocatable<T>) {
				Memcpy<T>(d, from_, count_);
			}
			else {
				for (I i = 0; i < count_; ++i) {
					if constexpr (nothrow_move_construct<T>) {
						new (d + i) T(std::move(from_[i]));
					}
					else {
						new (d + i) T(from_[i]);
					}
					from_[i].~T();
				}
			}
		}

		//拷贝左值并替换到index_，不保证强异常安全性
		constexpr void copyToIndexReplace(const T* from_, const I count_, const I index_) noexcept(noexcept_copy_assign) {
			T* d = data + index_;
			if constexpr (is_relocatable<T>) {
				Memcpy<T>(d, from_, count_);
			}
			else {
				for (I i = 0; i < count_; ++i) {
					d[i].~T();
					new (d + i) T(from_[i]);
				}
			}
		}
		//拷贝右值并替换到index_，不保证强异常安全性
		constexpr void moveToIndexReplace(T* from_, const I count_, const I index_) noexcept(noexcept_move_assign) {
			T* d = data + index_;
			if constexpr (is_relocatable<T>) {
				Memcpy<T>(d, from_, count_);
			}
			else {
				for (I i = 0; i < count_; ++i) {
					d[i].~T();
					if constexpr (nothrow_move_assign<T>) {
						new (d + i) T(std::move(from_[i]));
					}
					else {
						new (d + i) T(from_[i]);
					}
					from_[i].~T();
				}
			}
		}

		//从indexStart_开始的count_个元素向左移动moveDistance_个下标（注意如果noexcept_move==false则不保证强异常安全性）
		constexpr void moveToLeft(const I indexStart_, const I count_, const I moveDistance_) noexcept(noexcept_move) {
			if constexpr (is_relocatable<T>) {
				Memmove<T>(data + indexStart_ - moveDistance_, data + indexStart_, count_);
			}
			else {
				T* p = data + indexStart_;
				for (I i = 0; i < count_; ++i) {
					if constexpr (nothrow_move_construct<T>) {
						new (p - moveDistance_ + i) T(std::move(p[i]));
					}
					else {
						new (p - moveDistance_ + i) T(p[i]);
					}
					p[i].~T();
				}
			}
		}
		//从indexStart_开始的count_个元素向右移动moveDistance_个下标（注意如果noexcept_move==false则不保证强异常安全性）
		constexpr void moveToRight(const I indexStart_, const I count_, const I moveDistance_) noexcept(noexcept_move) {
			if constexpr (is_relocatable<T>) {
				Memmove<T>(data + indexStart_ + moveDistance_, data + indexStart_, count_);
			}
			else {
				T* p = data + indexStart_;
				for (I i = count_; i > 0;) { //从后向前遍历
					i--;
					if constexpr (nothrow_move_construct<T>) {
						new (p + moveDistance_ + i) T(std::move(p[i]));
					}
					else {
						new (p + moveDistance_ + i) T(p[i]);
					}
					p[i].~T();
				}
			}
		}

		//析构index_上的元素，如果元素的类型具有trivially destructible特性，则跳过析构
		constexpr void destory(const I index_) noexcept {
			if constexpr (!std::is_trivially_destructible_v<T>) {
				data[index_].~T();
			}
		}
		//从index_开始析构count_个元素，如果元素的类型具有trivially destructible特性，则跳过析构
		constexpr void destory(I index_, I count_) noexcept {
			if constexpr (!std::is_trivially_destructible_v<T>) {
				count_ += index_;
				for (; index_ < count_; ++index_) {
					data[index_].~T();
				}
			}
		}
		//析构全部元素，如果元素的类型具有trivially destructible特性，则跳过析构
		constexpr void destoryAll() noexcept {
			if constexpr (!std::is_trivially_destructible_v<T>) {
				for (I i = 0; i < count; ++i) {
					data[i].~T();
				}
			}
		}
		//交换两个下标的元素，注意该方法不会检测参数合法性
		constexpr void swap(I index1, I index2) noexcept(noexcept_move_assign) {
			T temp = std::move(data[index1]);
			data[index1] = std::move(data[index2]);
			data[index2] = std::move(temp);
		}
		//交换两个_arr_mem
		constexpr void swap(_arr_mem& a) noexcept {
			T* d = a.data; a.data = data; data = d;
			I c = a.count; a.count = count; count = c;
			I s = a.size; a.size = size; size = s;
		}

		//检查是否需要扩大容量，如果需要则扩大size后Free并申请更大的内存（不涉及元素的复制）
		constexpr void exceedThenRealloc(const I size_) noexcept {
			if (size < size_) {
				if (size == 0) {
					size = default_size;
				}
				do {
					size <<= 1;
				} while (size < size_);
				Free(data);
				data = Malloc<T>(size_);
			}
		}
		//获取需要申请的容量，如果需要扩大则获取扩大后的容量
		constexpr I getApplySize(const I size_) noexcept {
			I s = size;
			if (s < size_) {
				if (s == 0) {
					s = default_size;
				}
				do {
					s <<= 1;
				} while (s < size_);
				return s;
			}
			return s;
		}

		//检测是否需要扩容，如果需要则扩容并将原来的元素复制到新内存并返回true
		constexpr bool extend(const I size_) noexcept(noexcept_move) {
			bool needExtend = size < size_;
			if (needExtend) {
				I s = size;
				if (s == 0) {
					size = default_size;
					while (size < size_) {
						size <<= 1;
					};
					data = Malloc<T>(size);
					return needExtend;
				}
				do {
					s <<= 1;
				} while (s < size_);
				if constexpr (is_relocatable<T> || noexcept_move) {
					T* p = Malloc<T>(s);
					if constexpr (is_relocatable<T>) {
						Memcpy<T>(p, data, count);
					}
					else {
						for (I i = 0; i < count; ++i) {
							if constexpr (nothrow_move_construct<T>) {
								new (p + i) T(std::move(data[i]));
							}
							else {
								new (p + i) T(data[i]);
							}
							data[i].~T();
						}
					}
					Free(data);
					data = p;
				}
				else {
					auto m = _arr_mem_copy_and_swap<T, I>(count, s);
					m.copyAndSwap(data, data, count);
				}
				size = s;
			}
			return needExtend;
		}

		constexpr void setNull() noexcept {
			data = null;
			count = 0;
			size = 0;
		}

		//注意比较只看数量不看容量
		constexpr bool operator ==(const _arr_mem& a) const noexcept {
			if (this == &a) {
				return true;
			}
			if (count != a.count) {
				return false;
			}
			if constexpr (IsCanEqual<T>) {
				for (I i = 0; i < count; i++) {
					if (data[i] != a.data[i]) {
						return false;
					}
				}
				return true;
			}
			else {
				return Memcmp<T>(data, a.data, count) == 0;
			}
		}
		friend std::ostream& operator<<(std::ostream& os, const _arr_mem& a) {
			os << "count: " << a.count << ", size: " << a.size << ", data: [";
			for (I i = 0; i < a.count; ++i) {
				//4个一行
				//if (i % 4 == 0) {
				os << "\n";
				//}
				os << "\t[" << i << "]:\t";
				StreamSmartPrint(os, a.data[i]);
				os << ",";
			}
			os << "\n]";
			return os;
		}
		friend constexpr Hash Hasher(const _arr_mem& a) noexcept {
			Hash hash = Hasher(a.count);
			for (I i = 0; i < a.count; ++i) {
				hash.combine(a.data[i]);
			}
			return hash;
		}
	};

}