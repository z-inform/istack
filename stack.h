#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int decodeStackErr(int err);

class Stack{
    uint64_t canaryStat1;
    void* begPointer;
    int elSize;
    int currentSize;
    int maxSize;
    uint32_t stHash;
    uint32_t dtHash;
    uint64_t poison;
    uint64_t canaryStat2;

    int checkPoison(void* ptr);
    void fillPoison(void* ptr);
    void fillCanary();
    uint32_t calcStHash();
    uint32_t calcDtHash();
    void mvMem(void* src, int size, void* dst); 
    
    public:

    Stack(int _elSize);
    Stack(int _elSize, int _maxSize);
    ~Stack();
    Stack& operator=(Stack& arg);

    void dump();
    void setPoison(uint64_t _poison);
    void checkStack();
    int decodeErr(int err);
    void push(void* ptr);
    void pop(void* ptr);
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-arith"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wconversion"

#include "stack.cpp"

#pragma GCC diagnostic pop
