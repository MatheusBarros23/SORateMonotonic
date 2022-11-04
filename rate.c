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
    char procID[MAX_LINE]; //QUANDO PRINTAR COLOCAR O T antes do ID!! VERIFICAR ISSO - RESOLVIDO, voltei para string
    int periodT;
    int execT;

    int state;

    //att to save timeStamps
    int holdT;
    int killedT;
    int finishT;
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
int finishTimeH=0;
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
            }else { //por algum motivo o t4 nunca vai pra frente!! PQ? #Fazer um for especifico para quando tiver situação dos 4... Ou só um elseif desse if..
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
            int char2Del = 0;
            if (h == 0) {
                periodLimit = atoi(procString);
                printf("periodLimit: %d\n", periodLimit);
                h++;
            } else {
             //   memmove(&procString[char2Del], &procString[char2Del + 1],
               //         strlen(procString) - char2Del); //remover o T para só trabalhar com INT!! BUG com String
                int procCount;
                char **procArray = malloc(*procString * sizeof(char *));
                procArray = splitString(procString, &procCount);

                for (int i = 0; i <= 2; ++i) {
                    //  printf("procID: %s\n",procArray[i]);
                }
                sprintf(procStruct[procs].procID, "%s", procArray[0]);
               // procStruct[procs].procID = atoi(procArray[0]); //Prob de trabalhar com string RESOLVIDO!
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

        fprintf(arq,"EXECUTION BY RATE\n");
        //EXECUTION BY RATE
        while(should_run==1){
            //para saber qual pode ser executado agora!! (sempre o primeiro!)
            sortByArrivalT(procStruct, procCount);
            printStruct(procStruct,procCount);

            //execution!
            if (procStruct[0].waitT>0 || (Time%procStruct[0].periodT==0)){
                printf(" ENTRO P/ EXECUTAR o %s\n",procStruct[0].procID); //estou entrando
                for (int i = 0; i < procCount; ++i) {
                    if ((Time % procStruct[i].periodT == 0 || procStruct[i].periodT ==Time) && procStruct[i].waitT == 0) { //creio que o erro possa estar aqui!!
                        procStruct[i].waitT = procStruct[i].execT;
                    }
                }
                if(procStruct[0].waitT + Time <= periodLimit || procStruct[0].waitT>0){ //para quando conseguir executar normal (sem deadline, por enquanto)
                    //Só entra enquanto for menor dq o periodLimit! Vou fazer o tratamento de quanto falta, no else if()
                    while(procStruct[0].waitT <= procStruct[checkAllExecute(procStruct, procCount)].periodT && procStruct[0].waitT > 0){
                      //  printf("\t\tENTREI\n");
                        for (int i = 0; i < procCount; ++i) {
                            if ((Time % procStruct[i].periodT == 0 || procStruct[i].periodT ==Time) && procStruct[i].waitT == 0) { //creio que o erro possa estar aqui!!
                                procStruct[i].waitT = procStruct[i].execT;
                            }
                        }
                        Time++;

                        notIdle=0;
                        if(TimeIdle>0 && notIdle==0){
                            fprintf(arq,"idle for %d units\n",TimeIdle);
                        }
                        TimeIdle=0;
                        procStruct[0].waitT--;
                        procStruct[0].holdT++;
                        if(procStruct[0].holdT>procStruct[0].execT){
                            procStruct[0].holdT = procStruct[0].execT;
                        }
                        printf("\tTIME executando: %d\n",Time);

                        //Var para auxiliar o calc de tempo de cada execução, precisamente no F //Not Needded!
                            //TimeExecAux = procStruct[0].waitT;

                        //for para verificar um de prioridade MAIOR!
                        for (int i = 1; i < procCount; ++i) {
                            printf("\tTIME FOR CONDICIONAL: %d\n",Time);
                            if(procStruct[0].periodT > procStruct[i].periodT && Time%procStruct[i].periodT==0){ //Caso executando encontre um de prioridade MAIOR!
                                procStruct[i].waitT = procStruct[i].execT;
                                stopExec=1;
                                printf("ENTREI PRIORIDADE: stopExec = %d\n",stopExec);
                             //chegou prioridade! e guarda o wait!! - PENSAR COMO MOSTRAR ISSO!! QUANDO TERMINA NAO ENTRA AQUI!!
                                printf("[%s] for %d units - H\n",procStruct[0].procID,procStruct[0].execT-procStruct[0].waitT);
                                if(procStruct[0].waitT >0){ //só printar caso ainda tenha tempo!! Caso não, vai para as proximas condiçoes...
                                    fprintf(arq,"[%s] for %d units - H\n",procStruct[0].procID,  procStruct[0].execT-procStruct[0].waitT);
                                    procStruct[0].finishT = procStruct[0].execT-procStruct[0].waitT;
                                }
                          //verificar novo arrival!! COM WAITTIME
                            }
                            else if(Time%procStruct[0].periodT==0 && procStruct[0].waitT>0 && procStruct[0].waitT+Time<periodLimit || checkAllExecute(procStruct,procCount)!=0){
                                //PROB AQUI!!
                                for (int j = 0; j < procCount; ++j) {
                                    if(Time%procStruct[j].periodT==0 && procStruct[j].waitT>0 && (procStruct[j].waitT!=procStruct[j].execT)){ //prob quando um proc nunca chega a ser executado! :/
                                        lostTime = procStruct[j].waitT;
                                        printf("[%s] for %d units - L\n",procStruct[j].procID, lostTime);
                                        fprintf(arq,"[%s] for %d units - L\n",procStruct[j].procID, lostTime);
                                        procStruct[0].qtdlost++;
                                        lostTime=0;
                                        procStruct[j].waitT = procStruct[j].execT;
                                        j++;
                                    }
                                }
                            }else if(Time + procStruct[0].waitT >= periodLimit){
                                for (int j = 0; j < procCount; ++j) {
                                    if(Time >= periodLimit && procStruct[j].waitT>0){
                                        //verificar caso tenha chegado algum no final!! alem do 1...
                                        for (int k = 0; k < procCount; ++k) {
                                            if ((Time % procStruct[k].periodT == 0) && procStruct[k].waitT == 0) {
                                                procStruct[k].waitT = procStruct[k].execT;
                                            }
                                        }
                                        killTime = procStruct[j].waitT;
                                        if((procStruct[j].execT - killTime < 0 || procStruct[j].waitT == procStruct[j].execT) && Time == periodLimit){ //ou seja, não tiver sido executado... nem printo!

                                        }else if(Time <= periodLimit){
                                            fprintf(arq,"[%s] for %d units - K\n", procStruct[j].procID, procStruct[j].execT - killTime); //Resolver o prob de printar 2x no input4.txt
                                            if(procStruct[j].qtdkilled<1 ){
                                                procStruct[j].qtdkilled++;
                                                killTime = 0;
                                            }
                                        }else if(procStruct[j].qtdkilled<1 ){
                                            procStruct[j].qtdkilled++;
                                            killTime = 0;
                                        }else{
                                            should_run = 2;
                                            killTime = 0;
                                        }
                                    }
                                }
                            }
                        }

                    //FINISHED Time
                        if(procStruct[0].waitT==0 && should_run==1 && Time<periodLimit){
                            //EDITAR ISSO!! Printar só o exec time não faz sentido pq tem o lost time...
                          //  fprintf(arq,"TIME [T%d] for %d TIME - F\n",procStruct[0].procID, Time);
                            if(procStruct[0].execT - mdc(Time,procStruct[0].periodT)!=0 && Time%procStruct[0].periodT!=procStruct[0].execT){ //pela logica que fiz, esse calculo funcina melhor depois que passam o Tempo dos periodos!! por isso o if
                                if(procStruct[0].execT - mdc(Time,procStruct[0].periodT) <0){
                                    fprintf(arq,"[%s] for %d units - F\n",procStruct[0].procID,procStruct[0].execT );
                                }else {
                                    fprintf(arq,"[%s] for %d units - F\n",procStruct[0].procID, procStruct[0].execT-procStruct[0].finishT); //AQUI O ERRO, quando vem do lost!
                                }

                            }else if(Time%procStruct[0].periodT==procStruct[0].execT){
                                fprintf(arq,"[%s] for %d units - F\n",procStruct[0].procID, procStruct[0].holdT);
                                procStruct[0].holdT = 0;
                            }else{
                                fprintf(arq,"[%s] for %d units - F\n",procStruct[0].procID, mdc(Time,procStruct[0].periodT));
                            }
                            procStruct[0].qtdExec++;
                        }

                        if(stopExec==1){
                            //printar o que ficou em HOLD!!
                            break;
                        }

                    }
                }

                printStruct(procStruct,procCount);
            }
            else if(Time >= periodLimit){
                should_run=2;
            }else if(notIdle==1 || checkAllExecute(procStruct,procCount)==0) { //idle time!! (all .waitT=0 and not waiting)
              //  printf("TIME AFTER IDLE: %d\n",Time);
                TimeIdle++;
                Time++;
                //need a arrival CHECK!!
                for (int k = 0; k < procCount; ++k) {
                    if (Time == procStruct[k].periodT && procStruct[k].waitT == 0 && should_run==1) {
                        procStruct[k].waitT = procStruct[k].execT;
                      //  printf("TO AQUI2\n");
                    }else if(Time % procStruct[k].periodT ==0 && procStruct[k].waitT == 0 && Time >= procStruct[k].periodT && should_run==1){
                        procStruct[k].waitT = procStruct[k].execT;
                      //  printf("TO AQUI2\n");
                    }
                }
                printf("\tIDLE STRUCT: \n");
                printStruct(procStruct,procCount);
            }

        }
        fprintf(arq,"\n");
        fprintf(arq,"LOST DEADLINES \n");
        for (int i = 0; i < procCount; ++i) {
            fprintf(arq,"[%s] %d\n",procStruct[i].procID,procStruct[i].qtdlost);
        }

        fprintf(arq,"\n");
        fprintf(arq,"COMPLETE EXECUTION \n");
        for (int i = 0; i < procCount; ++i) {
            fprintf(arq,"[%s] %d\n",procStruct[i].procID,procStruct[i].qtdExec);
        }

        fprintf(arq,"\n");
        fprintf(arq,"KILLED \n");
        for (int i = 0; i < procCount; ++i) {
            fprintf(arq,"[%s] %d\n",procStruct[i].procID,procStruct[i].qtdkilled);
        }

        fclose(arq);
    }

//Error referentes ao argvs
    else if (argc == 1) {
        printf("Please, inform some input file\n");
    } else if (argc > 2) {
        printf("Sorry, but more files were informed than supported\n");
    }
    return 0;
}

//Testar fazer um for pela quantidade e fazer a marcação de quando os processos vão chegar!! De modo a fazer o "relogio"

//time + execT == 165 - KILL

/*AINDA FALTA O: IDLE
                 Killed
*/
