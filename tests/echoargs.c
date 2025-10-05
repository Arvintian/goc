#include <stdio.h>

int main(int argc, char *args[])
{
    int a;
    scanf("%d",&a);
    printf("%d\n",a);
    for (int i = 0; i < argc; i++)
    {
        printf("argc[%d] -- %s\n", i, args[i]);
    }
}