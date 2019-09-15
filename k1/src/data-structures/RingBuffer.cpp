#include "../../include/RingBuffer.hpp"

template <class T>
DataStructures::RingBuffer<T>::RingBuffer() {

}

template <class T>
void DataStructures::RingBuffer<T>::put(T item) {

    buffer[head] = item;

    if(isFull) {

        tail = (tail + 1) % maxSize;
    
    }

    head = (head + 1) % maxSize;

    isFull = (head == tail);

}

template <class T>
T DataStructures::RingBuffer<T>::get() {

    if(empty()) {
        return T();
    }

    //Read data and advance the tail, there is now a space in the RingBuffer
    T val = buffer[tail];
    isFull = false;
    tail = (tail + 1) % maxSize;

    return val;

}

template <class T>
void DataStructures::RingBuffer<T>::reset() {

    head = tail;
    isFull = false;

}

template <class T>
bool DataStructures::RingBuffer<T>::empty() const {

    //if head and tail are equal and RingBuffer is not full, then RingBuffer is empty
    return (!isFull && (head == tail));

}

template <class T>
bool DataStructures::RingBuffer<T>::full() const {

    return isFull;

}

template <class T>
unsigned int DataStructures::RingBuffer<T>::capacity() const {

    return maxSize;

}

template <class T>
unsigned int DataStructures::RingBuffer<T>::size() const {

    unsigned int size = maxSize;

    if(!isFull) {

        if(head >= tail) {

            size = head - tail;

        } else {

            size = maxSize + head - tail;

        }
    }

    return size;

}