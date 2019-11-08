#ifndef RING_BUFFER_HPP
#define RING_BUFFER_HPP

#include "io/bwio.hpp"
#include "user/syscall/UserSyscall.hpp"

namespace DataStructures {

// NOTE: T must have a default constructor!!
template<class T, unsigned int length>
class RingBuffer {

    private:
        unsigned int head;
        unsigned int tail;
        const unsigned int maxSize = length;
        bool isFull;
        T buffer[length];

    public:
        RingBuffer<T, length>() {

            head = 0;
            tail = 0;
            isFull = false;

        }

        // NOTE: if buffer is full, then will ignore the push
        // Returns 0 if successful push, 1 otherwise
        int push(T item) {

            if(isFull) {
                bwprintf(COM2, "Ring Buffer - TID %d tried to push to a full RingBuffer\n\r", MyTid());
                return 1;
            }

            buffer[head] = item;

            head = (head + 1) % maxSize;

            isFull = (head == tail);

            return 0;

        }


        // NOTE: if buffer is empty then will return T()
        T pop() {

            if(empty()) {

                return T();

            }

            //Read data and advance the tail, there is now a space in the RingBuffer
            T val = buffer[tail];
            isFull = false;
            tail = (tail + 1) % maxSize;

            return val;

        }

        T *peekLast() {

            if (empty()) {

                return nullptr;

            }

            return &buffer[(head - 1) % maxSize];

        }

        void reset() {

            head = tail;
            isFull = false;

        }

        bool empty() const {

            //if head and tail are equal and RingBuffer is not full, then RingBuffer is empty
            return (!isFull && (head == tail));

        }

        bool full() const {

            return isFull;

        }

        unsigned int capacity() const {

            return maxSize;

        }

        unsigned int size() const {

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

};

}

#endif
