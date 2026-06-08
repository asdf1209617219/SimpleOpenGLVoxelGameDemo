#include <dme/item/inventory/inventory.h>
#include <dme/item/proto/all.h>
#include <dme/ui/item/inventory_ui.h>

namespace dme::item {

    Inventory::Inventory(uint size) noexcept : inventoryUI(null), arr(new ItemStack[size]()), size(size)
    {}

    void Inventory::setSize(uint size_) noexcept {
        if (size != size_) {
            ItemStack* ptr = new ItemStack[size_]();
            std::memcpy(ptr, arr, size_ * sizeof(ItemStack));
            delete[] arr;
            arr = ptr;
            uint oldSize = size;
            size = size_;

            for (uint i = size_ ? size_ - 1 : 0; i < oldSize; i++) {
                updateInventoryUI(i);
            }
        }
    }
    uint Inventory::getSize() const noexcept {
        return size;
    }
    uint Inventory::addItem(const ItemStack& item) noexcept {
        if (item.isEmpty()) {
            return 0;
        }
        uint count = item.getCount();
        auto protoId = item.getProtoId();

        bool notFindEmpty = true;
        uint emptyIndex = 0;
        for (uint i = 0; i < size; i++) {
            auto& stack = arr[i];
            if (notFindEmpty && stack.isEmpty()) {
                //寻找到空的格子
                emptyIndex = i;
                notFindEmpty = false;
                continue;
            }
            if (stack.getProtoId() == protoId) {
                //将物品放入未满的相同物品格子
                uint oldCount = count;
                count = stack.addCount(count);
                if (count != oldCount) {
                    updateInventoryUI(i);
                }
                if (count == 0) {
                    return 0;
                }
            }
        }
        if (!notFindEmpty) {
            //放入空的格子
            auto& stack = arr[emptyIndex];
            stack.setProto(item.getProto());
            stack.setCount(count);
            updateInventoryUI(emptyIndex);
            return 0;
        }
        return count;
    }
    uint Inventory::subItem(const ItemStack& item) noexcept {
        if (item.isEmpty()) {
            return 0;
        }
        uint count = item.getCount();
        auto protoId = item.getProtoId();

        for (uint i = 0; i < size; i++) {
            auto& stack = arr[i];
            if (stack.getProtoId() == protoId) {
                uint oldCount = count;
                count = stack.subCount(count);
                if (count != oldCount) {
                    updateInventoryUI(i);
                }
                if (count == 0) {
                    return 0;
                }
            }
        }
        return count;
    }
    void Inventory::swapItem(uint index, ItemStack& item) noexcept {
        ItemStack temp = std::move(item);
        if (index >= size) {
            index = size - 1;
        }
        auto& stack = arr[index];
        item = std::move(stack);
        stack = std::move(temp);
        updateInventoryUI(index);
    }
    void Inventory::updateInventoryUI(uint index) const {
        auto p = inventoryUI;
        while (p) {
            p->updateItemStack(index);
            p = p->getNextInventoryUI();
        }
    }

    ItemStack* Inventory::getItem(uint a) noexcept {
        if (a >= size) {
            return null;
        }
        return arr + a;
    }
    const ItemStack* Inventory::getItem(uint a) const noexcept {
        if (a >= size) {
            return null;
        }
        return arr + a;
    }

    ItemStack& Inventory::operator[](uint a) noexcept {
        if (a >= size) {
            return arr[size - 1];
        }
        return arr[a];
    }
    const ItemStack& Inventory::operator[](uint a) const  noexcept {
        if (a >= size) {
            return arr[size - 1];
        }
        return arr[a];
    }

}