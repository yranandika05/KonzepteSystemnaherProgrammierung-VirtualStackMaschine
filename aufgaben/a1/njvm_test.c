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

#define MAXITEMS 100

int sp = 0; // stackpointer

int stack[MAXITEMS];

// METHODE for OPCODE

void pushc(int x) {
    stack[sp] = x;
    sp++;
}

void add() {
    stack[sp - 2] = stack[sp - 2] + stack[sp - 1];
    stack[sp - 1] = 0;
    sp--;
}

void sub() {
    stack[sp - 2] = stack[sp - 2] - stack[sp - 1];
    stack[sp] = 0;
    sp--;
}

void mul() {
    stack[sp - 2] = stack[sp - 2] * stack[sp - 1];
    stack[sp] = 0;
    sp--;
}

void divide() {
    stack[sp - 2] = stack[sp - 2] / stack[sp - 1];
    stack[sp] = 0;
    sp--;
}

void mod() {
    stack[sp - 2] = stack[sp - 2] % stack[sp - 1];
    stack[sp] = 0;
    sp--;
}

void rdint() {
    int input;
    scanf("%d", &input);
    stack[sp] = input;
    sp++;
}

void wrint() {
    printf("%d", stack[sp - 1]);
    stack[sp - 1] = 0;
    sp--;
}

void rdchr() {
    char input;
    scanf("%c", &input);
    stack[sp] = input;
    sp++;
}

void wrchr() {
    printf("%c", stack[sp - 1]);
    stack[sp - 1] = 0;
    sp--;
}

void print(unsigned int instruction) {
    int opcode;
    int immediate;

    opcode = instruction >> 24;
    immediate = SIGN_EXTEND(instruction & 0x00FFFFFF);

    if (opcode == 1) {
        printf("push\t%d\n", immediate);
    } else if (opcode == 2) {
        printf("add\n");
    } else if (opcode == 3) {
        printf("sub\n");
    } else if (opcode == 4) {
        printf("mul\n");
    } else if (opcode == 5) {
        printf("div\n");
    } else if (opcode == 6) {
        printf("mod\n");
    } else if (opcode == 7) {
        printf("rdint\n");
    } else if (opcode == 8) {
        printf("wrint\t\n");
    } else if (opcode == 9) {
        printf("rdchr\n");
    } else if (opcode == 10) {
        printf("wrchr\t\n");
    } else {
        printf("halt\n");
    }
}

void execute(unsigned int instruction) {
    int opcode;
    int immediate;

    opcode = instruction >> 24;
    immediate = SIGN_EXTEND(instruction & 0x00FFFFFF);

    if (opcode == 1) {
        pushc(immediate);
    } else if (opcode == 2) {
        add();
    } else if (opcode == 3) {
        sub();
    } else if (opcode == 4) {
        mul();
    } else if (opcode == 5) {
        divide();
    } else if (opcode == 6) {
        mod();
    } else if (opcode == 7) {
        rdint();
    } else if (opcode == 8) {
        wrint();
    } else if (opcode == 9) {
        rdchr();
    } else if (opcode == 10) {
        wrchr();
    }
}

int breaker(unsigned int instruction) {
    int opcode = instruction >> 24;
    if (opcode == 0) {
        return 0;
    }
    return 1;
}

// AUFGABE

unsigned int prog1[] = { (PUSHC << 24) | IMMEDIATE(3), 
                            (PUSHC << 24) | IMMEDIATE(4),
                            (ADD << 24),
                            (PUSHC << 24) | IMMEDIATE(10),
                            (PUSHC << 24) | IMMEDIATE(6),
                            (SUB << 24),
                            (MUL << 24),
                            (WRINT << 24),
                            (PUSHC << 24) | IMMEDIATE(10),
                            (WRCHR << 24),
                            (HALT << 24)
                            };

unsigned int prog2[] = { (PUSHC << 24) | IMMEDIATE(-2),
                            (RDINT << 24),
                            (MUL << 24),
                            (PUSHC << 24) | IMMEDIATE(3),
                            (ADD << 24),
                            (WRINT << 24),
                            (PUSHC << 24) | IMMEDIATE('\n'),
                            (WRCHR << 24),
                            (HALT << 24)
                            };

unsigned int prog3[] = { (RDCHR << 24),
                            (WRINT << 24),
                            (PUSHC << 24) | IMMEDIATE('\n'),
                            (WRCHR << 24),
                            (HALT << 24)
                            };                           


 // MAIN


int main (int argc, char *argv[]) {
    unsigned int instruction;
    
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            int version = strcmp(argv[i], "--version");
            int help = strcmp(argv[i], "--help");
            int pr1 = strcmp(argv[i], "--prog1");
            int pr2 = strcmp(argv[i], "--prog2");
            int pr3 = strcmp(argv[i], "--prog3");

            if (version == 0) {
                printf("Ninja Virtual Machine version 0 (compiled Sep 23 2015, 10:36:52)\n");
            } else if (help == 0) {
                printf("usage: ./njvm [option] [option] ...\n\t--version\t\tshow version and exit\n\t--help\t\t\tshow this help and exit\n");                
            } else if (pr1 == 0 || pr2 == 0 || pr3 == 0) {
                printf("Ninja Virtual Machine started\n");
                int pc = 0;

                for (;;) {
                    if (pr1 == 0) {
                        instruction = prog1[pc];
                    } else if (pr2 == 0) {
                        instruction = prog2[pc];
                    } else if (pr3 == 0) {
                        instruction = prog3[pc];
                    }
                    
                    if (pc < 10) {
                        printf("[00%d]:\t", pc);
                    } else if (pc < 99) {
                        printf("[0%2d]:\t", pc);
                    } else {
                        printf ("[%3d]:\t", pc);
                    }
                    print(instruction);
                    if (breaker(instruction) == 0) {
                        break;
                    }
                    pc++;
                }
                
                int i = 0;
                for (;;) {
                    if (pr1 == 0) {
                        instruction = prog1[i];
                    } else if (pr2 == 0) {
                        instruction = prog2[i];
                    } else if (pr3 == 0) {
                        instruction = prog3[i];
                    }
                    
                    execute(instruction);
                    if (breaker(instruction) == 0) {
                        break;
                    }
                    i++;
                }
                printf("Ninja Virtual Machine stopped\n");
                
            } else {
                printf("unknown command line argument '%s', try './njvm --help'", argv[i]);
            }
        }
    } else {
        printf("Ninja Virtual Machine started\n");
        
        printf("Ninja Virtual Machine stopped\n");
    }

    return 0;
    
}

