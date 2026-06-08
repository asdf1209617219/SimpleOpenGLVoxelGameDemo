#pragma once
#include <dme/core/hash.h>
namespace dme {
	//两个uint合并为一个ulong
	constexpr ulong UintToUlong(uint high, uint low) noexcept {
		return (static_cast<ulong>(high) << int_bit) | static_cast<ulong>(low);
	}
	//返回x的下一个2的指数（0会返回1）
	template<typename T>
	constexpr T NextPowerOfTwo(T x) noexcept {
		x |= (x >> 1);
		x |= (x >> 2);
		x |= (x >> 4);
		if constexpr (bit_size<T> >= 16) {
			x |= (x >> 8);
		}
		if constexpr (bit_size<T> >= 32) {
			x |= (x >> 16);
		}
		if constexpr (bit_size<T> >= 64) {
			x |= (x >> 32);
		}
		return x + 1;
	}
	//是否是2的指数
	template<typename T>
	inline constexpr bool IsPowerOfTwo(T x) noexcept {
		return x != 0 && (x & (x - 1)) == 0;
	}

	//最高位bit设置为1
	template<typename T>
	inline constexpr T HighBitSetOne(T a) noexcept {
		return high_bit_one<T> | a;
	}
	//最高位bit设置为0
	template<typename T>
	inline constexpr T HighBitSetZero(T a) noexcept {
		return except_high_bit_one<T> &a;
	}
	//最高位bit是否为1
	template<typename T>
	inline constexpr bool HighBitIsOne(T a) noexcept {
		return high_bit_one<T> &a;
	}

	//求最高位的1出现的位置，比如1则返回1，0->0，2->2，4->3，7->3，8->4，注意只允许整数使用该函数，并且函数内强制使用逻辑右移
	template<IsInteger T>
	inline constexpr uint BitWidth(T a) noexcept {
		/*
		using unsigned_type = get_unsigned_type<T>;

		unsigned_type n = static_cast<unsigned_type>(a);
		uint width = 0;
		uint shift = bit_size<unsigned_type> >> 1;
		do {
			unsigned_type temp = n >> shift;
			if (temp) {
				width += shift;
				n = temp;
			}
			shift >>= 1;
		} while (shift);
		if (n) {
			width += 1;
		}
		return width;
		*/

		return std::bit_width<get_unsigned_type<T>>(a);
	}

	//将传入参数位置的bit以及后面的bit都设置为1（比如0返回0，1->1，2->3，3->7），函数内强制使用逻辑右移，注意返回类型默认为uint
	template<IsInteger T = uint>
	inline constexpr T BitFull(uint width) noexcept {
		return (bit_max<get_unsigned_type<T>>) >> (bit_size<T> -width);
	}

	//将最高位的1后面都设置为1，注意只允许整数使用该函数，并且函数内强制使用逻辑右移
	template<IsInteger T>
	inline constexpr T BitFill(T a) noexcept {
		/*
		using unsigned_type = get_unsigned_type<T>;

		unsigned_type n = static_cast<unsigned_type>(a);
		uint shift = bit_size<unsigned_type> >> 1;
		do {
			n = n >> shift;
			shift >>= 1;
		} while (shift);

		return n;
		*/

		return BitFull<T>(BitWidth<T>(a));
	}

	//将x向上取到2的指数（如果x是2的指数则返回x）
	template<IsInteger T>
	inline constexpr T CeilPowerOfTwo(T x) noexcept {
		using unsigned_type = get_unsigned_type<T>;
		unsigned_type n = static_cast<unsigned_type>(x);
		unsigned_type i = static_cast<unsigned_type>(1) << BitWidth<unsigned_type>(n);
		return (n << 1) == i ? n : i;
	}

	template<typename T>
	inline constexpr void Swap(T& a, T& b) {
		T temp = std::move(a);
		a = std::move(b);
		b = std::move(temp);
	}

	//申请对齐的内存，byte_count表示要申请的字节数
	DME_INLINE void* MallocByte(size_t byte_count, size_t byte_alignment = Config::memory_alignment) noexcept {
#if DME_WINDOWS_FAMILY
		return ::_aligned_malloc(byte_count, byte_alignment);
#elif DME_LINUX_FAMILY || DME_SWITCH
		return ::memalign(byte_alignment, byte_size);
#else
		return ::malloc(byte_size);
#endif
	}
	//申请对齐的内存，byte_count表示要申请的字节数
	DME_INLINE void* MallocByte32(uint byte_count, uint byte_alignment = Config::memory_alignment) noexcept {
#if DME_WINDOWS_FAMILY
		return ::_aligned_malloc(byte_count, byte_alignment);
#elif DME_LINUX_FAMILY || DME_SWITCH
		return ::memalign(byte_alignment, byte_size);
#else
		return ::malloc(byte_size);
#endif
	}

	template<typename T>
	//申请对齐的内存，返回具有类型的指针，count表示要申请的对象个数
	DME_INLINE T* Malloc(size_t obj_count, size_t byte_alignment = Config::memory_alignment) noexcept {
#if DME_WINDOWS_FAMILY
		return reinterpret_cast<T*>(::_aligned_malloc(obj_count * sizeof(T), byte_alignment));
#elif DME_LINUX_FAMILY || DME_SWITCH
		return reinterpret_cast<T*>(::memalign(byte_alignment, obj_count * sizeof(T)));
#else
		return reinterpret_cast<T*>(::malloc(obj_count * sizeof(T)));
#endif
	}
	template<typename T>
	//申请对齐的内存，返回具有类型的指针，count表示要申请的对象个数
	DME_INLINE T* Malloc32(uint obj_count, uint byte_alignment = Config::memory_alignment) noexcept {
#if DME_WINDOWS_FAMILY
		obj_count *= sizeof(T);
		return reinterpret_cast<T*>(::_aligned_malloc(obj_count, byte_alignment));
#elif DME_LINUX_FAMILY || DME_SWITCH
		return reinterpret_cast<T*>(::memalign(byte_alignment, obj_count));
#else
		return reinterpret_cast<T*>(::malloc(obj_count));
#endif
	}

	//Malloc申请的空间必须通过该函数释放
	DME_INLINE void Free(void* ptr) noexcept {
#if DME_WINDOWS_FAMILY
		::_aligned_free(ptr);
#elif DME_LINUX_FAMILY || DME_SWITCH
		::free(ptr);
#else
		::free(ptr);
#endif
	}

	//与std::memmove相同
	DME_INLINE void MemmoveByte(void* target, const void* src, size_t byte_count) noexcept {
		std::memmove(target, src, byte_count);
	}
	//与std::memmove相同
	DME_INLINE void MemmoveByte32(void* target, const void* src, uint byte_count) noexcept {
		std::memmove(target, src, byte_count);
	}
	template<typename T>
	//内存移动函数，可以传入具有类型的指针，count表示要移动的对象个数
	DME_INLINE void Memmove(T* target, const T* src, size_t obj_count) noexcept {
		std::memmove(target, src, obj_count * sizeof(T));
	}
	template<typename T>
	//内存移动函数，可以传入具有类型的指针，count表示要移动的对象个数
	DME_INLINE void Memmove32(T* target, const T* src, uint obj_count) noexcept {
		std::memmove(target, src, obj_count * sizeof(T));
	}

	//与std::memcpy相同
	DME_INLINE void MemcpyByte(void* target, const void* src, size_t byte_count) noexcept {
		std::memcpy(target, src, byte_count);
	}
	//与std::memcpy相同
	DME_INLINE void MemcpyByte32(void* target, const void* src, uint byte_count) noexcept {
		std::memcpy(target, src, byte_count);
	}
	template<typename T>
	//内存拷贝函数，可以传入具有类型的指针，count表示要拷贝的对象个数
	DME_INLINE void Memcpy(T* target, const T* src, size_t obj_count) noexcept {
		std::memcpy(target, src, obj_count * sizeof(T));
	}
	template<typename T>
	//内存拷贝函数，可以传入具有类型的指针，count表示要拷贝的对象个数
	DME_INLINE void Memcpy32(T* target, const T* src, uint obj_count) noexcept {
		std::memcpy(target, src, obj_count * sizeof(T));
	}

	//与std::memset相同
	DME_INLINE void MemsetByte(void* target, uchar val, size_t byte_count) noexcept {
		std::memset(target, val, byte_count);
	}
	//与std::memset相同
	DME_INLINE void MemsetByte32(void* target, uchar val, uint byte_count) noexcept {
		std::memset(target, val, byte_count);
	}
	template<typename T>
	//内存设置函数，可以传入具有类型的指针，count表示要设置的对象个数
	DME_INLINE void Memset(T* target, uchar val, size_t obj_count) noexcept {
		std::memset(target, val, obj_count * sizeof(T));
	}
	template<typename T>
	//内存设置函数，可以传入具有类型的指针，count表示要设置的对象个数
	DME_INLINE void Memset32(T* target, uchar val, uint obj_count) noexcept {
		std::memset(target, val, obj_count * sizeof(T));
	}

	//与std::memcmp相同
	DME_INLINE sint MemcmpByte(const void* data1, const void* data2, size_t byte_count) noexcept {
		return std::memcmp(data1, data2, byte_count);
	}
	//与std::memcmp相同
	DME_INLINE sint MemcmpByte32(const void* data1, const void* data2, uint byte_count) noexcept {
		return std::memcmp(data1, data2, byte_count);
	}
	template<typename T>
	//内存比较函数，可以传入具有类型的指针，count表示要比较的对象个数
	DME_INLINE sint Memcmp(const T* data1, const T* data2, size_t obj_count) noexcept {
		return std::memcmp(data1, data2, obj_count * sizeof(T));
	}
	template<typename T>
	//内存比较函数，可以传入具有类型的指针，count表示要比较的对象个数
	DME_INLINE sint Memcmp32(const T* data1, const T* data2, uint obj_count) noexcept {
		return std::memcmp(data1, data2, obj_count * sizeof(T));
	}
	/*
	template<>
	//与std::memcmp相同
	DME_INLINE sint Memcmp<void>(const void* data1, const void* data2, size_t byte_count) noexcept {
		return std::memcmp(data1, data2, byte_count);
	}
	*/

	//申请两倍的新内存，将旧内存memcpy到新内存，并回收旧内存
	DME_INLINE void MemExtendByte(void*& memory, const size_t old_byte_count, size_t& old_byte_size) noexcept {
		old_byte_size *= 2;
		void* newMemory = MallocByte(old_byte_size);
		MemcpyByte(newMemory, memory, old_byte_count);
		Free(memory);
		memory = newMemory;
	}
	template<typename T, IsBaseNumber S>
	//申请两倍的新内存，将旧内存memcpy到新内存，并回收旧内存，可以传入具有类型的指针，count表示要拷贝的对象个数
	DME_INLINE void MemExtend(T*& memory, const size_t old_count, S& old_size) noexcept {
		old_size *= 2;
		T* newMemory = Malloc<T>(old_size);
		Memcpy<T>(newMemory, memory, old_count);
		Free(memory);
		memory = newMemory;
	}




	//打印对象的字节
	template <typename T>
	std::ostream& StreamPrintByte(std::ostream& os, const T& obj) {
		const uchar* p = reinterpret_cast<const uchar*>(std::addressof(obj));
		auto flags = os.flags();
		os << std::hex; //十六进制
		for (size_t i = 0; i < sizeof(T); ++i) {
			//以两位十六进制、大写、补零的形式输出
			os << std::setw(2) << std::uppercase << std::setfill('0') << static_cast<sint>(p[i]) << " ";
		}
		os.flags(flags); // 恢复输出格式
		return os;
	}
	//如果支持输出流就直接打印，否则打印它的内存原始字节
	template <typename T>
	std::ostream& StreamSmartPrint(std::ostream& os, const T& obj) {
		if constexpr (IsCanStreamOut<T>) {
			os << obj;
		}
		else {
			StreamPrintByte(os, obj);
		}
		return os;
	}

	namespace core {
		//由于使用了指针的高6位存储下标，所以只能支持64位系统
		template<typename T>
		struct CompressPtr {
			static constexpr uint _obj_pool_shift_count = BitWidth(size_t_bit) - 1;
			static constexpr uint _obj_pool_index_shift_count = size_t_bit - _obj_pool_shift_count;
			static constexpr ulong _obj_pool_raw_ptr_mask = ulong_max >> _obj_pool_shift_count;
			static constexpr ulong _obj_pool_raw_ptr_mask_inverse = ~_obj_pool_raw_ptr_mask;

			union {
				uchar* ptr;
				ulong value;
			};
			constexpr explicit CompressPtr(uchar* rawPtr) noexcept : ptr(rawPtr) {}
			constexpr CompressPtr(T* rawPtr, size_t index) noexcept : value(
				reinterpret_cast<size_t>(rawPtr) | (index << _obj_pool_index_shift_count)
			) {}

			constexpr bool isNull() noexcept {
				return ptr == null;
			}
			constexpr T* getRaw() noexcept {
				return reinterpret_cast<T*>(value & _obj_pool_raw_ptr_mask);
			}
			constexpr const T* getRaw() const noexcept {
				return reinterpret_cast<const T*>(value & _obj_pool_raw_ptr_mask);
			}
			constexpr uchar getIndex() const noexcept {
				return static_cast<uchar>(value >> _obj_pool_index_shift_count);
			}
			//修改index
			constexpr void setIndex(size_t index) noexcept {
				value = value & _obj_pool_raw_ptr_mask | (index << _obj_pool_index_shift_count);
			}

			constexpr void setMax() noexcept {
				value = ulong_max;
			}
			constexpr void setZero() noexcept {
				ptr = null;
			}
			constexpr void setRawAndIndex(void* ptr_, size_t index_) noexcept {
				value = reinterpret_cast<size_t>(ptr_) | (index_ << _obj_pool_index_shift_count);
			}

			constexpr T& operator*() noexcept {
				return *getRaw();
			}
			constexpr const T& operator*() const noexcept {
				return *getRaw();
			}
			constexpr T* operator->() noexcept {
				return getRaw();
			}
			constexpr const T* operator->() const noexcept {
				return getRaw();
			}
			//验证指针是否能支持修改高6位的值
			static constexpr bool HighestSixBitsIsZero(void* ptr_) noexcept {
				return (reinterpret_cast<size_t>(ptr_) & _obj_pool_raw_ptr_mask_inverse) == 0;
			}
		};

		template<ulong DefaultSize_>
		constexpr ulong _get_next_power_of_two_size(ulong size) noexcept {
			size = (size < DefaultSize_ ? DefaultSize_ : size) - 1;
			return NextPowerOfTwo(size);
		}
		template<ulong DefaultSize_>
		constexpr ulong _get_next_power_of_two_hash_size(ulong size) noexcept {
			size = (size < DefaultSize_ ? DefaultSize_ : size) - 1;
			return NextPowerOfTwo((size / 4) * 5); // TODO 暂时设置为1.25倍大小，防止初始化列表赋值时超过平衡因子
		}

		//有符号索引的基础模板类，默认初始化为std::numeric_limits<T>::min()该值也定义为null
		template<IsSigned T>
		struct _signed_id_base {
			T v;

			//默认初始化为-1
			constexpr _signed_id_base() noexcept : v(-1) {};
			constexpr _signed_id_base(T v) noexcept : v(v) {}

			//std::numeric_limits<T>::min()
			static constexpr T Min = std::numeric_limits<T>::min();
			//std::numeric_limits<T>::max()
			static constexpr T Max = std::numeric_limits<T>::max();
			//0
			static constexpr T Zero = 0;
			//1
			static constexpr T One = 1;
			//std::numeric_limits<T>::min()
			static constexpr T Null = std::numeric_limits<T>::min();

			//v = 0
			inline constexpr void setZero() noexcept {
				v = 0;
			}
			//v = Null
			inline constexpr void setNull() noexcept {
				v = Null;
			}
			//return v == Null
			inline constexpr bool isNull() const noexcept {
				return v == Null;
			}
			//return v != Null
			inline constexpr bool notNull() const noexcept {
				return v != Null;
			}
			//return !v
			inline constexpr bool isZero() const noexcept {
				return !v;
			}
			//return v == Min
			inline constexpr bool isMin() const noexcept {
				return v == Min;
			}
			//return v == Max
			inline constexpr bool isMax() const noexcept {
				return v == Max;
			}
			//return v > 0
			inline constexpr bool greaterThanZero() const noexcept {
				return v > 0;
			}
			//return v < 0
			inline constexpr bool lessThanZero() const noexcept {
				return v < 0;
			}
			//return v & high_bit_one<T>
			inline constexpr bool highBitIsOne() const noexcept {
				return v & high_bit_one<T>;
			}
			//return !(v & high_bit_one<T>)
			inline constexpr bool highBitIsZero() const noexcept {
				return !(v & high_bit_one<T>);
			}
			//return v & except_high_bit_one<T>
			inline constexpr T exceptHighBit() const noexcept {
				return v & except_high_bit_one<T>;
			}

			//v = (v & except_high_bit_one<T>) | (static_cast<std::make_unsigned<T>::type>(isOne) << (bit_size<T> - 1))
			inline constexpr void setHighBit(bool isOne) noexcept {
				//注意逻辑右移与算数右移的区别，一般用逻辑右移比较多(也就是unsigned的右移)
				v = (v & except_high_bit_one<T>) | (static_cast<std::make_unsigned<T>::type>(isOne) << (bit_size<T> -1));
			}
			//v = v | high_bit_one<T>;
			inline constexpr void setHighBitOne() noexcept {
				v = v | high_bit_one<T>;
			}
			//v = v & except_high_bit_one<T>;
			inline constexpr void setHighBitZero() noexcept {
				v = v & except_high_bit_one<T>;
			}
			//v = (v & high_bit_one<T>) | (i & except_high_bit_one<T>)
			inline constexpr void setExceptHighBit(T i) noexcept {
				v = (v & high_bit_one<T>) | (i & except_high_bit_one<T>);
			}
			//return v | high_bit_one<T>;
			inline constexpr T getHighBitOne() const noexcept {
				return v | high_bit_one<T>;
			}
			//return v & except_high_bit_one<T>;
			inline constexpr T getHighBitZero() const noexcept {
				return v & except_high_bit_one<T>;
			}

			inline constexpr operator T() const noexcept {
				return v;
			}
			friend constexpr Hash Hasher(const _signed_id_base& a) noexcept {
				return Hasher(a.v);
			}
		};
		template<IsSigned T>
		struct _signed_id_constexpr_base {
			//std::numeric_limits<T>::min()
			static constexpr _signed_id_base<T> Min = _signed_id_base<T>::Min;
			//std::numeric_limits<T>::max()
			static constexpr _signed_id_base<T> Max = _signed_id_base<T>::Max;
			//0
			static constexpr _signed_id_base<T> Zero = _signed_id_base<T>::Zero;
			//1
			static constexpr _signed_id_base<T> One = _signed_id_base<T>::One;
			//std::numeric_limits<T>::min()
			static constexpr _signed_id_base<T> Null = _signed_id_base<T>::Null;
		};

		//数据
		template<IsSigned T>
		struct _signed_id_data_base {
			_signed_id_base<T> type; //数据类型
			_signed_id_base<T> index; //数据索引

			constexpr _signed_id_data_base() noexcept : type(), index() {};
			constexpr _signed_id_data_base(_signed_id_base<T> type, _signed_id_base<T> index) noexcept : type(type), index(index) {}

			inline constexpr void setNull() noexcept {
				type.setNull();
				index.setNull();
			}
			inline constexpr bool isNull() const noexcept {
				return type.isNull() || index.isNull();
			}
			inline constexpr bool notNull() const noexcept {
				return type.notNull() && index.notNull();
			}
			friend constexpr Hash Hasher(const _signed_id_data_base& a) noexcept {
				return Hash::Combine(a.type, a.index);
			}
		};


		//无符号索引基础模板类，默认初始化为std::numeric_limits<T>::max()该值也定义为null
		template<IsUnsigned T>
		struct _unsigned_id_base {
			T v;

			//默认初始化为std::numeric_limits<T>::max()
			constexpr _unsigned_id_base() noexcept : v(std::numeric_limits<T>::max()) {};
			constexpr _unsigned_id_base(T v) noexcept : v(v) {}

			//0
			static constexpr T Min = 0;
			//std::numeric_limits<T>::max()
			static constexpr T Max = std::numeric_limits<T>::max();
			//0
			static constexpr T Zero = 0;
			//1
			static constexpr T One = 1;
			//std::numeric_limits<T>::max()
			static constexpr T Null = std::numeric_limits<T>::max();

			//v = 0
			inline constexpr void setZero() noexcept {
				v = 0;
			}
			//v = Max
			inline constexpr void setNull() noexcept {
				v = Max;
			}
			//return v == Null
			inline constexpr bool isNull() const noexcept {
				return v == Null;
			}
			//return v != Null
			inline constexpr bool notNull() const noexcept {
				return v != Null;
			}
			//return !v
			inline constexpr bool isZero() const noexcept {
				return !v;
			}
			//return v == 0
			inline constexpr bool isMin() const noexcept {
				return v == Min;
			}
			//return v == Max
			inline constexpr bool isMax() const noexcept {
				return v == Max;
			}
			//return v > 0
			inline constexpr bool greaterThanZero() const noexcept {
				return v > 0;
			}
			//return false
			inline constexpr bool lessThanZero() const noexcept {
				return false;
			}

			//return v & high_bit_one<T>
			inline constexpr bool highBitIsOne() const noexcept {
				return v & high_bit_one<T>;
			}
			//return !(v & high_bit_one<T>)
			inline constexpr bool highBitIsZero() const noexcept {
				return !(v & high_bit_one<T>);
			}
			//return v & except_high_bit_one<T>
			inline constexpr T exceptHighBit() const noexcept {
				return v & except_high_bit_one<T>;
			}

			//v = (v & except_high_bit_one<T>) | (static_cast<T>(isOne) << (bit_size<T> - 1))
			inline constexpr void setHighBit(bool isOne) noexcept {
				v = (v & except_high_bit_one<T>) | (static_cast<T>(isOne) << (bit_size<T> -1));
			}
			//v = v | high_bit_one<T>;
			inline constexpr void setHighBitOne() noexcept {
				v = v | high_bit_one<T>;
			}
			//v = v & except_high_bit_one<T>;
			inline constexpr void setHighBitZero() noexcept {
				v = v & except_high_bit_one<T>;
			}
			//v = (v & high_bit_one<T>) | (i & except_high_bit_one<T>)
			inline constexpr void setExceptHighBit(T i) noexcept {
				v = (v & high_bit_one<T>) | (i & except_high_bit_one<T>);
			}
			//return v | high_bit_one<T>;
			inline constexpr T getHighBitOne() const noexcept {
				return v | high_bit_one<T>;
			}
			//return v & except_high_bit_one<T>;
			inline constexpr T getHighBitZero() const noexcept {
				return v & except_high_bit_one<T>;
			}

			inline constexpr operator T() const noexcept {
				return v;
			}
			friend constexpr Hash Hasher(const _unsigned_id_base& a) noexcept {
				return Hasher(a.v);
			}
		};

		template<IsUnsigned T>
		struct _unsigned_id_constexpr_base {
			//0
			static constexpr _unsigned_id_base<T> Min = _unsigned_id_base<T>::Min;
			//std::numeric_limits<T>::max()
			static constexpr _unsigned_id_base<T> Max = _unsigned_id_base<T>::Max;
			//0
			static constexpr _unsigned_id_base<T> Zero = _unsigned_id_base<T>::Zero;
			//1
			static constexpr _unsigned_id_base<T> One = _unsigned_id_base<T>::One;
			//std::numeric_limits<T>::max()
			static constexpr _unsigned_id_base<T> Null = _unsigned_id_base<T>::Null;
		};

		//数据
		template<IsUnsigned T>
		struct _unsigned_id_data_base {
			_unsigned_id_base<T> type; //数据类型
			_unsigned_id_base<T> index; //数据索引

			constexpr _unsigned_id_data_base() noexcept : type(), index() {};
			constexpr _unsigned_id_data_base(_unsigned_id_base<T> type, _unsigned_id_base<T> index) noexcept : type(type), index(index) {}

			inline constexpr void setNull() noexcept {
				type.setNull();
				index.setNull();
			}
			inline constexpr bool isNull() const noexcept {
				return type.isNull() || index.isNull();
			}
			inline constexpr bool notNull() const noexcept {
				return type.notNull() && index.notNull();
			}
			friend constexpr Hash Hasher(const _unsigned_id_data_base& a) noexcept {
				return Hash::Combine(a.type, a.index);
			}
		};
	}

	using scid = typename core::_signed_id_base<schar>;
	using scidc = typename core::_signed_id_constexpr_base<schar>;
	using scidd = typename core::_signed_id_data_base<schar>;

	using ssid = typename core::_signed_id_base<sshort>;
	using ssidc = typename core::_signed_id_constexpr_base<sshort>;
	using ssidd = typename core::_signed_id_data_base<sshort>;

	using sid = typename core::_signed_id_base<sint>;
	using sidc = typename core::_signed_id_constexpr_base<sint>;
	using sidd = typename core::_signed_id_data_base<sint>;

	using slid = typename core::_signed_id_base<slong>;
	using slidc = typename core::_signed_id_constexpr_base<slong>;
	using slidd = typename core::_signed_id_data_base<slong>;


	using ucid = typename core::_unsigned_id_base<uchar>;
	using ucidc = typename core::_unsigned_id_constexpr_base<uchar>;
	using ucidd = typename core::_unsigned_id_data_base<uchar>;

	using usid = typename core::_unsigned_id_base<ushort>;
	using usidc = typename core::_unsigned_id_constexpr_base<ushort>;
	using usidd = typename core::_unsigned_id_data_base<ushort>;

	using uid = typename core::_unsigned_id_base<uint>;
	using uidc = typename core::_unsigned_id_constexpr_base<uint>;
	using uidd = typename core::_unsigned_id_data_base<uint>;

	using ulid = typename core::_unsigned_id_base<ulong>;
	using ulidc = typename core::_unsigned_id_constexpr_base<ulong>;
	using ulidd = typename core::_unsigned_id_data_base<ulong>;

}

namespace std {
	template<dme::IsSigned T>
	struct hash<dme::core::_signed_id_base<T>> {
		inline size_t operator ()(const dme::core::_signed_id_base<T>& a) const noexcept {
			return Hasher(a);
		}
	};
	template<dme::IsSigned T>
	struct hash<dme::core::_signed_id_data_base<T>> {
		inline size_t operator ()(const dme::core::_signed_id_data_base<T>& a) const noexcept {
			return Hasher(a);
		}
	};
	template<dme::IsUnsigned T>
	struct hash<dme::core::_unsigned_id_base<T>> {
		inline size_t operator ()(const dme::core::_unsigned_id_base<T>& a) const noexcept {
			return Hasher(a);
		}
	};
	template<dme::IsUnsigned T>
	struct hash<dme::core::_unsigned_id_data_base<T>> {
		inline size_t operator ()(const dme::core::_unsigned_id_data_base<T>& a) const noexcept {
			return Hasher(a);
		}
	};
}