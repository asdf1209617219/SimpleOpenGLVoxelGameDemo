#pragma once
#include <dme/core/utils.h>
namespace dme::core {

	template <typename C>
	class _array_const_iterator {
	public:
		using iterator_type = _array_const_iterator;
		using iterator_category = std::random_access_iterator_tag;
		using container = C;

		using value_type = container::value_type;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		using reference = value_type&;
		using const_reference = const value_type&;

		using size_type = container::size_type;
		using difference_type = container::difference_type;
	protected:
		const container* p;
		size_type index;
	public:

		constexpr _array_const_iterator() noexcept : p(null), index(0) {}
		constexpr explicit _array_const_iterator(const container* p, size_type index = 0) noexcept : p(p), index(index) {}

		//注意isend时index的值>=count()，isbegin时index的值为0
		constexpr bool isbegin() const noexcept {
			DME_ASSERT(p != null, "called on invalidated iterator");
			return index == 0;
		}
		//注意isend时index的值>=count()，isbegin时index的值为0
		constexpr bool isend() const noexcept {
			DME_ASSERT(p != null, "called on invalidated iterator");
			return index >= p->count();
		}
		//获取原始元素指针
		constexpr const_pointer ptr() const noexcept {
			DME_ASSERT(p != null, "called on invalidated iterator");
			return p->data() + index;
		}
		constexpr const_reference operator*() const noexcept {
			DME_ASSERT(p != null, "called on invalidated iterator");
			return (*p)[index];
		}
		constexpr const_pointer operator->() const noexcept {
			DME_ASSERT(p != null, "called on invalidated iterator");
			return p->data() + index;
		}
		constexpr iterator_type& operator++() noexcept {
			++index;
			return *this;
		}
		constexpr iterator_type& operator--() noexcept {
			--index;
			return *this;
		}
		constexpr iterator_type operator++(int) noexcept {
			iterator_type temp = *this;
			++*this;
			return temp;
		}
		constexpr iterator_type operator--(int) noexcept {
			iterator_type temp = *this;
			--*this;
			return temp;
		}
		constexpr iterator_type& operator+=(const difference_type offset) noexcept {
			index += offset;
			return *this;
		}
		constexpr iterator_type& operator-=(const difference_type offset) noexcept {
			index -= offset;
			return *this;
		}
		constexpr iterator_type operator+(const difference_type offset) const noexcept {
			iterator_type temp = *this;
			temp += offset;
			return temp;
		}
		constexpr iterator_type operator-(const difference_type offset) const noexcept {
			iterator_type temp = *this;
			temp -= offset;
			return temp;
		}

		friend constexpr iterator_type operator+(const difference_type offset, iterator_type it) noexcept {
			it += offset;
			return it;
		}
		constexpr difference_type operator-(const iterator_type& it) const noexcept {
			DME_ASSERT(p == it.p, "iterators incompatible");
			return index - it.index;
		}
		constexpr const_reference operator[](const difference_type index_) const noexcept {
			return *(*this + index_);
		}
		constexpr bool operator==(const iterator_type& it) const noexcept {
			DME_ASSERT(p == it.p, "iterators incompatible");
			return index == it.index;
		}
		constexpr std::strong_ordering operator<=>(const iterator_type& it) const noexcept {
			DME_ASSERT(p == it.p, "iterators incompatible");
			if (index < it.index) return std::strong_ordering::less;
			if (index == it.index) return std::strong_ordering::equal;
			return std::strong_ordering::greater;
		}
	};

	template <typename C>
	class _array_iterator : public _array_const_iterator<C> {
	public:
		using base_type = _array_const_iterator<C>;
		using base_type::base_type;

		using iterator_type = _array_iterator;
		using iterator_category = base_type::iterator_category;
		using container = C;

		using value_type = base_type::value_type;
		using pointer = base_type::pointer;
		using const_pointer = base_type::const_pointer;
		using reference = base_type::reference;
		using const_reference = base_type::const_reference;

		using size_type = base_type::size_type;
		using difference_type = base_type::difference_type;

		constexpr pointer ptr() const noexcept {
			return const_cast<pointer>(base_type::ptr());
		}
		constexpr reference operator*() const noexcept {
			return const_cast<reference>(base_type::operator*());
		}
		constexpr pointer operator->() const noexcept {
			return const_cast<pointer>(base_type::operator->());
		}
		constexpr iterator_type& operator++() noexcept {
			base_type::operator++();
			return *this;
		}
		constexpr iterator_type& operator--() noexcept {
			base_type::operator--();
			return *this;
		}
		constexpr iterator_type operator++(int) noexcept {
			iterator_type temp = *this;
			base_type::operator++();
			return temp;
		}
		constexpr iterator_type operator--(int) noexcept {
			iterator_type temp = *this;
			base_type::operator--();
			return temp;
		}
		constexpr iterator_type& operator+=(const difference_type offset) noexcept {
			base_type::operator+=(offset);
			return *this;
		}
		constexpr iterator_type& operator-=(const difference_type offset) noexcept {
			base_type::operator-=(offset);
			return *this;
		}
		constexpr iterator_type operator+(const difference_type offset) const noexcept {
			iterator_type temp = *this;
			temp += offset;
			return temp;
		}
		constexpr iterator_type operator-(const difference_type offset) const noexcept {
			iterator_type temp = *this;
			temp -= offset;
			return temp;
		}

		friend constexpr iterator_type operator+(const difference_type offset, iterator_type it) noexcept {
			it += offset;
			return it;
		}
		constexpr difference_type operator-(const iterator_type& it) const noexcept {
			return base_type::operator-(it);
		}
		constexpr reference operator[](const difference_type index_) const noexcept {
			return const_cast<reference>(base_type::operator[](index_));
		}
		constexpr bool operator==(const iterator_type& it) const noexcept {
			return base_type::operator==(it);
		}
		constexpr std::strong_ordering operator<=>(const iterator_type& it) const noexcept {
			return base_type::operator<=>(it);
		}
	};


	template <typename C>
	class _tree_const_iterator {
	public:
		using iterator_type = _tree_const_iterator;
		using iterator_category = std::bidirectional_iterator_tag;

		using container = C;
		using mem_type = container::mem_type;
		using node_type = container::node_type;
		static constexpr bool SOO = node_type::SOO;
	protected:
		const mem_type* mem;
		const node_type* node;
	public:
		using value_type = node_type::value_type;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		using reference = value_type&;
		using const_reference = const value_type&;

		constexpr _tree_const_iterator() noexcept : mem(null), node(null) {}
		constexpr _tree_const_iterator(const mem_type* mem, const node_type* node) noexcept : mem(mem), node(node) {}

		constexpr bool isbegin() const noexcept {
			DME_ASSERT(mem != null, "called on invalidated iterator");
			return node == mem->firstNode();
		}
		constexpr bool isend() const noexcept {
			DME_ASSERT(mem != null, "called on invalidated iterator");
			return node == mem_type::end_node;
		}
		constexpr const_pointer ptr() const noexcept {
			return node ? node->getDataPtr() : null;
		}
		constexpr const_reference operator*() const noexcept {
			return *(node->getDataPtr());
		}
		constexpr const_pointer operator->() const noexcept {
			return node->getDataPtr();
		}
		constexpr iterator_type& operator++() noexcept {
			DME_ASSERT(mem != null, "called on invalidated iterator");
			node = mem->nodeNext(const_cast<node_type*>(node));
			return *this;
		}
		constexpr iterator_type& operator--() noexcept {
			DME_ASSERT(mem != null, "called on invalidated iterator");
			node = mem->nodePrevious(const_cast<node_type*>(node));
			return *this;
		}
		constexpr iterator_type operator++(int) noexcept {
			iterator_type temp = *this;
			++*this;
			return temp;
		}
		constexpr iterator_type operator--(int) noexcept {
			iterator_type temp = *this;
			--*this;
			return temp;
		}

		constexpr bool operator==(const iterator_type& it) const noexcept {
			DME_ASSERT(mem == it.mem, "iterators incompatible");
			return node == it.node;
		}
		constexpr std::strong_ordering operator<=>(const iterator_type& it) const noexcept {
			DME_ASSERT(mem == it.mem, "iterators incompatible");
			if (node == it.node) return std::strong_ordering::equal;
			if (node != mem_type::end_node && (it.node == mem_type::end_node || *node->getDataPtr() < *it.node->getDataPtr())) return std::strong_ordering::less;
			return std::strong_ordering::greater;
		}
	};

	template <typename C>
	class _tree_iterator : public _tree_const_iterator<C> {
	public:
		using base_type = _tree_const_iterator<C>;
		using base_type::base_type;
		using iterator_type = _tree_iterator;
		using iterator_category = base_type::iterator_category;

		using container = C;
		using node_type = container::node_type;
		static constexpr bool SOO = node_type::SOO;

		using value_type = base_type::value_type;
		using pointer = base_type::pointer;
		using const_pointer = base_type::const_pointer;
		using reference = base_type::reference;
		using const_reference = base_type::const_reference;

		constexpr pointer ptr() const noexcept {
			return const_cast<pointer>(base_type::ptr());
		}
		constexpr reference operator*() const noexcept {
			return const_cast<reference>(base_type::operator*());
		}
		constexpr pointer operator->() const noexcept {
			return const_cast<pointer>(base_type::operator->());
		}
		constexpr iterator_type& operator++() noexcept {
			base_type::operator++();
			return *this;
		}
		constexpr iterator_type& operator--() noexcept {
			base_type::operator--();
			return *this;
		}
		constexpr iterator_type operator++(int) noexcept {
			iterator_type temp = *this;
			base_type::operator++();
			return temp;
		}
		constexpr iterator_type operator--(int) noexcept {
			iterator_type temp = *this;
			base_type::operator--();
			return temp;
		}
		constexpr bool operator==(const iterator_type& it) const noexcept {
			return base_type::operator==(it);
		}
		constexpr std::strong_ordering operator<=>(const iterator_type& it) const noexcept {
			return base_type::operator<=>(it);
		}
	};


	template <typename C>
	class _obj_pool_const_iterator {
	public:
		using iterator_type = _obj_pool_const_iterator;
		using iterator_category = std::forward_iterator_tag;
		using container = C;

		using value_type = container::value_type;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		using reference = value_type&;
		using const_reference = const value_type&;

		using size_type = container::size_type;
		using difference_type = container::difference_type;

		using comp_ptr = container::comp_ptr;
		using map_type = container::map_type;
		using block_type = container::block_type;
		static constexpr size_t bitmap_size = container::bitmap_size;
	protected:
		const container* p;
		ucid blockIndex;
		uint bitmapIndex;
		size_t mapIndex;
	public:
		constexpr _obj_pool_const_iterator() noexcept : p(null), blockIndex(), bitmapIndex(0), mapIndex(0) {}
		constexpr explicit _obj_pool_const_iterator(const container* p) noexcept : p(p), blockIndex(), bitmapIndex(0), mapIndex(0)
		{
			if (p->blocksCount == 0) {
				return;
			}
			for (blockIndex = p->maxBlockIndex; blockIndex.notNull();) {
				block_type& block = p->blocks[blockIndex];
				size_t index = 0;
				map_type* map = block.map;
				while (index < block.size) {
					size_t bitmap = map->bitmap;
					if (map->hasFlag()) { //跳过
						index += bitmap * bitmap_size;
						map += bitmap;
					}
					else {
						uint s = std::countr_zero(bitmap); //bit右边连续的0的个数
						if (s < bitmap_size) {
							index += s;
							bitmapIndex = index % bitmap_size;
							mapIndex = map - block.map;
							return;
						}
						index += bitmap_size;
						++map;
					}
				}
				blockIndex = block.preBlock;
			}
		}
		constexpr _obj_pool_const_iterator(const container* p, ucid blockIndex) noexcept
			: p(p), blockIndex(blockIndex), bitmapIndex(0), mapIndex(0) {
		}

		constexpr bool isbegin() const noexcept {
			DME_ASSERT(p != null, "called on invalidated iterator");
			return blockIndex == p->maxBlockIndex && bitmapIndex = 0 && mapIndex = 0;
		}
		constexpr bool isend() const noexcept {
			DME_ASSERT(p != null, "called on invalidated iterator");
			return blockIndex.isNull();
		}
		constexpr const_pointer ptr() const noexcept {
			DME_ASSERT(p != null && blockIndex.notNull(), "called on invalidated iterator");
			return p->blocks[blockIndex].start + bitmapIndex + mapIndex * bitmap_size;
		}

		constexpr const_reference operator*() const noexcept {
			DME_ASSERT(p != null && blockIndex.notNull(), "called on invalidated iterator");
			return *(p->blocks[blockIndex].start + bitmapIndex + mapIndex * bitmap_size);
		}
		constexpr const_pointer operator->() const noexcept {
			DME_ASSERT(p != null && blockIndex.notNull(), "called on invalidated iterator");
			return p->blocks[blockIndex].start + bitmapIndex + mapIndex * bitmap_size;
		}
		constexpr iterator_type& operator++() noexcept {
			while (blockIndex.notNull()) {
				block_type& block = p->blocks[blockIndex];
				++bitmapIndex;
				if (bitmapIndex == bitmap_size) {
					bitmapIndex = 0;
					++mapIndex;
				}
				size_t index = mapIndex * bitmap_size + bitmapIndex;
				while (index < block.size) {
					map_type& map = block.map[mapIndex];
					size_t bitmap = map.bitmap;
					if (map.hasFlag()) { //跳过
						mapIndex += bitmap;
					}
					else {
						uint s = std::countr_zero(bitmap >> bitmapIndex); //bit右边连续的0的个数
						if (s < bitmap_size) {
							bitmapIndex += s;
							return *this;
						}
						++mapIndex;
					}
					bitmapIndex = 0;
					index = mapIndex * bitmap_size;
				}
				blockIndex = block.preBlock;
				bitmapIndex = 0;
				mapIndex = 0;
			}
			return *this;
		}
		constexpr iterator_type operator++(int) noexcept {
			iterator_type temp = *this;
			++*this;
			return temp;
		}

		constexpr bool operator==(const iterator_type& it) const noexcept {
			DME_ASSERT(p == it.p, "iterators incompatible");
			return blockIndex == it.blockIndex && mapIndex == it.mapIndex && bitmapIndex == it.bitmapIndex;
		}
		/*
		constexpr std::strong_ordering operator<=>(const iterator_type& it) const noexcept {
			DME_ASSERT(p == it.p, "iterators incompatible");
			if (blockIndex == it.blockIndex && mapIndex == it.mapIndex && bitmapIndex == it.bitmapIndex) return std::strong_ordering::equal;
			if (blockIndex < it.blockIndex
				|| (blockIndex == it.blockIndex && mapIndex < it.mapIndex)
				|| (blockIndex == it.blockIndex && mapIndex == it.mapIndex && bitmapIndex < it.bitmapIndex)
				) return std::strong_ordering::less;
			return std::strong_ordering::greater;
		}
		*/
	};

	template <typename C>
	class _obj_pool_iterator : public _obj_pool_const_iterator<C> {
	public:
		using base_type = _obj_pool_const_iterator<C>;
		using base_type::base_type;

		using iterator_type = _obj_pool_iterator;
		using iterator_category = base_type::iterator_category;
		using container = C;

		using value_type = base_type::value_type;
		using pointer = base_type::pointer;
		using const_pointer = base_type::const_pointer;
		using reference = base_type::reference;
		using const_reference = base_type::const_reference;

		using size_type = base_type::size_type;
		using difference_type = base_type::difference_type;

		using comp_ptr = base_type::comp_ptr;
		using map_type = base_type::map_type;
		using block_type = base_type::block_type;
		static constexpr size_t bitmap_size = base_type::bitmap_size;
	public:
		constexpr pointer ptr() const noexcept {
			return const_cast<pointer>(base_type::ptr());
		}
		constexpr reference operator*() const noexcept {
			return const_cast<reference>(base_type::operator*());
		}
		constexpr pointer operator->() const noexcept {
			return const_cast<pointer>(base_type::operator->());
		}
		constexpr iterator_type& operator++() noexcept {
			base_type::operator++();
			return *this;
		}
		constexpr iterator_type operator++(int) noexcept {
			iterator_type temp = *this;
			base_type::operator++();
			return temp;
		}

		constexpr bool operator==(const iterator_type& it) const noexcept {
			return base_type::operator==(it);
		}
		/*
		constexpr std::strong_ordering operator<=>(const iterator_type& it) const noexcept {
			return base_type::operator<=>(it);
		}
		*/
	};



	template <typename C>
	class _flat_hash_mem_const_iterator {
	public:
		using iterator_type = _flat_hash_mem_const_iterator;
		using iterator_category = std::forward_iterator_tag;
		using container = C;

		using value_type = container::value_type;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		using reference = value_type&;
		using const_reference = const value_type&;

		using size_type = container::size_type;
		using difference_type = container::difference_type;

		using mem_type = container::mem_type;
		using group_type = mem_type::group_type;
	protected:
		const mem_type* p;
		size_type index;
	public:
		constexpr _flat_hash_mem_const_iterator() noexcept : p(null), index(0) {}
		constexpr _flat_hash_mem_const_iterator(const mem_type* p) noexcept : p(p), index(0)
		{
			ulong size = p->size;
			for (; index < size; index += mem_type::group_byte) {
				group_type group = *reinterpret_cast<const group_type*>(p->ctrls + index);
				group = ~group & mem_type::check_mask; //查找字节最高位是0的
				uint c = std::countr_zero(group);
				if (c < mem_type::group_bit) {
					index = index + (c / mem_type::group_byte);
					if (index >= size) {
						index = size;
					}
					return;
				}
			}
			index = size;
		}
		constexpr _flat_hash_mem_const_iterator(const mem_type* p, size_t index) noexcept
			: p(p), index(index) {
		}

		constexpr bool isbegin() const noexcept {
			DME_ASSERT(p != null, "called on invalidated iterator");
			return index == 0;
		}
		constexpr bool isend() const noexcept {
			DME_ASSERT(p != null, "called on invalidated iterator");
			return index >= p->size;
		}
		constexpr const_pointer ptr() const noexcept {
			DME_ASSERT(p != null, "called on invalidated iterator");
			return p->slots + index;
		}
		constexpr const_reference operator*() const noexcept {
			DME_ASSERT(p != null, "called on invalidated iterator");
			return p->slots[index];
		}
		constexpr const_pointer operator->() const noexcept {
			DME_ASSERT(p != null, "called on invalidated iterator");
			return p->slots + index;
		}
		constexpr iterator_type& operator++() noexcept {
			DME_ASSERT(p != null, "called on invalidated iterator");
			++index;
			ulong size = p->size;
			for (; index < size; index += mem_type::group_byte) {
				group_type group = *reinterpret_cast<const group_type*>(p->ctrls + index);
				group = ~group & mem_type::check_mask; //查找字节最高位是0的
				uint c = std::countr_zero(group);
				if (c < mem_type::group_bit) {
					index = index + (c / mem_type::group_byte);
					if (index >= size) {
						index = size;
					}
					return *this;
				}
			}
			index = size;
			return *this;
		}
		constexpr iterator_type operator++(int) noexcept {
			iterator_type temp = *this;
			++*this;
			return temp;
		}

		constexpr bool operator==(const iterator_type& it) const noexcept {
			DME_ASSERT(p == it.p, "iterators incompatible");
			return index == it.index;
		}
	};

	template <typename C>
	class _flat_hash_mem_iterator : public _flat_hash_mem_const_iterator<C> {
	public:
		using base_type = _flat_hash_mem_const_iterator<C>;
		using base_type::base_type;

		using iterator_type = _flat_hash_mem_iterator;
		using iterator_category = base_type::iterator_category;
		using container = C;

		using value_type = base_type::value_type;
		using pointer = base_type::pointer;
		using const_pointer = base_type::const_pointer;
		using reference = base_type::reference;
		using const_reference = base_type::const_reference;

		using size_type = base_type::size_type;
		using difference_type = base_type::difference_type;

		using mem_type = base_type::mem_type;
		using group_type = base_type::group_type;
	public:
		constexpr pointer ptr() const noexcept {
			return const_cast<pointer>(base_type::ptr());
		}
		constexpr reference operator*() const noexcept {
			return const_cast<reference>(base_type::operator*());
		}
		constexpr pointer operator->() const noexcept {
			return const_cast<pointer>(base_type::operator->());
		}
		constexpr iterator_type& operator++() noexcept {
			base_type::operator++();
			return *this;
		}
		constexpr iterator_type operator++(int) noexcept {
			iterator_type temp = *this;
			base_type::operator++();
			return temp;
		}

		constexpr bool operator==(const iterator_type& it) const noexcept {
			return base_type::operator==(it);
		}
	};

}