#pragma once
#include <dme/core/config.h>
namespace dme{

    //包装hash值，以及一些计算hash的算法
    struct Hash {
    public:
        ulong v; //存储hash值

        //隐式转换
        constexpr operator ulong() const {
            return v;
        }
        //显式构造
        constexpr explicit Hash(ulong hash) noexcept : v(hash) {}

        //计算所有参数的hash值并按顺序合并
        template<typename FirstArg, typename... Args>
        static constexpr Hash Combine(const FirstArg& first, const Args&... args) noexcept {
            ulong hash = Hasher(first).v;
            ((hash ^= (Hasher(args).v + 0x9e3779b9ULL + (hash << 6) + (hash >> 2))), ...);
            return Hash(hash);
        }

        //计算所有参数的hash值并进行顺序无关的合并 //TODO 需使用更复杂的hash方法优化hash的质量
        template<typename FirstArg, typename... Args>
        static constexpr Hash CombineUnordered(const FirstArg& first, const Args&... args) noexcept {
            ulong hash = Hasher(first).v;
            ((hash += Hasher(args).v), ...);
            return Hash(hash);
        }

        //将其他hash值合并到自身，返回自己
        template<typename... Args>
        constexpr Hash& combine(const Args&... args) noexcept {
            ((v ^= (Hasher(args).v + 0x9e3779b9ULL + (v << 6) + (v >> 2))), ...);
            return *this;
        }

        //与其他hash值进行顺序无关的合并，返回自己 //TODO 需使用更复杂的hash方法优化hash的质量
        template<typename... Args>
        constexpr Hash& combineUnordered(const Args&... args) noexcept {
            ((v += Hasher(args).v), ...);
            return *this;
        }

        friend std::ostream& operator<<(std::ostream& os, const Hash& a) {
            auto flags = os.flags();
            os << std::hex;
            os << a.v;
            os.flags(flags);
            return os;
        }
    private:
        static constexpr ulong PRIME1 = 11400714785074694791ULL;
        static constexpr ulong PRIME2 = 14029467366897019727ULL;
        static constexpr ulong PRIME3 = 1609587929392839161ULL;
        static constexpr ulong PRIME4 = 9650029242287828579ULL;
        static constexpr ulong PRIME5 = 2870177450012600261ULL;

        static constexpr ulong default_seed = 2015331143067805392ULL;

        //读取4字节数据并转换为uint，注意大小端问题，默认是小端
        static constexpr uint endian32(const char* p) noexcept {
            if consteval {
                return uint(p[0]) | (uint(p[1]) << 8)
                    | (uint(p[2]) << 16) | (uint(p[3]) << 24);
            }
            else {
                return *reinterpret_cast<const uint*>(p);
            }
        }
	    //读取8字节数据并转换为ulong，注意大小端问题，默认是小端
        static constexpr ulong endian64(const char* p) noexcept {
            if consteval {
                return ulong(p[0]) | (ulong(p[1]) << 8)
                    | (ulong(p[2]) << 16) | (ulong(p[3]) << 24)
                    | (ulong(p[4]) << 32) | (ulong(p[5]) << 40)
                    | (ulong(p[6]) << 48) | (ulong(p[7]) << 56);
            }
            else {
                return *reinterpret_cast<const ulong*>(p);
            }
        }
        //循环左移
        static constexpr ulong rotl(ulong x, sint r) {
            return (x << r) | (x >> (64 - r));
        }
        static constexpr ulong mix1(const ulong h, const ulong prime, sint rshift) noexcept {
            return (h ^ (h >> rshift)) * prime;
        }
        static constexpr ulong mix2(const ulong p, const ulong v = 0) noexcept {
            return rotl(v + p * PRIME2, 31) * PRIME1;
        }
        static constexpr ulong mix3(const ulong h, const ulong v) noexcept {
            return (h ^ mix2(v)) * PRIME1 + PRIME4;
        }

        static constexpr ulong fetch64(const char* p, const ulong v = 0) noexcept {
            return mix2(endian64(p), v);
        }
        static constexpr ulong fetch32(const char* p) noexcept {
            return endian32(p) * PRIME1;
        }
        static constexpr ulong fetch8(const char* p) noexcept {
            return uchar(*p) * PRIME5;
        }
        static constexpr Hash xxh64impl(const char* p, ulong len, ulong seed) {
            ulong l = len;
            if (len >= 32) {
                ulong v1 = seed + PRIME1 + PRIME2;
                ulong v2 = seed + PRIME2;
                ulong v3 = seed;
                ulong v4 = seed - PRIME1;
                do {
                    v1 = fetch64(p, v1);
                    v2 = fetch64(p + 8, v2);
                    v3 = fetch64(p + 16, v3);
                    v4 = fetch64(p + 24, v4);
                    p += 32;
                    l -= 32;
                } while (l >= 32);
                seed = rotl(v1, 1) + rotl(v2, 7) + rotl(v3, 12) + rotl(v4, 18);
                seed = mix3(seed, v1);
                seed = mix3(seed, v2);
                seed = mix3(seed, v3);
                seed = mix3(seed, v4);
            }
            else {
                seed += PRIME5;
            }
            seed += len;
            while (l >= 8) {
                seed = rotl(seed ^ fetch64(p), 27) * PRIME1 + PRIME4;
                p += 8;
                l -= 8;
            }
            while (l >= 4) {
                seed = rotl(seed ^ fetch32(p), 23) * PRIME2 + PRIME3;
                p += 4;
                l -= 4;
            }
            while (l > 0) {
                seed = rotl(seed ^ fetch8(p), 11) * PRIME1;
                p++;
                l--;
            }
            seed = mix1(seed, PRIME2, 33);
            seed = mix1(seed, PRIME3, 29);
            seed = mix1(seed, 1, 32);
            return Hash(seed);
        }

    public:
        //xxhash64算法，返回64位hash值，参数p为输入数据指针，len为输入数据长度，seed为可选的种子值默认为default_seed
        static constexpr Hash XXH64(const char* ptr, ulong len, ulong seed = default_seed) noexcept {
            return xxh64impl(ptr, len, seed);
        }
	    //xxhash64算法，返回64位hash值，参数p为输入数据指针，len为输入数据长度，seed为可选的种子值默认为default_seed
        static Hash XXH64(const void* ptr, ulong len, ulong seed = default_seed) noexcept {
            return xxh64impl(reinterpret_cast<const char*>(ptr), len, seed);
        }

    private:
        //#if defined(_WIN64)
        static constexpr size_t _FNV_offset_basis_ = 14695981039346656037Ui64;
        static constexpr size_t _FNV_prime_ = 1099511628211Ui64;
        //#else
        //inline constexpr size_t _FNV_offset_basis_ = 2166136261U;
        //inline constexpr size_t _FNV_prime_ = 16777619U;
        //#endif
    public:
	    //FNV-1a算法
        template <class T>
        static constexpr Hash FNV1a(const T& object) noexcept {
            const uchar* first = &reinterpret_cast<const uchar&>(object);
            ulong hash = _FNV_offset_basis_;
            for (size_t i = 0; i < sizeof(T); ++i) {
                hash = (hash ^ static_cast<ulong>(first[i])) * _FNV_prime_;
            }
            return Hash(hash);
        }
	    //FNV-1a算法，处理数组，参数object为数组指针，length为数组长度
        template <class T>
        static constexpr Hash FNV1a(const T* object, const size_t length) noexcept {
            if (object == null || length == 0) {
                return 0;
            }
            const uchar* first = reinterpret_cast<const uchar*>(object);
            size_t size = sizeof(T) * length;
            ulong hash = _FNV_offset_basis_;
            for (size_t i = 0; i < size; ++i) {
                hash ^= static_cast<ulong>(first[i]);
                hash *= _FNV_prime_;
            }
            return Hash(hash);
        }
    };

    //FNV-1a特化string
    template <>
    inline constexpr Hash Hash::FNV1a<stdstr>(const stdstr& object) noexcept {
        const char* first = object.c_str();
        size_t size = object.size();
        ulong hash = _FNV_offset_basis_;
        for (size_t i = 0; i < size; ++i) {
            hash ^= static_cast<ulong>(first[i]);
            hash *= _FNV_prime_;
        }
        return Hash(hash);
    }

    //计算c风格字符串长度，不包括结尾的'\0'
    constexpr size_t GetCStrLen(const char* str) noexcept {
        size_t len = 0;
        while (str[len] != '\0') {
            ++len;
        }
        return len;
    }

    /*
    // 计算hash函数，如果传入参数是Hash类，则直接返回
    // 在类里只需要定义这个友元函数即可支持Hash（利用ADL机制）
    friend constexpr Hash Hasher(const Float3& a) noexcept {
        return Hash::Combine(a.x, a.y, a.z);
    }
    */


    constexpr Hash Hasher(const char* s, size_t len) noexcept {
        return Hash::XXH64(s, len);
    }
    template<size_t N>
    constexpr bool operator==(const char(&a)[N], const stdstr& s) noexcept {
        size_t len = N - 1;
	    if (len != s.size()) {
            return false;
        }
        return MemcmpByte(&a, s.c_str(), len) == 0;
    }

    //默认hash方法
    template<typename T>
    constexpr Hash Hasher(const T& a) noexcept {
        if constexpr (std::is_same_v<Hash, remove_rcv<T>>) { // Hash类型
            return a;
        }
        else if constexpr (IsFloat<remove_rcv<T>>) { // 浮点类型，解决正负0hash不一致的问题
            return Hash::FNV1a(a == 0 ? 0 : a);
        }
        else if constexpr (std::is_same_v<char, std::remove_extent_t<T>> && std::is_array_v<T>) { // c风格字符串字面量类型
            return Hasher(a, sizeof(T) - 1);
        }
        else if constexpr (std::is_same_v<const char*, remove_rcv<T>>) { // c风格字符串类型
		    return Hasher(a, GetCStrLen(a));
        }
        else if constexpr (std::is_same_v<stdstr, remove_rcv<T>>) { // std::string类型
            return Hasher(a.c_str(), a.size());
        }
        else if constexpr (sizeof(T) < 32) { // 字节大小小于32的类型
            return Hash::FNV1a(a);
        }
        else {
            return Hash::XXH64(std::addressof(a), sizeof(T));
        }
    }

}