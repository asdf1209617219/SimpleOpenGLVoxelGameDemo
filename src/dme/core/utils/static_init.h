#pragma once
#include <dme/core/utils/flat_map.h>
#include <dme/core/utils/obj_pool.h>
namespace dme {
	template<typename T>
	class _StaticInitByte;

	namespace core {
		class _static_init_func_ {
		public:
			static inline auto& _map() noexcept {
				static auto map = FlatMap<void*, uid>(256);
				return map;
			}
			static inline auto& _arr() noexcept {
				static auto arr = DynArr<Pair<void*, void(*)(void*)>>(256);
				return arr;
			}

			static void _add(void* ptr, void(*func)(void*)) noexcept {
				_map().addIfAbsent(ptr, _arr().count());
				_arr().add({ ptr, func });
			}
			static uid _getIndex(void* ptr) noexcept {
				auto r = _map().get(ptr);
				if (r) {
					return r->val;
				}
				return uid::Null;
			}
			static auto& _get(uint i) noexcept {
				return _arr()[i];
			}

			//由Game调用该方法
			static void _init_all_var() {
				auto& arr = _arr();
				uint c = arr.count();
				for (uint i = 0; i < c; ++i) {
					auto& elem = arr[i];
					if (elem.key) {
						elem.val(elem.key);
						elem.key = null;
						_map().remove(elem.key);
					}
				}
				_map().release();
				_arr().release();
			}

			static void _call(void* ptr) {
				auto i = _getIndex(ptr);
				if (i.notNull()) {
					auto& elem = _get(i);
					if (elem.key) {
						elem.val(elem.key);
						elem.key = null;
					}
					_map().remove(ptr);
				}
			}
			/*
			static void _call(const void* ptr) {
				auto i = _getIndex(const_cast<void*>(ptr));
				if (i.notNull()) {
					auto& elem = _get(i);
					if (elem.key) {
						elem.val(elem.key);
						elem.key = null;
					}
					_map().remove(const_cast<void*>(ptr));
				}
			}
			*/

			/*
			template<typename P>
			static P _init(P param) {
				_call(std::addressof(param));
				std::forward();
				return param;
			}
			*/

			template<typename P>
			static auto _init(P&& param) -> decltype(std::forward<P>(param)) {
				if constexpr (std::is_reference_v<P>) {
					const void* ptr = std::addressof(param);
					_call(const_cast<void*>(ptr));
					return std::forward<P>(param);
				}
				else {
					return std::forward<P>(param);
				}
			}
		};
	}

	//宏DME_STATIC_INIT的工具类，用于占用静态变量初始化的内存
	template<typename T>
	class _StaticInitByte {
		friend class core::_static_init_func_;
	private:
		static constexpr size_t _size = sizeof(T);

		uchar _byte[_size];
	public:
		explicit _StaticInitByte() noexcept : _byte{ 0 } {}

		T& ref() noexcept {
			return reinterpret_cast<T&>(*this);
		}
		T* ptr() noexcept {
			return reinterpret_cast<T*>(this);
		}
	};

	//#define _DME_SINIT_ARGS_(param) ( std::is_lvalue_reference_v<decltype(param)> ? core::_static_init_func_::_init<decltype(param)>(param) : (param))

	#define _DME_SINIT_ARGS_(param) core::_static_init_func_::_init(param)
	#define _DME_SINIT_CHECK_ARGS_(param) core::_static_init_func_::_init(param);

	// 通过在lambda内静态变量分配内存空间，然后返回对该空间的引用，初始化会推迟到Game::Init()中进行
	// 确定不会受到变量初始化顺序不一致影响的可以不用该宏（比如Singleton::GetInstance()）
	// 参数1：静态变量的类型，如：uint
	// 不定参数：构造函数的参数
	// 使用实例：在头文件的类声明中写：static inline auto& Var1 = DME_SINIT(uint, 1);
	#define DME_SINIT(type, ...) *[] { \
		using _Type_ = type; \
		static _StaticInitByte<_Type_> _obj; \
		core::_static_init_func_::_add( \
			std::addressof(_obj), \
			[](void* _var_) { \
				new (_var_) _Type_(DME_EVAL_ARGS(_DME_SINIT_ARGS_, __VA_ARGS__)); \
			} \
		); \
		return _obj.ptr(); \
	}()

	// 通过在lambda内静态变量分配内存空间，然后返回对该空间的引用，通过func的返回值对静态变量进行构造，初始化会推迟到Game::Init()中进行
	// 确定不会受到变量初始化顺序不一致影响的可以不用该宏（比如Singleton::GetInstance()）
	// 参数1：返回对象的函数名，如：Class1::Create
	// 不定参数：构造函数的参数
	// 使用实例：在头文件的类声明中写：static inline auto& Var1 = DME_SINIT_FUNC(Class1::Create, 1, 2);
	#define DME_SINIT_FUNC(func, ...) *[] { \
		using _Type_ = decltype(func(__VA_ARGS__)); \
		static _StaticInitByte<_Type_> _obj; \
		core::_static_init_func_::_add( \
			std::addressof(_obj), \
			[](void* _var_) { \
				new (_var_) _Type_(func(DME_EVAL_ARGS(_DME_SINIT_ARGS_, __VA_ARGS__))); \
			} \
		); \
		return _obj.ptr(); \
	}()

	// 通过ObjPool分配内存空间，然后返回对该空间的引用，初始化会推迟到Game::Init()中进行
	// 确定不会受到变量初始化顺序不一致影响的可以不用该宏（比如Singleton::GetInstance()）
	// 参数1：静态变量的类型，如：uint
	// 不定参数：构造函数的参数
	// 使用实例：在头文件的类声明中写：static inline auto& Var1 = DME_SINIT_POOL(uint, 1);
	#define DME_SINIT_POOL(type, ...) *[] { \
		using _Type_ = type; \
		_Type_* _ptr = ObjPool<_Type_>::Alloc(); \
		core::_static_init_func_::_add( \
			_ptr, \
			[](void* _var_) { \
				new (_var_) _Type_(DME_EVAL_ARGS(_DME_SINIT_ARGS_, __VA_ARGS__)); \
			} \
		); \
		return _ptr; \
	}()

	//在DME_SINIT_POOL_AFTER中指代对象的指针，使用实例：DME_SINIT_POOL_AFTER(uint, Class1::Init(DME_SINIT_PTR); , 1);
	#define DME_SINIT_PTR reinterpret_cast<_Type_*>(_var_)

	// 通过ObjPool分配内存空间，然后返回对该空间的引用，初始化会推迟到Game::Init()中进行，构造完对象后会执行函数，函数参数为对象的指针
	// 确定不会受到变量初始化顺序不一致影响的可以不用该宏（比如Singleton::GetInstance()）
	// 参数1：静态变量的类型，如：uint
	// 参数2：构造完成后执行的代码，如：Class1::Init();
	// 不定参数：构造函数的参数
	// 使用实例：在头文件的类声明中写：static inline auto& Var1 = DME_SINIT_POOL_AFTER(uint, Class1::Init();, 1);
	#define DME_SINIT_POOL_AFTER(type, afterDo, ...) *[] { \
		using _Type_ = type; \
		_Type_* _ptr = ObjPool<_Type_>::Alloc(); \
		core::_static_init_func_::_add( \
			_ptr, \
			[](void* _var_) { \
				new (_var_) _Type_(DME_EVAL_ARGS(_DME_SINIT_ARGS_, __VA_ARGS__)); \
				do { \
					afterDo \
				} while (0); \
			} \
		); \
		return _ptr; \
	}()

	// 先返回空对象来分配内存空间，然后通过func的返回值对静态变量进行初始化，初始化会推迟到Game::Init()中进行
	// 确定不会受到变量初始化顺序不一致影响的可以不用该宏（比如Singleton::GetInstance()）
	// 参数1：静态变量的变量名，如：Class1::Var1
	// 参数1：返回对象的函数名，如：Class1::Create
	// 不定参数：构造函数的参数
	// 使用实例：在头文件的类声明中写：static inline auto Var1 = DME_SVAR_DELAY_FUNC(Var1, Class1::Create, 1, 2);
	#define DME_SVAR_DELAY_FUNC(varName, func, ...) [] { \
		using _Type_ = decltype(func(__VA_ARGS__)); \
		core::_static_init_func_::_add( \
			std::addressof(varName), \
			[](void* _var_) { \
				new (_var_) _Type_(func(DME_EVAL_ARGS(_DME_SINIT_ARGS_, __VA_ARGS__))); \
			} \
		); \
		return _Type_(); \
	}()

	//检查函数的参数是否初始化，等到参数初始化后才会调用函数并返回
	//使用实例：static inline auto& Var1 = DME_SINIT(Class1, DME_SINIT_EVAL_FUNC_PARAM(Class1::Create, Class1::ObjPool, Class2::ObjPool));
	#define DME_SINIT_EVAL_FUNC_PARAM(funcName, ...) funcName(DME_EVAL_ARGS(_DME_SINIT_ARGS_, __VA_ARGS__))

	//强制指定第一个参数需求的其他静态变量，等到需求的变量初始化后才会返回第一个参数，比如第一个参数为静态函数返回值而静态函数又需求其他静态变量的情况
	//使用实例：static inline auto& Var1 = DME_SINIT(Class1, DME_SINIT_PARAM(Class1::Create(), Class1::ObjPool));
	#define DME_SINIT_PARAM(param, ...) [] { \
		DME_EVAL_FOREACH(_DME_SINIT_CHECK_ARGS_, __VA_ARGS__) \
		return (param); \
	}()

}