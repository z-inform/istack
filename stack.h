#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

class Stack{
    uint64_t canaryStat1;
    void* begPointer;
    int elSize;
    int currentSize;
    int maxSize;
    uint64_t poison;
    uint64_t canaryStat2;

    int checkPoison(void* ptr);
    void fillPoison(void* ptr);
    void fillCanary();
    
    public:

    Stack(int _elSize);
    Stack(int _elSize, int _maxSize);
    ~Stack();

    void dump();
    void setPoison(uint64_t _poison);
    void checkStack();
    int decodeErr(int err);
    void push(void* ptr);
    void pop(void* ptr);
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-arith"

#include "stack.cpp"

#pragma GCC diagnostic pop
