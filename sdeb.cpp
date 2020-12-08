#include <stdio.h>
#include <stdlib.h>
#include "stack.h"
#include <stdint.h>

int tryNorm();
int tryPop();
int tryPush();

int main(){

    /*Stack stk(sizeof(int));
    stk.dump();

    int val1 = 10;
    int val2 = 49;
    int val3 = 10;
    try{
    stk.push(&val1);
    stk.push(&val2);
    stk.push(&val1);
    stk.dump();
    stk.pop(&val3);
    printf("Popped %d\n", val3);
    stk.pop(&val3);
    printf("Popped %d\n", val3);
    stk.pop(&val3);
    printf("Popped %d\n", val3);
    stk.dump();
    }
    catch(int exc){
        printf("caught %d\n", exc);
    }*/

    for(int i = 0; i < 50; i++) tryNorm();
    for(int i = 0; i < 50; i++) tryPop();

    return 0;
}

int tryNorm(){

    try{
    double popped = 0;
    int poppedCount = 0;

    int max = 10;
    Stack stk(sizeof(double), 1 + rand() % max);
    max = 45;
    int cycles = 5 + (rand() % max);
    double* vals = (double*) calloc(cycles + 1, sizeof(double));
    for(int i = 1; i < cycles; i++){
        vals[i] = (rand() % 1000) / (rand() % 1000 + 1);
        stk.push((void*) &vals[i]);
        if( rand() % 2 == 0 ){
            poppedCount++;
            stk.pop((void*) &popped);
            if( popped - vals[i] > 1e-4 ) printf("Incorrect pop on [%d] cycle. Expected value: [%lg]. Value popped: [%lg]\n", i, vals[i], popped);
        }
    }
    free(vals);
    }
    catch(int exc){
        printf("Caught exception [%d]\n", exc);
    }
    return 0;
}

int tryPop(){
    try{
    double val = 0;
    int max = 10;
    Stack stk(sizeof(double));
    int cycles = rand() % max + 1;
    
    for(int i = 0; i < cycles; i++){
        val = rand() / (rand() + 1);
        stk.push(&val);
    }

    for(int i = 0; i < cycles + 5; i++) stk.pop(&val);
    }
    catch(int exc){
        if( exc != EMPTYPOP ) printf("Multiple pops caught an unexpected exception [%d]\n", exc);
        return 1;
    }
    return 0;
}



