#pragma once
#include <dme/event/data/all.h>
namespace dme::event {
	//负责Event的注册和运行
	template<typename T, typename EventData>
	class EventManager;

	//传入需要调用的函数的参数，将执行延后，使其通过事件系统统一执行
	template<typename T, typename EventData>
	class EventGroup {
		friend EventManager<T, EventData>;
	private:
		static inline void(*_func)(T& obj, EventData& data) = null;
		static inline DynArr<T*> _obj = DynArr<T*>(16);
		static inline DynArr<EventData> _data = DynArr<EventData>(16);

		static void Init(void(*func)(T& obj, EventData& data)) {
			_func = func;
		}
		static void Run() {
			auto c = _data.count();
			for (uint i = 0; i < c; i++) {
				_func(*_obj[i], _data[i]);
			}
		}
		static void ClearData() {
			_obj.clear();
			_data.clear();
		}
	public:
		using Manager = EventManager<T, EventData>;

		//保存将要调用的函数的参数，等到事件系统执行事件时再执行
		static void Push(T& obj, const EventData& data) {
			_obj.add(std::addressof(obj));
			_data.add(data);
		}
	};

	template<typename T, typename EventData>
	class EventManager {
	public:
		using Event = EventGroup<T, EventData>;

		static void Register(void(*func)(T& obj, EventData& data)) {
			Event::Init(func);
		}
		static void Trigger() {
			Event::Run();
		}
		static void ClearData() {
			Event::ClearData();
		}
	};

	//方块被破坏事件，破坏后需要处理区块内的方块数据以及渲染用的数据
	using BlockDestroyEvent = EventGroup<block::BlockProto, BlockPlaceData>;
	//方块加载事件，区块加载时触发，加载方块数据
	using BlockLoadEvent = EventGroup<block::BlockProto, BlockLoadData>;
	//方块被放置事件，放置后需要处理区块内的方块数据以及渲染用的数据
	using BlockPlaceEvent = EventGroup<block::BlockProto, BlockPlaceData>;
	//物品被使用事件
	using ItemUseEvent = EventGroup<item::ItemProto, ItemUseData>;
}
