#pragma once
#include <dme/core/utils/mem/flat_key_hash_mem.h>
namespace dme::core {
	template<typename M, bool IsPair, typename K, typename V>
	struct _flat_hash_mem_copy_and_swap<M, IsPair, K, V, true, true> {
		using mem_type = M;
		using key_type = K;
		using val_type = V;
		using slot_type = mem_type::slot_type;
		using list_type = mem_type::list_type;
		using pool_type = mem_type::pool_type;

		using comp_ptr = pool_type::comp_ptr;
		using pool_value_type = pool_type::value_type;

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

		constexpr void copyLValAndSwap(const ulong fromSize, const pool_type* fromPool, const ulong newSize, mem_type& mem) noexcept(mem_type::noexcept_copy) {
			size = _get_next_power_of_two_size<default_size>(newSize);
			Pair<uchar*, slot_type*> p = mem_type::CreateCtrls(size);
			ctrls = p.key;
			slots = p.val;
			size_t newCount = 0;
			pool_type::CopyInitAndEachPtr(*fromPool, mem.pool, [&](comp_ptr ptr, const list_type& obj) noexcept {
				Hash hash = GetElemHash(obj);
				ulong h1 = hash.v >> h1_shift;
				ulong h2 = hash.v & h2_mask;
				ulong index = h1 % size;
				while (true) {
					group_type group = *reinterpret_cast<group_type*>(ctrls + index);
					group = group & check_mask; //查找empty
					uint c = std::countr_zero(group);
					if (c < group_bit) {
						index = (index + (c / group_byte)) % size;
						new (slots + index) slot_type(ptr);
						ctrls[index] = static_cast<uchar>(h2);
						if (index < group_byte - 1) {
							ctrls[size + index] = static_cast<uchar>(h2);
						}
						newCount++;
						break;
					}
					index = (index + group_byte) % size;
				}
			});
			Free(mem.ctrls);

			mem.ctrls = ctrls;
			mem.slots = slots;
			mem.count = newCount;
			mem.size = size;
			ctrls = null;
		}

		constexpr void copyListAndSwap(const ulong newSize, const list_type* list_, mem_type& mem) noexcept(mem_type::noexcept_copy) {
			size = _get_next_power_of_two_size<default_size>(newSize);
			Pair<uchar*, slot_type*> p = mem_type::CreateCtrls(size);
			ctrls = p.key;
			slots = p.val;
			pool_type::CopyListInitAndEachPtr(list_, newSize, mem.pool, [&](comp_ptr ptr, const list_type& obj) noexcept {
				Hash hash = GetElemHash(obj);
				ulong h1 = hash.v >> h1_shift;
				ulong h2 = hash.v & h2_mask;
				ulong index = h1 % size;
				while (true) {
					group_type group = *reinterpret_cast<group_type*>(ctrls + index);
					group = group & check_mask; //查找empty
					uint c = std::countr_zero(group);
					if (c < group_bit) {
						index = (index + (c / group_byte)) % size;
						new (slots + index) slot_type(ptr);
						ctrls[index] = static_cast<uchar>(h2);
						if (index < group_byte - 1) {
							ctrls[size + index] = static_cast<uchar>(h2);
						}
						break;
					}
					index = (index + group_byte) % size;
				}
			});
			Free(mem.ctrls);

			mem.ctrls = ctrls;
			mem.slots = slots;
			mem.count = newSize;
			mem.size = size;
			ctrls = null;
		}

		constexpr Hash GetElemHash(const list_type& obj) noexcept {
			if constexpr (IsPair) {
				return Hasher(obj.key);
			}
			else {
				return Hasher(obj);
			}
		}

		constexpr ~_flat_hash_mem_copy_and_swap() {
			if (ctrls) [[unlikely]] { //拷贝时触发异常的分支
				Free(ctrls);
				ctrls = null;
			}
		}
	};

	template<bool IsPair, typename K, typename V>
	struct _flat_hash_mem<IsPair, K, V, true, true> {
		using group_type = ulong;
		static constexpr uint group_byte = sizeof(group_type);
		static constexpr uint group_bit = group_byte * char_bit;

		using key_type = K;
		using key_ptr = K*;
		using key_comp_ptr = CompressPtr<K>;
		using val_type = V;
		using value_ptr = V*;
		using value_comp_ptr = CompressPtr<V>;
		using pair_type = Pair<K, V>;
		using pair_ptr = Pair<K, V>*;
		using pair_comp_ptr = CompressPtr<Pair<K, V>>;

		using slot_type = std::conditional_t<IsPair, pair_comp_ptr, key_comp_ptr>;
		using list_type = std::conditional_t<IsPair, pair_type, key_type>;
		using pool_type = _obj_pool_mem<list_type>;
		using value_type = list_type;

		using mem_type = _flat_hash_mem<IsPair, K, V, true, true>;
		using copy_util = _flat_hash_mem_copy_and_swap<mem_type, IsPair, K, V, true, true>;

		using size_type = ulong;
		using difference_type = std::make_signed_t<size_type>;

		static constexpr ulong slot_size = sizeof(slot_type);
		static constexpr uint h1_shift = 7;
		static constexpr ulong h2_mask = 0b01111111;

		//static constexpr uchar flag_full =	0b0hhhhhhh;
		static constexpr uchar flag_empty = 0b10000000;
		static constexpr uchar flag_delete = 0b11111110;
		static constexpr uchar flag_sentinel = 0b11111111;

		static constexpr ulong check_mask = 0x8080808080808080Ui64;
		static constexpr ulong copy_8bytes = 0x0101010101010101Ui64;

		static constexpr bool noexcept_copy = is_relocatable<list_type> || nothrow_copy_construct<list_type>;
		static constexpr bool noexcept_move = is_relocatable<list_type> || nothrow_move_construct<list_type> || nothrow_copy_construct<list_type>;

		template<typename... Args>
		static constexpr bool get_noexcept_slot() {
			return nothrow_construct<slot_type, Args...>;
		}
		template<typename... Args>
		static constexpr bool get_noexcept_slot_val() {
			return nothrow_construct<val_type, Args...>;
		}


		static constexpr ulong default_size = 8 * Config::default_init_size;

		uchar* ctrls;
		slot_type* slots;
		ulong count; //包括delete
		ulong size;
		pool_type pool;


		constexpr _flat_hash_mem() noexcept : ctrls(null), slots(null), count(0), size(0), pool() {}
		constexpr _flat_hash_mem(ulong size_) noexcept : ctrls(null), slots(null), count(0), size(_get_next_power_of_two_size<default_size>(size_)), pool() {
			init(size);
		}
		constexpr _flat_hash_mem(const _flat_hash_mem& a) noexcept(noexcept_copy) : ctrls(null), slots(null), count(0), size(0), pool() {
			copyInit(a);
		}
		constexpr _flat_hash_mem(_flat_hash_mem&& a) noexcept : ctrls(a.ctrls), slots(a.slots), count(a.count), size(a.size), pool(std::move(a.pool)) {
			a.setNull();
		}
		constexpr _flat_hash_mem(std::initializer_list<list_type> list_) noexcept(noexcept_copy) : ctrls(null), slots(null), count(0), size(0), pool() {
			copyListInit(list_.begin(), list_.size());
		}
		constexpr _flat_hash_mem& operator =(const _flat_hash_mem& a) noexcept(noexcept_copy) {
			if (this != std::addressof(a)) {
				copyAndCoverThis(a);
			}
			return *this;
		}
		constexpr _flat_hash_mem& operator =(_flat_hash_mem&& a) noexcept {
			if (this != std::addressof(a)) {
				destoryAllElemPool();
				ctrls = a.ctrls;
				slots = a.slots;
				count = a.count;
				size = a.size;
				new (&pool) pool_type(std::move(a.pool));

				a.setNull();
			}
			return *this;
		}
		constexpr _flat_hash_mem& operator =(std::initializer_list<list_type> list_) noexcept(noexcept_copy) {
			copyListAndCoverThis(list_.begin(), list_.size());
			return *this;
		}
		constexpr ~_flat_hash_mem() {
			release();
		}
		//清除所有元素并回收内存
		constexpr void release() noexcept {
			if (ctrls) {
				destoryAllElemPool();
				setNull();
			}
		}

		//删除所有元素并回收内存
		constexpr void destoryAllElemPool() noexcept {
			//析构success_count个元素，如果元素的类型具有trivially destructible特性，则跳过析构
			if constexpr (!std::is_trivially_destructible_v<slot_type>) {
				for (ulong i = 0; i < size; i += group_byte) {
					group_type group = *reinterpret_cast<group_type*>(ctrls + i);
					group = ~group & check_mask; //查找字节最高位是0的
					while (true) {
						uint c = std::countr_zero(group);
						if (c < group_bit) {
							ulong index = i + (c / group_byte);
							slot_type& slot = slots[index];
							pool.remove(slot);
							slot.~slot_type();
							group &= group - 1; //消除最右边的1
						}
						else {
							break;
						}
					}
				}
			}
			pool.freeBlocks();
			Free(ctrls);
		}

		constexpr void setNull() noexcept {
			ctrls = null;
			slots = null;
			count = 0;
			size = 0;
			pool.setNull();
		}
		constexpr void init(size_t size_) noexcept {
			Pair<uchar*, slot_type*> r = CreateCtrls(size_);
			ctrls = r.key;
			slots = r.val;
			pool.init(size);
		}
		//拷贝左值初始化（强异常安全性）
		constexpr void copyInit(const _flat_hash_mem& a) noexcept(noexcept_copy) {
			if (!a.size) {
				return;
			}
			auto m = copy_util();
			m.copyLValAndSwap(a.size, a.pool, a.size, *this);
		}
		//列表初始化（强异常安全性）
		constexpr void copyListInit(const list_type* from_, const ulong newCount) noexcept(noexcept_copy) {
			if (!newCount) {
				return;
			}
			auto m = copy_util();
			m.copyListAndSwap(newCount, from_, *this);
		}
		//拷贝左值并覆盖当前内存，自动析构原内存（强异常安全性）
		constexpr void copyAndCoverThis(const _flat_hash_mem& a) noexcept(noexcept_copy) {
			if (!a.count) {
				destoryAllElemPool();
				setNull();
				return;
			}
			auto m = copy_util();
			m.copyLValAndSwap(a.size, a.pool, a.size, *this);
		}
		//拷贝初始化列表并覆盖当前内存，自动析构原内存（强异常安全性）
		constexpr void copyListAndCoverThis(const list_type* from_, const ulong newCount) noexcept(noexcept_copy) {
			if (!newCount) {
				destoryAllElemPool();
				setNull();
				return;
			}
			auto m = copy_util();
			m.copyListAndSwap(newCount, from_, *this);
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
			return { memory, reinterpret_cast<slot_type*>(memory + ctrlsPaddingByte) };
		}

		constexpr void extend() noexcept(noexcept_move) {
			if (!size) {
				size = default_size;
				init(size);
				return;
			}
			if (count > (size / 8) * 7) { //平衡因子0.875
				//扩容和rehash
				Free(ctrls);
				size *= 2;
				Pair<uchar*, slot_type*> r = CreateCtrls(size);
				ctrls = r.key;
				slots = r.val;
				pool.eachWithBlockIndex([&](list_type& obj, uchar blockIndex) noexcept {
					Hash hash = GetElemHash(obj);
					ulong h1 = hash.v >> h1_shift;
					ulong h2 = hash.v & h2_mask;
					ulong index = h1 % size;
					while (true) {
						group_type group = *reinterpret_cast<group_type*>(ctrls + index);
						group = group & check_mask; //查找empty
						uint c = std::countr_zero(group);
						if (c < group_bit) {
							index = (index + (c / group_byte)) % size;
							new (slots + index) slot_type(std::addressof(obj), blockIndex);
							ctrls[index] = static_cast<uchar>(h2);
							if (index < group_byte - 1) {
								ctrls[size + index] = static_cast<uchar>(h2);
							}
							break;
						}
						index = (index + group_byte) % size;
					}
				});
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



		//传入自定义key和hash，使用自定义比较函数来查找key是否存在，函数传入UserKeyType、hash、slot中的key，返回比较结果
		template<typename UserKeyType, typename EqualFunc>
		constexpr ulid findExistCtrlByUserKeyTypeAndHash(const UserKeyType& key, Hash hash, const EqualFunc& func) const noexcept {
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
						if (func(key, GetKey(slots[i]))) {
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
			return { ulidc::Null, static_cast<uchar>(h2) };
		}

		//验证key是否相等
		static constexpr bool CheckKeyEqual(const K& key, const slot_type& slot) noexcept {
			if constexpr (IsPair) {
				return key == slot->key;
			}
			else {
				return key == *slot;
			}
		}
		static constexpr const K& ObjGetKey(const list_type& obj) noexcept {
			if constexpr (IsPair) {
				return obj.key;
			}
			else {
				return obj;
			}
		}
		static constexpr const K& GetKey(const slot_type& slot) noexcept {
			if constexpr (IsPair) {
				return slot->key;
			}
			else {
				return *slot;
			}
		}
		static constexpr Hash GetElemHash(const list_type& obj) noexcept {
			if constexpr (IsPair) {
				return Hasher(obj.key);
			}
			else {
				return Hasher(obj);
			}
		}

		//遍历元素
		constexpr void each(const std::function<void(list_type&)>& func) const {
			pool.each(func);
		}
		//遍历元素，传入函数的第二个参数如果修改为true，则跳过剩下的元素直接结束
		constexpr void each(const std::function<void(list_type&, bool&)>& func) const {
			pool.each(func);
		}


		//不存在则添加，已存在则替换
		template<typename KT = K> requires(!IsPair)
		constexpr list_type* add(KT&& key) noexcept(get_noexcept_slot<KT>())
		{
			extend();
			Pair<ulid, uchar> r = findExistCtrlOrEmptyCtrl(key);
			slot_type* ptr = slots + r.key;
			if (r.val == flag_delete) { //已存在
				ptr->getRaw()->~list_type();
				new (ptr->getRaw()) list_type(std::forward<KT>(key));
			}
			else {
				new (ptr) slot_type(pool.add(std::forward<KT>(key)));
				ctrls[r.key] = r.val;
				if (r.key < group_byte - 1) {
					ctrls[size + r.key] = r.val;
				}
				count++;
			}
			return ptr->getRaw();
		}
		//不存在则添加，已存在则替换val
		template<typename KT = K, typename VT = V> requires(IsPair)
		constexpr list_type* add(KT&& key, VT&& val) noexcept(get_noexcept_slot<KT, VT>())
		{
			extend();
			Pair<ulid, uchar> r = findExistCtrlOrEmptyCtrl(key);
			slot_type* ptr = slots + r.key;
			if (r.val == flag_delete) { //已存在
				ptr->getRaw()->val.~V();
				new (std::addressof(ptr->getRaw()->val)) V(std::forward<VT>(val));
			}
			else {
				new (ptr) slot_type(pool.add(std::forward<KT>(key), std::forward<VT>(val)));
				ctrls[r.key] = r.val;
				if (r.key < group_byte - 1) {
					ctrls[size + r.key] = r.val;
				}
				count++;
			}
			return ptr->getRaw();
		}

		//如果不是已存在的值则添加
		template<class KT = K> requires(!IsPair)
		constexpr list_type* addIfAbsent(KT&& key) noexcept(get_noexcept_slot<KT>())
		{
			extend();
			Pair<ulid, uchar> r = findExistCtrlOrEmptyCtrl(key);
			slot_type* ptr = slots + r.key;
			if (r.val == flag_delete) return ptr->getRaw(); //已存在
			new (ptr) slot_type(pool.add(std::forward<KT>(key)));
			ctrls[r.key] = r.val;
			if (r.key < group_byte - 1) {
				ctrls[size + r.key] = r.val;
			}
			count++;
			return ptr->getRaw();
		}
		//如果不是已存在的值则添加
		template<class KT = K, class VT = V> requires(IsPair)
		constexpr list_type* addIfAbsent(KT&& key, VT&& val) noexcept(get_noexcept_slot<KT, VT>())
		{
			extend();
			Pair<ulid, uchar> r = findExistCtrlOrEmptyCtrl(key);
			slot_type* ptr = slots + r.key;
			if (r.val == flag_delete) return ptr->getRaw(); //已存在
			new (ptr) slot_type(pool.add(std::forward<KT>(key), std::forward<VT>(val)));
			ctrls[r.key] = r.val;
			if (r.key < group_byte - 1) {
				ctrls[size + r.key] = r.val;
			}
			count++;
			return ptr->getRaw();
		}

		//通过key替换val，如果没找到key则返回null，只有IsPair时可以使用该函数
		template<class VT = V> requires(IsPair)
		constexpr list_type* replace(const K& key, VT&& val) noexcept(get_noexcept_slot_val<VT>())
		{
			if (!size) {
				return null;
			}
			ulid r = findExistCtrl(key);
			if (r.isNull()) return null; //没找到
			auto rawPtr = slots[r].getRaw();
			rawPtr->~V();
			new (rawPtr) V(std::forward<VT>(val));
			return rawPtr;
		}

		//删除
		constexpr bool remove(const K& key) noexcept {
			if (!size) {
				return false;
			}
			ulid r = findExistCtrl(key);
			if (r.isNull()) return false;
			pool.remove(slots[r]);
			//如果后一个ctrl是empty的，则可直接设置为empty
			ctrls[r] = (ctrls[r + 1] == flag_empty) ? flag_empty : flag_delete;
			if (r < group_byte - 1) {
				ctrls[size + r] = ctrls[r];
			}
			return true;
		}

		//查找
		constexpr list_type* find(const K& key) const noexcept {
			if (!size) {
				return null;
			}
			ulid r = findExistCtrl(key);
			if (r.isNull()) return null;
			return slots[r].getRaw();
		}

		//传入自定义key和hash，使用自定义比较函数来查找key是否存在，函数传入UserKeyType、slot中的key，返回比较结果
		template<typename UserKeyType, typename EqualFunc>
		constexpr list_type* findByUserKeyTypeAndHash(const UserKeyType& key, Hash hash, EqualFunc&& func) const noexcept {
			if (!size) {
				return null;
			}
			ulid r = findExistCtrlByUserKeyTypeAndHash(key, hash, func);
			if (r.isNull()) return null;
			return slots[r].getRaw();
		}

		//清除所有元素
		constexpr void clear() noexcept {
			if (!size) return;
			pool.destoryAllElemAndReserveEmptyMaxBlock();
			size_t ctrlsByte = size + group_byte - 1;
			MemsetByte(ctrls, flag_empty, ctrlsByte);
		}

		//注意这里只对K进行比较
		constexpr bool operator ==(const _flat_hash_mem& a) const noexcept {
			if (this == &a) {
				return true;
			}
			if (count != a.count) {
				return false;
			}
			bool r = true;
			pool.each([&](list_type& obj, bool& isBreak) noexcept {
				const K& key = ObjGetKey(obj);
				if (!a.find(key)) {
					isBreak = true;
					r = false;
				}
			});
			return r;
		}

		friend std::ostream& operator<<(std::ostream& os, const _flat_hash_mem& a) {
			os << "count: " << a.count << ", size: " << a.size << ", data: [";
			ulong printCount = 0;
			a.pool.each([&](list_type& obj) noexcept {
				os << "\n\t[" << printCount << "]:\t";
				StreamSmartPrint(os, obj);
				os << ",";
				++printCount;
			});
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