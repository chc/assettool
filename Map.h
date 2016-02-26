#ifndef _CORE_MAP_H
#define _CORE_MAP_H

#include "Iterator.h"
namespace Core {
	template<typename T, typename T2>
	class MapItem {
	public:
		MapItem() : key() {
			initalized = false;
		}
		MapItem(T _key) : key(_key) {
			initalized = true;
		};
		T key;
		T2 value;
		bool initalized;
	};
	template<typename T, typename T2>
	class Map {
	public:
		Iterator<Map<T, T2>, MapItem<T, T2> *> begin() {
			return Iterator<Map<T, T2>, MapItem<T, T2> *>(*this, 0);
		}
		Iterator<Map<T, T2>, MapItem<T, T2> *> end() {
			return Iterator<Map<T, T2>, MapItem<T, T2> *>(*this, size());
		}
		Map(const Map& m) {
			num_elements = m.num_elements;
			items = new MapItem<T, T2>[m.num_elements];
			for(int i=0;i<m.num_elements;i++) {
				items[i] = m.items[i];
			}
		}
		Map() {
			initialize(10);
		}
		Map(int num_elements) {
			initialize(num_elements);
		}
		~Map() {
			uninitalize();
		}
		Map<T, T2>& operator=(Map<T, T2>& other) {
			uninitalize();
			num_elements = other.num_elements;
			items = new MapItem<T, T2>[num_elements];
			for(int i=0;i<num_elements;i++) {
				items[i] = other.items[i];
			}
			return *this;
		}
		MapItem<T, T2> *get(int idx) {
			int c = 0;
			for(int i=0;i<num_elements;i++) {
				if(items[i].initalized) {
					if(c++ == idx) {
						return &items[i];
					}
				}
			}
			return NULL;
		}
		T2 &operator[](T idx) {
			MapItem<T, T2> *item = findItemByKey(idx);
			if(item) {
				item->initalized = true;
				return item->value;
			}
			item = findFirstFreeItem();
			item->initalized = true;
			return item->value;
		}
		void add(T key, T2 val) {
			MapItem<T, T2> *item = findItemByKey(key);
			if(!item) {
				item = findFirstFreeItem();
				if(item == NULL) {
					AddItemSlots(num_elements * 2);
					item = findFirstFreeItem();
				}
			}
			if(item) {
				item->key = key;
				item->value = val;
				item->initalized = true;
			}
		}
		int size() {
			int count = 0;
			for(int i=0;i<num_elements;i++) {
				if(items[i].initalized) {
					count++;
				}
			}
			return count;
		}
	private:
		MapItem<T, T2> *findItemByKey(T key) {
			for(int i=0;i<num_elements;i++) {
				if(items[i].key == key) {
					return &items[i];
				}
			}
			return NULL;
		}
		MapItem<T, T2> *findFirstFreeItem() {
			for(int i=0;i<num_elements;i++) {
				if(!items[i].initalized) {
					return &items[i];
				}
			}
			return NULL;
		}
		void AddItemSlots(int count) {
			num_elements += count;
			MapItem<T, T2> *new_items = new MapItem<T, T2>[num_elements];
			for(int i=0;i<(num_elements-count);i++) {
				new_items[i] = items[i];
			}
			delete [] items;
			items = new_items;
		}
		void initialize(int num_elements) {
			this->num_elements = num_elements;
			items = new MapItem<T, T2>[num_elements];
		}
		void uninitalize() {
			if(items) {
				delete[] items;
			}
		}
		int num_elements;
		MapItem<T, T2> *items;
	};
};
#endif