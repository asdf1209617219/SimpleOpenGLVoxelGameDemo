#pragma once
#include <dme/core/macro.h>

#include <algorithm>
#include <array>
#include <atomic>

#include <bit>
#include <bitset>

#include <cerrno>
#include <cfloat>
#include <chrono>
#include <climits>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include <exception>

#include <format>
#include <fstream>
#include <functional>

#include <iostream>

#include <limits>

#include <map>
#include <memory>
#include <memory_resource>
#include <mutex>

#include <numbers>

#include <optional>

#include <print>

#include <random>

#include <variant>

#include <set>
#include <shared_mutex>
#include <span>
#include <sstream>
#include <string>

#include <thread>
#include <type_traits>

#include <unordered_map>
#include <unordered_set>
#include <utility>

#include <vector>

namespace dme {
	typedef int8_t   schar;
	typedef int16_t  sshort;
	typedef int32_t  sint;
	typedef int64_t  slong;
	typedef uint8_t  uchar;
	typedef uint16_t ushort;
	typedef uint32_t uint;
	typedef uint64_t ulong;

	constexpr std::nullptr_t null = nullptr;
	constexpr uint ptr_size = sizeof(void*);
	constexpr uint byte_bit = CHAR_BIT;

	constexpr uint float_size = sizeof(float);
	constexpr uint float_bit = float_size * byte_bit;
	constexpr uint double_size = sizeof(double);
	constexpr uint double_bit = double_size * byte_bit;

	constexpr uint char_size = sizeof(schar);
	constexpr uint char_bit = char_size * byte_bit;
	constexpr uchar char_high_bit_mask = 1 << (char_bit - 1);
	constexpr uchar char_high_bit_mask_inverse = ~char_high_bit_mask;

	constexpr uint short_size = sizeof(sshort);
	constexpr uint short_bit = short_size * byte_bit;
	constexpr ushort short_high_bit_mask = 1 << (short_bit - 1);
	constexpr ushort short_high_bit_mask_inverse = ~short_high_bit_mask;

	constexpr uint int_size = sizeof(sint);
	constexpr uint int_bit = int_size * byte_bit;
	constexpr uint int_high_bit_mask = 1 << (int_bit - 1);
	constexpr uint int_high_bit_mask_inverse = ~int_high_bit_mask;

	constexpr uint long_size = sizeof(slong);
	constexpr uint long_bit = long_size * byte_bit;
	constexpr ulong long_high_bit_mask = static_cast<ulong>(1) << (long_bit - 1);
	constexpr ulong long_high_bit_mask_inverse = ~long_high_bit_mask;

	constexpr uint size_t_size = sizeof(size_t);
	constexpr uint size_t_bit = size_t_size * byte_bit;
	constexpr size_t size_t_high_bit_mask = static_cast<size_t>(1) << (size_t_bit - 1);
	constexpr size_t size_t_high_bit_mask_inverse = ~size_t_high_bit_mask;

	constexpr schar schar_min = std::numeric_limits<schar>::min();
	constexpr schar schar_max = std::numeric_limits<schar>::max();
	constexpr uchar uchar_min = std::numeric_limits<uchar>::min();
	constexpr uchar uchar_max = std::numeric_limits<uchar>::max();
	constexpr sshort sshort_min = std::numeric_limits<sshort>::min();
	constexpr sshort sshort_max = std::numeric_limits<sshort>::max();
	constexpr ushort ushort_min = std::numeric_limits<ushort>::min();
	constexpr ushort ushort_max = std::numeric_limits<ushort>::max();
	constexpr sint sint_min = std::numeric_limits<sint>::min();
	constexpr sint sint_max = std::numeric_limits<sint>::max();
	constexpr uint uint_min = std::numeric_limits<uint>::min();
	constexpr uint uint_max = std::numeric_limits<uint>::max();
	constexpr slong slong_min = std::numeric_limits<slong>::min();
	constexpr slong slong_max = std::numeric_limits<slong>::max();
	constexpr ulong ulong_min = std::numeric_limits<ulong>::min();
	constexpr ulong ulong_max = std::numeric_limits<ulong>::max();
	constexpr size_t size_t_min = std::numeric_limits<size_t>::min();
	constexpr size_t size_t_max = std::numeric_limits<size_t>::max();

	constexpr float float_min = std::numeric_limits<float>::min();
	constexpr float float_max = std::numeric_limits<float>::max();
	constexpr float float_pinf = std::numeric_limits<float>::infinity();
	constexpr float float_ninf = -std::numeric_limits<float>::infinity();
	constexpr float float_epsilon = std::numeric_limits<float>::epsilon();
	constexpr float float_nan = std::numeric_limits<float>::quiet_NaN();

	constexpr double double_min = std::numeric_limits<double>::min();
	constexpr double double_max = std::numeric_limits<double>::max();
	constexpr double double_pinf = std::numeric_limits<double>::infinity();
	constexpr double double_ninf = -std::numeric_limits<double>::infinity();
	constexpr double double_epsilon = std::numeric_limits<double>::epsilon();
	constexpr double double_nan = std::numeric_limits<double>::quiet_NaN();

	//设定各种基础类型的类型ID
	struct BaseTypeId {
		static constexpr uint Void = 0;
		static constexpr uint Bool = 1;
		static constexpr uint Char = 2;
		static constexpr uint Schar = 3;
		static constexpr uint Uchar = 4;
		static constexpr uint Sshort = 5;
		static constexpr uint Ushort = 6;
		static constexpr uint Sint = 7;
		static constexpr uint Uint = 8;
		static constexpr uint Slong = 9;
		static constexpr uint Ulong = 10;
		static constexpr uint Float = 11;
		static constexpr uint Double = 12;
	};

	template<typename T>
	constexpr uint base_type_id = BaseTypeId::Void;

	template<>
	constexpr uint base_type_id<void> = BaseTypeId::Void;
	template<>
	constexpr uint base_type_id<bool> = BaseTypeId::Bool;
	template<>
	constexpr uint base_type_id<char> = BaseTypeId::Char;
	template<>
	constexpr uint base_type_id<schar> = BaseTypeId::Schar;
	template<>
	constexpr uint base_type_id<uchar> = BaseTypeId::Uchar;
	template<>
	constexpr uint base_type_id<sshort> = BaseTypeId::Sshort;
	template<>
	constexpr uint base_type_id<ushort> = BaseTypeId::Ushort;
	template<>
	constexpr uint base_type_id<sint> = BaseTypeId::Sint;
	template<>
	constexpr uint base_type_id<uint> = BaseTypeId::Uint;
	template<>
	constexpr uint base_type_id<slong> = BaseTypeId::Slong;
	template<>
	constexpr uint base_type_id<ulong> = BaseTypeId::Ulong;
	template<>
	constexpr uint base_type_id<float> = BaseTypeId::Float;
	template<>
	constexpr uint base_type_id<double> = BaseTypeId::Double;

	//转为schar类型
	template<typename T>
	inline constexpr schar toc(T a) noexcept {
		return static_cast<schar>(a);
	}
	//转为uchar类型
	template<typename T>
	inline constexpr uchar touc(T a) noexcept {
		return static_cast<uchar>(a);
	}
	//转为sshort类型
	template<typename T>
	inline constexpr sshort tos(T a) noexcept {
		return static_cast<sshort>(a);
	}
	//转为ushort类型
	template<typename T>
	inline constexpr ushort tous(T a) noexcept {
		return static_cast<ushort>(a);
	}
	//转为sint类型
	template<typename T>
	inline constexpr sint toi(T a) noexcept {
		return static_cast<sint>(a);
	}
	//转为uint类型
	template<typename T>
	inline constexpr uint toui(T a) noexcept {
		return static_cast<uint>(a);
	}
	//转为slong类型
	template<typename T>
	inline constexpr slong tol(T a) noexcept {
		return static_cast<slong>(a);
	}
	//转为ulong类型
	template<typename T>
	inline constexpr ulong toul(T a) noexcept {
		return static_cast<ulong>(a);
	}
	//转为float类型
	template<typename T>
	inline constexpr float tof(T a) noexcept {
		return static_cast<float>(a);
	}
	//转为double类型
	template<typename T>
	inline constexpr double tod(T a) noexcept {
		return static_cast<double>(a);
	}
	//转为size_t类型
	template<typename T>
	inline constexpr size_t tost(T a) noexcept {
		return static_cast<size_t>(a);
	}

	/*
	//指针转换
	template<typename T>
	inline constexpr T* CastPtr(void* a) noexcept {
		return reinterpret_cast<T*>(a);
	}
	//引用转换
	template<typename T, typename R>
	inline constexpr T& CastRef(R& a) noexcept {
		return reinterpret_cast<T&>(a);
	}
	*/


	using stdstr = ::std::string;
}