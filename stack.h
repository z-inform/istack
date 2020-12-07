#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define BADPTR 1
#define BADCURSZ 2
#define BADMAXSZ 3
#define EMPTYPOP 4
#define POISON 0xEEEEEEEE

class Stack{
    void* begPointer;
    int elSize;
    int currentSize;
    int maxSize;
    uint64_t poison;
    int errCode;

    void checkStack();
    int checkPoison(void* ptr);
    void fillPoison(void* ptr);
    
    public:

    Stack(int _elSize);
    Stack(int _elSize, int _maxSize);
    ~Stack();

    void dump();
    int getErr(){
        return errCode;
    }
    void push(void* ptr);
    void pop(void* ptr);
};

#include "stack.cpp"

