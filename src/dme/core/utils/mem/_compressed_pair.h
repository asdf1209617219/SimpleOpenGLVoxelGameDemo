#pragma once
#include <dme/core/utils.h>
namespace dme::core {
	//键值对，K为空类时进行空基类优化，仅在容器内部使用
	template <class K, class V>
	class _compressed_pair : private K {
	public:
		using key_type = K;
		using val_type = V;

		V val;

		template <class KT = K, class VT = V> requires(std::is_constructible_v<K, KT>&& std::is_constructible_v<V, VT>)
			constexpr _compressed_pair(KT&& key_, VT&& val_) noexcept(std::is_nothrow_constructible_v<K, KT>&& std::is_nothrow_constructible_v<V, VT>)
			: K(std::forward<KT>(key_)), val(std::forward<VT>(val_)) {}

		constexpr K& key() noexcept {
			return *this;
		}

		constexpr const K& key() const noexcept {
			return *this;
		}
	};
}
