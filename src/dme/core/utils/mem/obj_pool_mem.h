#pragma once
#include <dme/core/utils/mem/iterator.h>
namespace dme::core {
	template<typename = void>
	struct _obj_pool_map {
		union {
			_obj_pool_map* next;
			size_t value;
		};
		size_t bitmap;

		constexpr _obj_pool_map() noexcept : next(null), bitmap(0) {}
		//没有下一个freelist
		static constexpr _obj_pool_map CreateEmptyFreelist() noexcept {
			_obj_pool_map r = _obj_pool_map();
			r.value = size_t_high_bit_mask_inverse;
			return r;
		}
		//不进行处理
		static constexpr _obj_pool_map CreateRaw(_obj_pool_map* rawPtr) noexcept {
			_obj_pool_map r = _obj_pool_map();
			r.next = rawPtr;
			return r;
		}

		//获取去掉最高bit位的指针
		constexpr const _obj_pool_map* getRaw() const noexcept {
			return reinterpret_cast<_obj_pool_map*>(value & size_t_high_bit_mask_inverse);
		}
		//获取去掉最高bit位的指针
		constexpr _obj_pool_map* getRaw() noexcept {
			return reinterpret_cast<_obj_pool_map*>(value & size_t_high_bit_mask_inverse);
		}
		//设置的指针并保留原本的最高位
		constexpr void setRaw(_obj_pool_map* rawPtr) noexcept {
			value = value & size_t_high_bit_mask | reinterpret_cast<size_t>(rawPtr);
		}

		//没有下一个freelist
		static constexpr bool RawNotHasNext(_obj_pool_map* rawPtr) noexcept {
			return reinterpret_cast<size_t>(rawPtr) == size_t_high_bit_mask_inverse;
		}
		static constexpr bool RawHasNext(_obj_pool_map* rawPtr) noexcept {
			return reinterpret_cast<size_t>(rawPtr) != size_t_high_bit_mask_inverse;
		}
		constexpr bool hasFlag() const noexcept {
			return value & size_t_high_bit_mask;
		}
		constexpr bool notHasFlag() const noexcept {
			return (value & size_t_high_bit_mask) == 0;
		}
		constexpr void setFlag() noexcept {
			value |= size_t_high_bit_mask;
		}
		constexpr void removeFlag() noexcept {
			value &= size_t_high_bit_mask_inverse;
		}
		//bitmap右边连续的1的个数
		constexpr uint rone() const noexcept {
			return std::countr_one(bitmap);
		}
		//bitmap右边连续的0的个数
		constexpr uint rzero() const noexcept {
			return std::countr_zero(bitmap);
		}
		//bitmap某bit设置为1
		constexpr void setOne(uint index) noexcept {
			bitmap |= size_t(1) << index;
		}
		//bitmap某bit设置为0
		constexpr void setZero(uint index) noexcept {
			bitmap &= ~(size_t(1) << index);
		}
		//bitmap某bit设置是1
		constexpr bool bitIsOne(uint index) noexcept {
			return bitmap & (size_t(1) << index);
		}
		//bitmap某bit设置是0
		constexpr bool bitIsZero(uint index) noexcept {
			return (bitmap & (size_t(1) << index)) == 0;
		}
		//bitmap已满
		constexpr bool bitmapIsFull() noexcept {
			return bitmap == size_t_max;
		}
		//bitmap全空
		constexpr bool bitmapIsEmpty() noexcept {
			return bitmap == 0;
		}
		//设置为除最高位为1，其余全为0
		constexpr void setFlagAndEmpty() noexcept {
			value = size_t_high_bit_mask;
		}
		//设置next为null
		constexpr void setEmpty() noexcept {
			next = null;
		}

		friend std::ostream& operator<<(std::ostream& os, const _obj_pool_map& a) {
			return os << "next: " << a.next << ", bitmap: " << std::bitset<size_t_bit>(a.bitmap);
		}
	};

	template<typename T>
	struct _obj_pool_mem_block {
		ucid preBlock; //上一个更小的block
		ucid nextBlock; //下一个更大的block
		ucid hasEmptyBlock; //下一个有空闲位置的block的下标
		ucid freeBlockIndex; //当block被删除后，用于保存上个被全部删除的block的下标
		T* start; //当前内存块存储对象的开始地址
		size_t count; //当前内存块保存对象数量，数量减到0时，如果不是当前最大的对象池，则可回收
		size_t size; //当前内存块最大对象数量
		size_t mapCount; //最大map数量
		//保存下一个freelist的指针，如果除了最高位bit全为1，则表示没有下一个了
		_obj_pool_map<void>* freelist;
		//指向freelist和bitmap的起始地址，1个freelist和1个bitmap为一对
		//如果bitmap里的元素被全部删除，则将对应的freelist最高位标记为1，bitmap里改为保存遍历时需要连续跳过的空bitmap数
		//bitmap中用1表示占用，0为删除
		_obj_pool_map<void>* map;

		constexpr _obj_pool_mem_block() noexcept : preBlock(), nextBlock(), hasEmptyBlock(), freeBlockIndex(),
			start(null), count(0), size(0), mapCount(0), freelist(reinterpret_cast<_obj_pool_map<void>*>(size_t_high_bit_mask_inverse)), map(null) {}
		constexpr _obj_pool_mem_block(T* start, size_t size, _obj_pool_map<void>* map, size_t mapCount) noexcept : preBlock(), nextBlock(), hasEmptyBlock(), freeBlockIndex(),
			start(start), count(0), size(size), mapCount(mapCount), freelist(reinterpret_cast<_obj_pool_map<void>*>(size_t_high_bit_mask_inverse)), map(map) {}
	};


	//拷贝并交换，利用RAII保证强异常安全性
	template<typename M>
	struct _obj_pool_mem_copy_and_swap {
		using mem_type = M;
		using value_type = mem_type::value_type;
		using block_type = mem_type::block_type;
		using comp_ptr = mem_type::comp_ptr;
		using map_type = _obj_pool_map<void>;
		static constexpr size_t map_size = sizeof(map_type);
		static constexpr size_t bitmap_size = size_t_bit;
		static constexpr ulong default_size = mem_type::default_size;

		block_type block = block_type();
		size_t success_count = 0;

		constexpr void copyLVal(const mem_type& from_) noexcept(mem_type::noexcept_copy) {
			mem_type::CreateBlock(&block, from_.getNewSize());
			for (ucid blockIndex = from_.maxBlockIndex; blockIndex.notNull();) {
				block_type& b = from_.blocks[blockIndex];
				blockIndex = b.preBlock;
				size_t index = 0;
				map_type* map = b.map;
				while (index < b.size) {
					size_t bitmap = map->bitmap;
					if (map->hasFlag()) { //跳过
						index += bitmap * bitmap_size;
						map += bitmap;
					}
					else {
						while (true) {
							uint i = std::countr_zero(bitmap); //bit右边连续的0的个数
							if (i < bitmap_size) {
								new (block.start + success_count) value_type(b.start[index + i]);
								//设置新block的bitmap
								block.map[success_count / bitmap_size].setOne(success_count % bitmap_size);
								++success_count;
								bitmap &= bitmap - 1; //消掉最靠近右边的1
							}
							else {
								break;
							}
						}
						index += bitmap_size;
						++map;
					}
				}
			}
		}

		template<typename F>
		constexpr void copyLValAndEachPtr(const mem_type& from_, F&& func) noexcept(mem_type::noexcept_copy) {
			mem_type::CreateBlock(&block, from_.getNewSize());
			for (ucid blockIndex = from_.maxBlockIndex; blockIndex.notNull();) {
				block_type& b = from_.blocks[blockIndex];
				blockIndex = b.preBlock;
				size_t index = 0;
				map_type* map = b.map;
				while (index < b.size) {
					size_t bitmap = map->bitmap;
					if (map->hasFlag()) { //跳过
						index += bitmap * bitmap_size;
						map += bitmap;
					}
					else {
						while (true) {
							uint i = std::countr_zero(bitmap); //bit右边连续的0的个数
							if (i < bitmap_size) {
								new (block.start + success_count) value_type(b.start[index + i]);
								//设置新block的bitmap
								block.map[success_count / bitmap_size].setOne(success_count % bitmap_size);
								func(comp_ptr(block.start + success_count, 0), block.start[success_count]);
								++success_count;
								bitmap &= bitmap - 1; //消掉最靠近右边的1
							}
							else {
								break;
							}
						}
						index += bitmap_size;
						++map;
					}
				}
			}
		}

		constexpr void copyRVal(const value_type* from_, const size_t newSize) noexcept(mem_type::noexcept_move) {
			size_t size = _get_next_power_of_two_size<default_size>(newSize);
			mem_type::CreateBlock(&block, size);
			for (; success_count < newSize; ++success_count) {
				if constexpr (is_relocatable<value_type>) {
					if constexpr (sizeof(value_type) <= Config::memory_alignment && std::is_trivially_copyable_v<value_type>) {
						new (block.start + success_count) value_type(std::move(from_[success_count]));
					}
					else {
						MemcpyByte(block.start + success_count, from_ + success_count, sizeof(value_type));
					}
				}
				else if constexpr (mem_type::noexcept_move) {
					if constexpr (nothrow_move_construct<value_type>) {
						new (block.start + success_count) value_type(std::move(from_[success_count]));
					}
					else {
						new (block.start + success_count) value_type(from_[success_count]);
					}
					from_[success_count]->~value_type();
				}
				else {
					new (block.start + success_count) value_type(from_[success_count]); //全部拷贝完成后删除
				}
				//设置新block的bitmap
				block.map[success_count / bitmap_size].setOne(success_count % bitmap_size);
			}
		}

		template<typename F>
		constexpr void copyListAndEachPtr(const value_type* from_, const size_t newSize, F&& func) noexcept(mem_type::noexcept_move) {
			size_t size = _get_next_power_of_two_size<default_size>(newSize);
			mem_type::CreateBlock(&block, size);
			if constexpr (is_relocatable<value_type>) {
				Memcpy<value_type>(block.start, from_, newSize);
				success_count = newSize;
			}
			else {
				for (; success_count < newSize; ++success_count) {
					new (block.start + success_count) value_type(from_[success_count]);
				}
			}
			for (size_t i = 0; i < newSize; i += bitmap_size) {
				if (newSize - i < bitmap_size) {
					block.map[i / bitmap_size].bitmap = size_t_max >> (bitmap_size - (newSize - i));
				}
				else {
					block.map[i / bitmap_size].bitmap = size_t_max;
				}
			}
			for (size_t i = 0; i < newSize; ++i) {
				func(comp_ptr(block.start + i, 0), block.start[i]);
			}
		}

		//将拷贝好的资源交换给传入的对象
		constexpr void swap(mem_type& to_, const bool needClear) noexcept {
			if (needClear) {
				to_.destoryAllElemAndBlocks();
			}
			to_.blocks = Malloc<block_type>(2);
			to_.blocksCount = 1;
			to_.blocksSize = 2;
			to_.maxBlockIndex = 0;
			to_.freeBlockIndex.setNull();
			to_.hasEmptyBlock.setNull();
			block.count = success_count;
			to_.blocks[0] = block;
			block.map = null;
			block.start = null;
		}

		constexpr ~_obj_pool_mem_copy_and_swap() {
			if (block.start) [[unlikely]] { //拷贝时触发异常的分支
				//析构success_count个元素，如果元素的类型具有trivially destructible特性，则跳过析构
				if constexpr (!std::is_trivially_destructible_v<value_type>) {
					for (value_type* p = block.start; p < block.start + success_count; ++p) {
						p->~value_type();
					}
				}
				Free(block.map);
				block.start = null;
			}
		}
	};

	//具有指针稳定性的对象池，每一个内存块配合一个记录了对象删除情况的数组方便遍历
	template<typename T>
	struct _obj_pool_mem {
		using size_type = size_t;
		using difference_type = std::make_signed_t<size_type>;
		using value_type = T;

		using iterator = _obj_pool_iterator<_obj_pool_mem>;
		using const_iterator = _obj_pool_const_iterator<_obj_pool_mem>;

		using copy_util = _obj_pool_mem_copy_and_swap<_obj_pool_mem>;
		using comp_ptr = CompressPtr<T>;
		using map_type = _obj_pool_map<void>;
		using block_type = _obj_pool_mem_block<T>;
		static constexpr size_t map_size = sizeof(map_type);
		static constexpr size_t block_size = sizeof(block_type);
		static constexpr size_t bitmap_size = size_t_bit;
		static constexpr size_t default_size = 8 * Config::default_init_size;

		//默认构造函数
		static constexpr bool noexcept_default = is_relocatable<T> || nothrow_default_construct<T>;
		//拷贝
		static constexpr bool noexcept_copy = is_relocatable<T> || nothrow_copy_construct<T>;
		//所有权转移
		static constexpr bool noexcept_move = is_relocatable<T> || nothrow_move_construct<T> || nothrow_copy_construct<T>;

		block_type* blocks; //可以认为不会超过64个block(指针高6位可保存下标)，block一旦申请，处于blocks的下标就不会改变
		uchar blocksCount;
		uchar blocksSize;
		uchar maxBlockIndex; //当前blocks里最大的block的下标，优先使用
		ucid freeBlockIndex; //上个被全部删除的block的下标，bit全为1表示空，不会记载maxBlockIndex
		ucid hasEmptyBlock; //有空闲位置的block的下标

		constexpr _obj_pool_mem() noexcept : blocks(null), blocksCount(0), blocksSize(0), maxBlockIndex(0),
			freeBlockIndex(), hasEmptyBlock() {}
		constexpr _obj_pool_mem(size_t size) noexcept : blocks(null), blocksCount(0), blocksSize(0), maxBlockIndex(0),
			freeBlockIndex(), hasEmptyBlock() 
		{
			size = _get_next_power_of_two_size<default_size>(size);
			init(size);
		}
		constexpr _obj_pool_mem(const _obj_pool_mem& a) noexcept(noexcept_copy) : blocks(null), blocksCount(0), blocksSize(0), maxBlockIndex(0),
			freeBlockIndex(), hasEmptyBlock()
		{
			copyInit(a);
		}
		constexpr _obj_pool_mem(_obj_pool_mem&& a) noexcept : 
			blocks(a.blocks), blocksCount(a.blocksCount), blocksSize(a.blocksSize), maxBlockIndex(a.maxBlockIndex),
			 freeBlockIndex(a.freeBlockIndex), hasEmptyBlock(a.hasEmptyBlock)
		{
			a.setNull();
		}
		constexpr _obj_pool_mem(std::initializer_list<T> list_) noexcept : blocks(null), blocksCount(0), blocksSize(0), maxBlockIndex(0),
			freeBlockIndex(), hasEmptyBlock()
		{
			moveInit(list_.begin(), list_.size());
		}
		constexpr _obj_pool_mem& operator =(const _obj_pool_mem& a) noexcept(noexcept_copy) {
			if (this != std::addressof(a)) {
				copyAndCoverThis(a);
			}
			return *this;
		}
		constexpr _obj_pool_mem& operator =(_obj_pool_mem&& a) noexcept {
			if (this != std::addressof(a)) {
				destoryAllElemAndBlocks();
				blocks = a.blocks;
				blocksCount = a.blocksCount;
				blocksSize = a.blocksSize;
				maxBlockIndex = a.maxBlockIndex;
				freeBlockIndex = a.freeBlockIndex;
				hasEmptyBlock = a.hasEmptyBlock;
				a.setNull();
			}
			return *this;
		}
		constexpr _obj_pool_mem& operator =(std::initializer_list<T> list_) noexcept(noexcept_copy) {
			moveAndCoverThis(list_.begin(), list_.size());
			return *this;
		}

		~_obj_pool_mem() {
			if (!blocks) return;
			destoryAllElemAndBlocks();
			setNull();
		}


		iterator begin() noexcept {
			return iterator(this);
		}
		const_iterator begin() const noexcept {
			return const_iterator(this);
		}
		const_iterator cbegin() const noexcept {
			return const_iterator(this);
		}
		iterator end() noexcept {
			return iterator(this, ucidc::Null);
		}
		const_iterator end() const noexcept {
			return const_iterator(this, ucidc::Null);
		}
		const_iterator cend() const noexcept {
			return const_iterator(this, ucidc::Null);
		}

		//拷贝别的内存池时需要申请的大小（调用时blocks不能为空）
		constexpr size_t getNewSize() const noexcept{
			size_t size = 0;
			for (ucid blockIndex = maxBlockIndex; blockIndex.notNull();) {
				size += blocks[blockIndex].count;
				blockIndex = blocks[blockIndex].preBlock;
			}
			return _get_next_power_of_two_size<default_size>(size);
		}
		constexpr size_t getTotalCount() const noexcept {
			size_t count = 0;
			if (blocks == null) return count;
			for (ucid blockIndex = maxBlockIndex; blockIndex.notNull();) {
				count += blocks[blockIndex].count;
				blockIndex = blocks[blockIndex].preBlock;
			}
			return count;
		}

		constexpr bool isNull() const noexcept {
			return blocks == null;
		}
		constexpr void setNull() noexcept {
			blocks = null;
			blocksCount = 0;
			blocksSize = 0;
			maxBlockIndex = 0;
			freeBlockIndex.setNull();
			hasEmptyBlock.setNull();
		}
		constexpr void init(size_t size) noexcept {
			blocks = Malloc<block_type>(2);
			blocksCount = 1;
			blocksSize = 2;
			maxBlockIndex = 0;
			freeBlockIndex.setNull();
			hasEmptyBlock.setNull();
			CreateBlock(blocks, size);
		}

		//拷贝左值初始化（强异常安全性）
		constexpr void copyInit(const _obj_pool_mem& a) noexcept(noexcept_copy) {
			if (!a.blocks) return;
			auto m = copy_util();
			m.copyLVal(a);
			m.swap(*this, false);
		}
		//拷贝右值初始化（强异常安全性）
		constexpr void moveInit(const T* from_, const size_t count_) noexcept(noexcept_move) {
			if (!count_) return;
			auto m = copy_util();
			m.copyRVal(from_, count_);
			m.swap(*this, false);
		}
		//拷贝左值并覆盖当前内存，自动析构原内存（强异常安全性）
		constexpr void copyAndCoverThis(const _obj_pool_mem& a) noexcept(noexcept_copy) {
			if (!a.blocks) {
				destoryAllElemAndBlocks();
				setNull();
				return;
			}
			auto m = copy_util();
			m.copyLVal(a);
			m.swap(*this, true);
		}
		//拷贝右值并覆盖当前内存，自动析构原内存（强异常安全性）
		constexpr void moveAndCoverThis(const T* from_, const size_t count_) noexcept(noexcept_move) {
			if (!count_) {
				destoryAllElemAndBlocks();
				setNull();
				return;
			}
			auto m = copy_util();
			m.copyRVal(from_, count_);
			m.swap(*this, true);
		}
		//拷贝并遍历返回所有comp_ptr
		template<typename F>
		static constexpr void CopyInitAndEachPtr(const _obj_pool_mem& from_, _obj_pool_mem& to_, F&& func) noexcept(noexcept_copy) {
			if (!from_.blocks) return;
			auto m = copy_util();
			m.copyLValAndEachPtr(from_, std::forward<F>(func));
			m.swap(to_, false);
		}
		//拷贝并遍历返回所有comp_ptr
		template<typename F>
		static constexpr void CopyListInitAndEachPtr(const value_type* from_, const size_t count_, _obj_pool_mem& to_, F&& func) noexcept(noexcept_copy) {
			if (!count_) return;
			auto m = copy_util();
			m.copyListAndEachPtr(from_, count_, std::forward<F>(func));
			m.swap(to_, false);
		}

		constexpr uchar addBlock() noexcept {
			uchar blockIndex;
			if(freeBlockIndex.notNull()) {
				blockIndex = freeBlockIndex;
				freeBlockIndex = blocks[blockIndex].freeBlockIndex;
			}
			else {
				if (blocksCount == blocksSize) { //extend
					MemExtend<block_type>(blocks, blocksCount, blocksSize);
				}
				blockIndex = blocksCount;
				++blocksCount;
			}
			block_type* block = blocks + blockIndex;
			CreateBlock(block, blocks[maxBlockIndex].size * 2);
			block->preBlock = maxBlockIndex;
			blocks[maxBlockIndex].nextBlock = blockIndex;
			maxBlockIndex = blockIndex;
			return blockIndex;
		}
		static constexpr void CreateBlock(block_type* block, size_t size) noexcept {
			size_t mapCount = size / bitmap_size;
			if (mapCount == 0) mapCount = 1;
			size_t mapByte = mapCount * map_size;
			if constexpr (map_size % Config::memory_alignment != 0) {
				if (mapByte % Config::memory_alignment != 0) {
					mapByte += Config::memory_alignment - (mapByte % Config::memory_alignment);
				}
			}
			size_t byteCount = mapByte + sizeof(T) * size;
			uchar* memory = Malloc<uchar>(byteCount);
			DME_ASSERT(comp_ptr::HighestSixBitsIsZero(memory + byteCount), "highest 6 bits of the memory address must be zero");
			new (block) block_type(reinterpret_cast<T*>(memory + mapByte), size, reinterpret_cast<map_type*>(memory), mapCount);
			MemsetByte(memory, 0, mapByte);
		}
		//必须block里元素为0且不是最大的block时调用
		constexpr void removeBlock(uchar blockIndex) noexcept {
			block_type& block = blocks[blockIndex];
			Free(block.map);
			if (block.preBlock.notNull()) {
				blocks[block.preBlock].nextBlock = block.nextBlock;
			}
			if (block.nextBlock.notNull()) {
				blocks[block.nextBlock].preBlock = block.preBlock;
			}
			block.start = null; //用start标记block为空
			if (map_type::RawHasNext(block.freelist)) {
				if (hasEmptyBlock == blockIndex) {
					hasEmptyBlock = block.hasEmptyBlock;
				}
				else {
					for (ucid i = hasEmptyBlock; i.notNull(); i = blocks[i].hasEmptyBlock) {
						if (blocks[i].hasEmptyBlock == blockIndex) {
							blocks[i].hasEmptyBlock = block.hasEmptyBlock;
							break;
						}
					}
				}
			}
			block.freeBlockIndex = freeBlockIndex;
			freeBlockIndex = blockIndex;
		}

		//添加对象（强异常安全）
		template<typename O = T> requires(IsSameOrConvertible<O, T>)
		constexpr comp_ptr add(O&& obj) noexcept(noexcept_move) {
			T* ptr;
			uchar blockIndex;
			if (!blocksSize) {
				init(default_size);
			}
			if (blocks[maxBlockIndex].count < blocks[maxBlockIndex].size) { //优先使用最大block
				blockIndex = maxBlockIndex;
				block_type& block = blocks[blockIndex];
				map_type* map = block.freelist;
				if (map_type::RawNotHasNext(map)) {
					ptr = block.start + block.count;
					new (ptr) T(std::forward<O>(obj));
					block.map[block.count / bitmap_size].setOne(block.count % bitmap_size);
				}
				else { //存在freelist
					ptr = addHandleFreelist(std::forward<O>(obj), blockIndex, block, map);
				}
				block.count++;
			}
			else if (hasEmptyBlock.notNull()) { //最大block空间已满，但是存在freelist
				blockIndex = hasEmptyBlock;
				block_type& block = blocks[blockIndex];
				map_type* map = block.freelist;
				ptr = addHandleFreelist(std::forward<O>(obj), blockIndex, block, map);
				block.count++;
			}
			else { //所有block全满
				blockIndex = addBlock();
				block_type& block = blocks[blockIndex];
				ptr = block.start;
				new (ptr) T(std::forward<O>(obj));
				block.map->bitmap = 1;
				block.count++;
			}
			return comp_ptr(ptr, blockIndex);
		}
		template<typename O> requires(IsSameOrConvertible<O, T>)
		constexpr T* addHandleFreelist(O&& obj, uchar blockIndex, block_type& block, map_type* map) noexcept(noexcept_move) {
			T* ptr;
			while (map->hasFlag()) { //说明bitmap存的是需要跳过的连续的空bitmap数量
				if (map->bitmap > 1) {//如果不是单独的空bitmap
					size_t mapIndex = map - block.map;
					//查看下一个
					if (mapIndex == block.mapCount - 1 || (map + 1)->notHasFlag()) { //说明在结尾
						ptr = block.start + mapIndex * bitmap_size;
						new (ptr) T(std::forward<O>(obj));

						size_t skip = map->bitmap - 1;
						(map - skip)->bitmap = skip; //修改开头
						if (skip > 1) { //修改上一个
							(map - 1)->bitmap = skip;
						}
						map->removeFlag();
						map->bitmap = 1; //占用1个
						return ptr;
					}
					else {
						//查看上一个
						if (mapIndex == 0 || (map - 1)->notHasFlag()) { //说明在开头
							size_t skip = map->bitmap - 1;
							mapIndex = map + skip - block.map;

							ptr = block.start + mapIndex * bitmap_size;
							new (ptr) T(std::forward<O>(obj));

							map->bitmap = skip; //修改开头
							if (skip > 1) { //修改结尾前一个
								(map + skip - 1)->bitmap = skip;
							}
							//跳到结尾
							map += skip;
							map->removeFlag();
							map->bitmap = 1; //占用1个
							return ptr;
						}
						else { //说明在中间
							//则先不使用这块地方，取出这里原本保存的freelist
							block.freelist = map->getRaw();
							map->setFlagAndEmpty(); //删除原本的freelist
							map = block.freelist;
						}
					}
				}
				else {
					size_t mapIndex = map - block.map;
					ptr = block.start + mapIndex * bitmap_size;
					new (ptr) T(std::forward<O>(obj));

					map->removeFlag();
					map->bitmap = 1; //占用1个
					return ptr;
				}
			}
			//一定不含flag
			size_t mapIndex = map - block.map;
			uint bitmapIndex = map->rone(); //bitmap右边连续的1的个数
			ptr = block.start + mapIndex * bitmap_size + bitmapIndex;
			new (ptr) T(std::forward<O>(obj));

			map->setOne(bitmapIndex);
			if (map->bitmapIsFull()) { //当前bitmap已满
				block.freelist = map->next; //替换freelist
				map->setEmpty();
				if (blockIndex != maxBlockIndex && map_type::RawNotHasNext(block.freelist)) { //无空闲空间，需要移出hasEmptyBlock
					hasEmptyBlock = block.hasEmptyBlock;
					block.hasEmptyBlock.setNull();
				}
			}
			return ptr;
		}

		/*
		//申请对象空间，返回压缩指针
		constexpr comp_ptr alloc() noexcept {
			T* ptr;
			uchar blockIndex;
			if (!blocksSize) {
				init(default_size);
			}
			if (blocks[maxBlockIndex].count < blocks[maxBlockIndex].size) { //优先使用最大block
				blockIndex = maxBlockIndex;
				block_type& block = blocks[blockIndex];
				map_type* map = block.freelist;
				if (map_type::RawNotHasNext(map)) {
					ptr = block.start + block.count;
					block.map[block.count / bitmap_size].setOne(block.count % bitmap_size);
				}
				else { //存在freelist
					ptr = allocHandleFreelist(blockIndex, block, map);
				}
				block.count++;
			}
			else if (hasEmptyBlock.notNull()) { //最大block空间已满，但是存在freelist
				blockIndex = hasEmptyBlock;
				block_type& block = blocks[blockIndex];
				map_type* map = block.freelist;
				ptr = allocHandleFreelist(blockIndex, block, map);
				block.count++;
			}
			else { //所有block全满
				blockIndex = addBlock();
				block_type& block = blocks[blockIndex];
				ptr = block.start;
				block.map->bitmap = 1;
				block.count++;
			}
			return comp_ptr(ptr, blockIndex);
		}
		constexpr T* allocHandleFreelist(uchar blockIndex, block_type& block, map_type* map) noexcept {
			while (map->hasFlag()) { //说明bitmap存的是需要跳过的连续的空bitmap数量
				if (map->bitmap > 1) {//如果不是单独的空bitmap
					size_t mapIndex = map - block.map;
					//查看下一个
					if (mapIndex == block.mapCount - 1 || (map + 1)->notHasFlag()) { //说明在结尾
						size_t skip = map->bitmap - 1;
						(map - skip)->bitmap = skip; //修改开头
						if (skip > 1) { //修改上一个
							(map - 1)->bitmap = skip;
						}
						goto remove_flag;
					}
					else {
						//查看上一个
						if (mapIndex == 0 || (map - 1)->notHasFlag()) { //说明在开头
							size_t skip = map->bitmap - 1;
							map->bitmap = skip; //修改开头
							if (skip > 1) { //修改结尾前一个
								(map + skip - 1)->bitmap = skip;
							}
							//跳到结尾
							map += skip;
							goto remove_flag;
						}
						else { //说明在中间
							//则先不使用这块地方，取出这里原本保存的freelist
							block.freelist = map->getRaw();
							map->setFlagAndEmpty(); //删除原本的freelist
							map = block.freelist;
						}
					}
				}
				else {
				remove_flag:
					map->removeFlag();
					map->bitmap = 1; //占用1个
					size_t mapIndex = map - block.map;
					return block.start + mapIndex * bitmap_size;
				}
			}
			//一定不含flag
			size_t mapIndex = map - block.map;
			uint bitmapIndex = map->rone(); //bitmap右边连续的1的个数
			map->setOne(bitmapIndex);
			if (map->bitmapIsFull()) { //当前bitmap已满
				block.freelist = map->next; //替换freelist
				map->setEmpty();
				if (blockIndex != maxBlockIndex && map_type::RawNotHasNext(block.freelist)) { //无空闲空间，需要移出hasEmptyBlock
					hasEmptyBlock = block.hasEmptyBlock;
					block.hasEmptyBlock.setNull();
				}
			}
			return block.start + mapIndex * bitmap_size + bitmapIndex;
		}
		*/

		//回收对象空间
		constexpr void remove(comp_ptr ptr) noexcept {
			T* raw = ptr.getRaw();
			uchar blockIndex = ptr.getIndex();
			DME_ASSERT(blockIndex < blocksCount, "subscript exceeds the block count");
			block_type& block = blocks[blockIndex];
			DME_ASSERT(raw >= block.start, "illegal pointer");
			DME_ASSERT((reinterpret_cast<uchar*>(raw) - reinterpret_cast<uchar*>(block.start)) % sizeof(T) == 0, "illegal pointer");
			size_t index = raw - block.start;
			DME_ASSERT(index < block.size, "subscript exceeds the size");
			size_t mapIndex = index / bitmap_size;
			map_type* map = block.map + mapIndex;
			DME_ASSERT(map->notHasFlag(), "double free");
			uint bitmapIndex = index % bitmap_size;
			DME_ASSERT(map->bitIsOne(bitmapIndex), "double free");
			raw->~T();

			block.count--;
			if (blockIndex != maxBlockIndex) {
				if (block.count == 0) {
					removeBlock(blockIndex);
					return;
				}
				if (map_type::RawNotHasNext(block.freelist)) { //当前block不在hasEmptyBlock中
					block.hasEmptyBlock = hasEmptyBlock;
					hasEmptyBlock = blockIndex;
				}
			}
			if (!map->next) { //未加入freelist
				map->next = block.freelist;
				block.freelist = map;
			}
			map->setZero(bitmapIndex);
			if (map->bitmapIsEmpty()) {
				map->setFlag();
				//查看前后是否有bitmap全空的
				size_t mapIndex = map - block.map;
				size_t skip;
				if (mapIndex > 0 && (map - 1)->hasFlag()) { //前面有
					map_type* startMap = map - 1;
					startMap -= startMap->bitmap - 1;

					if (mapIndex < block.mapCount - 1 && (map + 1)->hasFlag()) { //后面有
						map_type* startMap2 = map + 1;
						skip = startMap->bitmap + 1 + startMap2->bitmap;
						startMap->bitmap = skip;
						(startMap2 + startMap2->bitmap - 1)->bitmap = skip;
						startMap2->bitmap = skip;
					}
					else {
						skip = startMap->bitmap + 1;
						startMap->bitmap = skip;
					}
				}
				else {
					if (mapIndex < block.mapCount - 1 && (map + 1)->hasFlag()) { //后面有
						map_type* startMap = map + 1;
						skip = startMap->bitmap + 1;
						(startMap + skip - 2)->bitmap = skip;
					}
					else {
						skip = 1;
					}
				}
				map->bitmap = skip;
			}
		}

		void testPrint() const {
			auto& os = std::cout << "blocksCount :" << +blocksCount << "\n";
			os << "blocksSize :" << +blocksSize << "\n";
			os << "maxBlockIndex :" << +maxBlockIndex << "\n";
			os << "freeBlockIndex :" << +freeBlockIndex << "\n";
			os << "hasEmptyBlock :" << +hasEmptyBlock << "\n";
			for (uint bi = 0; bi < blocksCount; bi++) {
				block_type& block = blocks[bi];
				if (!block.start) continue;
				os << "blocks[" << bi << "]: {\n";
				os << "\tpreBlock: " << +block.preBlock << "\n";
				os << "\tnextBlock: " << +block.nextBlock << "\n";
				os << "\thasEmptyBlock: " << +block.hasEmptyBlock << "\n";
				os << "\tcount: " << +block.count << "\n";
				os << "\tsize: " << +block.size << "\n";
				os << "\tmapCount: " << block.mapCount << "\n";
				os << "\tfreelist: " << block.freelist << "\n";
				os << "\tfreelistIndex: " << block.freelist - block.map << "\n";
				for (size_t i = 0; i < block.mapCount; i++) {
					map_type& map = block.map[i];
					os << "\t[" << i << "]: " << map << "\n";
					//打印元素
					if (!map.hasFlag()) {
						size_t bitmap = map.bitmap;
						while (true) {
							size_t index = std::countr_zero(bitmap); //bit右边连续的0的个数
							if (index < bitmap_size) {
								index = index + i * bitmap_size;
								os << "\tobj[" << index << "]: ";
								StreamSmartPrint(os, block.start[index]);
								bitmap &= bitmap - 1; //消掉最靠近右边的1
							}
							else {
								break;
							}
						}
					}
				}
				os << "}\n";
			}
		}


		//回收所有元素和blocks的内存
		constexpr void destoryAllElemAndBlocks() noexcept {
			for (ucid blockIndex = maxBlockIndex; blockIndex.notNull();) {
				block_type& block = blocks[blockIndex];
				blockIndex = block.preBlock;
				if constexpr (!std::is_trivially_destructible_v<T>) {
					size_t index = 0;
					map_type* map = block.map;
					while (index < block.size) {
						size_t bitmap = map->bitmap;
						if (map->hasFlag()) { //跳过
							index += bitmap * bitmap_size;
							map += bitmap;
						}
						else {
							while (true) {
								uint i = std::countr_zero(bitmap); //bit右边连续的0的个数
								if (i < bitmap_size) {
									block.start[index + i].~T();
									bitmap &= bitmap - 1; //消掉最靠近右边的1
								}
								else {
									break;
								}
							}
							index += bitmap_size;
							++map;
						}
					}
				}
				Free(block.map);
			}
			Free(blocks);
		}

		//回收所有元素和block的内存，但是保留内存为空的最大block
		constexpr void destoryAllElemAndReserveEmptyMaxBlock() noexcept {
			for (ucid blockIndex = maxBlockIndex; blockIndex.notNull();) {
				block_type& block = blocks[blockIndex];
				blockIndex = block.preBlock;
				if constexpr (!std::is_trivially_destructible_v<T>) {
					size_t index = 0;
					map_type* map = block.map;
					while (index < block.size) {
						size_t bitmap = map->bitmap;
						if (map->hasFlag()) { //跳过
							index += bitmap * bitmap_size;
							map += bitmap;
						}
						else {
							while (true) {
								uint i = std::countr_zero(bitmap); //bit右边连续的0的个数
								if (i < bitmap_size) {
									block.start[index + i].~T();
									bitmap &= bitmap - 1; //消掉最靠近右边的1
								}
								else {
									break;
								}
							}
							index += bitmap_size;
							++map;
						}
					}
				}
				if (blockIndex != maxBlockIndex) {
					Free(block.map);
				}
			}
			blocksCount = 1;
			block_type& maxBlock = blocks[maxBlockIndex];
			new (blocks) block_type(maxBlock.start, maxBlock.size, maxBlock.map, maxBlock.mapCount);
			MemsetByte(maxBlock.map, 0, reinterpret_cast<uchar*>(maxBlock.start) - reinterpret_cast<uchar*>(maxBlock.map));
			if (maxBlockIndex != 0) {
				maxBlock.start = null;
			}
			maxBlockIndex = 0;
			freeBlockIndex.setNull();
			hasEmptyBlock.setNull();
		}
		//回收所有元素和block的内存，但是保留内存为空的最大block，并且每次析构元素前调用函数
		constexpr void destoryAllElemAndReserveEmptyMaxBlockAndHandle(const std::function<void(T&)>& func) noexcept {
			for (ucid blockIndex = maxBlockIndex; blockIndex.notNull();) {
				block_type& block = blocks[blockIndex];
				blockIndex = block.preBlock;
				if constexpr (!std::is_trivially_destructible_v<T>) {
					size_t index = 0;
					map_type* map = block.map;
					while (index < block.size) {
						size_t bitmap = map->bitmap;
						if (map->hasFlag()) { //跳过
							index += bitmap * bitmap_size;
							map += bitmap;
						}
						else {
							while (true) {
								uint i = std::countr_zero(bitmap); //bit右边连续的0的个数
								if (i < bitmap_size) {
									T& obj = block.start[index + i];
									func(obj);
									obj.~T();
									bitmap &= bitmap - 1; //消掉最靠近右边的1
								}
								else {
									break;
								}
							}
							index += bitmap_size;
							++map;
						}
					}
				}
				if (blockIndex != maxBlockIndex) {
					Free(block.map);
				}
			}
			blocksCount = 1;
			block_type& maxBlock = blocks[maxBlockIndex];
			new (blocks) block_type(maxBlock.start, maxBlock.size, maxBlock.map, maxBlock.mapCount);
			MemsetByte(maxBlock.map, 0, reinterpret_cast<uchar*>(maxBlock.start) - reinterpret_cast<uchar*>(maxBlock.map));
			if (maxBlockIndex != 0) {
				maxBlock.start = null;
			}
			maxBlockIndex = 0;
			freeBlockIndex.setNull();
			hasEmptyBlock.setNull();
		}
		//当没有元素时，回收所有block的内存，但是保留内存为空的最大block
		constexpr void destoryBlocksAndReserveEmptyMaxBlock() noexcept {
			for (ucid blockIndex = blocks[maxBlockIndex].preBlock; blockIndex.notNull();) {
				block_type& block = blocks[blockIndex];
				blockIndex = block.preBlock;
				Free(block.map);
			}
			blocksCount = 1;
			block_type& maxBlock = blocks[maxBlockIndex];
			new (blocks) block_type(maxBlock.start, maxBlock.size, maxBlock.map, maxBlock.mapCount);
			MemsetByte(maxBlock.map, 0, reinterpret_cast<uchar*>(maxBlock.start) - reinterpret_cast<uchar*>(maxBlock.map));
			if (maxBlockIndex != 0) {
				maxBlock.start = null;
			}
			maxBlockIndex = 0;
			freeBlockIndex.setNull();
			hasEmptyBlock.setNull();
		}
		//当没有元素时，回收所有blocks的内存
		constexpr void freeBlocks() noexcept {
			if (blocks) {
				for (ucid blockIndex = maxBlockIndex; blockIndex.notNull();) {
					block_type& block = blocks[blockIndex];
					blockIndex = block.preBlock;
					Free(block.map);
				}
				Free(blocks);
			}
		}

		//遍历元素
		constexpr void each(const std::function<void(T&)>& func) const {
			for (ucid blockIndex = maxBlockIndex; blockIndex.notNull();) {
				block_type& block = blocks[blockIndex];
				blockIndex = block.preBlock;
				size_t index = 0;
				map_type* map = block.map;
				while (index < block.size) {
					size_t bitmap = map->bitmap;
					if (map->hasFlag()) { //跳过
						index += bitmap * bitmap_size;
						map += bitmap;
					}
					else {
						while (true) {
							uint i = std::countr_zero(bitmap); //bit右边连续的0的个数
							if (i < bitmap_size) {
								func(block.start[index + i]);
								bitmap &= bitmap - 1; //消掉最靠近右边的1
							}
							else {
								break;
							}
						}
						index += bitmap_size;
						++map;
					}
				}
			}
		}
		//遍历元素，传入函数的第二个参数如果修改为true，则跳过剩下的元素直接结束
		constexpr void each(const std::function<void(T&, bool&)>& func) const {
			bool isBreak = false;
			for (ucid blockIndex = maxBlockIndex; blockIndex.notNull();) {
				block_type& block = blocks[blockIndex];
				blockIndex = block.preBlock;
				size_t index = 0;
				map_type* map = block.map;
				while (index < block.size) {
					size_t bitmap = map->bitmap;
					if (map->hasFlag()) { //跳过
						index += bitmap * bitmap_size;
						map += bitmap;
					}
					else {
						while (true) {
							uint i = std::countr_zero(bitmap); //bit右边连续的0的个数
							if (i < bitmap_size) {
								func(block.start[index + i], isBreak);
								if (isBreak) return;
								bitmap &= bitmap - 1; //消掉最靠近右边的1
							}
							else {
								break;
							}
						}
						index += bitmap_size;
						++map;
					}
				}
			}
		}

		//遍历元素，参数还会传入当前blockIndex
		constexpr void eachWithBlockIndex(const std::function<void(T&, uchar)>& func) const {
			for (ucid blockIndex = maxBlockIndex; blockIndex.notNull();) {
				block_type& block = blocks[blockIndex];
				blockIndex = block.preBlock;
				size_t index = 0;
				map_type* map = block.map;
				while (index < block.size) {
					size_t bitmap = map->bitmap;
					if (map->hasFlag()) { //跳过
						index += bitmap * bitmap_size;
						map += bitmap;
					}
					else {
						while (true) {
							uint i = std::countr_zero(bitmap); //bit右边连续的0的个数
							if (i < bitmap_size) {
								func(block.start[index + i], blockIndex);
								bitmap &= bitmap - 1; //消掉最靠近右边的1
							}
							else {
								break;
							}
						}
						index += bitmap_size;
						++map;
					}
				}
			}
		}

		friend std::ostream& operator<<(std::ostream& os, const _obj_pool_mem& a) {
			size_t printCount = 0;
			os << "data: [";
			for (ucid blockIndex = maxBlockIndex; blockIndex.notNull();) {
				block_type& block = blocks[blockIndex];
				blockIndex = block.preBlock;
				size_t index = 0;
				map_type* map = block.map;
				while (index < block.size) {
					size_t bitmap = map->bitmap;
					if (map->hasFlag()) { //跳过
						index += bitmap * bitmap_size;
						map += bitmap;
					}
					else {
						while (true) {
							uint i = std::countr_zero(bitmap); //bit右边连续的0的个数
							if (i < bitmap_size) {
								os << "\n\t[" << printCount << "]:\t";
								StreamSmartPrint(os, block.start[index + i]);
								os << ",";
								++printCount;
								bitmap &= bitmap - 1; //消掉最靠近右边的1
							}
							else {
								break;
							}
						}
						index += bitmap_size;
						++map;
					}
				}
			}
			os << "\n]";
			return os;
		}

		friend constexpr Hash Hasher(const _obj_pool_mem& a) noexcept {
			Hash hash = Hasher(0);
			each([&](const T& elem) {
				hash.combineUnordered(elem);
			});
			return hash;
		}
	};
}