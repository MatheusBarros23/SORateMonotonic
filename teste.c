#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <fcntl.h>

struct abc{
    int a;
    char b;
    float c;
};
int main()
{
    struct abc obj[2];
    obj[0].a = 4;
    obj[0].b = 'd';
    obj[0].c = 5.5;
    obj[1].a = 5;
    obj[1].b = 'f';
    obj[1].c = 8.2;
    printf("[0]a=%d,[0]b=%c,[0]c=%f\n",obj[0].a,obj[0].b,obj[0].c);
    printf("[1]a=%d,[1]b=%c,[1]c=%f\n",obj[1].a,obj[1].b,obj[1].c);
    return 0;
}