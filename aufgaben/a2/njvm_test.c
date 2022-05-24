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


#define MAXITEMS 1000

int opcode;
int immediate;
int opresult; // result of each operation
int pc;
int sp = 0; // stackpointer
int fp = 0; // framepointer
int binaryData[5];
int* sda;   //static data area
unsigned int* programmSpeicher;
unsigned int instruction = 99;
char* fileName;
int dataPosition = -1;
char* dataName;
int stopPoint = -1;

int stack[MAXITEMS];   //Stack

//GENERATE DATA STATIC AREA and PROGRAM MEMORY
void speicherReservierung(int programmSpeicherLength, int dataStaticAreaLength){
    sda = (int*)malloc(dataStaticAreaLength * sizeof(int));
    if(sda == NULL){
        printf("The data static area is not generated! \n");
        exit(1);

    }
    programmSpeicher = (unsigned int*)malloc(programmSpeicherLength* sizeof(int));
    if(programmSpeicher == NULL){
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
    opresult = popValue() + popValue();
    pushValue(opresult);
}

void sub(void) {
    opresult = popValue() - popValue();
    pushValue(opresult);
}

void mul(void) {
    opresult = popValue() * popValue();
    pushValue(opresult);
}

void divide(void) {
    opresult = popValue();
    if(opresult == 0){
        printf("divisor cannot be 0");
        exit(2);
    }
    opresult = popValue() / opresult;
    pushValue(opresult);
}

void mod(void) {
  opresult = popValue() % popValue();
    pushValue(opresult);
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
    if(indexGlobal < binaryData[3]){
        pushValue(sda[indexGlobal]);
    }else{
printf("The entered index is outside the stack data area.\n");
    }
}

void popg(int indexGlobal){
    if(indexGlobal < binaryData[3]){
        sda[indexGlobal]= popValue();
    }else{
        printf("The entered index is outside the stack data area\n");
    }
}

//Allocate Stack Frame
void asf(int asfLength){           
    if((fp + asfLength)<1000){
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
    }
}

void execute(int instruction){
    opcode = instruction >> 24;
    immediate = SIGN_EXTEND(instruction & 0x00FFFFFF);
   
   switch (opcode)
    {
    case HALT:
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
   
    default:
        printf("Error \n");execute(instruction);
        break;
    }   
}

void runInstruktion(void){
    if(pc==0){
        printf("Ninja Virtual Machine started\n");
    }
    if(stopPoint>1){
        while(instruction!=0){
            instruction = programmSpeicher[pc];
            pc++;
            print(instruction);
             }
            pc=0; 

       while((pc!= stopPoint)&&(instruction!=0)){
        instruction = programmSpeicher[pc];
        pc++;
        execute(instruction);
            if(instruction==0){
                exit(0);
            }
        } 
    }else{
        while(instruction!=0){
            instruction = programmSpeicher[pc];
            pc++;
            print(instruction);
             }
            pc=0; 

        while(instruction!=0){
            instruction = programmSpeicher[pc];
            pc++;
            execute(instruction);
        }
        pc=0; 
        exit(0);
    }
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
        if(((argv[i][0]) == '-')&&((argv[i][1]) == '-')&&((argv[i][2]) == 'h')){
            printf("Usage: ./njvm [options] <code file>\n");
            printf("  --version         show version and exit\n");
            printf("  --help           show this help and  exit\n");
            exit(0);
        }
    }
    for( i=0; i<argc;i++ ){
        if(((argv[i][0]) == '-')&&((argv[i][1]) == '-')&&((argv[i][2]) == 'v')){
            printf("Ninja Virtual Machine version 2 (compiled Sep 23 2015, 10:36:53)\n");
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
                        
            speicherReservierung(binaryData[2],binaryData[3]);
            if(fread(programmSpeicher,sizeof(unsigned int),binaryData[2],filePointer)!=binaryData[2]){
                printf("anzahl der Inestruktion nicht genug\n");
                exit(1);
            }

            runInstruktion();

            free(programmSpeicher);
            free(sda);  
            fclose(filePointer);
    }            
    return 0;
    
}


