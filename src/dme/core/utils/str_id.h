#pragma once
#include <dme/core/utils/obj_pool.h>
#include <dme/core/utils/hash_set.h>

namespace dme {
	class strid;
	namespace core {
		struct _hash_str {
			const Hash hash;
			const stdstr str;
			constexpr _hash_str(const stdstr& s) noexcept : hash(Hasher(s)), str(s) {}
			constexpr _hash_str(stdstr&& s) noexcept : hash(Hasher(s)), str(std::move(s)) {}
			constexpr _hash_str(Hash hash, const stdstr& s) noexcept : hash(hash), str(s) {}
			constexpr _hash_str(Hash hash, stdstr&& s) noexcept : hash(hash), str(std::move(s)) {}

			constexpr bool operator ==(const _hash_str& a) const noexcept {
				return hash == a.hash && str == a.str;
			}
			friend constexpr Hash Hasher(const _hash_str& a) noexcept {
				return a.hash;
			}
		};

		class _hash_str_pool {
		private:
			friend strid;

			inline static HashSet<_hash_str> strSet = []() noexcept {
				HashSet<_hash_str> set = HashSet<_hash_str>(256);
				set.add(_hash_str(""));
				return set;
			}();

			inline static const _hash_str* get(const stdstr& s) noexcept {
				return strSet.get(_hash_str(s));
			}

			inline static _hash_str* addIfAbsent(Hash h, const stdstr& s) noexcept {
				return strSet.addIfAbsent(_hash_str(h, s));
			}
			inline static _hash_str* addIfAbsent(Hash h, stdstr&& s) noexcept {
				return strSet.addIfAbsent(_hash_str(h, std::move(s)));
			}
		};
	}

	//指向字符串和hash，相同字符串只保存一个实例。目前只保存只读字符串，暂不支持修改字符串内容
	class strid {
	private:
		const core::_hash_str* ptr;
	public:
		template<size_t N>
		strid(const char(&s)[N]) noexcept {
			ptr = core::_hash_str_pool::addIfAbsent(Hasher(s, N - 1), stdstr(s, N - 1));
		}
		strid() noexcept : ptr(core::_hash_str_pool::get("")) {}
		strid(const stdstr& s) noexcept {
			ptr = core::_hash_str_pool::addIfAbsent(Hasher(s), s);
		}
		strid(stdstr&& s) noexcept {
			ptr = core::_hash_str_pool::addIfAbsent(Hasher(s), std::move(s));
		}


		constexpr Hash hash() const noexcept {
			return ptr->hash;
		}
		constexpr const stdstr& str() const noexcept {
			return ptr->str;
		}
		constexpr const char* cstr() const noexcept {
			return ptr->str.c_str();
		}

		constexpr bool operator ==(strid a) const noexcept {
			return ptr == a.ptr;
		}
		constexpr strid& operator =(strid a) noexcept {
			ptr = a.ptr;
			return *this;
		}

		strid operator+(strid a) const noexcept {
			return strid(str() + a.str());
		}
		strid& operator+=(strid a) noexcept {
			return *this = strid(str() + a.str());
		}
		strid operator+(const stdstr& a) const noexcept {
			return strid(str() + a);
		}
		strid& operator+=(const stdstr& a) noexcept {
			return *this = strid(str() + a);
		}
		strid operator+(const char* a) const noexcept {
			return strid(str() + a);
		}
		strid& operator+=(const char* a) noexcept {
			return *this = strid(str() + a);
		}

		friend strid operator+(const stdstr& a, strid b) noexcept {
			return strid(a + b.str());
		}
		friend strid operator+(const char* a, strid b) noexcept {
			return strid(a + b.str());
		}

		friend constexpr Hash Hasher(const strid& a) noexcept {
			return a.ptr->hash;
		}
		friend std::ostream& operator<<(std::ostream& os, const strid& a) {
			return os << a.str();
		}
	};
}
