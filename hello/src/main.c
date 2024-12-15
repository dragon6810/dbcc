#include <stdio.h>

// Main function
int main() 
{
    int x = 5; // Declare and initialize x
    int y = 10;
    int z = x + y;

    if (z > 10) 
    {
        printf("z is greater than 10\n");
    } 
    else 
    {
        printf("z is not greater than 10\n");
    }

    int y_new = 20;
    int z2 = x + y_new;
    printf("z2 is %d\n", z2);

    return 0;
}