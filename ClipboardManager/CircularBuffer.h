#pragma once
template <class E>
class CircularBuffer {
public:
	CircularBuffer(int m) {
		max = m;
		start = 0;
		count = 0;
		buffer = std::make_unique<E[]>(max);
	}
	~CircularBuffer() {}
	void addElement(const E& e);
	void deleteElement(int i);
	E getElement(int i) const;

	bool isEmpty() const;
	bool isFull() const;
	int size() const;
	int capacity() const;
	void clear();

private:
	int max;
	int start;
	int count;
	std::unique_ptr<E[]> buffer;

	int getIndex(int i) const;
};


template <class E> void CircularBuffer<E>::addElement(const E& e) {
	if (isFull()) {
		buffer[start] = e;
		start = (start + 1) % max;
	}
	else {
		buffer[getIndex(count)] = e;
		count++;
	}
}
template <class E> void CircularBuffer<E>::deleteElement(int i) {
	if (i >= 0 && i < count) {
		for (int j = i; j < count - 1; j++) {
			buffer[getIndex(j)] = buffer[getIndex(j + 1)];
		}
		count--;
	}
}
template <class E> E CircularBuffer<E>::getElement(int i) const {
	if (i >= 0 && i < count) {
		return buffer[getIndex(i)];
	}
}


template <class E> bool CircularBuffer<E>::isEmpty() const {
	return count == 0;
}
template <class E> bool CircularBuffer<E>::isFull() const {
	return count == max;
}
template <class E> int CircularBuffer<E>::size() const {
	return count;
}
template <class E> int CircularBuffer<E>::capacity() const {
	return max;
}
template <class E> void CircularBuffer<E>::clear() {
	count = 0;
	start = 0;
}
template <class E> int CircularBuffer<E>::getIndex(int i) const {
	return (start + i) % max;
}