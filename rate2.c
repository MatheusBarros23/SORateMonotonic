#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

FILE *pnt;

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
    int procID; //QUANDO PRINTAR COLOCAR O T antes do ID!! VERIFICAR ISSO
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
int Time=-1;
int TimeIdle=0;
int stopExec=0;
int notIdle=0;
int lostTime=0;
int killTime=0;
int temp=0;
int auxCheckExec;
int taskAtual;


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
    struct ProcStruct temp;                                 //ENTENDER QUEM EH O i e o J!!
    for (int i = 0; i < procCount-1; ++i) {
        for (int j = i+1; j < procCount; ++j) { //FAZER% ISSO SEPARADO!! NAO DA NO MESMO CAMPO!!
            if(proc[i].periodT > proc[j].periodT){
                printf("ENTREI AQUI1\n");
                temp = proc[i];
                proc[i] = proc[j];
                proc[j] = temp;
            }
        }
    }
}

int checkAllExecute(struct ProcStruct proc[], int procCount){  //Selection Sort para ordenar a Struct pelo periodo
    for (int i = 0; i < procCount; ++i) {
        if(procStruct[i].waitT!=0){
            return i;
        }
    }
    return 0;
}

int checkExecuteTime(struct ProcStruct proc[], int procCount, int Time){  //Selection Sort para ordenar a Struct pelo periodo
    int noneCheck=0;
    for (int i = 0; i < procCount; ++i) {
        if(procStruct[i].waitT > 0 && Time%procStruct[i].periodT==0){
            noneCheck++;
            return i;
        }else if(procStruct[i].waitT==0){
            printf("t%d ENCERROU!!\n",procStruct[i].periodT);
        }
    }
    if(noneCheck==0) {
        return 0;
    }
}

//preciso PROCURAR pelo maior prioridade com tempo de execução maior que 0

void printStruct(struct ProcStruct proc[], int procCount){
    //verificar a struct
    for (int i = 0; i < procCount; ++i) {
        printf("procStruct[%d].procID: T%d\n",i,procStruct[i].procID);
        printf("procStruct[%d].periodT: %d\n",i,procStruct[i].periodT);
        printf("procStruct[%d].execT: %d\n",i,procStruct[i].execT);
        printf("procStruct[%d].waitT: %d\n",i,procStruct[i].waitT);
        //printf("procStruct[%d].qtdExec: %d\n",i,procStruct[i].qtdExec);
    }
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
                procStruct[procs].state = HOLD;
                if (procStruct[procs].state == HOLD) {
                    procStruct[procs].waitT = procStruct[procs].execT;
                }
                procs++;
                free(procArray);
            }
        }
        fclose(pnt);

        sortByArrivalT(procStruct, procCount);

        //verificar a struct
        printStruct(procStruct, procCount);


        while(should_run==1){
            //para saber qual pode ser executado agora!! (sempre o primeiro!)
            sortByArrivalT(procStruct, procCount);

            Time++;
            printf("Time: %d\n",Time);

           taskAtual = checkExecuteTime(procStruct,procCount,Time);
            printf("taskAtual: %d\n",taskAtual);
           if(Time%procStruct[taskAtual].periodT==0 && (checkExecuteTime(procStruct,procCount,Time))){
               printf("Entro para executar T%d\n",procStruct[taskAtual].procID);
               procStruct[taskAtual].waitT--;
           }else{
               printf("DEVERIA TA AQUI!!\n");
           }


            //verificando struct
            printStruct(procStruct,procCount);
            printf("------------------------------\n");
            //Saida!! Quando o Time > periodLimit
            if(Time > periodLimit){ //a saida de quando chega no periodLimit
                exit(0);
            }
        }
    }
    return 0;
}