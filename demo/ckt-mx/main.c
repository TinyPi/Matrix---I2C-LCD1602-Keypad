#include <stdio.h>

extern int server_init();

int main(int argc, char **argv)
{
    
    printf("welcome ! this is server routine\r\n");
    server_init();	
    
    return 0;
}
