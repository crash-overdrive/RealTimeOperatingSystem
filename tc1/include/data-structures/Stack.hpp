#ifndef STACK_HPP
#define STACK_HPP

#include "io/bwio.hpp"

namespace DataStructures {

// NOTE: T must have a default constructor!!
template<class T, unsigned int length>
class Stack {

    private:
        unsigned int head;
        unsigned int maxSize;
        T buffer[length];

    public:
        Stack<T, length>() {
            head = 0;
            maxSize = length;
        }
        // NOTE: if buffer is full, then will ignore the push
        // Returns 1 if successful push, 0 otherwise
        int push(T item) {
            if(full()) {
                bwprintf(COM2, "Stack - Stack Overflow\n\r");
                return 0;
            }
            buffer[head] = item;
            head = (head + 1);
            return 1;
        }


        // NOTE: if buffer is empty then will return T()
        T pop() {
            if (empty()) {
                return T();
            } else {
                head = head - 1;
                return buffer[head];
            }
        }

        T peek() {
            if (empty()) {
                return T();
            } else {
                return buffer[head-1];
            }
        }

        void reset() {
            head = 0;
        }

        bool empty() const {
            return (head == 0);
        }

        bool full() const {
            return (head >= length);
        }

        unsigned int capacity() const {
            return maxSize;
        }

        unsigned int size() const {
            return head;
        }

};

}

#endif
