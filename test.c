#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <fcntl.h>

int main(){
    int stemp = 50;
    int stemp2 = 80;
    int execT = 25;
    int limit = 165;
    int count =0;
    int count2 =0;
    int timeExecuted=0;
    int timeExecuted2=0;
    int lost=0;
    //i == TIME
    int mmc2=5;

    for (int i = 0; i < limit; ++i) {
        if(i%stemp==0){
            count++;
        }else if(i%mmc2==0 && i%stemp2==0){
            count2++;
        }
    }
    printf("count: %d\n",count);
    printf("count2: %d\n",count2);

    return 0;
}