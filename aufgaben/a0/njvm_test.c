 #include <stdio.h>
 #include <string.h>
 #include <stdlib.h>


 int main(int argc, char * argv[]){

    char version[100] = "Ninja Virtual Maschine version 0 (compiled Sep 2015, 10:36:52)";
    char help[150] = "usage: ./njvm [option] [option]...\n --version     show version and exit\n --help        show this help and exit";

    if (argc > 1){
        if(strcmp(argv[1] , "--version") == 0){
            printf("%s", version);
        }else if(strcmp(argv[1] , "--help") == 0){
            printf("%s", help);
        }else {
            printf("unknown command line argument '%s' , try './njvm --help" , argv[1]);
        }
    }else {
        printf("Ninja Virtual Machine started \n");
        printf("Ninja Virtual Machine stopped");
    }   



 }