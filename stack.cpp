#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define BADPTR 1
#define BADCURSZ 2
#define BADMAXSZ 3
#define EMPTYPOP 4
#define CANST1 5
#define CANST2 6
#define CANVALS1 7
#define CANVALS2 8
#define WRSTHASH 9
#define WRDTHASH 10

const uint64_t canaryVal = 0xD18AD0B5D18AD18C;

Stack::Stack(int _elSize):
    canaryStat1(canaryVal),
    begPointer(nullptr),
    elSize(_elSize),
    currentSize(0),
    maxSize(1),
    stHash(0),
    dtHash(0),
    poison(0xEEEEEEEEEEEEEEEE),
    elDump(nullptr),
    canaryStat2(canaryVal) {
        begPointer = malloc( (16 + _elSize) + (8 - _elSize % 8));
        if( begPointer == nullptr ){
            throw BADPTR;
        }
        else{
            begPointer += 8;
            fillCanary();
            fillPoison(begPointer);
            uint32_t newStHash = calcStHash();
            uint32_t newDtHash = calcDtHash();
            this -> stHash = newStHash;
            this -> dtHash = newDtHash;
        }
}

Stack::Stack(int _elSize, int _maxSize):
    canaryStat1(canaryVal),
    begPointer(nullptr),
    elSize(_elSize),
    currentSize(0),
    maxSize(_maxSize),
    stHash(0),
    dtHash(0),
    poison(0xEEEEEEEEEEEEEEEE),
    elDump(nullptr),
    canaryStat2(canaryVal) {
        begPointer = malloc(_maxSize * _elSize + 16 + (8 - (_maxSize * _elSize) % 8));
        if( begPointer == nullptr ){
            throw BADPTR;
        }
        else{
            begPointer += 8;
            fillCanary();
            for(int i = 0; i < _maxSize; i++) fillPoison(begPointer + i*_elSize);
            uint32_t newStHash = calcStHash();
            uint32_t newDtHash = calcDtHash();
            stHash = newStHash;
            dtHash = newDtHash;
        }
}

Stack& Stack::operator=(Stack& arg){
    this -> canaryStat1 = arg.canaryStat1;
    free(this -> begPointer);
    this -> begPointer = malloc(arg.maxSize * arg.elSize + 16 + (8 - (arg.maxSize * arg.elSize) % 8));
    if(begPointer == NULL) throw BADPTR;
    else{
        arg.mvMem(arg.begPointer - 8, arg.maxSize * arg.elSize + 16 + (8 - (arg.maxSize * arg.elSize) % 4), this -> begPointer);
        this -> begPointer += 8;
        elSize = arg.elSize;
        currentSize = arg.currentSize;
        maxSize = arg.maxSize;
        poison = arg.poison;
        elDump = arg.elDump;
        canaryStat2 = arg.canaryStat2;
        stHash = 0;
        dtHash = 0;
        uint32_t newStHash = calcStHash();
        uint32_t newDtHash = calcDtHash();
        stHash = newStHash;
        dtHash = newDtHash;
    }
    return *this;
}

void Stack::mvMem(void* src, int size, void* dst){
    for(int i = 0; i < size; i++){
        *( (uint8_t*) dst + i) = *( (uint8_t*) src + i);
    }
}

uint32_t Stack::calcStHash(){
    uint32_t hash = 0;
    int i = 0;
    int size = sizeof(this -> begPointer) + sizeof(this -> elSize) + sizeof(this -> maxSize) + sizeof(this -> stHash) + sizeof(this -> dtHash) + sizeof(this -> poison) + sizeof(this -> elDump); 
    size += 4 - (size % 4); 
    uint32_t* mvPtr = (uint32_t*) calloc(1, size);
    uint8_t* ptr = (uint8_t*) mvPtr;

    mvMem(&(this -> begPointer), sizeof(this -> begPointer), ptr);
    ptr += sizeof(this -> begPointer);
    mvMem(&(this -> elSize), sizeof(this -> elSize), ptr);
    ptr += sizeof(this -> elSize);
    mvMem(&(this -> currentSize), sizeof(this -> currentSize), ptr);
    ptr += sizeof(this -> currentSize);
    mvMem(&(this -> maxSize), sizeof(this -> maxSize), ptr);
    ptr += sizeof(this -> maxSize);
    mvMem(&(this -> stHash), sizeof(this -> stHash), ptr);
    ptr += sizeof(this -> stHash);
    mvMem(&(this -> dtHash), sizeof(this -> dtHash), ptr);
    ptr += sizeof(this -> dtHash);
    mvMem(&(this -> poison), sizeof(this -> poison), ptr);
    ptr += sizeof(this -> poison);
    mvMem(&(this -> elDump), sizeof(this -> elDump), ptr);



    for(i = 0; i < size / 4; i++){
        hash += *(mvPtr + i);
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    free(mvPtr);

    return hash;
}

uint32_t Stack::calcDtHash(){
    uint32_t hash = 0;
    int i = 0;
    int size = (this -> elSize) * (this -> maxSize); 
    size += 4 - (size % 4);
    uint32_t* ptr = (uint32_t*) calloc(1, size);
    mvMem(this -> begPointer, (this -> elSize) * (this -> maxSize), ptr);

    for(i = 0; i < size / 4; i++){
        hash += *(ptr + i);
        hash += hash << 10;
        hash ^= hash >> 6;
    }

    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;

    free(ptr);

    return hash;    
}

void Stack::setPoison(uint64_t _poison){   
    this -> poison = _poison;
    for(int i = this -> currentSize; i < maxSize; i++){
        fillPoison(this -> begPointer + i * (this -> elSize));
    }

    this -> stHash = 0;
    this -> dtHash = 0;
    uint32_t newStHash = calcStHash();
    uint32_t newDtHash = calcDtHash();
    this -> stHash = newStHash;
    this -> dtHash = newDtHash;
}

Stack::~Stack(){
        free(begPointer);
        begPointer = (void*) -1;
        currentSize = 0;
        maxSize = 0;
}

void Stack::fillCanary(){
    *(uint64_t*) (this -> begPointer - 8) = canaryVal;
    *(uint64_t*) (this -> begPointer + maxSize * elSize + (8 - (maxSize * elSize) % 8)) = canaryVal;
}

void Stack::pop(void* ptr){
    this -> checkStack();

    if( this -> currentSize > 0 ){
        uint8_t* charptr = (uint8_t*) ptr;
        for(int i = 0; i < (this -> elSize); i++){
            *(charptr + i) = *( (uint8_t*) ((this -> begPointer) + (this -> currentSize - 1) * (this -> elSize) + i));  
        }
        fillPoison((this -> begPointer) + (this -> currentSize - 1) * (this -> elSize));
        (this -> currentSize)--;        
    }
    else throw EMPTYPOP;    

    this -> stHash = 0;
    this -> dtHash = 0;
    uint32_t newStHash = calcStHash();
    uint32_t newDtHash = calcDtHash();
    this -> stHash = newStHash;
    this -> dtHash = newDtHash;

    this -> checkStack();
}

void Stack::push(void* ptr){
    
    this -> checkStack();

    if( (this -> currentSize) >= (this -> maxSize) - 2 ){
        this -> begPointer = realloc(this -> begPointer - 8, 2 * (this -> maxSize) * (this -> elSize) + 16 + (8 - (2 * (this -> elSize) * (this -> maxSize) % 8)));
        if( (this -> begPointer == nullptr) ) throw BADPTR;
        
        this -> maxSize = 2 * (this -> maxSize);
        begPointer += 8;
        fillCanary();

        for(int i = (this -> currentSize); i < (this -> maxSize); i++){
            fillPoison(begPointer + i * (this -> elSize));
        }

    }

    for(int i = 0; i < (this -> elSize); i++){
        *( (uint8_t*) begPointer + (this -> currentSize) * (this -> elSize) + i) = *( (uint8_t*) ptr + i);
    }

    (this -> currentSize)++;

    this -> stHash = 0;
    this -> dtHash = 0;
    uint32_t newStHash = calcStHash();
    uint32_t newDtHash = calcDtHash();
    this -> stHash = newStHash;
    this -> dtHash = newDtHash;

    this -> checkStack();
}

int Stack::decodeErr(int err){
    switch( err ) {
            case BADPTR:     printf("STACK ERROR[%d]: Pointer on stack beginning not initialized or NULL. Stack might have been deleted.\n", BADPTR); break;
            case BADCURSZ:   printf("STACK ERROR[%d]: Current stack size (number of elements) exceeds max size or is below zero\n", BADCURSZ); break;
            case BADMAXSZ:   printf("STACK ERROR[%d]: Max stack size (max number of elements) below or equals zero;\n", BADMAXSZ); break;
            case EMPTYPOP:   printf("STACK ERROR[%d]: Pop was called on an empty stack\n", EMPTYPOP); break;
            case CANST1:     printf("STACK ERROR[%d]: First canary of the stack stats was corrupted\n", CANST1); break;
            case CANST2:     printf("STACK ERROR[%d]: Second canary of the stack stats was corrupted\n", CANST2); break;
            case CANVALS1:   printf("STACK ERROR[%d]: First canary of the actual stack contents was corrupted\n", CANVALS1); break;
            case CANVALS2:   printf("STACK ERROR[%d]: Second canary of the actual stack contents was corrupted\n", CANVALS2); break;
            case WRSTHASH:   printf("STACK ERROR[%d]: Stack stats hash doesn't match stored hash\n", WRSTHASH); break; 
            case WRDTHASH:   printf("STACK ERROR[%d]: Stack data hash doesn't match stored hash\n", WRDTHASH); break;
            default:         printf("You've caught an unknown exception. Seems like it was not generated by the stack\n"); return 1; break;
    }

    return 0;
}

void Stack::dump(){
    this -> checkStack();

    void (*print)(void*) = nullptr;
    if( this -> elDump != nullptr) print = this -> elDump;
        
    printf("---------------------Stack dump---------------------\n");
    printf("Stack dump\n");
    printf("Stack addr:\t%p\n", this);
    printf("Poison value:\t0x%lX\n", this -> poison);
    printf("\nStruct dump:\n");
    printf("\tPointer on stack beginning:\t%p\n", this -> begPointer);
    printf("\tCurrent stack size:\t        %d\n", this -> currentSize);
    printf("\tMax stack size:\t  \t        %d\n", this -> maxSize);
    printf("\nContents dump:\n");
    for( int i = 0; i < (this -> maxSize); i++){ 
        if( i == (this -> currentSize) ) printf("cur -> ");
        printf("\t[%d]:\t", i);
        if( print == nullptr ) hexPrint(this -> begPointer + i * (this -> elSize));
        else print(this -> begPointer + i * (this -> elSize));
        if( checkPoison((this -> begPointer) + i * (this -> elSize)) == 1  ) printf(" : POISON\n");
        else printf("\n");    
    }
    printf("----------------------------------------------------\n");
}

void Stack::hexPrint(void* ptr){
    for(int k = 0; k < (this -> elSize); k++){
        uint8_t val = *( (uint8_t*) ptr + k); 
        if( val <= 0xF ) printf("0");
        printf("%X ", val);
    }
}

void Stack::setPrint(void (*print)(void*)){
    this -> checkStack();

    this -> elDump = print;
    this -> stHash = 0;
    this -> dtHash = 0;
    uint32_t newStHash = calcStHash();
    uint32_t newDtHash = calcDtHash();
    this -> stHash = newStHash;
    this -> dtHash = newDtHash;

    this -> checkStack();
}

void Stack::checkStack(){
    uint32_t oldDtHash = this -> dtHash;
    this -> dtHash = 0;
    uint32_t oldStHash = this -> stHash;
    this -> stHash = 0;

    if( oldStHash != calcStHash() ) throw WRSTHASH;
    else if( oldDtHash != calcDtHash() ) throw WRDTHASH;
    else if( (this -> begPointer == (void*) -1) || (this -> begPointer == NULL) ) throw BADPTR;
    else if( *(uint64_t*)(this -> begPointer - 8) != canaryVal ) throw CANVALS1;
    else if( *(uint64_t*)(this -> begPointer + (this -> elSize) * (this -> maxSize) + (8 - (((this -> elSize) * (this -> maxSize)) % 8))) != canaryVal) throw CANVALS2;
    else if( this -> canaryStat1 != canaryVal ) throw CANST1;
    else if( this -> canaryStat2 != canaryVal ) throw CANST2;
    else if( (this -> currentSize < 0) || (this -> currentSize > this -> maxSize) ) throw BADCURSZ;
    else if( this -> maxSize <= 0 ) throw BADMAXSZ;
    
    this -> dtHash = oldDtHash;
    this -> stHash = oldStHash;
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

int decodeStackErr(int err){
    switch( err ) {
            case BADPTR:     printf("STACK ERROR[%d]: Pointer on stack beginning not initialized or NULL. Stack might have been deleted.\n", BADPTR); break;
            case BADCURSZ:   printf("STACK ERROR[%d]: Current stack size (number of elements) exceeds max size or is below zero\n", BADCURSZ); break;
            case BADMAXSZ:   printf("STACK ERROR[%d]: Max stack size (max number of elements) below or equals zero;\n", BADMAXSZ); break;
            case EMPTYPOP:   printf("STACK ERROR[%d]: Pop was called on an empty stack\n", EMPTYPOP); break;
            case CANST1:     printf("STACK ERROR[%d]: First canary of the stack stats was corrupted\n", CANST1); break;
            case CANST2:     printf("STACK ERROR[%d]: Second canary of the stack stats was corrupted\n", CANST2); break;
            case CANVALS1:   printf("STACK ERROR[%d]: First canary of the actual stack contents was corrupted\n", CANVALS1); break;
            case CANVALS2:   printf("STACK ERROR[%d]: Second canary of the actual stack contents was corrupted\n", CANVALS2); break;
            case WRSTHASH:   printf("STACK ERROR[%d]: Stack stats hash doesn't match stored hash\n", WRSTHASH); break; 
            case WRDTHASH:   printf("STACK ERROR[%d]: Stack data hash doesn't match stored hash\n", WRDTHASH); break;
            default:         printf("You've caught an unknown exception. Seems like it was not generated by the stack\n"); return 1; break;
    }

    return 0;
}





