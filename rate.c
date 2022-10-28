#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <fcntl.h>

FILE *pnt;

#define MAX_LINE 20
#define MAX_CMDS 20
#define F "F"
#define L "L"
#define K "K"
#define H "H"


struct ProcStruct
{
    int size;
    int procID; //QUANDO PRINTAR COLOCAR O T antes do ID!!
    int periodT;
    int execT;
    int waitT;
};
struct ProcStruct procStruct[MAX_CMDS];

int periodLimit=0;
char *args_proc[MAX_LINE/2];
char *arrayProcID[MAX_LINE/2];
int Time=0;

int should_run=1;

char **splitString(char *string, int *cmdCount) {
    char delim =' ';
    *cmdCount =1;
    char **array = malloc(*cmdCount * sizeof(char *)); //criar dinamicamente array
    array[0] = &string[0]; //armazena no array a posicao de cada comando
    for (int i = 1; string[i] != 0; i++) {
        if (string[i] == delim) {
            (*cmdCount)++;
            array = realloc(array, sizeof(char *) * (*cmdCount));
            array[(*cmdCount) - 1] = &string[i + 1];
            string[i] = 0;
        }
        //  printf("array[i]: %s %i\n",*array,i);
    }
    return array;
}

void sortByArrivalT(struct ProcStruct proc[], int procCount){  //Selection Sort para ordenar a Struct pelo periodo
    struct ProcStruct temp;
    for (int i = 0; i < procCount-1; ++i) {
        for (int j = i+1; j < procCount; ++j) {
            if(proc[i].periodT > proc[j].periodT){
                temp = proc[i];
                proc[i] = proc[j];
                proc[j] = temp;
            }
        }
    }
}

int executeTime(struct ProcStruct proc[], int procCount){  //Selection Sort para ordenar a Struct pelo periodo
    //
    return 0;
}

int main(int argc, char* argv[]) {

    if(argc>1&&argc<=2) {
        printf("Get inputs in file: %s\n", argv[1]);

        pnt = fopen(argv[1], "r");
        if (pnt == NULL) {
            printf("Arquivo não existe! Tente novamente\n");
            exit(0);
        }
        fclose(pnt);

        char c, letra = '\n';
        int procCount = 0;

        fopen(argv[1], "r");
        //Lendo o arquivo 1 por 1
        while (fread(&c, sizeof(char), 1, pnt)) {
            if (c == letra) {
                procCount++;
            }
        }
        fclose(pnt);
        printf("Processos: %d\n", procCount);

        char *procString = malloc(MAX_LINE * sizeof(char *)); //criar dinamicamente array

        pnt = fopen(argv[1], "r");
        int h = 0;
        int procs = 0;
        while (fscanf(pnt, "%[^\n] ", procString) != EOF) {
            // printf("procSting: %s\n",procString);
            int char2Del = 0;
            if (h == 0) {
                periodLimit = atoi(procString);
                printf("periodLimit: %d\n", periodLimit);
                h++;
            } else {
                memmove(&procString[char2Del], &procString[char2Del + 1],
                        strlen(procString) - char2Del); //remover o T para só trabalhar com INT!! BUG com String
                int procCount;
                char **procArray = malloc(*procString * sizeof(char *));
                procArray = splitString(procString, &procCount);

                for (int i = 0; i <= 2; ++i) {
                    //  printf("procID: %s\n",procArray[i]);
                }

                procStruct[procs].procID = atoi(procArray[0]);
                procStruct[procs].periodT = atoi(procArray[1]);
                procStruct[procs].execT = atoi(procArray[2]);
                procs++;
                free(procArray);
            }
        }

        fclose(pnt);

        sortByArrivalT(procStruct, procCount);

        //verificar a struct
        /*for (int i = 0; i < 3; ++i) {
            printf("procStruct[%d].procID: T%d\n",i,procStruct[i].procID);
            printf("procStruct[%d].periodT: %d\n",i,procStruct[i].periodT);
            printf("procStruct[%d].execT: %d\n",i,procStruct[i].execT);
            printf("procStruct[%d].waitT: %d\n",i,procStruct[i].waitT);
        }*/

        while (Time <= periodLimit && should_run==1){

            if (Time>periodLimit){
                printf("Tratar quando passa direto! para pegar o que sobra!!");
            }
        }

    //Error referentes ao argvs
    }
    else if (argc == 1) {
        printf("Please, inform some input file\n");
    } else if (argc > 2) {
        printf("Sorry, but more files were informed than supported\n");
    }
    return 0;
}
//Create a array list to store all necessary information!!
//Testar fazer um for pela quantidade e fazer a marcação de quando os processos vão chegar!! De modo a fazer o "relogio"

//time + execT == 165 - KILL