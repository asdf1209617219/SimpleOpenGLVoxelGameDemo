#pragma once
#include <dme/core/utils.h>
namespace dme {

	//只支持增加不支持删除的数组(支持更新)，如果要删除则需要删除整个数组重建，支持长度不同的对象存入数组
	//只支持可平凡移动的对象（析构时不会调用对象的析构方法）
	class MultiSizeArr {
	private:
		//存放指向对象的指针的数组
		uchar** ptrArr;
		//指针数组当前数量
		uint ptrCount;
		//指针数组最大数量
		uint ptrMax;
		//具体对象数据存放地址开始
		uchar* start;
		//当前存储对象所占字节数
		uint byteCount;
		//可存储最大字节数
		uint byteMax;

		template<typename BaseTypePoint, typename Point>
		constexpr void setIndexAndPtrSingle(void(*setIndexFunc)(BaseTypePoint, uid) noexcept, Point& ptr) noexcept {
			ptr = reinterpret_cast<Point>(start + byteCount);
			ptrArr[ptrCount] = reinterpret_cast<uchar*>(ptr);
			byteCount += toui(TypeSize(ptr));

			setIndexFunc(ptr, ptrCount);
			ptrCount++;
		}

		constexpr void setNull() noexcept {
			ptrArr = null;
			ptrCount = 0;
			ptrMax = 0;
			start = null;
			byteCount = 0;
			byteMax = 0;
		}

	public:
		constexpr MultiSizeArr() noexcept : ptrArr(null), ptrCount(0), ptrMax(0), start(null), byteCount(0), byteMax(0) {}
		MultiSizeArr(uint ptrCount, uint byteCount) noexcept : ptrArr(null), ptrCount(0), ptrMax(ptrCount), start(null), byteCount(0), byteMax(byteCount) {
			void* memory = MallocByte(tost(ptrCount * ptr_size + byteCount));
			ptrArr = reinterpret_cast<uchar**>(memory);
			start = reinterpret_cast<uchar*>(ptrArr + ptrMax);
		}
		MultiSizeArr(const MultiSizeArr& a) noexcept : ptrCount(a.ptrCount), ptrMax(a.ptrMax), byteCount(a.byteCount), byteMax(a.byteMax)
		{
			size_t total = tost(ptrMax * ptr_size + byteMax);
			void* memory = MallocByte(total);
			ptrArr = reinterpret_cast<uchar**>(memory);
			start = reinterpret_cast<uchar*>(ptrArr + ptrMax);
			MemcpyByte(memory, a.ptrArr, total);
		}
		MultiSizeArr(MultiSizeArr&& a) noexcept : ptrArr(a.ptrArr), ptrCount(a.ptrCount), ptrMax(a.ptrMax),
			start(a.start), byteCount(a.byteCount), byteMax(a.byteMax)
		{
			a.setNull();
		}

		~MultiSizeArr() {
			if (ptrArr) {
				Free(ptrArr);
				setNull();
			}
		}

		constexpr void clear() noexcept {
			ptrCount = 0;
			byteCount = 0;
		}

		//该函数必须传入指针，以及一个设置对象内index的函数
		template<typename BaseTypePoint, typename... Points>
		constexpr void setIndexAndPtr(void(*setIndexFunc)(BaseTypePoint, uid) noexcept, Points&... args) {
			if (ptrArr) {
				return;
			}
			uint count = sizeof...(args); //元素数量
			uint size = toui((TypeSize(args) + ...)); //元素所占字节数

			void* memory = MallocByte(tost(count) * ptr_size + size);
			ptrArr = reinterpret_cast<uchar**>(memory);
			ptrMax = count;
			byteMax = size;
			start = reinterpret_cast<uchar*>(ptrArr + ptrMax);

			(setIndexAndPtrSingle(setIndexFunc, args), ...);
		}


		//元素指针和元素下标
		struct MultiSizeArrIndex {
			uchar* ptr; //元素指针
			uid index; //表示数组中第几个元素

			constexpr MultiSizeArrIndex() noexcept : ptr(null), index() {}
			constexpr MultiSizeArrIndex(uchar* ptr, uid index) noexcept : ptr(ptr), index(index) {}
		};
		//返回当前指针数量
		uint getPtrCount() const noexcept {
			return ptrCount;
		}
		//设置最大容量，只允许ptrArr为null时设置
		void setSize(uint count, uint size) noexcept {
			if (ptrArr) {
				return;
			}
			ptrArr = reinterpret_cast<uchar**>(MallocByte(tost(count * ptr_size + size)));
			ptrMax = count;
			byteMax = size;
			start = reinterpret_cast<uchar*>(ptrArr + ptrMax);
		}
		//申请一块空间用于存放对象
		MultiSizeArrIndex apply(uint size) noexcept {
			uint newByteCount = byteCount + size;
			if (byteMax < newByteCount || ptrMax == ptrCount) { //扩容
				uint newByteMax = byteMax * 2;
				while (newByteMax <= newByteCount) {
					newByteMax *= 2;
				}
				uint newPtrMax = ptrMax * 2;

				void* memory = MallocByte(tost(newPtrMax * ptr_size + newByteMax));
				uchar** newPtrArr = reinterpret_cast<uchar**>(memory);
				uchar* newStart = reinterpret_cast<uchar*>(newPtrArr + newPtrMax);
				MemcpyByte(newPtrArr, ptrArr, tost(ptrCount) * ptr_size);
				MemcpyByte(newStart, start, byteCount);
				Free(ptrArr);

				ptrArr = newPtrArr;
				start = newStart;
				ptrMax = newPtrMax;
				byteMax = newByteMax;
			}
			MultiSizeArrIndex r = MultiSizeArrIndex(start + byteCount, ptrCount);
			ptrArr[ptrCount] = r.ptr;
			ptrCount++;
			byteCount = newByteCount;
			return r;
		}
		//申请一块空间用于存放对象，如果发生了扩容，第二个参数会设置为true
		MultiSizeArrIndex apply(uint size, bool& expansion) noexcept {
			uint newByteCount = byteCount + size;
			if (byteMax < newByteCount || ptrMax == ptrCount) { //扩容
				uint newByteMax = byteMax * 2;
				while (newByteMax <= newByteCount) {
					newByteMax *= 2;
				}
				uint newPtrMax = ptrMax * 2;

				void* memory = MallocByte(tost(newPtrMax * ptr_size + newByteMax));
				uchar** newPtrArr = reinterpret_cast<uchar**>(memory);
				uchar* newStart = reinterpret_cast<uchar*>(newPtrArr + newPtrMax);
				MemcpyByte(newPtrArr, ptrArr, tost(ptrCount) * ptr_size);
				MemcpyByte(newStart, start, byteCount);
				Free(ptrArr);

				ptrArr = newPtrArr;
				start = newStart;
				ptrMax = newPtrMax;
				byteMax = newByteMax;
				expansion = true;
			}
			MultiSizeArrIndex r = MultiSizeArrIndex(start + byteCount, ptrCount);
			ptrArr[ptrCount] = r.ptr;
			ptrCount++;
			byteCount = newByteCount;
			return r;
		}
		//获取指针
		uchar* getPtr(uid index) const noexcept {
			DME_ASSERT(index < ptrMax, "subscript exceeds the ptrMax");
			return ptrArr[index];
		}
		//遍历
		constexpr void each(const std::function<void(uchar*)>& func) const noexcept {
			for (uint i = 0; i < ptrCount; i++) {
				func(ptrArr[i]);
			}
		}
	};

}