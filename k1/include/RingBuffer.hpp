#ifndef RING_BUFFER_H
#define RING_BUFFER_H


namespace DataStructures {

// NOTE: T must have a default constructor!!
template<class T, unsigned int length>
class RingBuffer {

    private:
        unsigned int head;
        unsigned int tail;
        const unsigned int maxSize = length;
        bool isFull;
        T* buffer[length];

    public:
        RingBuffer<T, length>() {

            head = 0;
            tail = 0;
            isFull = false;

        }

        // NOTE: if buffer is full, then put will overwrite the oldest value
        void push(T* item) {

            buffer[head] = item;

            if(isFull) {

                tail = (tail + 1) % maxSize;
            
            }

            head = (head + 1) % maxSize;

            isFull = (head == tail);

        }

        // NOTE: if buffer was full and values were overwritten, then will return the oldest value in the buffer
        // NOTE: if buffer is empty then will return nullptr
        T* pop() {

            if(empty()) {

                return nullptr;
                
            }

            //Read data and advance the tail, there is now a space in the RingBuffer
            T* val = buffer[tail];
            isFull = false;
            tail = (tail + 1) % maxSize;

            return val;

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