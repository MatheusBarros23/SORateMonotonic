#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

FILE *pnt;
FILE *arq;

#define MAX_LINE 20
#define MAX_CMDS 20
#define FINISHED 'F'
#define LOST 'L'
#define KILLED 'K'
#define HOLD 'H'
#define EXECUTING 'E'


struct ProcStruct
{
    int size;
    char procID[MAX_LINE]; //QUANDO PRINTAR COLOCAR O T antes do ID!! VERIFICAR ISSO
    int periodT;
    int execT;

    int state;

    //att to save timeStamps
    int holdT;
    int killedT;
    int lostT;
    int waitT;

    //att to store limits
    int qtdExec;
    int qtdlost;
    int qtdkilled;
};
struct ProcStruct procStruct[MAX_CMDS];

int periodLimit=0;
char *args_proc[MAX_LINE/2];
char *arrayProcID[MAX_LINE/2];
int Time=0;
int TimeIdle=0;
int stopExec=0;
int notIdle=0;
int lostTime=0;
int killTime=0;
int temp=0;
int auxCheckExec;
int TimeExecAux=0;
int arrivalFinal=0;

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

void printStruct(struct ProcStruct proc[], int procCount){
    //verificar a struct
    for (int i = 0; i < procCount; ++i) {
        printf("procStruct[%d].procID: %s\n",i,procStruct[i].procID);
        printf("procStruct[%d].periodT: %d\n",i,procStruct[i].periodT);
        printf("procStruct[%d].execT: %d\n",i,procStruct[i].execT);
        printf("procStruct[%d].waitT: %d\n",i,procStruct[i].waitT);
        printf("procStruct[%d].holdT: %d\n",i,procStruct[i].holdT);
        printf("procStruct[%d].qtdKilled: %d\n",i,procStruct[i].qtdkilled);
    }
}

void sortByArrivalT(struct ProcStruct proc[], int procCount){  //Sort para ordenar a Struct pelo periodo
    struct ProcStruct temp;
    for (int i = 0; i < procCount-1; ++i) {
        for (int j = i+1; j < procCount; ++j) {
            if(proc[i].periodT > proc[j].periodT && proc[i].waitT>0){
                temp = proc[i];
                proc[i] = proc[j];
                proc[j] = temp;
            }
            else if(proc[i].waitT <= 0){
                temp = proc[j];
                proc[j] = proc[i];
                proc[i] = temp;
            }else { //por algum motivo o t4 nunca vai pra frente!! PQ?
                for (int k = 1; k < procCount; ++k) {
                    if(proc[k].waitT>0 && proc[k-1].waitT<=0){
                        temp = proc[k];
                        proc[k] = proc[k-1];
                        proc[k-1] = temp;
                    }
                }
            }
        }
    }
}

int checkAllExecute(struct ProcStruct proc[], int procCount){
    for (int i = 0; i < procCount; ++i) {
        if(procStruct[i].waitT!=0){
            return i;
        }
    }
    return 0;
}

int mdc(int num1, int num2) {
    int resto;
    do {
        resto = num1 % num2;
        num1 = num2;
        num2 = resto; //Podia ser recursividade, mas preferi assim por enq
    } while (resto != 0);
    return num1;
}

//preciso PROCURAR pelo maior prioridade com tempo de execução maior que 0




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
        arq = fopen("rate_mprb.out", "w+");

        int h = 0;
        int procs = 0;
        while (fscanf(pnt, "%[^\n] ", procString) != EOF) {
            // printf("procSting: %s\n",procString);
          // int char2Del = 0;
            if (h == 0) {
                periodLimit = atoi(procString);
                printf("periodLimit: %d\n", periodLimit);
                h++;
            } else {
               // memmove(&procString[char2Del], &procString[char2Del + 1],
               //         strlen(procString) - char2Del); //remover o T para só trabalhar com INT!! BUG com String
                int procCount;
                char **procArray = malloc(*procString * sizeof(char *));
                procArray = splitString(procString, &procCount);

                for (int i = 0; i <= 2; ++i) {
                    //  printf("procID: %s\n",procArray[i]);
                }
                sprintf(procStruct[procs].procID, "%s", procArray[0]);
                //procStruct[procs].procID = procArray[0];
                procStruct[procs].periodT = atoi(procArray[1]);
                procStruct[procs].execT = atoi(procArray[2]);
                procStruct[procs].state = HOLD;
                if(procStruct[procs].state==HOLD){
                    procStruct[procs].waitT=procStruct[procs].execT;
                }
                procs++;
                free(procArray);
            }
        }
        fclose(pnt);

        sortByArrivalT(procStruct, procCount);

        //verificar a struct
        printStruct(procStruct,procCount);
    }
    return 0;
}