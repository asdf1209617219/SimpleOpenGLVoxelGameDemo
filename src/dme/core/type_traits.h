#pragma once
#include <dme/core/base.h>
namespace dme {

	//移除类型中的指针、引用、顶级const和volatile限定符，返回原始类型
	template <typename T>
	using remove_prcv = typename std::remove_pointer_t<std::remove_reference_t<std::remove_cv_t<T>>>;

	//移除类型中的引用、顶级const和volatile限定符
	template <typename T>
	using remove_rcv = typename std::remove_reference_t<std::remove_cv_t<T>>;

	//第一个类型是后面类型中任意类型时返回true
	template <typename T, typename... Ts>
	constexpr bool is_any_of = (std::is_same_v<T, Ts> || ...);

	//是否枚举类
	template <typename T>
	constexpr bool is_enum = std::is_enum_v<std::remove_cv_t<T>>;
	//是否无符号整数
	template <typename T>
	constexpr bool is_unsigned = is_any_of<std::remove_cv_t<T>, uchar, ushort, uint, ulong>;
	//不是无符号整数
	template <typename T>
	constexpr bool not_unsigned = !is_unsigned<T>;
	//是否有符号整数
	template <typename T>
	constexpr bool is_signed = is_any_of<std::remove_cv_t<T>, schar, sshort, sint, slong>;
	//不是有符号整数
	template <typename T>
	constexpr bool not_signed = !is_signed<T>;
	//是否整数
	template <typename T>
	constexpr bool is_integer = is_any_of<std::remove_cv_t<T>, schar, uchar, sshort, ushort, sint, uint, slong, ulong>;
	//是否浮点数
	template <typename T>
	constexpr bool is_float = is_any_of<std::remove_cv_t<T>, float, double>;
	//是否布尔类型
	template <typename T>
	constexpr bool is_bool = std::is_same_v<std::remove_cv_t<T>, bool>;
	//是否int8的类型
	template <typename T>
	constexpr bool is_int8 = is_any_of<std::remove_cv_t<T>, schar, uchar>;
	//是否int16的类型
	template <typename T>
	constexpr bool is_int16 = is_any_of<std::remove_cv_t<T>, sshort, ushort>;
	//是否指针类型
	template <typename T>
	constexpr bool is_pointer = std::is_pointer_v<T>;
	//是否为基本数字类型（包括bool）
	template <typename T>
	constexpr bool is_base_number = is_any_of<std::remove_cv_t<T>, bool, schar, uchar, sshort, ushort, sint, uint, slong, ulong, float, double>;
	//是否可以安全使用负号
	template <typename T>
	constexpr bool is_safe_negative_sign = is_any_of<std::remove_cv_t<T>, schar, sshort, sint, slong, float, double>;

	//T是否为Template模板的实例
	template <typename T, template <typename...> class Template>
	constexpr bool is_instance_of_template = false;

	//T是否为Template模板的实例
	template <template <typename...> class Template, typename... Args>
	constexpr bool is_instance_of_template<Template<Args...>, Template> = true;

	//返回两个类型中更大的类型，如果两个类型大小相同则返回第一个类型
	template <typename A, typename B>
	using get_bigger_type = typename std::conditional_t<sizeof(A) < sizeof(B), B, A> ;
	//返回浮点数类型相同大小对应的整数类型
	template <typename T>
	using get_integer_type = typename std::conditional_t<sizeof(T) >= sizeof(slong), slong, sint>;
	//返回整数类型相同大小对应的浮点数类型
	template <typename T>
	using get_float_type = typename std::conditional_t<sizeof(T) >= sizeof(double), double, float>;
	//获取有符号类型
	template <typename T>
	using get_signed_type = std::make_signed_t<T>;
	//获取无符号类型
	template <typename T>
	using get_unsigned_type = std::make_unsigned_t<T>;

	//返回两个类型之间进行算术运算的结果类型
	template <typename A, typename B>
	using arithmetic_return_type = decltype(A(0) + B(0));


	//

	//第一个类型Base是否是第二个类型T的基类
	template <typename Base, typename T>
	concept IsBaseOf = std::is_base_of_v<Base, T>;
	//是枚举类
	template <typename T>
	concept IsEnum = is_enum<T>;
	//是无符号整数
	template <typename T>
	concept IsUnsigned = is_unsigned<T>;
	//不是无符号整数
	template <typename T>
	concept NotUnsigned = not_unsigned<T>;
	//是有符号整数
	template <typename T>
	concept IsSigned = is_signed<T>;
	//不是有符号整数
	template <typename T>
	concept NotSigned = not_signed<T>;
	//是整数
	template <typename T>
	concept IsInteger = is_integer<T>;
	//是int8类型
	template <typename T>
	concept IsInt8 = is_int8<T>;
	//是int16类型
	template <typename T>
	concept IsInt16 = is_int16<T>;
	//是指针类型
	template <typename T>
	concept IsPointer = is_pointer<T>;
	//是布尔类型
	template <typename T>
	concept IsBool = is_bool<T>;
	//是浮点数
	template <typename T>
	concept IsFloat = is_float<T>;
	//是基本数字类型（包括bool）
	template <typename T>
	concept IsBaseNumber = is_base_number<T>;
	//可以安全使用负号
	template <typename T>
	concept IsSafeNegativeSign = is_safe_negative_sign<T>;
	//不可以安全使用负号
	template <typename T>
	concept NotSafeNegativeSign = !is_safe_negative_sign<T>;
	//能向size_t赋值
	template <typename T>
	concept IsBaseNumberAssignment = requires(T a, size_t b) {
		{ b = a };
	};
	//可计算
	template <typename T>
	concept IsArithmetic = std::is_arithmetic_v<T>;
	//可简单进行hash
	template <typename T>
	concept IsEnableSimpleHash = std::is_enum_v<T> || std::is_integral_v<T> || std::is_floating_point_v<T> || std::is_pointer_v<T>;

	//Func为可调用类型且返回类型为Ret参数类型为Args
	template<typename Func, typename Ret, typename... Args>
	concept IsInvocableRet = requires(Func&& f, Args&&... args) {
		{ f(std::forward<Args>(args)...) } -> std::same_as<Ret>;
	};

	//支持 std::ostream << T
	template<typename T>
	concept IsCanStreamOut = requires(std::ostream & os, const T & obj) {
		{ os << obj } -> std::same_as<std::ostream&>;
	};
	//支持 T1 < T2
	template<typename T1, typename T2 = T1>
	concept IsCanLess = requires(const T1 & a, const T2 & b) {
		{ a < b } -> std::same_as<bool>;
	};
	//支持 T1 == T2
	template<typename T1, typename T2 = T1>
	concept IsCanEqual = requires(const T1 & a, const T2 & b) {
		{ a == b } -> std::same_as<bool>;
	};

	struct Hash;
	template<typename T>
	constexpr Hash Hasher(const T& a) noexcept;
	//支持 Hasher
	template<typename T>
	concept IsCanHash = requires(const T& a) {
		{ Hasher(a) } -> std::same_as<Hash>;
	};

	//Param_是T或可隐式转换为T的类型
	template<typename Param_, typename T>
	concept IsSameOrConvertible = std::is_same_v<Param_, T> || std::is_convertible_v<Param_, T>;

	//返回类型所占bit数
	template <typename T>
	constexpr uint bit_size = sizeof(T) * byte_bit;

	//返回该类型所有bit均为1的数
	template <typename T>
	constexpr T bit_max = ~T(0);

	//返回该类型最高位bit为1的数
	template <typename T>
	constexpr T high_bit_one = static_cast<std::make_unsigned<T>::type>(1) << (bit_size<T> -1);

	//返回该类型除了最高位bit均为1的数
	template <typename T>
	constexpr T except_high_bit_one = ~high_bit_one<T>;

	//会将指针和引用转换为原始类型并返回类型大小
	template<typename T>
	constexpr size_t type_size = sizeof(remove_prcv<T>);

	//会将指针和引用转换为原始类型并返回类型大小
	template<typename T>
	inline constexpr size_t TypeSize(const T& obj) noexcept {
		return type_size<T>;
	}

	//获取枚举类的值
	template<IsEnum T> 
	inline constexpr auto EnumValue(T e) noexcept {
		return static_cast<std::underlying_type_t<T>>(e);
	}


	//检查类型中是否有一个名为IsRelocatable的static constexpr bool的成员并且值为true（该方式会被继承，目前采用模板特化）
	//template<typename T>
	//concept ConstexprTrue_IsRelocatable = requires {
	//	requires bool(T::IsRelocatable) == true;
	//	requires std::is_same_v<decltype(T::IsRelocatable), const bool>;
	//};

	namespace core {
		template <class T, class = void>
		struct _Is_implicitly_default_constructible : std::false_type {
		};

		template <class T>
		void _Implicitly_default_construct(const T&) {};

		template <class T>
		struct _Is_implicitly_default_constructible < T, std::void_t<decltype(_Implicitly_default_construct<T>({})) >> : std::true_type {
		};
	}

	//T的默认构造函数是否为显式
	template<typename T>
	constexpr bool explicit_default_construct = !core::_Is_implicitly_default_constructible<T>::value;

	//如果T是trivially relocatable类型，则可以直接使用Memcpy进行扩容时的元素复制，否则需要调用每个元素的移动、复制构造函数
	//用户通过特化该模板并将值设为true来指定某个类型为trivially relocatable类型，默认值为std::is_trivially_copyable_v<T>
	template<typename T>
	constexpr bool is_relocatable = std::is_trivially_copyable_v<T>;

	//默认构造函数是否不会抛出异常
	template<typename T>
	constexpr bool nothrow_default_construct = std::is_nothrow_default_constructible_v<T>;
	//拷贝构造函数是否不会抛出异常
	template<typename T>
	constexpr bool nothrow_copy_construct = std::is_nothrow_copy_constructible_v<T>;
	//拷贝赋值函数是否不会抛出异常
	template<typename T>
	constexpr bool nothrow_copy_assign = std::is_nothrow_copy_assignable_v<T>;
	//移动构造函数是否不会抛出异常
	template<typename T>
	constexpr bool nothrow_move_construct = std::is_nothrow_move_constructible_v<T>;
	//移动赋值函数是否不会抛出异常
	template<typename T>
	constexpr bool nothrow_move_assign = std::is_nothrow_move_assignable_v<T>;
	//析构函数是否不会抛出异常
	template<typename T>
	constexpr bool nothrow_destruct = std::is_nothrow_destructible_v<T>;

	//构造函数是否不会抛出异常
	template<typename T, typename... Args>
	constexpr bool nothrow_construct = std::is_nothrow_constructible_v<T, Args...>;
	//赋值函数是否不会抛出异常
	template<typename From_, typename To_>
	constexpr bool nothrow_assign = std::is_nothrow_assignable_v<From_, To_>;


	//不可构造，用于工具类
	struct Unconstructible {
		Unconstructible() = delete;
		Unconstructible(const Unconstructible&) = delete;
		Unconstructible(Unconstructible&&) = delete;
		Unconstructible& operator=(const Unconstructible&) = delete;
		Unconstructible& operator=(Unconstructible&&) = delete;
	};
	//单例模板
	template<typename T>
	class Singleton {
	protected:
		Singleton() = default; // 保护构造函数
		Singleton(const Singleton&) = delete; // 禁止拷贝构造
		Singleton(Singleton&&) = delete; // 禁止移动构造
		Singleton& operator=(const Singleton&) = delete; // 禁止拷贝赋值
		Singleton& operator=(Singleton&&) = delete; // 禁止移动赋值
	public:
		//获取单例
		static T& GetInstance() noexcept {
			static T instance;
			return instance;
		}
	};

}