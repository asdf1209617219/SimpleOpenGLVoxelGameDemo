#pragma once
#include <dme/core/utils.h>
namespace dme {
	//表示数组中的起始下标和结束下标（左闭右开）
	struct ArrSpan {
		uint start; //起始下标
		uint end; //等于起始下标加个数
		constexpr ArrSpan() noexcept : start(0), end(0) {}
		constexpr ArrSpan(uint start, uint end) noexcept : start(start), end(end) {}

		constexpr uint count() const noexcept {
			return end - start;
		}
		constexpr bool isEmpty() const noexcept {
			return start == end;
		}

		constexpr bool operator ==(const ArrSpan& a) const noexcept {
			return start == a.start && end == a.end;
		}
		friend std::ostream& operator<<(std::ostream& os, const ArrSpan& a) {
			return os << "start: " << a.start << ", end: " << a.end;
		}
		friend constexpr Hash Hasher(const ArrSpan& a) noexcept {
			return Hash::Combine(a.start, a.end);
		}
	};

	//用于记录数组中的空闲空间
	class ArrFreeSpan {
	private:
		struct elem {
			uint start;
			union {
				uint end;
				uint count;
			};

			constexpr elem() noexcept : start(0), end(0) {}
			constexpr elem(uint start, uint end) noexcept : start(start), end(end) {}

			constexpr bool countLess(const elem& a) const noexcept {
				return count < a.count || (count == a.count && start < a.start);
			}
			constexpr bool countLess(uint start_, uint count_) const noexcept {
				return count < count_ || (count == count_ && start < start_);
			}
		};
		constexpr static uint default_size = Config::default_init_size * 4;

		elem* _data;
		uint _count;
		uint _size;

		void setNull() noexcept {
			_data = null;
			_count = 0;
			_size = 0;
		}
		void extend() noexcept {
			if (_count == _size) {
				_size *= 2;
				elem* newData = Malloc32<elem>(_size * 2);
				Memcpy32<elem>(newData, _data, _count);
				Memcpy32<elem>(newData + _size, _data + _count, _count);
				Free(_data);
				_data = newData;
			}
		}
		//找不到则返回_size + _count
		uint findCountMin(uint count) noexcept {
			return findCountElem(0, count);
		}
		//找不到则返回_size + _count
		uint findCountElem(uint start, uint count) noexcept {
			uint i1 = _size;
			uint i2 = _size + _count;
			while (i1 < i2) { //二分查找
				uint i = i1 + ((i2 - i1) / 2);
				if (_data[i].countLess(start, count)) {
					i1 = i + 1;
				}
				else {
					i2 = i;
				}
			}
			return i1;
		}
		//相当于查找end等于参数的
		uid findHead(uint start) noexcept {
			uint i1 = 0;
			uint i2 = _count;
			uid index;
			while (i1 < i2) { //二分查找
				uint i = i1 + ((i2 - i1) / 2);
				if (start < _data[i].end) {
					i2 = i;
				}
				else if (_data[i].end < start) {
					i1 = i + 1;
				}
				else {
					index = i;
					break;
				}
			}
			return index;
		}
		//相当于查找start等于参数的
		uid findTail(uint end) noexcept {
			uint i1 = 0;
			uint i2 = _count;
			uid index;
			while (i1 < i2) { //二分查找
				uint i = i1 + ((i2 - i1) / 2);
				if (end < _data[i].start) {
					i2 = i;
				}
				else if (_data[i].start < end) {
					i1 = i + 1;
				}
				else {
					index = i;
					break;
				}
			}
			return index;
		}
		void updateCountElem(uid index, uint start, uint count) noexcept {
			_count--;
			if (index < _size + _count) {
				Memmove32<elem>(_data + index, _data + index + 1, _size + _count - index);
			}
			insertCountElem(start, count);
			_count++;
		}
		void insertCountElem(uint start, uint count) noexcept {
			uint index = findCountElem(start, count);
			if (index < _size + _count) {
				Memmove32<elem>(_data + index + 1, _data + index, _size + _count - index);
			}
			auto& elem = _data[index];
			elem.start = start;
			elem.count = count;
		}
		void insertSpanElem(uint start, uint end) noexcept {
			uint i1 = 0;
			uint i2 = _count;
			while (i1 < i2) { //二分查找
				uint i = i1 + ((i2 - i1) / 2);
				auto& elem = _data[i];
				if (elem.start < start) {
					i1 = i + 1;
				} else {
					i2 = i;
				}
			}
			if (i1 < _count) {
				Memmove32<elem>(_data + i1 + 1, _data + i1, _count - i1);
			}
			auto& elem = _data[i1];
			elem.start = start;
			elem.end = end;
		}
		void addElems(uint start, uint end, uint count) noexcept {
			extend();
			insertCountElem(start, count);
			insertSpanElem(start, end);
			_count++;
		}
		void removeElems(uint spanIndex, uint countIndex) noexcept {
			_count--;
			if (countIndex < _size + _count) {
				Memmove32<elem>(_data + countIndex, _data + countIndex + 1, _size + _count - countIndex);
			}
			if (spanIndex < _count) {
				Memmove32<elem>(_data + spanIndex, _data + spanIndex + 1, _count - spanIndex);
			}
		}
		void _dealloc(uint start, uint end, uint count, uid headIndex, uid tailIndex) noexcept {
			if (headIndex.isNull()) { //头部无空闲空间
				if (tailIndex.isNull()) { //尾部无空闲空间
					addElems(start, end, count);
				}
				else {
					auto& e = _data[tailIndex];
					e.start = start;
					count += e.end - end;
					uint countIndex = findCountElem(end, e.end - end);
					updateCountElem(countIndex, start, count);
				}
			}
			else {
				if (tailIndex.isNull()) { //尾部无空闲空间
					auto& e = _data[headIndex];
					e.end = end;
					count += start - e.start;
					uint countIndex = findCountElem(e.start, start - e.start);
					updateCountElem(countIndex, e.start, count);
				}
				else {
					auto& e = _data[headIndex];
					e.end = _data[tailIndex].end;
					count += start - e.start + e.end - end;
					uint countIndex = findCountElem(e.start, start - e.start);
					removeElems(tailIndex, countIndex);
					countIndex = findCountElem(end, e.end - end);
					updateCountElem(countIndex, e.start, count);
				}
			}
		}

	public:
		explicit ArrFreeSpan(uint size = 0) noexcept : _data(null), _count(0), _size(0) {
			if (size == 0) return;
			_data = Malloc32<elem>(default_size * 2);
			_count = 1;
			_size = default_size;
			_data[0] = elem(0, size);
			_data[default_size] = elem(0, size);
		}
		ArrFreeSpan(const ArrFreeSpan& a) noexcept : _data(null), _count(a._count), _size(a._size) {
			if (_size == 0) return;
			_data = Malloc32<elem>(a._size * 2);
			if (_count) {
				Memcpy32<elem>(_data, a._data, _count);
				Memcpy32<elem>(_data + _size, a._data + _size, _count);
			}
		}
		ArrFreeSpan(ArrFreeSpan&& a) noexcept : _data(a._data), _count(a._count), _size(a._size) {
			a.setNull();
		}
		ArrFreeSpan& operator =(const ArrFreeSpan& a) noexcept {
			if (this != &a) {
				Free(_data);
				_count = a._count;
				_size = a._size;
				if (_size) {
					_data = Malloc32<elem>(a._size * 2);
					if (_count) {
						Memcpy32<elem>(_data, a._data, _count);
						Memcpy32<elem>(_data + _size, a._data + _size, _count);
					}
				}
				else {
					_data = null;
				}
			}
			return *this;
		}
		ArrFreeSpan& operator =(ArrFreeSpan&& a) noexcept {
			if (this != &a) {
				Free(_data);
				_data = a._data;
				_count = a._count;
				_size = a._size;
				a.setNull();
			}
			return *this;
		}
		~ArrFreeSpan(){
			release();
		}

		//申请需要的元素个数，传入对应数组的容量，返回申请的空间位置，如果需要扩容，则会修改传入的参数arrSize
		ArrSpan alloc(uint allocCount, uint& arrSize) noexcept {
			if (allocCount == 0) {
				return ArrSpan();
			}
			if (_size == 0) { //一般来说_size为0时arrSize才有可能为0（即对应的数组未初始化）
				_data = Malloc32<elem>(default_size * 2);
				_count = 1;
				_size = default_size;

				if (arrSize == 0) {
					arrSize = Config::default_init_size;
					while (arrSize < allocCount) {
						arrSize *= 2;
					}
				}
				_data[0] = elem(allocCount, arrSize);
				_data[_size] = elem(allocCount, arrSize - allocCount);
				return ArrSpan(0, allocCount);
			}
			uid countIndex = findCountMin(allocCount);
			if (countIndex == _size + _count) { //未找到足够的空间
				//正常来说这里arrSize不可能为0
				extend();
				if (_count && _data[_count - 1].end == arrSize) { //在数组结尾有空闲空间
					auto& endElem = _data[_count - 1];
					countIndex = findCountElem(endElem.start, endElem.end - endElem.start);
					uint newSpace = endElem.end - endElem.start + arrSize;
					arrSize *= 2;
					while (newSpace <= allocCount) {
						newSpace += arrSize;
						arrSize *= 2;
					}
					ArrSpan r = ArrSpan(endElem.start, endElem.start + allocCount);
					endElem.start = r.end;
					endElem.end = arrSize;
					updateCountElem(countIndex, endElem.start, endElem.end - endElem.start);
					return r;
				}
				else {
					uint newSpace = arrSize;
					ArrSpan r = ArrSpan(newSpace, newSpace + allocCount);
					arrSize *= 2;
					while (newSpace <= allocCount) {
						newSpace += arrSize;
						arrSize *= 2;
					}
					_data[_count] = elem(r.end, arrSize);
					insertCountElem(r.end, arrSize - r.end);
					_count++;
					return r;
				}
			}
			else {
				auto& countElem = _data[countIndex];
				if (countElem.count == allocCount) { //正好用完
					auto r = ArrSpan(countElem.start, countElem.start + countElem.count);
					removeElems(findTail(countElem.start), countIndex);
					return r;
				}
				else {
					auto& endElem = _data[findTail(countElem.start)];
					auto r = ArrSpan(endElem.start, endElem.start + allocCount);
					endElem.start = r.end;
					countElem.start = r.end;
					countElem.count -= allocCount;
					return r;
				}
			}
		}

		//尝试在原地回收或扩展空间，如果后面没有足够的空间，则进行alloc
		ArrSpan realloc(const ArrSpan& arrSpan, uint newCount, uint& arrSize) noexcept {
			return realloc(arrSpan.start, arrSpan.end, newCount, arrSize);
		}
		//尝试在原地回收或扩展空间，如果后面没有足够的空间，则进行alloc
		ArrSpan realloc(uint start, uint end, uint newCount, uint& arrSize) noexcept {
			if (end <= start) {
				return alloc(newCount, arrSize);
			}
			uint oldCount = end - start;
			if (newCount == oldCount) {
				return ArrSpan(start, end);
			}
			if (newCount < oldCount) {
				uid tailIndex = findTail(end);
				if (tailIndex.isNull()) {
					uint count = oldCount - newCount;
					end = start + newCount;
					addElems(end, end + count, count);
				}
				else {
					end = start + newCount;
					auto& e = _data[tailIndex];
					uint countIndex = findCountElem(e.start, e.end - e.start);
					e.start = end;
					updateCountElem(countIndex, end, e.end - end);
				}
				return ArrSpan(start, end);
			}
			uid tailIndex = findTail(end);
			if (tailIndex.notNull()) {
				auto& e = _data[tailIndex];
				uint count = e.end - e.start + oldCount;
				if (newCount < count) {
					end = start + newCount;
					uint countIndex = findCountElem(e.start, e.end - e.start);
					e.start = end;
					updateCountElem(countIndex, end, count - newCount);
					return ArrSpan(start, end);
				}
				else if (newCount == count) {
					end = start + newCount;
					uint countIndex = findCountElem(e.start, e.end - e.start);
					removeElems(tailIndex, countIndex);
					return ArrSpan(start, end);
				}
			}
			_dealloc(start, end, oldCount, findHead(start), tailIndex);
			return alloc(newCount, arrSize);
		}

		//回收申请的空间，不要在ArrFreeSpan未初始化时调用，传入的参数必须与未使用空间是不相交的（左闭右开）且start小于end
		void dealloc(const ArrSpan& arrSpan) noexcept {
			return dealloc(arrSpan.start, arrSpan.end);
		}
		//回收申请的空间，不要在ArrFreeSpan未初始化时调用，传入的参数必须与未使用空间是不相交的（左闭右开）且start小于end
		void dealloc(uint start, uint end) noexcept {
			//DME_ASSERT(_size, "called on uninitialized object");
			//DME_ASSERT(start < end, "start cannot be greater than or equal to end");
			if (end <= start) {
				return;
			}
			_dealloc(start, end, end - start, findHead(start), findTail(end));
		}

		//清除所有元素
		constexpr void clear() noexcept {
			_count = 0;
		}
		//删除所有元素并回收内存
		void release() noexcept {
			if (_size) {
				Free(_data);
				setNull();
			}
		}

		void testPrint() const noexcept {
			std::cout << "count: " << _count << ", size: " << _size << ", spans: \n";
			for (uint i = 0; i < _count; ++i) {
				auto& e = _data[i];
				std::cout << "{ start: " << e.start << ", end: " << e.end << " }\n";
			}
			std::cout << "; counts: \n";
			for (uint i = _size; i < _size + _count; ++i) {
				auto& e = _data[i];
				std::cout << "{ start: " << e.start << ", count: " << e.count << " }\n";
			}
		}
	};
}