#include <stdio.h>

int main() 
{
    int x = 5;
    int y = 10;
    int z = x + y;

    if (z > 10) 
    {
        (void) printf("z is greater than 10\n");
    } 
    else 
    {
        (void) printf("z is not greater than 10\n");
    }

    return 0;
}