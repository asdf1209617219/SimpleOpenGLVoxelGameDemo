#pragma once
#include <dme/core/utils/dyn_arr.h>
#include <dme/core/utils/flat_map.h>
#include <dme/core/utils/str_id.h>
namespace dme {
	// id从1开始，0表示未初始化，不允许使用0作为id
	template<typename T>
	class IdMap : private Singleton<IdMap<T>> {
		friend Singleton<IdMap<T>>;
	private:
		std::unordered_map<uint, T> idMap;
		DynArr<uint> freeKey;
		uint maxId;

		IdMap() : idMap(), freeKey(), maxId(0) {};
	public:
		static uint Add(const T& elem) {
			auto& obj = Singleton<IdMap<T>>::GetInstance();
			uint id;
			if (obj.freeKey.count() > 0) {
				id = obj.freeKey[obj.freeKey.count() - 1];
				obj.freeKey.remove();
				obj.idMap.insert({ id, elem });
			}
			else {
				obj.maxId++;
				id = obj.maxId;
				obj.idMap.insert({ id, elem });
			}
			return id;
		}
		static T* Get(uint id) {
			auto& obj = Singleton<IdMap<T>>::GetInstance();

			auto it = obj.idMap.find(id);
			if (it != obj.idMap.end()) {
				return &it->second;
			}
			return null;
		}
		static void Remove(uint id) {
			auto& obj = Singleton<IdMap<T>>::GetInstance();

			auto it = obj.idMap.find(id);
			if (it != obj.idMap.end()) {
				obj.idMap.erase(it);
				obj.freeKey.add(id);
			}
		}
	};

}

