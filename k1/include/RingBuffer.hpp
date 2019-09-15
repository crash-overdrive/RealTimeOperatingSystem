#ifndef RINGBUFFER_H
    #define RINGBUFFER_H


    namespace DataStructures {

        template<class T>
        class RingBuffer {

            protected:
                unsigned int head = 0;
                unsigned int tail = 0;
                // TODO: change this value to depend on compile time variables, make adjustments in Makefile?
                constexpr static unsigned int maxSize = 3;
                bool isFull = false;
                T buffer[maxSize];

            public:
                RingBuffer<T>();
                // NOTE: if buffer is full, then put will overwrite the oldest value
                void put(T item);
                // NOTE: if buffer was full and values were overwritten, then will return the oldest value in the buffer
                T get();
                void reset();
                bool empty() const;
                bool full() const;
                unsigned int capacity() const;
                unsigned int size() const;

        };
    }
    
#endif