#pragma once
#include <dme/core/utils.h>
namespace dme{
//可动态扩展大小的bit数组，通过索引访问对应bit位数的值是否为1
class BitArr {
private:
	using type = uchar;
	static constexpr uint _type_bit = sizeof(type) * byte_bit;
	static constexpr type _mask = high_bit_one<type>;

	type* array; //数据
	uint bitSize; //总bit数
public:
	BitArr() noexcept : array(null), bitSize(0) {}
	explicit BitArr(uint bitCount) noexcept : array(null), bitSize(bitCount) {
		if (bitSize < _type_bit) {
			bitSize = _type_bit;
		}
		bitCount = bitSize % _type_bit;
		if (bitCount) {
			bitSize += _type_bit - bitCount;
		}
		array = Malloc<type>(bitSize / _type_bit);
		Memset<type>(array, 0, bitSize / _type_bit);
	}
	BitArr(BitArr&& a) noexcept : array(std::move(a.array)), bitSize(std::move(a.bitSize)) {
		a.array = null;
		a.bitSize = 0;
	}
	BitArr& operator =(BitArr&& a) noexcept {
		if (this != &a) {
			array = std::move(a.array);
			bitSize = std::move(a.bitSize);

			a.array = null;
			a.bitSize = 0;
		}
		return *this;
	}
	~BitArr() {
		if (!array) return;
		Free(array);
		array = null;
		bitSize = 0;
	}

	//获取大小
	uint size() const noexcept {
		return bitSize;
	}
	//获取对应位置的bit值是否是1
	bool get(uint index) const noexcept {
		DME_ASSERT(index < bitSize, "subscript exceeds the bitSize");
		uint d = index / _type_bit;
		uint r = index % _type_bit;
		return array[d] & (_mask >> r);
	}
	//设置对应位置的bit值
	void set(uint index, bool isOne) const noexcept {
		DME_ASSERT(index < bitSize, "subscript exceeds the bitSize");
		uint d = index / _type_bit;
		uint r = index % _type_bit;
		type& data = array[d];
		if (isOne) {
			data |= _mask >> r;
		}
		else {
			data &= ~(_mask >> r);
		}
	}
	//设置对应位置的bit值为1
	void setOne(uint index) const noexcept {
		DME_ASSERT(index < bitSize, "subscript exceeds the bitSize");
		uint d = index / _type_bit;
		uint r = index % _type_bit;
		type& data = array[d];
		data |= _mask >> r;
	}
	//设置对应位置的bit值为0
	void setZero(uint index) const noexcept {
		DME_ASSERT(index < bitSize, "subscript exceeds the bitSize");
		uint d = index / _type_bit;
		uint r = index % _type_bit;
		type& data = array[d];
		data &= ~(_mask >> r);
	}
	//扩展容量为当前容量的两倍
	void extend() noexcept {
		if (array) {
			uint count = bitSize / _type_bit;
			type* newArr = Malloc<type>(count * 2);
			Memcpy<type>(newArr, array, count);
			Memset<type>(newArr + count, 0, count);
			bitSize *= 2;
			Free(newArr);
		}
		else {
			bitSize = _type_bit;
			uint count = bitSize / _type_bit;
			array = Malloc<type>(count);
			Memset<type>(array, 0, count);
		}
	}

	constexpr bool operator ==(const BitArr& a) const noexcept {
		return array == a.array && bitSize == a.bitSize;
	}
	friend constexpr Hash Hasher(const BitArr& a) noexcept {
		return Hash::Combine(a.array, a.bitSize);
	}
};
}

namespace std {
	template<>
	struct hash<dme::BitArr> {
		size_t operator ()(const dme::BitArr& a) const noexcept {
			return Hasher(a);
		}
	};
}