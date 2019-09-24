
int Create(int priority, void (*function)()) {
    // assembly pushes priority and function on stack???
    // save rx, ry
    // set rx = priority
    // set ry = fn pointer
    // call swi,2
    // restore rx, ry
}


int MyTid();


int MyParentTid();


void Yield();


void Exit();
