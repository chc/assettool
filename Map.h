#ifndef _CORE_MAP_H
#define _CORE_MAP_H
#include "Iterator.h"
namespace Core {
	template<typename T, typename T2>
	class MapItem {
	public:
		MapItem(T _key) {
			key = _key;
		};
		T key;
		T2 value;
		bool initalized;
	};
	template<typename T, typename T2>
	class Map {
	public:
		Iterator<Map<T, T2>, MapItem<T, T2> *> begin() {
			return Iterator<Map<T, T2>, MapItem<T, T2> *>(this, 0);
		}
		Iterator<Map<T, T2>, MapItem<T, T2> *> end() {
			return Iterator<Map<T, T2>, MapItem<T, T2> *>(this, size());
		}
		Map(const Map& m) {
		}
		Map() {
			initialize(10);
		}
		Map(int num_elements) {
			initialize(num_elements);
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
			add(idx, NULL);
			item = findItemByKey(idx);
			item->initalized = true;
			return item->value;
		}
		void add(T key, T2 *val) {
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
				item->value = val ? *val : NULL;
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
			MapItem<T, T2> *new_items = (MapItem<T, T2> *)malloc(sizeof(MapItem<T, T2>) * num_elements);
			memset(new_items,0,sizeof(MapItem<T, T2>) * num_elements);
			memcpy(new_items,items,sizeof(MapItem<T, T2>) * (num_elements-count));
			free(items);
			items = new_items;
		}
		void initialize(int num_elements) {
			this->num_elements = num_elements;
			items = (MapItem<T, T2> *)malloc(num_elements * sizeof(MapItem<T, T2>));
			memset(items, 0, num_elements * sizeof(MapItem<T, T2>));
		}
		int num_elements;
		MapItem<T, T2> *items;
	};
};
#endif