#ifndef PRIORITY_QUEUE_HPP
#define PRIORITY_QUEUE_HPP

#include "io/bwio.hpp"
#include "RingBuffer.hpp"

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

        // NOTE: if ring buffer of priority is full, then will ignore the push
        // NOTE: if priority is not in the range then its ignored
        void push(T item, int priority) {

            if (priority >= 0 && priority < maxNumberOfPriorities) {
                int status = queue[priority].push(item);
                if(status == 1) {
                    bwprintf(COM2, "Ring Buffer - Tried to push to a full queue: %d\n\r", priority);
                }
            } else {
                bwprintf(COM2, "Priority Queue - Invalid Priority\n\r");
            }

        }

        // NOTE: if all RingBuffers were empty then will return nullptr
        T pop() {

            for(int i = 0; i < maxNumberOfPriorities; ++i) {
                // bwprintf(COM2, "Priority Queue - Queue %d size: %d\n\r", i, queue[i].size());
                if (!(queue[i].empty())) {

                    T val = queue[i].pop();
                    // bwprintf(COM2, "Priority Queue - Popped %d\n\r", val);
                    return val;

                }

            }

            return T();

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