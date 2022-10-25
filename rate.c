#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <fcntl.h>

FILE *pnt;

#define MAX_LINE 20

typedef struct
{
    int size;
    char* proc;
    int arrivalT;
    int execT;
}ProcStruct;


int periodLimit=0;
char *args_proc[MAX_LINE/2];

int main(int argc, char* argv[]) {

    while(argc>1&&argc<=2){
        printf("Get inputs in file: %s\n",argv[1]);

        pnt=fopen(argv[1],"r");
        if (pnt == NULL )
        {
            printf("Arquivo não existe! Tente novamente\n");
            exit(0);
        }
        fclose(pnt);

        char c, letra='\n';
        int procCount=0;

        fopen(argv[1],"r");
        //Lendo o arquivo 1 por 1
        while(fread (&c, sizeof(char), 1, pnt)) {
            if(c == letra) {
                procCount++;
            }
        }
        fclose(pnt);
        printf("Processos: %d\n",procCount);

        char *procString = malloc(MAX_LINE * sizeof(char *)); //criar dinamicamente array

        pnt=fopen(argv[1],"r");
        int h=0;
        while (fscanf(pnt, "%[^\n] ", procString) != EOF ){
            printf("procSting: %s\n",procString);
            if(h==0){
                periodLimit = atoi(procString);
                printf("periodLimit: %d\n",periodLimit);
                h++;
            }else{
                char *txt;
                txt = strtok(procString, " ");
                int k = 0;

                while (txt != NULL) {
                    args_proc[k] = txt;
                    txt = strtok(NULL, " ");
                    k++;
                }

                for (int i = 0; i < procCount; ++i) { //tenho que pensar aqui! Pq tanto o 0 vai em todos no primeiro, o 1 tbm vai em todos depois! Podia usar o h-1
                    printf("args_proc[%i]: %s\n",i,args_proc[i]);
                    ProcStruct procStruct = {procCount};
                    procStruct.proc = args_proc[0];
                    procStruct.arrivalT = atoi(args_proc[1]);
                    procStruct.execT = atoi(args_proc[2]);
                }
                h++;
            }
        }



        exit(0);
    }

    return 0;
}
