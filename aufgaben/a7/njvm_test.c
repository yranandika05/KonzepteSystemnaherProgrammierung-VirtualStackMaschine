 #include <stdio.h>
 #include <string.h>
 #include <stdlib.h>
 #include <ctype.h>
 #include <bigint.h>
 #include <support.h>
 #define IMMEDIATE(x) (x & 0x00FFFFFF)
 #define SIGN_EXTEND(i) ((i) & 0x00800000 ? (i) | 0xff000000 : (i) )     // for negative value
 #define MSB (1 << (8 * sizeof ( unsigned int) - 1))
 #define IS_PRIM(objektRef) ((( objektRef )->size & MSB) == 0)
 #define GET_SIZE(objektRef) ((objektRef)->size & ~MSB)
 #define GET_REFS(objektRef) ((ObjektRef *)(objektRef)->data)

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
int stopPoint = -1;
int isQuit = 0;
int debugIsOn = 0;
int opresult;

typedef struct objectType{
    unsigned int size;
    unsigned char data[1];
}*ObjektRef;

typedef struct stackType{
    int isObjektRef;
    union{
        ObjektRef objektRef;
        int value;
    }u;
}StackSlot;

StackSlot stack[MAXITEMS];        //Stack
ObjektRef *sda;                     //static data area
ObjektRef tempresult;                 // result of each operation
ObjektRef registerValue;

//GENERATE DATA STATIC AREA and PROGRAM MEMORY
void generateMemory(int programMemoryLength, int dataStaticAreaLength){
    sda = (ObjektRef*)malloc(dataStaticAreaLength * sizeof(int));
    if(sda == NULL){
        printf("The data static area is not generated! \n");
        exit(1);

    }

for(int k=0; k<dataStaticAreaLength; k++){
    sda[k]=NULL;
}

    programMemory = (unsigned int*)malloc(programMemoryLength* sizeof(int));
    if(programMemory == NULL){
        printf("The program memory is not generated! \n");
    }
}

ObjektRef ObjectSpeicher(int numObjRefs){
    ObjektRef objectAddress;
    objectAddress = (ObjektRef)malloc(sizeof(int) + (sizeof(ObjektRef)*numObjRefs));
    if(objectAddress != NULL){
        objectAddress->size = (unsigned int)(MSB | numObjRefs);
        for(int k=0; k<numObjRefs; k++){
            
            *(GET_REFS(objectAddress)+k) = NULL; 
        }
    }else {
        printf("Heap Memory is not generated.\n");
        exit(0);
    }
    return objectAddress;
}

// This section is copied from support.c in test folder

void fatalError(char *msg) {
  printf("Fatal error: %s\n", msg);
  exit(1);
}

void * newPrimObject(int dataSize) {
  ObjektRef bigObjRef;

  bigObjRef = malloc(sizeof(unsigned int) +
                  dataSize * sizeof(unsigned char));
  if (bigObjRef == NULL) {
    fatalError("newPrimObject() got no memory");
  }
  bigObjRef->size = dataSize;
  return bigObjRef;
}

void * getPrimObjectDataPointer(void * obj){
    ObjektRef oo = ((ObjektRef)  (obj));
    return oo->data;
}

//




// METHODE for OPCODE


int popValue(void){
    int popValue;
    if(sp > 0){
        sp--; 
        if(stack[sp].isObjektRef == 0){
            popValue = stack[sp].u.value;
        }else{
            printf("Cannot pop an object.\n");
            exit(2);
        }
    }else{
        printf("Stack is empty.\n");
        exit(1);
    }
    return popValue;
}

ObjektRef popObjekt(void){
    ObjektRef popObjekts;
    if(sp > 0){
        sp--;
        if(stack[sp].isObjektRef == 1){
            popObjekts = stack[sp].u.objektRef;
        }else{
            printf("Cannot pop a value.\n");
            exit(2);
        } 
    }else{
        printf("Stack is empty.\n");
        exit(1);
    }
    return popObjekts;
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
   if(sp < MAXITEMS){ 
        stack[sp].isObjektRef = 0;
         stack[sp].u.value = x;
        sp++;
   }else {
       printf("Stack is full.\n");
       exit(1);
   }
}

void pushObjekt(ObjektRef inputObjekt){
    if(sp < MAXITEMS){
        stack[sp].isObjektRef = 1;
        stack[sp].u.objektRef = inputObjekt;
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
    int input;
    scanf("%d", &input);
    bigFromInt(input);
    pushObjekt(bip.res);
}

void wrint(void) {
    bip.op1 = popObjekt();
    printf("%d", bigToInt());
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
    }else{
        pushObjekt(sda[indexGlobal]);
    }
}

void popg(int indexGlobal){
    if(indexGlobal >= binaryData[3]){
         printf("The entered index is outside the stack data area\n");
    }else{
        if(sp > 0){
            sda[indexGlobal] = popObjekt();   
        }
    }
}

//Allocate Stack Frame
void asf(int asfLength){           
    if((fp + asfLength)<MAXITEMS){
        pushValue(fp);
        fp = sp;
        sp = sp + asfLength;
        for(int i=fp ;i<sp ;i++){
            stack[i].isObjektRef=1;
            stack[i].u.objektRef=NULL;
        }
    }else{
        printf("There is not enough space to add a local area\n");
    }   
}

// releaseStackFame
void rsf(void){       
    sp = fp;
    fp = popStack();   
}

void pushl(int indexLocal){
    pushObjekt(stack[fp+indexLocal].u.objektRef);
}

void popl(int indexLocal){
    stack[fp+indexLocal].u.objektRef=popObjekt();
}

void eq(void){
    bip.op2 = popObjekt();
    bip.op1 = popObjekt();
    if(bigCmp()==1){
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
    if(bigCmp()!=1){
        bigFromInt(1);
        pushObjekt(bip.res);
    }else{
        bigFromInt(0);
        pushObjekt(bip.res);
    }
}

void lt(void){
    bip.op2 = popObjekt();
    bip.op1 = popObjekt();
    if(bigCmp()<0){
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
    if(bigCmp()>1){
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
    if(bigCmp()>=1){
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
    pc=popStack();
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
    bip.op1= popObjekt();
    pushObjekt(bip.op1);
    pushObjekt(bip.op1);
}

void new(int element){
    pushObjekt(ObjectSpeicher(element));
}

void getf(int index){
    bip.op1 = popObjekt();
    bip.op2 = *(GET_REFS(bip.op1) + index);
    pushObjekt(bip.op2);
}

void putf(int index){
    bip.op1 = popObjekt();
    bip.op2 = popObjekt();
    *(GET_REFS(bip.op2) + index) = bip.op1;
}

void newa(void){
    bip.op1 = popObjekt();
    int element = bigToInt();
    pushObjekt(ObjectSpeicher(element));
}

void getfa(){
    bip.op1 = popObjekt(); // INDEX
    bip.rem = popObjekt(); // ARRAY
    if(bigToInt()>=0 && bigToInt()<GET_SIZE(bip.rem)){
        pushObjekt(*(GET_REFS(bip.rem) + bigToInt()));
    }else{
        printf("ERROR : Out of Index.\n");
        exit(1);
    }
}

void putfa(){
    bip.op2 = popObjekt(); //VALUE
    bip.op1 = popObjekt(); //INDEX
    bip.rem = popObjekt(); //ARRAY
    if(bigToInt()>=0 && bigToInt()<GET_SIZE(bip.rem)){
        *(GET_REFS(bip.rem) + bigToInt()) = bip.op2;
    }else{
        printf("ERROR : Out of Index.\n");
        exit(1);
    }
}

void getsz(void){
    int size;
    bip.op1 = popObjekt();
    if(IS_PRIM(bip.op1)){
        bigFromInt(-1);
    }else{
        bigFromInt(GET_SIZE(bip.op1));
    }
    pushObjekt(bip.res);
}

void pushn(void){
    if(sp<MAXITEMS){
            stack[sp].isObjektRef = 1;
            stack[sp].u.objektRef = NULL;
            sp++;
    }else{
        printf("Stack is full. \n");
        exit(1);
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
        eq();
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
    if(stopPoint>1){
       while((pc!= stopPoint)&&(instruction!=0)){
        instruction = programMemory[pc];
        pc++;
        execute(instruction);
            if(instruction==0){
                exit(0);
            }
        } 
    }else{
        while(instruction!=0){
            instruction = programMemory[pc];
            pc++;
            execute(instruction);
        }
        pc=0; 
        exit(0);
    }
    instruction=99;    
}   

void inspectStack(void){
    int i,k;
    if(sp==0){
        printf("sp, fp    -->     %04d:   (xxxxxx)  xxxxxx\n",sp);
    }else{
        printf("sp      -->     %04d:   (xxxxxx) xxxxxx\n",sp);
        for(i=sp-1;i>fp;i--){
            if(stack[i].isObjektRef==1){
                printf("                %04d:   (objektReferenz) %p\n",i,stack[i].u.objektRef->data);
            }else{
                printf("                %04d:   (value) %d\n",i,stack[i].u.value);
            }
        }
        if(stack[fp].isObjektRef==1){
            printf("fp      -->     %04d:   (objektReferenz) %p\n",fp,stack[fp].u.objektRef->data);
        }else{
            printf("fp      -->     %04d:   (value) %d\n",fp,stack[fp].u.value);
        }
        if(fp!=0){
            for(k=fp-1;k>=0;k--){
                if(stack[k].isObjektRef==1){
                    printf("                %04d:   (objektReferenz) %p\n",k,stack[k].u.objektRef->data);
                }else{
                    printf("                %04d:   (value) %d\n",k,stack[k].u.value);
                }
            }
        }
    }
    printf("                ---- bottom of stack ----\n");
}
void inspectData(void){
    int i;
    for(i=0; i<binaryData[3]; i++){
        printf("data[%04d]:         (objektReferenz) %p\n",i,sda[i]->data);
    }
    printf("                ---- end of data ----\n");
}

void inspectObject(void){
    unsigned long objMemory=0;/*unsigned Long, da ist die Adresse so groß als 32 Bits ist*/
    int k, objectValue=0 , objectIsFound=0;/*checkt ob das Object falsche oder nicht gefunden ist*/
    printf("Object reference ?\n");
    scanf("%lx",&objMemory);/*bei scanf wird die ObjectSpreicher um 4 erhöht*/
    objMemory=objMemory-0x004;/*es muss darüber nachfragen. Warum wird 4 bites jedes mal hinzugefügt*/
    tempresult=(ObjektRef)objMemory;
    if(sp>0){
        for(k=0;k<sp;k++){/*läuft durch den Stack*/
            if((tempresult==(stack[k].u.objektRef))&&(stack[k].isObjektRef==1)){ 
                objectValue =*(int *)(stack[k].u.objektRef->data);/*print die value, die auf dem Stack gelegene Adresse*/
                objectIsFound=1;/*wenn das object gefunden ist, nimmt valueIsFound ein 1*/
                break;
            }
        }
    }
    if(objectIsFound==0 && binaryData[3]>0){/*wenn das Object nicht im Stack gefunden ist, dann wird er in der Static Area Static gesucht*/
        for(k=0;k<binaryData[3];k++){/*läuft durch den Static data Area*/
            if((tempresult==sda[k])){
                objectValue=*(int *)(sda[k]->data);
                objectIsFound=1;
                break;
            }
        }
    }
    if(objectIsFound==0){
        printf("Object not found\n");/*gibt Object not found aus, wenn es nicht richtig geschrieben ist, oder kein gibt*/
    }else{
        printf("Value: %d \n",objectValue);
    }
}

void list(void){
    int tempPC;
    tempPC = pc;
    pc = 0;
    while(pc < binaryData[2]){
        instruction = programMemory[pc];
        print(instruction);
        pc++;
    }
    instruction = 99;
    pc = tempPC;
    printf("                ----end of programm ----\n");

}

void breakPoint(void){
    scanf("%d",&stopPoint);
    if(stopPoint<0){
        stopPoint = -1;
        printf("DEBUG [breakpoint]: now cleared\n");
        stopPoint = stopPoint;
        printf("DEBUG [breakpoint]: now set at %d\n",stopPoint);
    }else{
        stopPoint = stopPoint;
        printf("DEBUG [breakpoint]: now set at %d\n",stopPoint);
    }
}

void step(void){
    instruction = programMemory[pc];
    pc++;
    execute(instruction);

    if(instruction==0){
        exit(99);
    }
}

void debugger(void){
    printf("DEBUG : file '%s' loaded (code size = %d, data size = %d)\n",dataName,binaryData[2], binaryData[3]);
    while(isQuit == 0){
        instruction = programMemory[pc];
        print(instruction);
        printf("DEBUG: inspect, list, breakpoint, step, run, quit?\n");
        scanf("%s",charMemory);
        if(strcmp(charMemory,"quit")==0){
            isQuit = 1;
            debugIsOn = 0;
            exit(99);
        }else if(strcmp(charMemory,"inspect")==0){
            printf("DEBUG [inspect]: stack, data, object?\n");
                scanf("%s",charMemory);   
                if(strcmp(charMemory,"stack")==0){
                    inspectStack();
                }else if(strcmp(charMemory,"data")==0){
                    inspectData();
                }else if(strcmp(charMemory,"object")==0){
                    inspectObject();  
            }   else{
                    printf("DEBUG: inspect, list, breakpoint, step, run, quit?\n");
                    scanf("%s",charMemory);
                }   
        }else if((strcmp(charMemory,"step")==0)){
            step();
        }else if(strcmp(charMemory,"list")==0){
            list();
        }else if(strcmp(charMemory,"breakpoint")==0){
            if(stopPoint!=-1){
                printf("DEBUG [breakpoint]: set at %d\n",stopPoint);
            }else{
                printf("DEBUG [breakpoint]: cleared\n");
            }
            printf("DEBUG [breakpoint]: address to set, -1 to clear, <ret> for no change?\n");
            breakPoint();
        }else if(strcmp(charMemory,"run")==0){
            runInstruction();   
            if(stopPoint<0){
                exit(0);
            }
        }        
    }
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
            printf("Usage: ./njvm [options] <code file>\n");
            printf("  --debug           start virtual machine in debug mode\n");
            printf("  --version         show version and exit\n");
            printf("  --help           show this help and  exit\n");
            exit(0);
        }else  if(((argv[i][0]) == '-')&&((argv[i][1]) == '-')&&((argv[i][2]) == 'v')&&((argv[i][3]) == 'e')&&((argv[i][4]) == 'r')&&((argv[i][5]) == 's')&&((argv[i][6]) == 'i')&&((argv[i][7]) == 'o')&&((argv[i][8]) == 'n')){
            printf("Ninja Virtual Machine version 4 (compiled Sep 23 2015, 10:36:55)\n");
            exit(0);
        }else if(strcmp(argv[1],"--debug") == 0){
            printf("Error: no code file specified\n");
            exit(1);
        }else if(argc>2 && strcmp(argv[2],"--debug")==0){
            debugIsOn = 1;
        }else if(((argv[i][0]) == '-')){
            printf("unknown option '%s' , try './njvm_test --help" , argv[i]);
            exit(0);
        }
    }
  
    if(numberOfInput<1){
        printf("Error: no code file specified\n");
        exit(1);
    }
    if(numberOfInput>1 ){
        printf("Error: more than one code file specified\n");
        exit(1);
    }
   
    

    if(dataPosition>0){

            filePointer =fopen(dataName,"r");
            
            if(filePointer == NULL){
                printf("Error: cannot open file '%s'\n",dataName);
                exit(1);
            }
            if(fread(binaryData,sizeof(int),4,filePointer)!=4){
                printf("Error: cannot read file '%s'\n",dataName);
                exit(1);
            }  
            if(binaryData[0]!= 0x46424a4e){
                printf("falsche Datei typ\n");
                exit(1);
            }
                        
            generateMemory(binaryData[2],binaryData[3]);
            if(fread(programMemory,sizeof(unsigned int),binaryData[2],filePointer)!=binaryData[2]){
                printf("anzahl der Inestruktion nicht genug\n");
                exit(1);
            }
            if(debugIsOn){
                debugger();
            }else{
                runInstruction();
            }
        
            free(programMemory);
            free(sda);  
            fclose(filePointer);
            
    }           
    return 0;   
}


