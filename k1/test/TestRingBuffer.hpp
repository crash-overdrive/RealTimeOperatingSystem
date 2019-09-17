#include "../include/RingBuffer.hpp"
#include "../include/Util.hpp" 

struct TestData {
    int priority;

    TestData() {
        priority = -1;
    }

    TestData (int p) {
        priority = p;
    }

    static bool comparator(TestData* test1, TestData* test2) {
        if (test1->priority > test2->priority) {
            return true;
        }
        return false;
    }

};

class TestRingBuffer {
    public:
        static void runTest() {
            int n1 = 501;
            int n2 = 301;
            int n3 = 110;
            int n4 = 100;
            int n5 = 101;
            int n6 = 102;
            int n7 = 999;
            int n8 = 700;

            TestData td1 = TestData(100);
            TestData td2 = TestData(300);
            TestData td3 = TestData(500);
            TestData td4 = TestData(10);
            TestData td5 = TestData(5);

            DataStructures::RingBuffer<int, 3> temp;

            Util::assert(temp.capacity() == 3);
            Util::assert(temp.empty() == true);

            temp.push(&n1);
            temp.push(&n2);
            temp.push(&n3);

            Util::assert(*temp.pop() == 501);
            Util::assert(temp.full() == false);

            temp.push(&n4);

            Util::assert(temp.full() == true);

            temp.push(&n5);
            temp.push(&n6);
            temp.push(&n7);

            Util::assert(temp.full() == true);
            Util::assert(*temp.pop() == 101);
            Util::assert(temp.full() == false);

            temp.push(&n8);

            Util::assert(temp.full() == true);
            Util::assert(*temp.pop() == 102);
            Util::assert(temp.full() == false);
            Util::assert(*temp.pop() == 999);
            Util::assert(temp.full() == false);
            Util::assert(*temp.pop() == 700);
            Util::assert(temp.empty() == true);



            DataStructures::RingBuffer<TestData, 5> ringBufferInt;

            Util::assert(ringBufferInt.capacity() == 5);
            Util::assert(ringBufferInt.empty() == true);
            Util::assert(ringBufferInt.full() == false);

            ringBufferInt.push(&td1);
            ringBufferInt.push(&td2);
            ringBufferInt.push(&td3);

            Util::assert(ringBufferInt.full() == false);
            Util::assert(ringBufferInt.pop()->priority == 100);
            Util::assert(ringBufferInt.full() == false);

            ringBufferInt.push(&td4);

            Util::assert(ringBufferInt.full() == false);

            ringBufferInt.push(&td5);
            ringBufferInt.push(&td1);
            ringBufferInt.push(&td2);

            Util::assert(ringBufferInt.full() == true);
            Util::assert(ringBufferInt.pop()->priority == 500);
            Util::assert(ringBufferInt.full() == false);

            ringBufferInt.push(&td3);

            Util::assert(ringBufferInt.full() == true);
            Util::assert(ringBufferInt.pop()->priority == 10);
            Util::assert(ringBufferInt.full() == false);
            Util::assert(ringBufferInt.pop()->priority == 5);
            Util::assert(ringBufferInt.full() == false);
            Util::assert(ringBufferInt.pop()->priority == 100);
            Util::assert(ringBufferInt.full() == false);
            Util::assert(ringBufferInt.pop()->priority == 300);
            Util::assert(ringBufferInt.full() == false);
            Util::assert(ringBufferInt.pop()->priority == 500);
            Util::assert(ringBufferInt.full() == false);
            Util::assert(ringBufferInt.empty() == true);
        }
};
