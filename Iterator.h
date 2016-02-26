#ifndef _CORE_ITERATOR_H
#define _CORE_ITERATOR_H
namespace Core {
	template<typename T, typename RT>
		class Iterator
	{
	public:
		Iterator(T vec, int index) {
			pos = index;
			this->vec = vec;
		}
		~Iterator() {
			
		}
		RT operator*() {
			return vec.get(pos);
		}
		bool operator==(Iterator<T, RT> vec) {
			return vec.pos == pos;
		}
		bool operator!=(Iterator<T, RT> vec) {
			return vec.pos != pos;
		}
		const Iterator<T, RT>& operator++(int) {
			pos++;
			return *this;
		}
		const Iterator<T, RT>& operator--(int) {
			pos--;
			return *this;
		}
	private:
		int pos;
		T vec;
	};
};
#endif
