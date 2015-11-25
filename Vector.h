#ifndef _CORE_VECTOR_H
#define _CORE_VECTOR_H
#include <stdlib.h>
#include <string.h>
#include "Iterator.h"
namespace Core {
	template<typename T>
	class VectorItem {
	public:
		T data;
		bool initalized;
	};
	template<typename T>
	class Vector {
	public:
		Iterator<Vector<T>, T> begin() {
			return Iterator<Vector<T>, T>(this, 0);
		}
		Iterator<Vector<T>, T> end() {
			return Iterator<Vector<T>, T>(this, size());
		}
		Vector(const Vector<T> &v) {
			item_count = 0;
			items = NULL;
		}
		Vector(int num_elements) {
			initalize(num_elements);
		}
		Vector() {
			initalize(10);
		}
		~Vector() {
			free(items);
		}
		void add(T val) {
			VectorItem<T> *item = findFirstFreeItem();
			if(item == NULL) {
				AddItemSlots(item_count*2);
				item = findFirstFreeItem();
			}
			if(item) {
				item->initalized = true;
				item->data = val;
			}
		}
		T operator[](int i) {
			VectorItem<T> *item = findItemByExternalIndex(i);
			if(item) {
				return item->data;
			}
			return (T)NULL;
		}
		Vector<T>& operator=(Vector& other) {
			other.item_count = item_count;
			other.items = (VectorItem<T>*)malloc(sizeof(VectorItem<T>) * item_count);
			memcpy(other.items,items, sizeof(VectorItem<T>) * item_count);
			return *this;
		}
		T get(int pos) {
			return operator[](pos);
		}
		int size() {
			int count = 0;
			for(int i=0;i<item_count;i++) {
				if(items[i].initalized) {
					count++;
				}
			}
			return count;
		}
	private:
		VectorItem<T> *findItemByExternalIndex(int idx) {
			int c = 0;
			for(int i=0;i<item_count;i++) {
				if(items[i].initalized) {
					if(c++ == idx) {
						return &items[i];
					}
				}
			}
			return NULL;
		}
		void initalize(int count) {
			items = (VectorItem<T> *)malloc(sizeof(VectorItem<T>) * count);
			printf("Init items: %p\n",items);
			memset(items,0,sizeof(VectorItem<T>) * count);
			item_count = count;
		}
		VectorItem<T> *findFirstFreeItem() {
			for(int i=0;i<item_count;i++) {
				if(!items[i].initalized) {
					return &items[i];
				}
			}
			return NULL;
		}
		void AddItemSlots(int count) {
			if(count == 0) count = 10;
			item_count += count;
			VectorItem<T> *new_items = (VectorItem<T> *)malloc(sizeof(VectorItem<T>) * item_count);
			memset(new_items,0,sizeof(VectorItem<T>) * item_count);
			printf("additemslots this %p\n",this);
			if(items) {
				memcpy(new_items,items,sizeof(VectorItem<T>) * (item_count-count));
				printf("Freeing in slots: %p\n",items);
				free(items);
			}
			items = new_items;
		}
		void DeleteItem(VectorItem<T> *item) {
			item->initalized = false;
		}
		
		VectorItem<T> *items;
		int item_count;
	};
};
#endif //_CORE_VECTOR_H