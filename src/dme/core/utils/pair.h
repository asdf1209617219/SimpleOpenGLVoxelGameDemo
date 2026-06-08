#pragma once
#include <dme/core/utils.h>
namespace dme {
	//键值对，自动优化空类
	template<typename K, typename V>
	struct Pair {
		DME_NO_UNIQUE_ADDRESS K key;
		DME_NO_UNIQUE_ADDRESS V val;

		using key_type = K;
		using val_type = V;

		template<typename = void> requires(std::is_default_constructible_v<K> && std::is_default_constructible_v<V>)
		constexpr explicit(explicit_default_construct<K> && explicit_default_construct<V>)
		Pair() noexcept(nothrow_default_construct<K>&& nothrow_default_construct<V>)
		: key(), val() {}

		/*
		template <class KT = K, class VT = V> requires(std::is_copy_constructible_v<KT> && std::is_copy_constructible_v<VT>)
		constexpr explicit(!(std::is_convertible_v<const KT&, KT> && std::is_convertible_v<const VT&, VT>))
		Pair(const K& key_, const V& val_) noexcept(nothrow_copy_construct<KT> && nothrow_copy_construct<VT>)
		: key(key_), val(val_) {
		}*/

		template <class KT = K, class VT = V> requires(std::is_constructible_v<K, KT> && std::is_constructible_v<V, VT>)
		constexpr explicit(!(std::is_convertible_v<KT, K> && std::is_convertible_v<VT, V>))
		Pair(KT&& key_, VT&& val_) noexcept(std::is_nothrow_constructible_v<K, KT> && std::is_nothrow_constructible_v<V, VT>)
		: key(std::forward<KT>(key_)), val(std::forward<VT>(val_)) {}
		
		constexpr Pair(const Pair&) = default;
		constexpr Pair(Pair&&) = default;
		constexpr Pair& operator=(const Pair&) = default;
		constexpr Pair& operator=(Pair&&) = default;

		template <class KT = K, class VT = V> requires(std::is_constructible_v<K, KT&> && std::is_constructible_v<V, VT&>)
		constexpr explicit(!(std::is_convertible_v<KT&, K> && std::is_convertible_v<VT&, V>))
		Pair(Pair<KT, VT>& a) noexcept(std::is_nothrow_constructible_v<K, KT&> && std::is_nothrow_constructible_v<V, VT&>)
		: key(a.key), val(a.val) {}

		template <class KT = K, class VT = V> requires(std::is_constructible_v<K, const KT&> && std::is_constructible_v<V, const VT&>)
		constexpr explicit(!(std::is_convertible_v<const KT&, K> && std::is_convertible_v<const VT&, V>))
		Pair(const Pair<KT, VT>& a) noexcept(std::is_nothrow_constructible_v<K, const KT&> && std::is_nothrow_constructible_v<V, const VT&>)
		: key(a.key), val(a.val) {}

		template <class KT = K, class VT = V> requires(std::is_constructible_v<K, KT> && std::is_constructible_v<V, VT>)
		constexpr explicit(!(std::is_convertible_v<KT, K> && std::is_convertible_v<VT, V>))
		Pair(Pair<KT, VT>&& a) noexcept(std::is_nothrow_constructible_v<K, KT> && std::is_nothrow_constructible_v<V, VT>)
		: key(std::forward<KT>(a.key)), val(std::forward<VT>(a.val)) {}

		template <class KT = K, class VT = V> requires(std::is_constructible_v<K, const KT> && std::is_constructible_v<V, const VT>)
		constexpr explicit(!(std::is_convertible_v<const KT, K> && std::is_convertible_v<const VT, V>))
		Pair(const Pair<KT, VT>&& a) noexcept(std::is_nothrow_constructible_v<K, const KT> && std::is_nothrow_constructible_v<V, const VT>)
		: key(std::forward<const KT>(a.key)), val(std::forward<const VT>(a.val)) {}



		template <class KT, class VT> requires(std::is_assignable_v<K&, const KT&> && std::is_assignable_v<V&, const VT&>)
		constexpr Pair& operator=(const Pair<KT, VT>& a)
		noexcept(std::is_nothrow_assignable_v<K&, const KT&> && std::is_nothrow_assignable_v<V&, const VT&>) {
			if (this != std::addressof(a)) {
				key = a.key;
				val = a.val;
			}
			return *this;
		}
		//TODO 考虑参考std实现支持mutable的类型
		template <class KT, class VT> requires(std::is_assignable_v<K&, KT> && std::is_assignable_v<V&, VT>)
			constexpr Pair& operator=(Pair<KT, VT>&& a)
			noexcept(std::is_nothrow_assignable_v<K&, KT> && std::is_nothrow_assignable_v<V&, VT>) {
			if (this != std::addressof(a)) {
				key = std::forward<KT>(a.key);
				val = std::forward<VT>(a.val);
			}
			return *this;
		}

		//注意等于只比较key
		constexpr bool operator ==(const Pair& a) const noexcept {
			if (this == &a) {
				return true;
			}
			if constexpr (IsCanEqual<K>) {
				return key == a.key;
			}
			else {
				return Memcmp<K>(std::addressof(key), std::addressof(a.key)) == 0;
			}
		}
		//注意小于只比较key
		constexpr bool operator <(const Pair& a) const noexcept {
			if (this == &a) {
				return false;
			}
			if constexpr (IsCanLess<K>) {
				return key < a.key;
			}
			else {
				return Memcmp<K>(std::addressof(key), std::addressof(a.key)) < 0;
			}
		}

		friend std::ostream& operator<<(std::ostream& os, const Pair& a) {
			os << "{ key:\t";
			StreamSmartPrint(os, a.key);
			os << ",\tval:\t";
			StreamSmartPrint(os, a.val);
			os << "}";
			return os;
		}
		//只对key进行hash
		friend constexpr Hash Hasher(const Pair& a) noexcept {
			return Hasher(a.key);
		}
	};

}

namespace std {
	template<typename K, typename V>
	struct hash<dme::Pair<K, V>> {
		inline size_t operator ()(const dme::Pair<K, V>& a) const noexcept {
			return Hasher(a);
		}
	};
}