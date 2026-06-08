#pragma once
#include <dme/core/utils/mem/obj_pool_mem.h>
#include <dme/core/utils/pair.h>
namespace dme::core {
	template<typename M, bool IsPair, typename K, typename V, bool StableK, bool StableV>
	struct _flat_hash_mem_copy_and_swap;

	template<typename M, bool IsPair, typename K, typename V>
	struct _flat_hash_mem_copy_and_swap<M, IsPair, K, V, false, false> {
		using mem_type = M;
		using key_type = K;
		using val_type = V;
		using pair_type = Pair<K, V>;
		using slot_type = mem_type::slot_type;

		static constexpr ulong default_size = mem_type::default_size;

		using group_type = mem_type::group_type;
		static constexpr uint group_byte = mem_type::group_byte;
		static constexpr uint group_bit = mem_type::group_bit;

		static constexpr uint h1_shift = mem_type::h1_shift;
		static constexpr ulong h2_mask = mem_type::h2_mask;

		static constexpr uchar flag_empty = mem_type::flag_empty;
		static constexpr uchar flag_delete = mem_type::flag_delete;
		static constexpr uchar flag_sentinel = mem_type::flag_sentinel;

		static constexpr ulong check_mask = mem_type::check_mask;
		static constexpr ulong copy_8bytes = mem_type::copy_8bytes;

		uchar* ctrls = null;
		slot_type* slots = null;
		ulong size = 0;

		//拷贝成功则清空原内存并将新的内存赋值给mem，拷贝过程中触发异常则原对象无影响
		constexpr void copyAndSwap(const ulong fromSize, const uchar* fromCtrls, const slot_type* fromSlots, const ulong newSize, mem_type& mem, const bool isRVal) noexcept(mem_type::noexcept_copy) {
			size = _get_next_power_of_two_size<default_size>(newSize);
			Pair<uchar*, slot_type*> p = mem_type::CreateCtrls(size);
			ctrls = p.key;
			slots = p.val;
			ulong newCount = 0;
			for (ulong i = 0; i < fromSize; i += group_byte) {
				group_type group = *reinterpret_cast<const group_type*>(fromCtrls + i);
				group = ~group & check_mask; //查找字节最高位是0的
				while (true) {
					uint c = std::countr_zero(group);
					if (c < group_bit) {
						ulong index = i + (c / group_byte);
						copyElem(fromSlots + index, isRVal); //拷贝之后清除原对象，除非拷贝不是noexcept的，则延迟到拷贝完成再清除
						++newCount;
						group &= group - 1; //消除最右边的1
					}
					else {
						break;
					}
				}
			}
			if constexpr (!mem_type::noexcept_move && !std::is_trivially_destructible_v<slot_type>) { //该分支完全拷贝完成后再清空原内存
				if (isRVal) {
					mem.destoryAllElem();
				}
			}

			Free(mem.ctrls);

			mem.ctrls = ctrls;
			mem.slots = slots;
			mem.count = newCount;
			mem.size = size;
			ctrls = null;
		}


		//拷贝成功则清空原内存并将新的内存赋值给mem，拷贝过程中触发异常则原对象无影响
		constexpr void copyAndSwap(const ulong newSize, const slot_type* fromSlots, mem_type& mem) noexcept(mem_type::noexcept_move) {
			size = _get_next_power_of_two_size<default_size>(newSize);
			Pair<uchar*, slot_type*> p = mem_type::CreateCtrls(size);
			ctrls = p.key;
			slots = p.val;
			for (ulong i = 0; i < newSize; ++i) {
				copyElem(fromSlots + i, true);
			}
			if constexpr (!mem_type::noexcept_move && !std::is_trivially_destructible_v<slot_type>) { //该分支完全拷贝完成后再清空原内存
				for (ulong i = 0; i < newSize; ++i) {
					fromSlots[i].~slot_type();
				}
			}
			mem.destoryAllElem();

			Free(mem.ctrls);

			mem.ctrls = ctrls;
			mem.slots = slots;
			mem.count = newSize;
			mem.size = size;
			ctrls = null;
		}

		//查找空位并拷贝进去
		constexpr void copyElem(const slot_type* old, const bool isRVal) noexcept(mem_type::noexcept_move) {
			Hash hash = GetElemHash(*old);
			ulong h1 = hash.v >> h1_shift;
			ulong h2 = hash.v & h2_mask;
			ulong index = h1 % size;
			while (true) {
				group_type group = *reinterpret_cast<group_type*>(ctrls + index);
				group = group & check_mask; //查找empty
				uint c = std::countr_zero(group);
				if (c < group_bit) {
					index = (index + (c / group_byte)) % size;
					goto start_copy;
				}
				index = (index + group_byte) % size;
			}
		start_copy:
			if constexpr (mem_type::noexcept_move) {
				if constexpr (is_relocatable<slot_type>) {
					if constexpr (sizeof(slot_type) <= Config::memory_alignment && std::is_trivially_copyable_v<slot_type>) {
						new (slots + index) slot_type(*old);
					}
					else {
						MemcpyByte(slots + index, old, sizeof(slot_type));
					}
				}
				else {
					if constexpr (nothrow_move_construct<slot_type>) {
						if (isRVal) {
							new (slots + index) slot_type(std::move(*old));
							old->~slot_type();
						}
						else {
							new (slots + index) slot_type(*old);
						}
					}
					else {
						new (slots + index) slot_type(*old);
						if (isRVal) {
							old->~slot_type();
						}
					}
				}
			}
			else {
				new (slots + index) slot_type(*old);
			}
			ctrls[index] = static_cast<uchar>(h2);
			if (index < group_byte - 1) {
				ctrls[size + index] = static_cast<uchar>(h2);
			}
		}

		constexpr Hash GetElemHash(const slot_type& slot) noexcept {
			if constexpr (IsPair) {
				return Hasher(slot.key);
			}
			else {
				return Hasher(slot);
			}
		}
		//验证key是否相等
		constexpr bool CheckKeyEqual(const slot_type& slot1, const slot_type& slot2) noexcept {
			if constexpr (IsPair) {
				return slot1.key == slot2.key;
			}
			else {
				return slot1 == slot2;
			}
		}

		constexpr ~_flat_hash_mem_copy_and_swap() {
			if (ctrls) [[unlikely]] { //拷贝时触发异常的分支
				//析构success_count个元素，如果元素的类型具有trivially destructible特性，则跳过析构
				if constexpr (!std::is_trivially_destructible_v<slot_type>) {
					for (ulong i = 0; i < size; i += group_byte) {
						group_type group = *reinterpret_cast<group_type*>(ctrls + i);
						group = ~group & check_mask; //查找字节最高位是0的
						while (true) {
							uint c = std::countr_zero(group);
							if (c < group_bit) {
								ulong index = i + (c / group_byte);
								slots[index].~slot_type();
								group &= group - 1; //消除最右边的1
							}
							else {
								break;
							}
						}
					}
				}
				Free(ctrls);
				ctrls = null;
			}
		}
	};

	template<bool IsPair, typename K, typename V, bool StableK, bool StableV>
	struct _flat_hash_mem;

	template<bool IsPair, typename K, typename V>
	struct _flat_hash_mem<IsPair, K, V, false, false> {
		static constexpr bool IsFlatSet = !IsPair;
		static constexpr bool IsFlatMap = IsPair;

		using group_type = ulong;
		static constexpr uint group_byte = sizeof(group_type);
		static constexpr uint group_bit = group_byte * char_bit;

		using key_type = K;
		using val_type = V;
		using pair_type = Pair<K, V>;
		using slot_type = std::conditional_t<IsPair, pair_type, K>;
		using value_type = slot_type;
		static constexpr ulong slot_size = sizeof(slot_type);
		using mem_type = _flat_hash_mem<IsPair, K, V, false, false>;
		using copy_util = _flat_hash_mem_copy_and_swap<mem_type, IsPair, K, V, false, false>;

		using size_type = ulong;
		using difference_type = std::make_signed_t<size_type>;

		static constexpr uint h1_shift = 7;
		static constexpr ulong h2_mask = 0b01111111;

		//static constexpr uchar flag_full =	0b0hhhhhhh;
		static constexpr uchar flag_empty =		0b10000000;
		static constexpr uchar flag_delete =	0b11111110;
		static constexpr uchar flag_sentinel =	0b11111111;

		static constexpr ulong check_mask =		0x8080808080808080Ui64;
		static constexpr ulong copy_8bytes =	0x0101010101010101Ui64;

		static constexpr bool noexcept_copy = is_relocatable<slot_type> || nothrow_copy_construct<slot_type>;
		static constexpr bool noexcept_move = is_relocatable<slot_type> || nothrow_move_construct<slot_type> || nothrow_copy_construct<slot_type>;

		template<typename... Args>
		static constexpr bool get_noexcept_slot() {
			return noexcept_move && nothrow_construct<slot_type, Args...>;
		}
		template<typename... Args>
		static constexpr bool get_noexcept_slot_val() {
			return noexcept_move && nothrow_construct<val_type, Args...>;
		}

		static constexpr ulong default_size = 8 * Config::default_init_size;

		uchar* ctrls;
		slot_type* slots;
		ulong count; //包括delete
		ulong size;

		constexpr _flat_hash_mem() noexcept : ctrls(null), slots(null), count(0), size(0) {}
		constexpr _flat_hash_mem(ulong size_) noexcept : ctrls(null), slots(null), count(0), size(_get_next_power_of_two_size<default_size>(size_)) {
			init(size);
		}
		constexpr _flat_hash_mem(const _flat_hash_mem& a) noexcept(noexcept_copy) : ctrls(null), slots(null), count(0), size(0) {
			copyInit(a);
		}
		constexpr _flat_hash_mem(_flat_hash_mem&& a) noexcept : ctrls(a.ctrls), slots(a.slots), count(a.count), size(a.size) {
			a.setNull();
		}
		constexpr _flat_hash_mem(std::initializer_list<slot_type> list_) noexcept(noexcept_copy) : ctrls(null), slots(null), count(0), size(0) {
			moveInit(list_.begin(), list_.size());
		}
		constexpr _flat_hash_mem& operator =(const _flat_hash_mem& a) noexcept(noexcept_copy) {
			if (this != std::addressof(a)) {
				copyAndCoverThis(a);
			}
			return *this;
		}
		constexpr _flat_hash_mem& operator =(_flat_hash_mem&& a) noexcept {
			if (this != std::addressof(a)) {
				destoryAllElem();
				Free(ctrls);
				ctrls = a.ctrls;
				slots = a.slots;
				count = a.count;
				size = a.size;

				a.setNull();
			}
			return *this;
		}
		constexpr _flat_hash_mem& operator =(std::initializer_list<slot_type> list_) noexcept(noexcept_copy) {
			moveAndCoverThis(list_.begin(), list_.size());
			return *this;
		}
		constexpr ~_flat_hash_mem() {
			release();
		}
		//清除所有元素并回收内存
		constexpr void release() noexcept {
			if (ctrls) {
				destoryAllElem();
				Free(ctrls);
				setNull();
			}
		}

		constexpr void destoryAllElem() noexcept {
			//析构success_count个元素，如果元素的类型具有trivially destructible特性，则跳过析构
			if constexpr (!std::is_trivially_destructible_v<slot_type>) {
				for (ulong i = 0; i < size; i += group_byte) {
					group_type group = *reinterpret_cast<group_type*>(ctrls + i);
					group = ~group & check_mask; //查找字节最高位是0的
					while (true) {
						uint c = std::countr_zero(group);
						if (c < group_bit) {
							ulong index = i + (c / group_byte);
							slots[index].~slot_type();
							group &= group - 1; //消除最右边的1
						}
						else {
							break;
						}
					}
				}
			}
		}

		constexpr void setNull() noexcept {
			ctrls = null;
			slots = null;
			count = 0;
			size = 0;
		}
		constexpr void init(size_t size_) noexcept {
			Pair<uchar*, slot_type*> r = CreateCtrls(size_);
			ctrls = r.key;
			slots = r.val;
		}
		//拷贝左值初始化（强异常安全性）
		constexpr void copyInit(const _flat_hash_mem& a) noexcept(noexcept_copy) {
			if (!a.size) {
				return;
			}
			auto m = copy_util();
			m.copyAndSwap(a.size, a.ctrls, a.slots, a.size, *this, false);
		}
		//拷贝右值初始化（强异常安全性）
		constexpr void moveInit(const slot_type* from_, const ulong newCount) noexcept(noexcept_move) {
			if (!newCount) {
				return;
			}
			auto m = copy_util();
			m.copyAndSwap(newCount, from_, *this);
		}
		//拷贝左值并覆盖当前内存，自动析构原内存（强异常安全性）
		constexpr void copyAndCoverThis(const _flat_hash_mem& a) noexcept(noexcept_copy) {
			if (!a.count) {
				destoryAllElem();
				Free(ctrls);
				return;
			}
			auto m = copy_util();
			m.copyAndSwap(a.size, a.ctrls, a.slots, a.size, *this, false);
		}
		//拷贝右值并覆盖当前内存，自动析构原内存（强异常安全性）
		constexpr void moveAndCoverThis(const slot_type* from_, const ulong newCount) noexcept(noexcept_move) {
			if (!newCount) {
				destoryAllElem();
				Free(ctrls);
				return;
			}
			auto m = copy_util();
			m.copyAndSwap(newCount, from_, *this);
		}



		static constexpr Pair<uchar*, slot_type*> CreateCtrls(size_t size_) noexcept {
			size_t slotsByte = slot_size * size_;
			size_t ctrlsByte = size_ + group_byte - 1;
			size_t ctrlsPaddingByte = ctrlsByte + 1;
			if (ctrlsPaddingByte % Config::memory_alignment != 0) {
				ctrlsPaddingByte += Config::memory_alignment - (ctrlsPaddingByte % Config::memory_alignment);
			}
			uchar* memory = Malloc<uchar>(ctrlsPaddingByte + slotsByte);
			MemsetByte(memory, flag_empty, ctrlsByte);
			MemsetByte(memory + ctrlsByte, flag_sentinel, ctrlsPaddingByte - ctrlsByte);
			return {memory, reinterpret_cast<slot_type*>(memory + ctrlsPaddingByte)};
		}

		constexpr void extend() noexcept(noexcept_move) {
			if (!size) {
				size = default_size;
				init(size);
				return;
			}
			if (count > (size / 8) * 7) { //平衡因子0.875
				//扩容和rehash
				auto m = copy_util();
				m.copyAndSwap(size, ctrls, slots, size * 2, *this, true);
			}
		}

		//key是否不存在，如果不存在，则返回可插入的位置和h2
		constexpr Pair<ulid, uchar> findEmptyCtrl(const K& key) const noexcept {
			Hash hash = Hasher(key);
			ulong h1 = hash.v >> h1_shift;
			ulong h2 = hash.v & h2_mask;
			ulong mask = copy_8bytes * h2;
			ulong index = h1 % size;
			while (true) {
				group_type group = *reinterpret_cast<const group_type*>(ctrls + index);
				group_type x = group ^ mask; //如果h2匹配则字节为0，不匹配则最高位为0且其余位必有1，如果是empty或delete则字节最高位为1
				x = (x - copy_8bytes) & (~x & check_mask); //如果字节是0，则设置为128，否则设置为0（由于减法可能会多次跨字节借位，有1/128概率误报，但不会匹配到empty和delete）
				while (true) {
					uint c = std::countr_zero(x);
					if (c < group_bit) { //h2匹配
						ulong i = (index + (c / group_byte)) % size;
						if (CheckKeyEqual(key, slots[i])) {
							return { ulidc::Null, static_cast<uchar>(h2) };
						}
						x &= x - 1; //消除最右边的1
					}
					else {
						x = group & (~group << 1) & check_mask; //查找是否有empty，如果有，则说明key没插入过
						c = std::countr_zero(x);
						if (c < group_bit) {
							index = h1 % size;
							while (true) {
								group = *reinterpret_cast<const group_type*>(ctrls + index);
								x = group & check_mask; //查找字节最高位是1的
								c = std::countr_zero(x);
								if (c < group_bit) {
									return { (index + (c / group_byte)) % size, static_cast<uchar>(h2) };
								}
								index = (index + group_byte) % size;
							}
						}
						break;
					}
				}
				index = (index + group_byte) % size;
			}
			return { ulidc::Null , static_cast<uchar>(h2) };
		}

		//key是否存在
		constexpr ulid findExistCtrl(const K& key) const noexcept {
			Hash hash = Hasher(key);
			ulong h1 = hash.v >> h1_shift;
			ulong h2 = hash.v & h2_mask;
			ulong mask = copy_8bytes * h2;
			ulong index = h1 % size;
			while (true) {
				group_type group = *reinterpret_cast<const group_type*>(ctrls + index);
				group_type x = group ^ mask; //如果h2匹配则字节为0，不匹配则最高位为0且其余位必有1，如果是empty或delete则字节最高位为1
				x = (x - copy_8bytes) & (~x & check_mask); //如果字节是0，则设置为128，否则设置为0（由于减法可能会多次跨字节借位，有1/128概率误报，但不会匹配到empty和delete）
				while (true) {
					uint c = std::countr_zero(x);
					if (c < group_bit) { //h2匹配
						ulong i = (index + (c / group_byte)) % size;
						if (CheckKeyEqual(key, slots[i])) {
							return i;
						}
						x &= x - 1; //消除最右边的1
					}
					else {
						x = group & (~group << 1) & check_mask; //查找是否有empty
						if (x) {
							return ulidc::Null;
						}
						break;
					}
				}
				index = (index + group_byte) % size;
			}
			return ulidc::Null;
		}
		
		//查找key是否存在，如果存在则返回地址且h2为flag_delete，如果不存在则返回可插入地址和h2
		constexpr Pair<ulid, uchar> findExistCtrlOrEmptyCtrl(const K& key) const noexcept {
			Hash hash = Hasher(key);
			ulong h1 = hash.v >> h1_shift;
			ulong h2 = hash.v & h2_mask;
			ulong mask = copy_8bytes * h2;
			ulong index = h1 % size;
			while (true) {
				group_type group = *reinterpret_cast<const group_type*>(ctrls + index);
				group_type x = group ^ mask; //如果h2匹配则字节为0，不匹配则最高位为0且其余位必有1，如果是empty或delete则字节最高位为1
				x = (x - copy_8bytes) & (~x & check_mask); //如果字节是0，则设置为128，否则设置为0（由于减法可能会多次跨字节借位，有1/128概率误报，但不会匹配到empty和delete）
				while (true) {
					uint c = std::countr_zero(x);
					if (c < group_bit) { //h2匹配
						ulong i = (index + (c / group_byte)) % size;
						if (CheckKeyEqual(key, slots[i])) {
							return { i , flag_delete };
						}
						x &= x - 1; //消除最右边的1
					}
					else {
						x = group & (~group << 1) & check_mask; //查找是否有empty
						c = std::countr_zero(x);
						if (c < group_bit) {
							index = h1 % size;
							while (true) {
								group = *reinterpret_cast<const group_type*>(ctrls + index);
								x = group & check_mask; //查找字节最高位是1的
								c = std::countr_zero(x);
								if (c < group_bit) {
									return { (index + (c / group_byte)) % size, static_cast<uchar>(h2)};
								}
								index = (index + group_byte) % size;
							}
						}
						break;
					}
				}
				index = (index + group_byte) % size;
			}
			return { ulidc::Null, static_cast<uchar>(h2) };
		}

		//验证key是否相等
		static constexpr bool CheckKeyEqual(const K& key, const slot_type& slot) noexcept {
			if constexpr (IsPair) {
				return key == slot.key;
			}
			else {
				return key == slot;
			}
		}
		static constexpr const K& GetKey(const slot_type& slot) noexcept {
			if constexpr (IsPair) {
				return slot.key;
			}
			else {
				return slot;
			}
		}


		//不存在则添加，已存在则替换
		template<typename KT = K> requires(!IsPair)
		constexpr slot_type* add(KT&& key) noexcept(get_noexcept_slot<KT>())
		{
			extend();
			Pair<ulid, uchar> r = findExistCtrlOrEmptyCtrl(key);
			slot_type* ptr = slots + r.key;
			if (r.val == flag_delete) { //已存在
				ptr->~K();
				new (ptr) K(std::forward<KT>(key));
			}
			else {
				new (ptr) slot_type(std::forward<KT>(key));
				ctrls[r.key] = r.val;
				if (r.key < group_byte - 1) {
					ctrls[size + r.key] = r.val;
				}
				count++;
			}
			return ptr;
		}
		//不存在则添加，已存在则替换val
		template<typename KT = K, typename VT = V> requires(IsPair)
		constexpr slot_type* add(KT&& key, VT&& val) noexcept(get_noexcept_slot<KT, VT>())
		{
			extend();
			Pair<ulid, uchar> r = findExistCtrlOrEmptyCtrl(key);
			slot_type* ptr = slots + r.key;
			if (r.val == flag_delete) { //已存在
				ptr->val.~V();
				new (std::addressof(ptr->val)) V(std::forward<VT>(val));
			}
			else {
				new (ptr) slot_type(std::forward<KT>(key), std::forward<VT>(val));
				ctrls[r.key] = r.val;
				if (r.key < group_byte - 1) {
					ctrls[size + r.key] = r.val;
				}
				count++;
			}
			return ptr;
		}

		//如果不是已存在的值则添加
		template<class KT = K> requires(!IsPair)
		constexpr slot_type* addIfAbsent(KT&& key) noexcept(get_noexcept_slot<KT>())
		{
			extend();
			Pair<ulid, uchar> r = findExistCtrlOrEmptyCtrl(key);
			slot_type* ptr = slots + r.key;
			if (r.val == flag_delete) return ptr; //已存在
			new (ptr) slot_type(std::forward<KT>(key));
			ctrls[r.key] = r.val;
			if (r.key < group_byte - 1) {
				ctrls[size + r.key] = r.val;
			}
			count++;
			return ptr;
		}
		//如果不是已存在的值则添加
		template<class KT = K, class VT = V> requires(IsPair)
		constexpr slot_type* addIfAbsent(KT&& key, VT&& val) noexcept(get_noexcept_slot<KT, VT>())
		{
			extend();
			Pair<ulid, uchar> r = findExistCtrlOrEmptyCtrl(key);
			slot_type* ptr = slots + r.key;
			if (r.val == flag_delete) return ptr; //已存在
			new (ptr) slot_type(std::forward<KT>(key), std::forward<VT>(val));
			ctrls[r.key] = r.val;
			if (r.key < group_byte - 1) {
				ctrls[size + r.key] = r.val;
			}
			count++;
			return ptr;
		}

		//通过key替换val，如果没找到key则返回null，只有IsPair时可以使用该函数
		template<class VT = V> requires(IsPair)
		constexpr slot_type* replace(const K& key, VT&& val) noexcept(get_noexcept_slot_val<VT>())
		{
			if (!size) {
				return null;
			}
			ulid r = findExistCtrl(key);
			if (r.isNull()) return null; //没找到
			slots[r].val.~V();
			new (std::addressof(slots[r].val)) V(std::forward<VT>(val));
			return slots + r;
		}

		//删除
		constexpr bool remove(const K& key) noexcept {
			if (!size) {
				return false;
			}
			ulid r = findExistCtrl(key);
			if (r.isNull()) return false;
			//如果后一个ctrl是empty的，则可直接设置为empty
			ctrls[r] = (ctrls[r + 1] == flag_empty) ? flag_empty : flag_delete;
			if (r < group_byte - 1) {
				ctrls[size + r] = ctrls[r];
			}
			slots[r].~slot_type();
			return true;
		}

		//查找
		constexpr slot_type* find(const K& key) const noexcept {
			if (!size) {
				return null;
			}
			ulid r = findExistCtrl(key);
			if (r.isNull()) return null;
			return slots + r;
		}

		//清除所有元素
		constexpr void clear() noexcept {
			if (!size) return;
			for (ulong i = 0; i < size; i += group_byte) {
				group_type group = *reinterpret_cast<const group_type*>(ctrls + i);
				group = ~group & check_mask; //查找字节最高位是0的
				while (true) {
					uint c = std::countr_zero(group);
					if (c < group_bit) {
						ulong index = i + (c / group_byte);
						slots[index].~slot_type();
						group &= group - 1; //消除最右边的1
					}
					else {
						break;
					}
				}
			}
			size_t ctrlsByte = size + group_byte - 1;
			MemsetByte(ctrls, flag_empty, ctrlsByte);
		}

		//遍历元素
		constexpr void each(const std::function<void(slot_type&)>& func) const {
			for (ulong i = 0; i < size; i += group_byte) {
				group_type group = *reinterpret_cast<const group_type*>(ctrls + i);
				group = ~group & check_mask; //查找字节最高位是0的
				while (true) {
					uint c = std::countr_zero(group);
					if (c < group_bit) {
						ulong index = i + (c / group_byte);
						func(slots[index]);
						group &= group - 1; //消除最右边的1
					}
					else {
						break;
					}
				}
			}
		}
		//遍历元素，传入函数的第二个参数如果修改为true，则跳过剩下的元素直接结束
		constexpr void each(const std::function<void(slot_type&, bool&)>& func) const {
			bool isBreak = false;
			for (ulong i = 0; i < size; i += group_byte) {
				group_type group = *reinterpret_cast<const group_type*>(ctrls + i);
				group = ~group & check_mask; //查找字节最高位是0的
				while (true) {
					uint c = std::countr_zero(group);
					if (c < group_bit) {
						ulong index = i + (c / group_byte);
						func(slots[index], isBreak);
						if (isBreak) return;
						group &= group - 1; //消除最右边的1
					}
					else {
						break;
					}
				}
			}
		}

		//注意这里只对K进行比较
		constexpr bool operator ==(const _flat_hash_mem& a) const noexcept {
			if (this == &a) {
				return true;
			}
			if (count != a.count) {
				return false;
			}
			for (ulong i = 0; i < size; i += group_byte) {
				group_type group = *reinterpret_cast<const group_type*>(ctrls + i);
				group = ~group & check_mask; //查找字节最高位是0的
				while (true) {
					uint c = std::countr_zero(group);
					if (c < group_bit) {
						ulong index = i + (c / group_byte);
						const K& key = GetKey(slots[index]);
						if (!a.find(key)) return false;
						group &= group - 1; //消除最右边的1
					}
					else {
						break;
					}
				}
			}
			return true;
		}

		friend std::ostream& operator<<(std::ostream& os, const _flat_hash_mem& a) {
			os << "count: " << a.count << ", size: " << a.size << ", data: [";
			ulong printCount = 0;
			for (ulong i = 0; i < a.size; i += group_byte) {
				group_type group = *reinterpret_cast<const group_type*>(a.ctrls + i);
				group = ~group & check_mask; //查找字节最高位是0的
				while (true) {
					uint c = std::countr_zero(group);
					if (c < group_bit) {
						ulong index = i + (c / group_byte);
						os << "\n\t[" << printCount << "]:\t";
						StreamSmartPrint(os, a.slots[index]);
						os << ",";
						++printCount;
						group &= group - 1; //消除最右边的1
					}
					else {
						break;
					}
				}
			}
			os << "\n]";
			return os;
		}

		friend constexpr Hash Hasher(const _flat_hash_mem& a) noexcept {
			Hash hash = Hasher(a.count);
			for (ulong i = 0; i < a.size; i += group_byte) {
				group_type group = *reinterpret_cast<const group_type*>(a.ctrls + i);
				group = ~group & check_mask; //查找字节最高位是0的
				while (true) {
					uint c = std::countr_zero(group);
					if (c < group_bit) {
						ulong index = i + (c / group_byte);
						hash.combineUnordered(a.slots[index]);
						group &= group - 1; //消除最右边的1
					}
					else {
						break;
					}
				}
			}
			return hash;
		}
	};
}