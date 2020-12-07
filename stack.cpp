#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define BADPTR 1
#define BADCURSZ 2
#define BADMAXSZ 3
#define EMPTYPOP 4


Stack::Stack(int _elSize):
    begPointer(nullptr),
    elSize(_elSize),
    currentSize(0),
    maxSize(1),
    poison(0xEEEEEEEEEEEEEEEE),
    errCode(0)  {        
        begPointer = calloc(1, _elSize);
        if( begPointer == nullptr ){
            errCode = BADPTR;
            throw BADPTR;
        }
        else{
            errCode = 0;
            fillPoison(begPointer);
        }
}

Stack::Stack(int _elSize, int _maxSize):
    begPointer(nullptr),
    elSize(_elSize),
    currentSize(0),
    maxSize(_maxSize),
    poison(0xEEEEEEEEEEEEEEEE),
    errCode(0)  {
        begPointer = calloc(_maxSize, _elSize);
        if( begPointer == nullptr ){
            errCode = BADPTR;
            throw BADPTR;
        }
        else{
            errCode = 0;
            for(int i = 0; i < _maxSize; i++) fillPoison(begPointer + i*_elSize);
        }
}

Stack::~Stack(){
        free(begPointer);
        begPointer = (void*) -1;
        currentSize = 0;
        maxSize = 0;
}


void Stack::pop(void* ptr){
    this -> checkStack();

    if( this -> currentSize != 0 ){
        uint8_t* charptr = (uint8_t*) ptr;

        for(int i = 0; i < (this -> elSize); i++){
            *(charptr + i) = *( (uint8_t*) (this -> begPointer) + i);  
        }
    }
    else throw EMPTYPOP;    
    this  -> checkStack();

}

void Stack::push(void* ptr){
    
    this -> checkStack();

    if( (this -> currentSize) >= (this -> maxSize) - 2 ){
        this -> begPointer = realloc(this -> begPointer, 2 * (this -> maxSize) * (this -> elSize));
        if( (this -> begPointer == nullptr) ) (this -> errCode) = BADPTR;

        this -> maxSize = 2 * (this -> maxSize);

        for(int i = (this -> currentSize); i < (this -> maxSize); i++){
            fillPoison(begPointer + i * (this -> elSize));
        }

    }

    for(int i = 0; i < (this -> elSize); i++){
        *( (uint8_t*) begPointer + (this -> currentSize) * (this -> elSize) + i) = *( (uint8_t*) ptr + i);
    }

    (this -> currentSize)++;

    this -> checkStack();

}

void Stack::dump(){
    this -> checkStack();
        
    printf("---------------------Stack dump---------------------\n");
    printf("Stack dump\n");
    printf("Stack addr:\t%p\n", this);
    printf("Poison value:\t0x%lX\n", this -> poison);
/*
    printf("Stack status:\t");
    if( this -> errCode == 0 ) {
        printf("OK\n");
    }
    else{
        printf("Error code %d:\t", errCode);
        switch( this -> errCode ) {
            case BADPTR:     printf("Pointer on stack beginning not initialized or NULL. Stack might have been deleted.\n"); break;
            case BADCURSZ:   printf("Current stack size (number of elements) exceeds max size\n"); break;
            case BADMAXSZ:   printf("Max stack size (max number of elements) below or equals zero;\n"); break;
        }
    }
        
    printf("\nStruct dump:\n");
    printf("\tPointer on stack beginning:\t%p\n", this -> begPointer);
    printf("\tCurrent stack size:\t        %d\n", this -> currentSize);
    printf("\tMax stack size:\t  \t        %d\n", this -> maxSize);

    if( this -> errCode != 0 ){
        printf("\nStack cannot be trusted and contents will not be displayed to avoid SEGFAULT\n");
    }
    else{
*/
    printf("\nContents dump:\n");
    for( int i = 0; i < (this -> maxSize); i++){ 
        if( i == (this -> currentSize) ) printf("cur -> ");
        printf("\t[%d]:\t", i);
        for(int k = 0; k < (this -> elSize); k++){
            uint8_t val = *( (uint8_t*) this -> begPointer + i*(this -> elSize) + k); 
            if( val <= 0xF ) printf("0");
            printf("%X ", val);
        }
        if( checkPoison((this -> begPointer) + i*(this -> elSize)) == 1  ) printf(" : POISON\n");
        else printf("\n");    
    }
    printf("----------------------------------------------------\n");
}

void Stack::checkStack(){
    if( (this -> begPointer == (void*) -1) || (this -> begPointer == NULL) ) throw BADPTR;
    else if( (this -> currentSize < 0) || (this -> currentSize > this -> maxSize) ) throw BADCURSZ;
    else if( this -> maxSize <= 0 ) throw BADMAXSZ;
}

int Stack::checkPoison(void* ptr){
    for(int i = 0;( i < (this -> elSize) ) && ( i < 8 ); i++){
        if( *((uint8_t*)(ptr + i)) != (this -> poison) << (7 - i) * 8 >> 56) return 0;
    }
    return 1;   
}

void Stack::fillPoison(void* ptr){
    uint8_t* charptr = (uint8_t*) ptr;
    for(int i = 0;( i < (this -> elSize) ) && ( i < 8 ); i++){
        *(charptr + i) = (this -> poison) << (7 - i) * 8 >> 56;
    }
}










