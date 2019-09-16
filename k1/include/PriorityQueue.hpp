#ifndef PRIORITY_QUEUE_H
    #define PRIORITY_QUEUE_H

    #include "../src/util/Utility.cpp"

    namespace DataStructures {

        // NOTE: T needs to have a default constructor!!
        // NOTE: T needs to be implementing atleast > operator
        template <class T, unsigned int length>
        class PriorityQueue {
            protected:
                const unsigned int maxSize = length;
                unsigned int heapSize;
                T heap[length];

                // to get index of parent of node at index "index"
                int parentIndex(int index) { 
                    
                    return (index-1)/2; 
                    
                } 
            
                // to get index of left child of node at index "index"
                int leftChildIndex(int index) { 

                    return (2*index + 1); 
                    
                } 
            
                // to get index of right child of node at index "index"
                int rightChildIndex(int index) { 
                    
                    return (2*index + 2); 
                    
                } 

                // to heapify a subtree with the root at given index 
                void MaxHeapify(int currentIndex) {

                    int leftIndex = leftChildIndex(currentIndex); 
                    int rightIndex = rightChildIndex(currentIndex); 
                    int largestElementIndex = currentIndex; 
                    
                    if (leftIndex < heapSize && heap[leftIndex] > heap[largestElementIndex]) {

                        largestElementIndex = leftIndex; 

                    }
                        
                    if (rightIndex < heapSize && heap[rightIndex] > heap[largestElementIndex]) {

                        largestElementIndex = rightIndex;

                    } 
                         
                    if (largestElementIndex != currentIndex) { 

                        Utility::Util::swap(&heap[currentIndex], &heap[largestElementIndex]); 
                        MaxHeapify(largestElementIndex); 

                    } 

                }
                

            public:
                PriorityQueue<T, length>() {

                    heapSize = 0;

                };

                // Inserts a new key 'key' 
                // Does nothing if queue is already full
                void push(T key) {
                    
                    if (heapSize == maxSize) { 

                        return; 

                    } 
                
                    // First insert the new key at the end of heap
                    heapSize++; 
                    int indexToBeInsertedAt = heapSize - 1; 
                    heap[indexToBeInsertedAt] = key; 
                
                    // Fix the max heap property if it is violated 
                    while (indexToBeInsertedAt != 0 && heap[indexToBeInsertedAt] > heap[parentIndex(indexToBeInsertedAt)]) {

                        Utility::Util::swap(&heap[indexToBeInsertedAt], &heap[parentIndex(indexToBeInsertedAt)]); 
                        indexToBeInsertedAt = parentIndex(indexToBeInsertedAt); 

                    } 

                }
            
                // Returns the maximum key (key at root) from max heap (and removes it from queue)
                // Returns T() if called on empty queue
                T pop() { 
                    
                    if (heapSize <= 0) {

                        return T();

                    }
 
                    if (heapSize == 1) { 

                        heapSize--; 
                        return heap[0]; 

                    } 
                
                    // Store the maximum value, and remove it from heap 
                    T root = heap[0]; 
                    heap[0] = heap[heapSize-1]; 
                    heapSize--; 
                    MaxHeapify(0); 
                
                    return root; 
                    
                } 

                void reset() {

                    heapSize = 0;

                }

                bool empty() const {

                    //if head and tail are equal and RingBuffer is not full, then RingBuffer is empty
                    return (heapSize <= 0);

                }

                bool full() const {

                    return (heapSize >= maxSize);

                }

                unsigned int capacity() const {
                    
                    return maxSize;
                
                }

                unsigned int size() const {

                    return heapSize;

                }

        };

    }

#endif