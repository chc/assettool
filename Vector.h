#ifndef _CORE_VECTOR_H
#define _CORE_VECTOR_H
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Iterator.h"
namespace Core {
	template<typename T>
	class VectorItem {
	public:
		VectorItem() {
			initalized = false;
		}
		VectorItem(const VectorItem<T> &v) {
			data = v.data;
			initalized = v.initalized;
		}
		T data;
		bool initalized;
	};
	template<typename T>
	class Vector {
	public:
		Iterator<Vector<T>, T> begin() {
			return Iterator<Vector<T>, T>(*this, 0);
		}
		Iterator<Vector<T>, T> end() {
			return Iterator<Vector<T>, T>(*this, size());
		}
		Vector(const Vector<T> &v) {
			item_count = v.item_count;
			items = new VectorItem<T>[v.item_count];
			for(int i=0;i<item_count;i++) {
				items[i] = v.items[i];
			}
		}
		Vector(int num_elements) {
			initalize(num_elements);
		}
		Vector() {
			initalize(10);
		}
		~Vector() {
			uninitalize();
		}
		T add(T val) {
			VectorItem<T> *item = findFirstFreeItem();
			if(item == NULL) {
				AddItemSlots(item_count*2);
				item = findFirstFreeItem();
			}
			if(item) {
				item->initalized = true;
				item->data = val;
			}
			return item->data;
		}
		T operator[](int i) {
			VectorItem<T> *item = findItemByExternalIndex(i);
			if(item) {
				return item->data;
			}
			return T();
		}
		Vector<T>& operator=(Vector& other) {
			uninitalize();
			item_count = other.item_count;
			items = new VectorItem<T>[item_count];
			for(int i=0;i<item_count;i++) {
				items[i].data = other.items[i].data;
				items[i].initalized = other.items[i].initalized;
			}
			return *this;
		}
		T get(int pos) {
			return operator[](pos);
		}
		void clear() {
			for(int i=0;i<item_count;i++) {
				if(items[i].initalized) {
					items[i].initalized = false;
				}
			}
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
		void uninitalize() {
			if(items)
				free(items);
		}
		void initalize(int count) {
			items = new VectorItem<T>[count];
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

			VectorItem<T> *new_items = new VectorItem<T>[item_count];
			if(items) {
				for(int i=0;i<(item_count-count);i++) {
					new_items[i] = items[i];
				}
				delete[] items;
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
#endif //_CORE_COLLECTION_H
