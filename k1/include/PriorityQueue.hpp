#ifndef PRIORITY_QUEUE_HPP
#define PRIORITY_QUEUE_HPP

#include "RingBuffer.hpp"
#include "Util.hpp"

namespace DataStructures {

// NOTE: Template class T needs to have a default constructor!!
// NOTE: Lower priority numbers means high priority
template <class T, unsigned int rows, unsigned int columns>
class PriorityQueue {

    private:
        const int maxNumberOfPriorities = rows;
        const int maxNumberOfSamePriorities = columns;
        RingBuffer<T, columns> queue[rows];

    public:
        PriorityQueue<T, rows, columns>() {

        }

        // NOTE: if ring buffer of priority is full, then put will overwrite the oldest value
        // NOTE: if priority is not in the range then its ignored
        void push(T* item, int priority) {

            if (priority >= 0 && priority < maxNumberOfPriorities) {

                // bwprintf(COM2, "Pushed with priority: %d \n", priority);
                queue[priority].push(item);

            }

        }

        // NOTE: if all RingBuffers were empty then will return nullptr
        T* pop() {

            for(int i = 0; i < maxNumberOfPriorities; ++i) {

                if (!(queue[i].empty())) {

                    T* val = queue[i].pop();
                    return val;

                }

            }

            return nullptr;

        }

        void reset() {

            for(int i = 0; i < maxNumberOfPriorities; ++i) {

                queue[i].reset();

            }

        }

        bool empty() const {

            bool isEmpty = true;

            for(int i = 0; i < maxNumberOfPriorities; ++i) {

                if (!(queue[i].empty())) {

                    isEmpty = false;
                    break;

                }

            }

            return isEmpty;

        }

};

}

#endif