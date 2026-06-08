#pragma once
#include <dme/core/utils.h>
namespace dme {
	//入侵式双向链表的节点，需配合IntrusiveList使用，第二个参数为函数指针，传入T*返回IntrusiveList*
	template<typename T>
	struct IntrusiveListNode {
		using list_node = IntrusiveListNode<T>;

		T* prev;
		T* next;

		constexpr IntrusiveListNode() noexcept : prev(null), next(null) {}
		constexpr IntrusiveListNode(T* prev, T* next) noexcept : prev(prev), next(next) {}
	};

	//第二个参数为函数指针，传入T&返回IntrusiveListNode<T>&
	template<typename T, auto Func> //requires(IsInvocableRet<decltype(Func), IntrusiveListNode<T>&, T&>)
	struct IntrusiveList {
		using list_node = IntrusiveListNode<T>;
		using list_type = IntrusiveList;
		using value_type = T;

		constexpr static void Remove(value_type& obj) noexcept {
			list_node& node = Func(obj);
			if (node.prev) {
				Func(*node.prev).next = node.next;
			}
			if (node.next) {
				Func(*node.next).prev = node.prev;
			}
			node.prev = null;
			node.next = null;
		}
		constexpr static void RemovePrev(value_type& obj) noexcept {
			list_node& node = Func(obj);
			if (node.prev) {
				list_node& n = Func(*node.prev);
				node.prev = n.prev;
				n.prev = null;
				n.next = null;
			}
		}
		constexpr static void RemoveNext(value_type& obj) noexcept {
			list_node& node = Func(obj);
			if (node.next) {
				list_node& n = Func(*node.next);
				node.next = n.next;
				n.prev = null;
				n.next = null;
			}
		}
		constexpr static void InsertPrev(value_type& obj, value_type& newObj) noexcept {
			list_node& node = Func(obj);
			list_node& newNode = Func(newObj);
			newNode.prev = node.prev;
			newNode.next = std::addressof(obj);
			if (node.prev) {
				Func(*node.prev).next = std::addressof(newObj);
			}
			node.prev = std::addressof(newObj);
		}
		constexpr static void InsertNext(value_type& obj, value_type& newObj) noexcept {
			list_node& node = Func(obj);
			list_node& newNode = Func(newObj);
			newNode.prev = std::addressof(obj);
			newNode.next = node.next;
			if (node.next) {
				Func(*node.next).prev = std::addressof(newObj);
			}
			node.next = std::addressof(newObj);
		}
	};
}