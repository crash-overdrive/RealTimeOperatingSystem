#include "../include/PriorityQueue.hpp"
#include "../include/Util.hpp" 

struct TestClass {
    int priority;

    TestClass() {
        priority = -1;
    }

    TestClass (int p) {
        priority = p;
    }

    static bool comparator(TestClass* test1, TestClass* test2) {
        if (test1->priority > test2->priority) {
            return true;
        }
        return false;
    }

};


bool isGreaterThanForChar(char* c1, char* c2) {
    if(*c1 > *c2) {
        return true;
    }
    return false;
}

bool isGreaterThanForInt(int* c1, int* c2) {
    if(*c1 > *c2) {
        return true;
    }
    return false;
}

class TestPriorityQueue {
    public:
        static void runTest() {

            char c1 = 'g';
            char c2 = 'a';
            char c3 = 'x';
            char c4 = 'z';
            char c5 = 'k';
            char c6 = 'p';
            char c7 = 'w';

            TestClass td1 = TestClass(100);
            TestClass td2 = TestClass(300);
            TestClass td3 = TestClass(500);
            TestClass td4 = TestClass(10);
            TestClass td5 = TestClass(5);
            TestClass td6 = TestClass(1000);
            TestClass td7 = TestClass(7000);
            
            DataStructures::PriorityQueue<char, 5> pqueueChar(isGreaterThanForChar);

            pqueueChar.push(&c1);
            pqueueChar.push(&c2);
            pqueueChar.push(&c3);
            pqueueChar.push(&c4);
            pqueueChar.push(&c5);
            pqueueChar.reset();

            Util::assert(pqueueChar.empty() == true);
            Util::assert(pqueueChar.full() == false);
            Util::assert(pqueueChar.size() == 0);
            
            pqueueChar.push(&c1);
            pqueueChar.push(&c2);
            pqueueChar.push(&c3);
            pqueueChar.push(&c4);
            pqueueChar.push(&c5);

            Util::assert(pqueueChar.empty() == false);
            Util::assert(pqueueChar.full() == true);
            Util::assert(pqueueChar.size() == 5);
            Util::assert(*pqueueChar.pop() == 'z');

            Util::assert(pqueueChar.empty() == false);
            Util::assert(pqueueChar.full() == false);
            Util::assert(pqueueChar.size() == 4);
            Util::assert(*pqueueChar.pop() == 'x');

            pqueueChar.push(&c6);
            pqueueChar.push(&c7);

            Util::assert(pqueueChar.empty() == false);
            Util::assert(pqueueChar.full() == true);
            Util::assert(pqueueChar.size() == 5);
            Util::assert(*pqueueChar.pop() == 'w');

            Util::assert(pqueueChar.empty() == false);
            Util::assert(pqueueChar.full() == false);
            Util::assert(pqueueChar.size() == 4);
            Util::assert(*pqueueChar.pop() == 'p');

            Util::assert(pqueueChar.empty() == false);
            Util::assert(pqueueChar.full() == false);
            Util::assert(pqueueChar.size() == 3);
            Util::assert(*pqueueChar.pop() == 'k');

            Util::assert(pqueueChar.empty() == false);
            Util::assert(pqueueChar.full() == false);
            Util::assert(pqueueChar.size() == 2);
            Util::assert(*pqueueChar.pop() == 'g');

            Util::assert(pqueueChar.empty() == false);
            Util::assert(pqueueChar.full() == false);
            Util::assert(pqueueChar.size() == 1);
            Util::assert(*pqueueChar.pop() == 'a');

            Util::assert(pqueueChar.empty() == true);
            Util::assert(pqueueChar.full() == false);
            Util::assert(pqueueChar.size() == 0);


            DataStructures::PriorityQueue<TestClass, 5> pqueue(TestClass::comparator);

            Util::assert(pqueue.capacity() == 5);

            pqueue.push(&td1);
            pqueue.push(&td2);
            pqueue.push(&td3);
            pqueue.push(&td4);
            pqueue.push(&td5);
            pqueue.reset();

            Util::assert(pqueue.empty() == true);
            Util::assert(pqueue.full() == false);
            Util::assert(pqueue.size() == 0);
            
            pqueue.push(&td1);
            pqueue.push(&td2);
            pqueue.push(&td3);
            pqueue.push(&td4);
            pqueue.push(&td5);

            Util::assert(pqueue.empty() == false);
            Util::assert(pqueue.full() == true);
            Util::assert(pqueue.size() == 5);
            Util::assert(pqueue.pop()->priority == 500);

            Util::assert(pqueue.empty() == false);
            Util::assert(pqueue.full() == false);
            Util::assert(pqueue.size() == 4);
            Util::assert(pqueue.pop()->priority == 300);

            pqueue.push(&td7);
            pqueue.push(&td6);

            Util::assert(pqueue.empty() == false);
            Util::assert(pqueue.full() == true);
            Util::assert(pqueue.size() == 5);
            Util::assert(pqueue.pop()->priority == 7000);

            Util::assert(pqueue.empty() == false);
            Util::assert(pqueue.full() == false);
            Util::assert(pqueue.size() == 4);
            Util::assert(pqueue.pop()->priority == 1000);

            Util::assert(pqueue.empty() == false);
            Util::assert(pqueue.full() == false);
            Util::assert(pqueue.size() == 3);
            Util::assert(pqueue.pop()->priority == 100);

            Util::assert(pqueue.empty() == false);
            Util::assert(pqueue.full() == false);
            Util::assert(pqueue.size() == 2);
            Util::assert(pqueue.pop()->priority == 10);
            
            Util::assert(pqueue.empty() == false);
            Util::assert(pqueue.full() == false);
            Util::assert(pqueue.size() == 1);
            Util::assert(pqueue.pop()->priority == 5);

            Util::assert(pqueue.empty() == true);
            Util::assert(pqueue.full() == false);
            Util::assert(pqueue.size() == 0);

        }
};