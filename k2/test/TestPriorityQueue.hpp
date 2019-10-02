#include "PriorityQueue.hpp"
#include "Util.hpp" 

struct TestClass {

    int priority;
    char* info;

    TestClass() {
        priority = -1;
        info = nullptr;
    }

    TestClass (int p, char* c) {
        priority = p;
        info = c;
    }

};

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
            
            DataStructures::PriorityQueue<char, 4, 2> pqueueChar;

            pqueueChar.push(&c1, 1);
            pqueueChar.push(&c2, 2);
            pqueueChar.push(&c3, 3);
            pqueueChar.push(&c4, 1);
            pqueueChar.push(&c5, 2);
            pqueueChar.reset();

            Util::assert(pqueueChar.empty() == true);
            Util::assert(pqueueChar.pop() == nullptr);
            
            pqueueChar.push(&c1, 1);
            pqueueChar.push(&c2, 2);
            pqueueChar.push(&c3, 3);
            pqueueChar.push(&c4, 1);
            pqueueChar.push(&c5, 2);

            Util::assert(pqueueChar.empty() == false);
            Util::assert(*pqueueChar.pop() == 'g');

            Util::assert(pqueueChar.empty() == false);
            Util::assert(*pqueueChar.pop() == 'z');

            pqueueChar.push(&c6, 1);
            pqueueChar.push(&c7, 1);

            Util::assert(pqueueChar.empty() == false);
            Util::assert(*pqueueChar.pop() == 'p');

            Util::assert(pqueueChar.empty() == false);
            Util::assert(*pqueueChar.pop() == 'w');

            Util::assert(pqueueChar.empty() == false);
            Util::assert(*pqueueChar.pop() == 'a');

            Util::assert(pqueueChar.empty() == false);
            Util::assert(*pqueueChar.pop() == 'k');

            Util::assert(pqueueChar.empty() == false);
            Util::assert(*pqueueChar.pop() == 'x');

            Util::assert(pqueueChar.empty() == true);
            Util::assert(pqueueChar.pop() == nullptr);

            
            TestClass td1 = TestClass(100, "first");
            TestClass td2 = TestClass(300, "second");
            TestClass td3 = TestClass(500, "third");
            TestClass td4 = TestClass(10, "fourth");
            TestClass td5 = TestClass(5, "fifth");
            TestClass td6 = TestClass(1000, "sixth");
            TestClass td7 = TestClass(7000, "seventh");
            TestClass td8 = TestClass(100, "eight");
            TestClass td9 = TestClass(100, "ninth");

            DataStructures::PriorityQueue<TestClass, 4, 3> pqueue;

            pqueue.push(&td1, 0);
            pqueue.push(&td2, 1);
            pqueue.push(&td3, 2);
            pqueue.push(&td4, 3);
            pqueue.push(&td5, 0);
            pqueue.reset();

            Util::assert(pqueue.empty() == true);
            Util::assert(pqueueChar.pop() == nullptr);
            
            pqueue.push(&td1, 0);
            pqueue.push(&td2, 1);
            pqueue.push(&td3, 2);
            pqueue.push(&td4, 3);
            pqueue.push(&td5, 0);
            pqueue.push(&td6, 1);
            pqueue.push(&td7, 2);
            pqueue.push(&td8, 3);
            pqueue.push(&td9, 0);
            pqueue.push(&td1, 1);
            pqueue.push(&td2, 2);
            pqueue.push(&td3, 3);

            TestClass* temp;

            Util::assert(pqueue.empty() == false);
            temp = pqueue.pop();
            Util::assert(temp->priority == td1.priority && temp->info == td1.info);

            Util::assert(pqueue.empty() == false);
            temp = pqueue.pop();
            Util::assert(temp->priority == td5.priority && temp->info == td5.info);

            Util::assert(pqueue.empty() == false);
            temp = pqueue.pop();
            Util::assert(temp->priority == td9.priority && temp->info == td9.info);

            Util::assert(pqueue.empty() == false);
            temp = pqueue.pop();
            Util::assert(temp->priority == td2.priority && temp->info == td2.info);

            Util::assert(pqueue.empty() == false);
            temp = pqueue.pop();
            Util::assert(temp->priority == td6.priority && temp->info == td6.info);

            pqueue.push(&td4, 0);
            
            Util::assert(pqueue.empty() == false);
            temp = pqueue.pop();
            Util::assert(temp->priority == td4.priority && temp->info == td4.info);

            Util::assert(pqueue.empty() == false);
            temp = pqueue.pop();
            Util::assert(temp->priority == td1.priority && temp->info == td1.info);

            Util::assert(pqueue.empty() == false);
            temp = pqueue.pop();
            Util::assert(temp->priority == td3.priority && temp->info == td3.info);

            Util::assert(pqueue.empty() == false);
            temp = pqueue.pop();
            Util::assert(temp->priority == td7.priority && temp->info == td7.info);

            Util::assert(pqueue.empty() == false);
            temp = pqueue.pop();
            Util::assert(temp->priority == td2.priority && temp->info == td2.info);

            Util::assert(pqueue.empty() == false);
            temp = pqueue.pop();
            Util::assert(temp->priority == td4.priority && temp->info == td4.info);

            Util::assert(pqueue.empty() == false);
            temp = pqueue.pop();
            Util::assert(temp->priority == td8.priority && temp->info == td8.info);

            Util::assert(pqueue.empty() == false);
            temp = pqueue.pop();
            Util::assert(temp->priority == td3.priority && temp->info == td3.info);

            Util::assert(pqueue.empty() == true);
            Util::assert(pqueueChar.pop() == nullptr);

    }
};