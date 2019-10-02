#include "TestRingBuffer.hpp"
#include "TestPriorityQueue.hpp"

extern "C"
int TestSuite() {
    
    TestRingBuffer::runTest();
    TestPriorityQueue::runTest();
    return 0;

}
