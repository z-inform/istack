#include <stdio.h>
#include <stdlib.h>

#define BADPTR 1
#define BADCURSZ 2
#define BADMAXSZ 3
#define POISON 0xEEEEEEEE

struct Stack{
    double* begPointer;
    int currentSize;
    int maxSize;
};

int checkStack(struct Stack toCheck);
int initStack(struct Stack *stk);
int initStackSize(struct Stack *stk, int stkSize);
int dumpStack(struct Stack stk);
int push(struct Stack *stk, double val);
int delStack(struct Stack *stk);
int pop(struct Stack *stk, double *val);

int pop(struct Stack *stk, double *val){

    if( checkStack(*stk) != 0) return checkStack(*stk);

    *val = (stk -> begPointer)[stk -> currentSize - 1];
    (stk -> begPointer)[--(stk -> currentSize)] = POISON;

    
    return checkStack(*stk);
}

int delStack(struct Stack *stk){

    free(stk -> begPointer);
    stk -> begPointer = (double*) -1;
    stk -> currentSize = 0;
    stk -> maxSize = 0;

    return 0;

}

int push(struct Stack *stk, double val){
    if( checkStack(*stk) != 0 ) return checkStack(*stk);

    if( (stk -> currentSize) >= (stk -> maxSize) - 2 ){
        stk -> begPointer = (double*) realloc(stk -> begPointer, 2 * (stk -> maxSize) * sizeof(double));
        if( (stk -> begPointer == NULL) ) return BADPTR;

        stk -> maxSize = 2 * (stk -> maxSize);

        for(int i = (stk -> currentSize); i < (stk -> maxSize); i++){
            (stk -> begPointer)[i] = POISON;
        }

    }

    (stk -> begPointer)[(stk -> currentSize)++] = val;
    
    return checkStack(*stk);
}

int initStackSize(struct Stack *stk, int stkSize){
    stk -> begPointer = (double*) calloc(stkSize, sizeof(double));
    if( stk -> begPointer != NULL){
        for(int i = 0; i < stkSize; i++){
            (stk -> begPointer)[i] = POISON;
        }
    }
     
    stk -> currentSize = 0;
    stk -> maxSize = stkSize;
    
    return checkStack(*stk);
}

int dumpStack(struct Stack stk){
    int errCode = checkStack(stk);
    
    printf("---------------------Stack dump---------------------\n");
    printf("Stack dump\n");
    printf("Stack addr:\t0x%X\n", &stk);

    printf("Stack status:\t");
    if( errCode == 0 ) {
        printf("OK\n");
    }
    else{
        printf("Error code %d:\t", errCode);
        switch( errCode ) {
            case BADPTR:     printf("Pointer on stack beginning not initialized or NULL. Stack might have been deleted.\n"); break;
            case BADCURSZ:   printf("Current stack size (number of elements) exceeds max size\n"); break;
            case BADMAXSZ:   printf("Max stack size (max number of elements) below or equals zero;\n"); break;
        }
    }
        
    printf("\nStruct dump:\n");
    printf("\tPointer on stack beginning:\t0x%X\n", stk.begPointer);
    printf("\tCurrent stack size:\t        %d\n", stk.currentSize);
    printf("\tMax stack size:\t  \t        %d\n", stk.maxSize);

    if( errCode != 0 ){
        printf("\nStack cannot be trusted and contents will not be displayed to avoid SEGFAULT\n");
    }
    else{
        printf("\nContents dump:\n");
        for( int i = 0; i < stk.maxSize; i++){ 
            if( i == stk.currentSize ) printf("cur -> ");
            printf("\t[%d]:\t%lg", i, stk.begPointer[i]);
            if( stk.begPointer[i] == POISON ) printf(" : POISON\n");
            else printf("\n");    
        }
    }
    printf("----------------------------------------------------\n");
    return 0;
}

int initStack(struct Stack *stk){
    stk -> begPointer = (double*) calloc(1, sizeof(double));
    if( stk -> begPointer != NULL ) (stk -> begPointer)[0] = POISON; 
    stk -> currentSize = 0;
    stk -> maxSize = 1;

    return checkStack(*stk);
}

int checkStack(struct Stack toCheck){
    if( (toCheck.begPointer == (double*) -1) || (toCheck.begPointer == NULL) ) return BADPTR;
    else if( (toCheck.currentSize < 0) || (toCheck.currentSize > toCheck.maxSize) ) return BADCURSZ;
    else if( toCheck.maxSize <= 0 ) return BADMAXSZ;
    else return 0;
}









