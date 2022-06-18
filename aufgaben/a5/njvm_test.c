 #include <stdio.h>
 #include <string.h>
 #include <stdlib.h>
 #define IMMEDIATE(x) (x & 0x00FFFFFF)
 #define SIGN_EXTEND(i) ((i) & 0x00800000 ? (i) | 0xff000000 : (i) )     // for negative value

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
int registerValue;

StackSlot stack[MAXITEMS];        //Stack
ObjektRef *sda;                     //static data area
ObjektRef opresult;                 // result of each operation

typedef struct objectType{
    unsigned int size;
    unsigned char data[1];
}*ObjektRef

typedef struct stackType{
    int isObjektRef;
    union{
        ObjektRef objektRef;
        int value;
    }u;
}StackSlot;



//GENERATE DATA STATIC AREA and PROGRAM MEMORY
void generateMemory(int programMemoryLength, int dataStaticAreaLength){
    sda = (int*)malloc(dataStaticAreaLength * sizeof(int));
    if(sda == NULL){
        printf("The data static area is not generated! \n");
        exit(1);

    }
    programMemory = (unsigned int*)malloc(programMemoryLength* sizeof(int));
    if(programMemory == NULL){
        printf("The program memory is not generated! \n");
    }
}

// METHODE for OPCODE


int popValue(){
    int popValue;
    if(sp > 0){
        popValue = stack[sp-1];
        sp--; 
    }else{
        printf("Stack is empty.\n");
        exit(1);
    }
    return popValue;
}

void pushValue(int x) {
   if(sp < MAXITEMS){ 
        stack[sp] = x;
        sp++;
   }else {
       printf("Stack is full.\n");
       exit(1);
   }
}

void add(void) {
    opresult = popValue();
    pushValue(popValue() + opresult);
}

void sub(void) {
    opresult = popValue();
    pushValue(popValue() - opresult);
}

void mul(void) {
    opresult = popValue();
    pushValue(popValue() * opresult);
}

void divide(void) {
    opresult = popValue();
    if(opresult == 0){
        printf("divisor cannot be 0");
        exit(2);
    }
    pushValue(popValue() / opresult);
}

void mod(void) {
    opresult = popValue;
    pushValue(popValue() % opresult);
}


void rdint(void) {
    int input;
    scanf("%d", &input);
    pushValue(input);
}

void wrint(void) {
    printf("%d", popValue());
}

void rdchr(void) {
    char input;
    scanf("%c", &input);
    pushValue(input);
}

void wrchr(void) {
    printf("%c", popValue());
}

void pushg(int indexGlobal){
    if(indexGlobal >= binaryData[3]){
        printf("The entered index is outside the stack data area.\n");
    }else{
        pushValue(sda[indexGlobal]);
    }
}

void popg(int indexGlobal){
    if(indexGlobal >= binaryData[3]){
         printf("The entered index is outside the stack data area\n");
    }else{
       sda[indexGlobal]= popValue();
    }
}

//Allocate Stack Frame
void asf(int asfLength){           
    if((fp + asfLength)<MAXITEMS){
        pushValue(fp);
        fp = sp;
        sp = sp + (asfLength);
    }else{
        printf("There is not enough space to add a local area\n");
    }   
}

// releaseStackFame
void rsf(void){       
    sp = fp;
    fp = popValue();   
}

void pushl(int indexLocal){
    pushValue(stack[fp+indexLocal]);
}

void popl(int indexLocal){
    stack[fp+indexLocal]=popValue();
}

void eq(void){
    if(popValue() == popValue()){
        pushValue(1);
    }else{
        pushValue(0);
    }
}

void ne(void){
     if(popValue() != popValue()){
        pushValue(1);
    }else{
        pushValue(0);
    }
}

void lt(void){
    opresult = popValue();
      if(popValue() < opresult){
        pushValue(1);
    }else{
        pushValue(0);
    }
}

void le(void){
    opresult = popValue();
    if(popValue() <= opresult){
        pushValue(1);
    }else{
        pushValue(0);
    }
}

void gt(void){
    opresult = popValue();
      if(popValue() > opresult){
        pushValue(1);
    }else{
        pushValue(0);
    }
}

void ge(void){
    opresult = popValue();
      if(popValue() >= opresult){
        pushValue(1);
    }else{
        pushValue(0);
    }
}

void jmp(int x){
    pc = x;
}

void brf(int x){
    opresult= popValue();
    if(opresult == 0){
        jmp(x);
    }
}

void brt(int x){
    opresult = popValue();
    if(opresult == 1){
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
    pushValue(registerValue);
}

void popr(void){
    registerValue=popValue();
}

void dup(void){
    pushValue(stack[sp-1]);
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
        pushValue(immediate);
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
    int i,j;
    if(sp == 0){
        printf("sp,fp       -->     %04d:   xxxx\n",sp);
    }else{
        if(sp == fp){
            printf("sp,fp   -->     %04d:   xxxx\n",sp);
            for(i=sp-1; i>=0;i--){
                printf("                %04d:   %d\n",i,stack[i]);
            }

        }else{
            printf("sp      -->  %04d:   xxx\n",sp);
            for(i=sp-1; i>fp;i--){
            printf("             %04d:   %d\n",i,stack[i]);
            }

            printf("fp      -->  %04d:   %d\n",fp,stack[fp]);
            if(fp!=0){
                for(j=fp-1; j>=0; j--){
                printf("             %04d:   %d\n",j,stack[j]);
                }
            } 
        }
    }
    printf("                ---- bottom of stack ----\n");
}

void inspectData(void){
    int i;
    for(i=0; i<binaryData[3]; i++){
        printf("data[%04d]:         %d\n",i,sda[i]);
    }
    printf("                ---- end of data ----\n");
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
            printf("DEBUG [inspect]: stack, data?\n");
                scanf("%s",charMemory);   
                if(strcmp(charMemory,"stack")==0){
                    inspectStack();
                }else if(strcmp(charMemory,"data")==0){
                    inspectData();
                }else{
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


