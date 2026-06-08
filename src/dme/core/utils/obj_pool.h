#pragma once
#include <dme/core/utils/pair.h>
namespace dme {
	namespace core {
		//保证通过freelist申请的空间上构造对象的强异常安全
		struct _obj_pool_freelist_except {
			uchar** ptr;
			uchar* freelist;
			bool end = false;
			_obj_pool_freelist_except(uchar** ptr, uchar* freelist) noexcept : ptr(ptr), freelist(freelist) {}

			~_obj_pool_freelist_except() {
				if (!end) {
					*ptr = freelist;
				}
			}
		};

		// 定长内存池，存入池中的对象的管理权，在每个对象头部前都会有一个64位整数用于标记对象的状态或freelist，
		// 未初始化							最新block通过剩余对象数判断是否遍历完成
		// ulong_max						有对象
		// 0~ulong_max-1					表示下一个freelist
		// 对象池析构时会遍历所有block释放里面的对象
		template<typename T>
		struct _obj_pool {
			//每个对象所占空间大小
			//static constexpr size_t t_size = size_t_size + ((sizeof(T) % size_t_size != 0) ? (sizeof(T) + size_t_size - (sizeof(T) % size_t_size)) : sizeof(T));

			//每个对象所占空间大小
			static constexpr size_t t_size = sizeof(T) < size_t_size ? size_t_size : sizeof(T);
			static constexpr size_t obj_flag = size_t_max;

			//指向当前可用内存地址开头
			uchar* start;
			//自由链表表头
			uchar* freelist;
			//当前申请内存已保存对象个数（remove不会减少）
			size_t count;
			//当前申请内存最大可容纳对象个数（每次申请新空间会翻倍）
			size_t size;

			void setNull() noexcept {
				start = null;
				freelist = null;
				count = 0;
				size = 0;
			}

			void extend(size_t size_) noexcept {
				/* //暂时不管内存回收，由程序结束后系统自动回收
				uchar* old = start;
				start = Malloc<uchar>(size_ * t_size + size_t_size * 2); //开头留2个size_t的大小
				//将开头设置为前一个内存的开头和前一个内存的容量
				*reinterpret_cast<uchar**>(start) = old;
				*reinterpret_cast<size_t*>(start + size_t_size) = size;
				start += size_t_size * 2; //移动2个size_t的大小
				size = size_;
				count = 0;
				*/
				start = Malloc<uchar>(size_ * t_size);
				size = size_;
				count = 0;
			}

			//申请能容纳多少对象的空间
			explicit _obj_pool(size_t count_) noexcept : start(null), freelist(null), count(0), size(0) {
				extend(count_);
			}
			_obj_pool(const _obj_pool&) = delete;
			_obj_pool(_obj_pool&&) = delete;
			_obj_pool& operator =(const _obj_pool&) = delete;
			_obj_pool& operator =(_obj_pool&&) = delete;

			~_obj_pool() { //TODO 需要解决单例模式的依赖问题，否则自动析构会报错（参考TaskGraph有向无环图DAG）
				/*
				if (!start) return;
				for (size_t i = 0; i < count; i++) {
					uchar* ptr = start + t_size * i;
					if (*reinterpret_cast<size_t*>(ptr) == obj_flag) {
						reinterpret_cast<T*>(ptr + size_t_size)->~T();
					}
				}
				uchar* memory = start - size_t_size * 2;
				uchar* pre = *reinterpret_cast<uchar**>(memory);
				size_t preSize = *reinterpret_cast<size_t*>(memory - size_t_size);
				Free(memory);
				while (pre) {
					memory = pre;
					pre = *reinterpret_cast<uchar**>(memory - size_t_size * 2);
					preSize = *reinterpret_cast<size_t*>(memory - size_t_size);
					for (size_t i = 0; i < preSize; i++) {
						uchar* ptr = memory + t_size * i;
						if (*reinterpret_cast<size_t*>(ptr) == obj_flag) {
							reinterpret_cast<T*>(ptr + size_t_size)->~T();
						}
					}
					Free(memory - size_t_size * 2);
				}
				setNull();
				*/
			}

			//删除对象，会调用析构函数，注意如果传入的指针不在该对象池中为UB
			void remove(T* obj) noexcept {
				/*
				if (!obj) return;
				obj->~T();
				*reinterpret_cast<uchar**>(reinterpret_cast<uchar*>(obj) - size_t_size) = freelist;
				freelist = reinterpret_cast<uchar*>(obj);
				*/
				if (!obj) return;
				obj->~T();
				*reinterpret_cast<uchar**>(obj) = freelist;
				freelist = reinterpret_cast<uchar*>(obj);
			}

			//申请对象空间并返回指针，注意内存空间是未初始化的，需使用placement new构造对象
			T* alloc() noexcept {
				T* ptr;
				if (freelist) {
					ptr = reinterpret_cast<T*>(freelist);
					freelist = *reinterpret_cast<uchar**>(freelist);
				}
				else {
					if (count == size) {
						extend(size * 2);
					}
					ptr = reinterpret_cast<T*>(start + count * t_size);
					count++;
				}
				return ptr;
			}

			//构造对象并返回指针
			template <typename... Args>// requires(std::is_constructible_v<T, Args...>)
			T* add(Args&&... args) noexcept(noexcept(T(std::forward<Args>(args)...))) {
				/*
				T* ptr;
				if (freelist) {
					ptr = reinterpret_cast<T*>(freelist);

					new (ptr) T(std::forward<Args>(args)...);

					uchar* preFreelist = *reinterpret_cast<uchar**>(freelist - size_t_size);
					*reinterpret_cast<size_t*>(freelist - size_t_size) = obj_flag;
					freelist = preFreelist;
				}
				else {
					if (count == size) {
						extend(size * 2);
					}
					uchar* p = start + size_t_size + count * t_size;
					ptr = reinterpret_cast<T*>(p);

					new (ptr) T(std::forward<Args>(args)...);

					*reinterpret_cast<size_t*>(p - size_t_size) = obj_flag;
					count++;
				}
				return ptr;
				*/

				T* ptr;
				if (freelist) {
					ptr = reinterpret_cast<T*>(freelist);
					if constexpr (noexcept(T(std::forward<Args>(args)...))) {
						freelist = *reinterpret_cast<uchar**>(freelist);
						new (ptr) T(std::forward<Args>(args)...);
					}
					else {
						auto e = _obj_pool_freelist_except(reinterpret_cast<uchar**>(freelist), *reinterpret_cast<uchar**>(freelist));
						new (ptr) T(std::forward<Args>(args)...);
						freelist = e.freelist;
						e.end = true;
					}
				}
				else {
					if (count == size) {
						extend(size * 2);
					}
					ptr = reinterpret_cast<T*>(start + count * t_size);
					new (ptr) T(std::forward<Args>(args)...);
					count++;
				}
				return ptr;
			}
		};

		template <typename T>
		concept _has_obj_pool_size = requires {
			//检查是否存在静态常量obj_pool_size
			requires std::is_same_v<decltype(T::obj_pool_size), const size_t>;
		};

		template<typename T>
		class _obj_pool_instance : Unconstructible {
		public:
			static _obj_pool<T>& GetObjPool(size_t size) noexcept {
				static _obj_pool<T> _pool = _obj_pool<T>(size);
				return _pool;
			}
		};
	}

	//子类可通过继承该模板管理对象的申请和释放，每种类只会生成一个ObjPool
	//类中通过设置static constexpr size_t obj_pool_size = ?;来控制ObjPool的初始大小
	template<typename T>
	class ObjPool {
	private:
		constexpr static size_t _get_pool_size() noexcept {
			if constexpr (core::_has_obj_pool_size<T>) {
				return T::obj_pool_size;
			}
			else {
				return 2;
			}
		}

		static inline core::_obj_pool<T>& _pool = core::_obj_pool_instance<T>::GetObjPool(_get_pool_size());
	protected:
		//删除对象，会调用析构函数，注意如果传入的指针不在该对象池中为UB
		static void Remove(T* obj) noexcept {
			_pool.remove(obj);
		}

		/*
		//构造对象并返回指针 //由于private问题所以只能将构造方法写在这里
		template <typename... Args> //requires(std::is_constructible_v<T, Args...>)
		static T* Add(Args&&... args) noexcept(noexcept(T(std::forward<Args>(args)...))) {
			static constexpr size_t t_size = size_t_size + ((sizeof(T) % size_t_size != 0) ? (sizeof(T) + size_t_size - (sizeof(T) % size_t_size)) : sizeof(T));
			static constexpr size_t obj_flag = size_t_max;

			T* ptr;
			if (_pool.freelist) {
				ptr = reinterpret_cast<T*>(_pool.freelist);

				new (ptr) T(std::forward<Args>(args)...);

				uchar* preFreelist = *reinterpret_cast<uchar**>(_pool.freelist - size_t_size);
				*reinterpret_cast<size_t*>(_pool.freelist - size_t_size) = obj_flag;
				_pool.freelist = preFreelist;
			}
			else {
				if (_pool.count == _pool.size) {
					_pool.extend(_pool.size * 2);
				}
				uchar* p = _pool.start + size_t_size + _pool.count * t_size;
				ptr = reinterpret_cast<T*>(p);

				new (ptr) T(std::forward<Args>(args)...);

				*reinterpret_cast<size_t*>(p - size_t_size) = obj_flag;
				_pool.count++;
			}
			return ptr;
		}
		*/

		//申请对象空间并返回指针，注意内存空间是未初始化的，需使用placement new构造对象
		static T* Alloc() noexcept {
			return _pool.alloc();
		}

		//构造对象并返回指针
		template <typename... Args> //requires(std::is_constructible_v<T, Args...>)
		static T* Add(Args&&... args) noexcept(noexcept(T(std::forward<Args>(args)...))) {
			static constexpr size_t t_size = sizeof(T) < size_t_size ? size_t_size : sizeof(T);

			T* ptr;
			if (_pool.freelist) {
				ptr = reinterpret_cast<T*>(_pool.freelist);
				if constexpr (noexcept(T(std::forward<Args>(args)...))) {
					_pool.freelist = *reinterpret_cast<uchar**>(_pool.freelist);
					new (ptr) T(std::forward<Args>(args)...);
				}
				else {
					auto e = core::_obj_pool_freelist_except(reinterpret_cast<uchar**>(_pool.freelist), *reinterpret_cast<uchar**>(_pool.freelist));
					new (ptr) T(std::forward<Args>(args)...);
					_pool.freelist = e.freelist;
					e.end = true;
				}
			}
			else {
				if (_pool.count == _pool.size) {
					_pool.extend(_pool.size * 2);
				}
				ptr = reinterpret_cast<T*>(_pool.start + _pool.count * t_size);
				new (ptr) T(std::forward<Args>(args)...);
				_pool.count++;
			}
			return ptr;
		}

	};
}