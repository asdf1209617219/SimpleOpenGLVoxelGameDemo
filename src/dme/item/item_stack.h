#pragma once
#include <dme/item/proto/item_proto.h>

namespace dme::item {

	//proto默认为Nono
	class ItemStack {
	private:
		const ItemProto* proto; //proto不可能为空
	protected:
		uint count;
		uint maxCount;
	public:
		//proto默认为Nono
		ItemStack() noexcept;
		ItemStack(const ItemProto& proto, uint count) noexcept;

		bool isEmpty() const noexcept;
		const ItemProto& getProto() const noexcept;
		void setProto(const ItemProto& proto) noexcept;

		uid getProtoId() const noexcept;

		uint getCount() const noexcept;
		void setCount(uint count) noexcept;
		uint addCount() noexcept;
		uint addCount(uint count) noexcept;
		uint subCount() noexcept;
		uint subCount(uint count) noexcept;
	};

}