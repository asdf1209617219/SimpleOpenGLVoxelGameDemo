#pragma once
#include <dme/core.h>
namespace dme::ui {
	struct UIMeshKey {
		sint zIndex;
		uint meshId;

		constexpr UIMeshKey() noexcept : zIndex(0), meshId(0) {}
		constexpr UIMeshKey(sint zIndex, uint meshId) noexcept : zIndex(zIndex), meshId(meshId) {}

		static constexpr UIMeshKey MinKey() noexcept {
			return UIMeshKey(sint_min, uint_min);
		}

		constexpr bool operator ==(const UIMeshKey& a) const noexcept {
			return zIndex == a.zIndex && meshId == a.meshId;
		}
		constexpr bool operator <(const UIMeshKey& a) const noexcept {
			return zIndex < a.zIndex || (zIndex == a.zIndex && meshId < a.meshId);
		}
		constexpr bool operator <=(const UIMeshKey& a) const noexcept {
			return zIndex < a.zIndex || (zIndex == a.zIndex && meshId <= a.meshId);
		}
	};
}