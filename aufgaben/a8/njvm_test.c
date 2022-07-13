 #include <stdio.h>
 #include <string.h>
 #include <stdlib.h>
 
 //#include <ctype.h>
 #include "bigint.h"
 #define IMMEDIATE(x) (x & 0x00FFFFFF)
 #define SIGN_EXTEND(i) ((i) & 0x00800000 ? (i) | 0xff000000 : (i) )     // for negative value
 #define MSB (1 << (8 * sizeof(unsigned int) -1))
 #define BHF (1 << (8 * sizeof(unsigned int) -2)) /* brokenHeartFlag*/
 #define IS_PRIM(objRef) ((((ObjRef)objRef)->size & (MSB | BHF)) == 0)
 #define GET_SIZE(objRef) (((~(MSB | BHF) & ((ObjRef)objRef)->size)))
 #define GET_REFS(objRef) ((ObjRef *)((ObjRef)objRef)->data)


// OPCODE

#define HALT 0          
#define PUSHC 1
#define ADD 2
#define SUB 3
#define MUL 4
#define DIV 5
#define MOD 6
#define RDINT 7 
#define WRINT 8
#define RDCHR 9
#define WRCHR 10
#define PUSHG 11
#define POPG 12
#define ASF 13
#define RSF 14
#define PUSHL 15
#define POPL 16
#define EQ 17
#define NE 18
#define LT 19
#define LE 20
#define GT 21
#define GE 22
#define JMP 23
#define BRF 24
#define BRT 25
#define CALL 26
#define RET 27
#define DROP 28
#define PUSHR 29
#define POPR 30
#define DUP 31
#define NEW 32
#define GETF 33
#define PUTF 34
#define NEWA 35
#define GETFA 36
#define PUTFA 37
#define GETSZ 38
#define PUSHN 39
#define REFEQ 40
#define REFNE 41


#define MAXITEMS 1000
#define KILOBYTE 1024

int opcode;
int immediate;
int pc;
int sp = 0; // stackpointer
int fp = 0; // framepointer
char charMemory[15];
int binaryData[5];
unsigned int* programMemory;
unsigned int instruction = 99;
char* fileName;
int dataPosition = -1;
char* dataName;
int opresult;
int sizeOfStack;
int sizeOfHeap;
int numberOfHeap = 0;
int defaultSizeOfHeap = 8192;
int defaultSizeOfStack = 64;


char *heap;
char *heapTarget;
char *toSpace;
char *heapFreePointer;
char *scan;
char *halfHeap;
char *endHeap;
char *tempPointer;


typedef struct {
    unsigned int size;
    unsigned char data[1];
}*ObjRef;


typedef struct {
    int isObjektRef;
    union Union{
        ObjRef objRef;
        int value;
    }u;
}StackSlot;

StackSlot *stack;        //Stack
ObjRef *sda;                     //static data area
ObjRef tempresult;                 // result of each operation
ObjRef registerValue;

//GENERATE HEAP, STACK, DATA STATIC AREA and PROGRAM MEMORY
void generateMemory(int programMemoryLength, int dataStaticAreaLength, int defaultSizeOfHeap, int defaultSizeOfStack){

    sizeOfHeap = defaultSizeOfHeap * KILOBYTE;
    heap = (char *)malloc(sizeOfHeap);
    if(heap == NULL){
        printf("Heap is not generated");
        exit(139);
    }

    sizeOfStack = defaultSizeOfStack * KILOBYTE;
    stack = (StackSlot *)malloc(sizeOfStack);
    if(stack == NULL){
        printf("Heap is not generated");
        exit(139);
    }
    
    sda = (ObjRef*)malloc(dataStaticAreaLength * sizeof(int));
    if(sda == NULL){
        printf("The data static area is not generated! \n");
        exit(139);
    }
    /*
    for(int k=0; k<dataStaticAreaLength; k++){
        if(sda[k] != NULL){
            sda[k] = NULL;
        }
    }*/

    programMemory = (unsigned int*)malloc(programMemoryLength* sizeof(int));
    if(programMemory == NULL){
        printf("The program memory is not generated! \n");
        exit(139);
    }

    heapTarget = heap;
    toSpace = (heap + (sizeOfHeap/2));
    halfHeap = toSpace;
    heapFreePointer = heapTarget;
    endHeap = (heap + sizeOfHeap);
}






//Section to prepare Garbage Collector//

ObjRef copyPhase(ObjRef orig){
    ObjRef copy;

    copy = (ObjRef)heapFreePointer;
    if(IS_PRIM(orig)){
        if((heapFreePointer + sizeof(unsigned int)+(sizeof(unsigned char)*GET_SIZE(orig)) <= halfHeap)){
            memcpy(copy,orig,sizeof(unsigned int)+(sizeof(unsigned char)*GET_SIZE(orig)));
            heapFreePointer += sizeof(unsigned int)+(sizeof(unsigned char)*GET_SIZE(orig));
        }else{
            printf("Heap Overflow.\n");
            exit(0);
        }
    }else{
        if((heapFreePointer + sizeof(unsigned int) + (sizeof(ObjRef)*GET_SIZE(orig)) <= halfHeap)){
            memcpy(copy,orig,(sizeof(unsigned int) + (sizeof(ObjRef)*GET_SIZE(orig))));
            heapFreePointer += sizeof(unsigned int) + (sizeof(ObjRef)*GET_SIZE(orig));
        }else{
            printf("Heap Overflow.\n");
            exit(0);
        }
    }
    return copy;
}





ObjRef relocate ( ObjRef orig) {
    ObjRef copy;
    if (orig == NULL) {
        /* relocate (nil) = nil */
        copy = NULL;
    } else if (orig->size && 0x00000004) {
        /* Objekt ist bereits kopiert , Forward - Pointer gesetzt */
        //copy = (ObjRef)(heapTarget + (orig->size & 0x3FFFFFFF));
        copy = (ObjRef)&orig->size;
    } else {
        /* Objekt muss noch kopiert werden */
        copy = copyPhase(orig);
        orig->size = (orig->size | 0x00000004);
        orig->size = (orig->size && 0x0000000c) | (copy && 0xFFFFFFF3);
    }
    /* Adresse des kopierten Objektes zurück */
    return copy;
}


void scanPhase(void){
    tempPointer = toSpace;
    toSpace = heapTarget;
    heapTarget = tempPointer;
    tempPointer = halfHeap;
    halfHeap = endHeap;
    endHeap = tempPointer; 
    heapFreePointer = heapTarget;
    registerValue = relocate(registerValue);
    
    for(int i=0; i<binaryData[3]; i++){
            sda[i] = relocate(sda[i]);
        }

    bip.op1 = relocate(bip.op1);
    bip.op2 = relocate(bip.op2);
    bip.res = relocate(bip.res);
    bip.rem = relocate(bip.rem);
    

  
    /*
    for(int i=0; i<sp+1; i++){
        if(stack[i].isObjektRef == 1){
            stack[i].u.objRef = relocate(stack[i].u.objRef);
        }
    } */   

    scan = heapFreePointer;
    while (scan != heapFreePointer ) {
        /* es gibt noch Objekte , die gescannt werden müssen */
        if(IS_PRIM((ObjRef)scan)){
            scan += sizeof(unsigned int) + GET_SIZE((ObjRef)scan) * sizeof(unsigned char);
        }else{
            for(int i=0;i<GET_SIZE((ObjRef)scan);i++){
                *(GET_REFS((ObjRef)scan)+i) = relocate(*(GET_REFS((ObjRef)scan)+i));
            }
            scan += sizeof(scan);               //(unsigned int) + (sizeof(ObjRef)*GET_SIZE((ObjRef)scan));
        }
    }
    

}

ObjRef pushHeap( int sizeOfObject){
    char* object;
    if((heapFreePointer + sizeOfObject) >= halfHeap){

       numberOfHeap=0;
       scanPhase();

    }
    object = heapFreePointer;
   
    if (object == NULL) {
        printf("object is NULL.\n");
        exit(0);
    }
    heapFreePointer += sizeOfObject;
    return (ObjRef)object;
}


ObjRef ObjectSpeicher(int numObjRefs){
    ObjRef objectAddress;
    objectAddress = (ObjRef)pushHeap(sizeof(int) + (sizeof(ObjRef)*numObjRefs));
    if(objectAddress == NULL){
        printf("Heap Memory is not generated.\n");
        exit(0);
    }else { 
        objectAddress->size = (unsigned int)(MSB | numObjRefs);
        for(int k=0; k<numObjRefs; k++){
            
            *(GET_REFS(objectAddress)+k) = NULL; 
        }
    }
    return objectAddress;
}
// This section is copied from support.c in test folder



void fatalError(char *msg) {
  printf("Fatal error: %s\n", msg);
  exit(1);
}

void *newPrimObject(int dataSize) {
  ObjRef bigObjRef;

  bigObjRef = pushHeap(sizeof(unsigned int) + dataSize * sizeof(unsigned char));
  if (bigObjRef == NULL) {
    fatalError("newPrimObject() got no memory");
  }
  bigObjRef->size = dataSize;
  return bigObjRef;
}

void * getPrimObjectDataPointer(void * obj){
    ObjRef oo = ((ObjRef)  (obj));
    return oo->data;
}

//

// METHODE for OPCODE


int popValue(void){
    int popValue;
    if(sp > 0){
        if(stack[sp-1].isObjektRef == 0){
            popValue = stack[sp-1].u.value;
            sp--;
        }else{
            printf("Cannot pop an object.\n");
            exit(0);
        }
    }else{
        printf("Stack is empty.\n");
        exit(0);
    }
    return popValue;
}

ObjRef popObjekt(void){
    ObjRef popObject;
    if(sp > 0){
        if(stack[sp-1].isObjektRef == 1){
             popObject = stack[sp-1].u.objRef;
             sp--;
        }else{
            printf("Cannot pop a value.\n");
            exit(2);
        } 
    }else{
        printf("Stack is empty.\n");
        exit(0);
    }
    return popObject;
}

int popStack(void){
    int popStacks;
    if(sp > 0){
        if(stack[sp-1].isObjektRef == 0){
            popStacks = popValue();
        }else { 
            tempresult = popObjekt();
            popStacks = *(int *)tempresult->data;
        }  
    }else{
        printf("Stack is empty.\n");
        exit(0);
    }
    
    return popStacks;
}

void pushValue(int x) {
   if(sp < (defaultSizeOfStack * 1024) / sizeof(StackSlot)){ 
        stack[sp].isObjektRef = 0;
         stack[sp].u.value = x;
        sp++;
   }else {
       printf("Stack is full.\n");
       exit(0);
   }
}

void pushObjekt(ObjRef inputObjekt){
    if(sp < (defaultSizeOfStack * 1024) / sizeof(StackSlot)){
        stack[sp].isObjektRef = 1;
        stack[sp].u.objRef = inputObjekt;
        sp++;
    }else if(sp >= MAXITEMS){
        printf("Stack is full.\n");
    }
}

void add(void) {
    bip.op2 = popObjekt();
    bip.op1 = popObjekt();
    bigAdd();
    pushObjekt(bip.res);
}

void sub(void) {
    bip.op2 = popObjekt();
    bip.op1 = popObjekt();
    bigSub();
    pushObjekt(bip.res);
}

void mul(void) {
    bip.op2 = popObjekt();
    bip.op1 = popObjekt();
    bigMul();
    pushObjekt(bip.res);
}

void divide(void) {
    bip.op2 = popObjekt();
    bip.op1 = popObjekt();
    bigDiv();
    pushObjekt(bip.res);
}

void mod(void) {
    bip.op2 = popObjekt();
    bip.op1 = popObjekt();
    bigDiv();
    pushObjekt(bip.rem);
}


void rdint(void) {
    fflush(stdout);
    bigRead(stdin);
    pushObjekt(bip.res);
}

void wrint(void) {
    bip.op1 = popObjekt();
    bigPrint(stdout);
}

void rdchr(void) {
    char input;
    scanf("%c", &input);
    bigFromInt(input);
    pushObjekt(bip.res);
}

void wrchr(void) {
    bip.op1 = popObjekt();
    printf("%c", bigToInt());
}

void pushg(int indexGlobal){
    if(indexGlobal >= binaryData[3]){
        printf("The entered index is outside the stack data area.\n");
        exit(0);
    }else{
        pushObjekt(sda[indexGlobal]);
        
    }
}

void popg(int indexGlobal){
    if(indexGlobal >= binaryData[3]){
         printf("The entered index is outside the stack data area\n");
         exit(0);
    }else{
        if(sp > 0){
            sda[indexGlobal] = popObjekt();   
        }
    }
}

//Allocate Stack Frame
void asf(int asfLength){           
    if((fp + asfLength)<(defaultSizeOfStack * 1024)){
        pushValue(fp);
        fp = sp;
        sp = sp + asfLength;
        for(int i=fp ;i<sp ;i++){
            stack[i].isObjektRef=1;
            stack[i].u.objRef=NULL;
        }
    }else{
        printf("There is not enough space to add a local area\n");
        exit(0);
    }   
}

// releaseStackFame
void rsf(void){       
    sp = fp;
    fp = popValue();   
}

void pushl(int indexLocal){
    pushObjekt(stack[fp+indexLocal].u.objRef);
}

void popl(int indexLocal){
    stack[fp+indexLocal].u.objRef=popObjekt();
}

void eq(void){
    bip.op2 = popObjekt();
    bip.op1 = popObjekt();
    if(bigCmp()==0){
        bigFromInt(1);
        pushObjekt(bip.res);
    }else{
        bigFromInt(0);
        pushObjekt(bip.res);
    }
}

void ne(void){
    bip.op2 = popObjekt();
    bip.op1 = popObjekt();
    if(bigCmp()!=0){
        bigFromInt(1);
        pushObjekt(bip.res);
    }else{
        bigFromInt(0);
        pushObjekt(bip.res);
    }
}

void lt(void){
    int comp;
    bip.op2 = popObjekt();
    bip.op1 = popObjekt();
    comp = bigCmp();
    if(comp < 0){
        bigFromInt(1);
        pushObjekt(bip.res);
    }else{
        bigFromInt(0);
        pushObjekt(bip.res);
    }
}

void le(void){
    bip.op2 = popObjekt();
    bip.op1 = popObjekt();
    if(bigCmp()<=0){
        bigFromInt(1);
        pushObjekt(bip.res);
    }else{
        bigFromInt(0);
        pushObjekt(bip.res);
    }
}

void gt(void){
    bip.op2 = popObjekt();
    bip.op1 = popObjekt();
    if(bigCmp()>0){
        bigFromInt(1);
        pushObjekt(bip.res);
    }else{
        bigFromInt(0);
        pushObjekt(bip.res);
    }
}

void ge(void){
    bip.op2 = popObjekt();
    bip.op1 = popObjekt();
    if(bigCmp()>=0){
        bigFromInt(1);
        pushObjekt(bip.res);
    }else{
        bigFromInt(0);
        pushObjekt(bip.res);
    }
}

void jmp(int x){
    pc = x;
}

void brf(int x){
    bip.op1 = popObjekt();
    bigFromInt(0);
    bip.op2 = bip.res;
    if(bigCmp()==0){
        jmp(x);
    }
}

void brt(int x){
    bip.op1 = popObjekt();
    bigFromInt(1);
    bip.op2 = bip.res;
    if(bigCmp()==0){
        jmp(x);
    }
}

void call(int callIndex){
    pushValue(pc);
    jmp(callIndex);
}

void ret(void){
    pc=popValue();
}

void drop(int number){
    sp=sp-number;
}

void pushr(void){
    pushObjekt(registerValue);
}

void popr(void){
    registerValue=popObjekt();
}

void dup(void){
    pushObjekt(stack[sp-1].u.objRef);
}

void new(int element){
    pushObjekt(ObjectSpeicher(element));
}

void getf(int index){
    bip.op1 = popObjekt();
    bip.op2 = GET_REFS(bip.op1)[index];
    pushObjekt(bip.op2);
}

void putf(int index){
    bip.op1 = popObjekt();
    bip.op2 = popObjekt();
    GET_REFS(bip.op2)[index] = bip.op1;
}

void newa(void){
    bip.op1 = popObjekt();
    int element = bigToInt();
    pushObjekt(ObjectSpeicher(element));
}

void getfa(){
    bip.op1 = popObjekt(); // INDEX
    bip.rem = popObjekt(); // ARRAY
    if(bigToInt()<0 && bigToInt()>=GET_SIZE((ObjRef)bip.rem)){
        printf("ERROR : Out of Index.\n");
        exit(0);       
    }else{
        pushObjekt(*(GET_REFS((ObjRef)bip.rem) + bigToInt()));
    }
}

void putfa(){
    bip.op2 = popObjekt(); //VALUE
    bip.op1 = popObjekt(); //INDEX
    bip.rem = popObjekt(); //ARRAY
    if(bigToInt()<0 && bigToInt()>GET_SIZE(bip.rem)){
        printf("ERROR : Out of Index.\n");
        exit(0);   
    }else{
        *(GET_REFS(bip.rem) + bigToInt()) = bip.op2;
    }
}

void getsz(void){
    bip.op1 = popObjekt();
    if(IS_PRIM(bip.op1)){
        bigFromInt(-1);
    }else{
        bigFromInt(GET_SIZE(bip.op1));
    }
    pushObjekt(bip.res);
}

void pushn(void){
    if(sp<(defaultSizeOfStack * 1024) / sizeof(StackSlot)){
            stack[sp].isObjektRef = 1;
            stack[sp].u.objRef = NULL;
            sp++;
    }else{
        printf("Stack is full. \n");
        exit(0);
    }
}

void refeq(){
    bip.op1 = popObjekt();
    bip.op2 = popObjekt();
    if(bip.op1 == bip.op2){
        bigFromInt(1);
    }else{
        bigFromInt(0);
    }
    pushObjekt(bip.res);
}   

void refne(){
    bip.op1 = popObjekt();
    bip.op2 = popObjekt();
    if(bip.op1 != bip.op2){
        bigFromInt(1);
    }else{
        bigFromInt(0);
    }
    pushObjekt(bip.res);
}



// PRINT INSTRUCTION
void print(unsigned int instruction) {
    
    opcode = instruction >> 24;
    immediate = SIGN_EXTEND(instruction & 0x00FFFFFF);

   switch (opcode)
    {
    case HALT:
        printf("%04d:      halt\n",pc);
        break;
    case PUSHC:
        printf("%04d:      pushc    %d\n",pc,immediate);
        break;
    case ADD:
        printf("%04d:      add\n",pc);
        break;
    case SUB:
        printf("%04d:      sub\n",pc);
        break;
    case MUL:
        printf("%04d:      mul\n",pc);
        break;
    case DIV:
        printf("%04d:      div\n",pc);
        break;
    case MOD:
        printf("%04d:      mod\n",pc);
        break;
    case RDINT:
        printf("%04d:      rdint\n",pc);
        break;
    case WRINT:
        printf("%04d:      wrint\n",pc);
        break;
    case RDCHR:
        printf("%04d:      rdchr\n",pc);
        break;
    case WRCHR:
        printf("%04d:      wrchr\n",pc);
        break;
    case PUSHG:
        printf("%04d:      pushg    %d\n",pc,immediate);
        break;
    case POPG:
        printf("%04d:      popg     %d\n",pc,immediate);
        break;
    case ASF:
        printf("%04d:      asf      %d\n",pc,immediate);
        break;
    case RSF:
        printf("%04d:      rsf      %d\n",pc,immediate);
        break;
    case PUSHL:
        printf("%04d:      pushl    %d\n",pc,immediate);
        break;
    case POPL:
        printf("%04d:      popl     %d\n",pc,immediate);
        break;
    case EQ:
        printf("%04d:      eq     \n",pc);
        break; 
    case NE:
        printf("%04d:      ne     \n",pc);
        break;
    case LT:
        printf("%04d:      lt     \n",pc);
        break;
    case LE:
        printf("%04d:      le     \n",pc);
        break;
    case GT:
        printf("%04d:      gt     \n",pc);
        break;
    case GE:
        printf("%04d:      ge     \n",pc);
        break;
    case JMP:
        printf("%04d:      jmp     %d\n",pc,immediate);
        break;
    case BRF:
        printf("%04d:      brf     %d\n",pc,immediate);
        break;
    case BRT:
        printf("%04d:      brt     %d\n",pc,immediate);
        break;
    case CALL:
        printf("%04d:      call     %d\n",pc,immediate);
        break;
    case RET:
        printf("%04d:      ret        \n",pc);
        break;
    case DROP:
        printf("%04d:      drop     %d\n",pc,immediate);
        break;
    case PUSHR:
        printf("%04d:      pushr      \n",pc);
        break;
    case POPR:
        printf("%04d:      popr       \n",pc);
        break;
    case DUP:
        printf("%04d:      dup        \n",pc);
        break;
    case NEW:
        printf("%04d:      new      %d\n",pc,immediate);
        break;
    case GETF:
        printf("%04d:      getf      %d\n",pc,immediate);
        break;
    case PUTF:
        printf("%04d:      putf      %d\n",pc,immediate);
        break;
    case NEWA:
        printf("%04d:      newa        \n",pc);
        break;
    case GETFA:
        printf("%04d:      getfa        \n",pc);
        break;
    case PUTFA:
        printf("%04d:      putfa        \n",pc);
        break;
    case GETSZ:
        printf("%04d:      getsz        \n",pc);
        break;
    case PUSHN:
        printf("%04d:      pushn        \n",pc);
        break;
    case REFEQ:
        printf("%04d:      refeq        \n",pc);
        break;
    case REFNE:
        printf("%04d:      refne        \n",pc);
        break;

    
    }
}

void execute(int instruction){
    opcode = instruction >> 24;
    immediate = SIGN_EXTEND(instruction & 0x00FFFFFF);
   
   switch (opcode)
    {
    case HALT:
        pushValue(immediate);
        printf("Ninja Virtual Machine stopped\n");
        break;
    case PUSHC:
        bigFromInt(immediate);
        pushObjekt(bip.res);
        break;
     case ADD:
        add();
        break;
    case SUB:
        sub();
        break;
    case MUL:
        mul(); 
        break;
    case DIV:
        divide();
        break;
    case MOD:
        mod();
        break;
    case RDINT:
        rdint();
        break;
    case WRINT:
        wrint();
        break;
    case RDCHR:
        rdchr();
        break;
    case WRCHR:
        wrchr();
        break;
    case PUSHG:
        pushg(immediate); 
        break;
    case POPG:
        popg(immediate);
        break;
    case ASF:
        asf(immediate);
        break;
    case RSF:
        rsf();
        break;
    case PUSHL:
        pushl(immediate);
        break;
    case POPL:
        popl(immediate);
        break;
      case EQ:
        eq();
        break; 
    case NE:
        ne();
        break;
    case LT:
        lt();
        break;
    case LE:
        le();
        break;
    case GT:
        gt(); 
       break;
    case GE:
        ge();
        break;
    case JMP:
        jmp(immediate);
        break;
    case BRF:
        brf(immediate);
        break;
    case BRT:
        brt(immediate);
        break;
    case CALL:
        call(immediate);
        break;
    case RET:
        ret();
        break;
    case DROP:
        drop(immediate);
        break;
    case PUSHR:
        pushr();
        break;
    case POPR:
        popr();
        break;
    case DUP:
        dup();
        break;
    case NEW:
        new(immediate);
        break;
    case GETF:
        getf(immediate);
        break;
    case PUTF:
        putf(immediate);
        break;
    case NEWA:
        newa();
        break;
    case GETFA:
        getfa();
        break;
    case PUTFA:
        putfa();
        break;
    case GETSZ:
        getsz();
        break;
    case PUSHN:
        pushn();
        break;
    case REFEQ:
        refeq();
        break;
    case REFNE:
        refne();
        break;
   
    default:
        printf("Error \n");execute(instruction);
        break;
    }   
}

void runInstruction(void){
    if(pc==0){
        printf("Ninja Virtual Machine started\n");
    }
    
    while(instruction!=0){
        instruction = programMemory[pc];
        pc++;
        execute(instruction);
    }
    pc=0; 
    exit(0);
    
    instruction=99;    
}   


 // MAIN


int main (int argc, char *argv[]) {
    int i, numberOfInput = 0;
    FILE * filePointer;
    
    for( i=0; i<argc;i++ ){
        if(((argv[i][strlen(argv[i])-1]) == 'n')&& ((argv[i][strlen(argv[i])-2]) == 'i')&&((argv[i][strlen(argv[i])-3]) == 'b')&&((argv[i][strlen(argv[i])-4]) == '.')){
            dataPosition=i;
            dataName=argv[i];
        }
        if(((argv[i][0]) != '-')&&((argv[i][1]) != '-')&& (i!=0)){
            numberOfInput++;
        }
    }

    for( i=0; i<argc;i++ ){
        if(((argv[i][0]) == '-')&&((argv[i][1]) == '-')&&((argv[i][2]) == 'h')&&((argv[i][3]) == 'e')&&((argv[i][4]) == 'l')&&((argv[i][5]) == 'p')){
            printf("Usage: ./njvm [options] <code file>\nOptions:\n");
            printf("        --stack <n>      set stack size to n KBytes (default: n = 64)\n");
            printf("        --heap <n>       set heap size to n KBytes (default: n = 8192)\n");
            printf("        --gcstats        show garbage collection statistics\n");
            printf("        --gcpurge        purge old objects after collection\n");
            printf("        --debug          start virtual machine in debug mode\n");
            printf("        --version        show version and exit\n");
            printf("        --help          show this help and  exit\n");
            exit(0);
        }else  if(((argv[i][0]) == '-')&&((argv[i][1]) == '-')&&((argv[i][2]) == 'v')&&((argv[i][3]) == 'e')&&((argv[i][4]) == 'r')&&((argv[i][5]) == 's')&&((argv[i][6]) == 'i')&&((argv[i][7]) == 'o')&&((argv[i][8]) == 'n')){
            printf("Ninja Virtual Machine version 8 (compiled Sep 23 2015, 10:36:55)\n");
            exit(0);
        }else if(strcmp(argv[1],"--debug") == 0){
            printf("Error: no code file specified\n");
            exit(0);
        }else if(argc>2 && strcmp(argv[2],"--debug")==0){
            
        }else if(argc>2 && strcmp(argv[i],"--stack")==0){
            if((argv[i+1]==NULL)){
                printf("Please input Stack size");
                exit(0);
            }   
            if((atoi(argv[i+1])==0)){
                printf("Stack Size cant be 0");
                exit(0);
            }
            numberOfInput --;
            defaultSizeOfStack=atoi(argv[i+1]);

        }else if(argc>2 && strcmp(argv[i],"--heap")==0){
            if(argv[i+1]==NULL){
                printf("Heap size is missing");
                exit(0);
            }
            if((atoi(argv[i+1])==0)){
                printf("illegal heap size");
                exit(0);
            }
            numberOfInput --;
            defaultSizeOfHeap=atoi(argv[i+1]);
            
        }else if(argc>2 && strcmp(argv[i],"--gcstats")==0){
            

        }else if(argc>2 && strcmp(argv[i],"--gcpurge")==0){
            

        }else if(((argv[i][0]) == '-')){
            printf("unknown option '%s' , try './njvm_test --help" , argv[i]);
            exit(0);
        }
    }
  
    if(numberOfInput<1){
        printf("Error: no code file specified\n");
        exit(0);
    }
    if(numberOfInput>1 ){
        printf("Error: more than one code file specified\n");
        exit(0);
    }


    if(dataPosition>0){

            filePointer =fopen(dataName,"r");
            
            if(filePointer == NULL){
                printf("Error: cannot open file '%s'\n",dataName);
                exit(0);
            }
            if(fread(binaryData,sizeof(int),4,filePointer)!=4){
                printf("Error: cannot read file '%s'\n",dataName);
                exit(0);
            }  
            if(binaryData[0]!= 0x46424a4e){
                printf("Input file is not a .bin data\n");
                exit(0);
            }
                        
            generateMemory(binaryData[2],binaryData[3], defaultSizeOfHeap, defaultSizeOfStack);
            if(fread(programMemory,sizeof(unsigned int),binaryData[2],filePointer)!=binaryData[2]){
                printf("anzahl der Inestruktion nicht genug\n");
                exit(0);
            }
            
            runInstruction();
            
            free(sda);  
            free(programMemory);
           
            fclose(filePointer);
            
    }           
    return 0;   
}


