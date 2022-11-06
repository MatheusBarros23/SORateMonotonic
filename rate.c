#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

FILE *pnt;
FILE *arq;

#define MAX_LINE 20
#define MAX_CMDS 20
#define HOLD 'H'
#define OUTPUT "rate_mprb.out"


struct ProcStruct
{

    char procID[MAX_LINE]; //QUANDO PRINTAR COLOCAR O T antes do ID!! VERIFICAR ISSO - RESOLVIDO, voltei para string
    int periodT;
    int execT;

    int state;

    //att to save timeStamps
    int holdT;
    int finishT;
    int waitT;

    //att to store limits
    int qtdExec;
    int qtdlost;
    int qtdkilled;
};
struct ProcStruct procStruct[MAX_CMDS];

int periodLimit=0;
int Time=0;
int TimeIdle=0;
int stopExec=0;
int notIdle=0;
int lostTime=0;
int killTime=0;
char *temp;
int holdAux=0;

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

//To Check the struct, not used in final version!
void printStruct(struct ProcStruct proc[], int procCount){
    //verificar a struct
    for (int i = 0; i < procCount; ++i) {
        printf("procStruct[%d].procID: %s\n",i,procStruct[i].procID);
        printf("procStruct[%d].periodT: %d\n",i,procStruct[i].periodT);
        printf("procStruct[%d].execT: %d\n",i,procStruct[i].execT);
        printf("procStruct[%d].waitT: %d\n",i,procStruct[i].waitT);
        printf("procStruct[%d].holdT: %d\n",i,procStruct[i].holdT);
        printf("procStruct[%d].finishT: %d\n",i,procStruct[i].finishT);
        printf("procStruct[%d].qtdKilled: %d\n",i,procStruct[i].qtdkilled);
        printf("procStruct[%d].qtdExec: %d\n",i,procStruct[i].qtdExec);
    }
}

//first used to ordenate the code, but after working well with qsort(), deprecated!
/*void sortByArrivalT(struct ProcStruct proc[], int procCount){  //Sort para ordenar a Struct pelo periodo
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
 */


int checkExecute(struct ProcStruct proc[], int procCount){
    for (int i = 0; i < procCount; ++i) {
        if(proc[i].waitT!=0){
            return i;
        }
    }
    return 0;
}

//Used to implement the logic behind the qsort()! Tested with <=4
int comparator(const void *a, const void *b)
{
    struct ProcStruct *periodA = (struct ProcStruct *)a;
    struct ProcStruct *periodB = (struct ProcStruct *)b;

    if(periodA->periodT < periodB->periodT && periodA->waitT > 0){

        return (periodA->periodT - periodB->periodT);
    }else if(periodA->waitT <=0){

        return (periodB->waitT - periodA->waitT);
    }else if(periodB->waitT > 0 && periodA->waitT <=0){

        return ( periodB->waitT -periodA->waitT );
    }

}


//used to implement some logic behind the execution time, but not used anymore!
/*int mdc(int num1, int num2) {
    int resto;
    do {
        resto = num1 % num2;
        num1 = num2;
        num2 = resto; //Podia ser recursividade, mas preferi assim por enq
    } while (resto != 0);
    return num1;
}*/


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
        //Lendo o arquivo 1 por 1, saber a qtd de tasks!
        while (fread(&c, sizeof(char), 1, pnt)) {
            if (c == letra) {
                procCount++;
            }
        }
        fclose(pnt);
        printf("Processos: %d\n", procCount);

        char *procString = malloc(MAX_LINE * sizeof(char *)); //criar dinamicamente array

        pnt = fopen(argv[1], "r");
        arq = fopen("rate_mprb.out", "w+"); //arquivo de saída conforme as especificacoes no PDF

        int h = 0;
        int procs = 0;
        while (fscanf(pnt, "%[^\n] ", procString) != EOF) {
            // printf("procSting: %s\n",procString);
            if (h == 0) {
                periodLimit = atoi(procString);
                printf("Time Limit: %d\n", periodLimit);
                h++;
            } else {
                int procCount;
                char **procArray = malloc(*procString * sizeof(char *));
                procArray = splitString(procString, &procCount);


                sprintf(procStruct[procs].procID, "%s", procArray[0]);
               // procStruct[procs].procID = atoi(procArray[0]); //Prob em trabalhar com string >> RESOLVIDO!
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
            printf("Output saved in file: %s\n",OUTPUT);


        //ordenar conforme as especificações do comparator!
        qsort(procStruct,procCount,sizeof(procStruct[0]),comparator);

        //verificar a struct
            //printStruct(procStruct,procCount);

        fprintf(arq,"EXECUTION BY RATE\n");
        //EXECUTION BY RATE
            while (should_run == 1) {
                //para saber qual pode ser executado agora!! (sempre o primeiro!)
               // sortByArrivalT(procStruct, procCount); // - FUNCIONA BEM COM <=3!! em cima, para >=4!!
                qsort(procStruct,procCount,sizeof(procStruct[0]),comparator); //Resolve o prob de ordenar +4

                /*Check the Struct*/
                //printStruct(procStruct, procCount);

                //execution!
                if (procStruct[0].waitT > 0 || (Time % procStruct[0].periodT == 0)) {
                     //estou entrando
                    for (int i = 0; i < procCount; ++i) {
                        if ((Time % procStruct[i].periodT == 0 || procStruct[i].periodT == Time) &&
                            procStruct[i].waitT == 0) { //creio que o erro possa estar aqui!!
                            procStruct[i].waitT = procStruct[i].execT;
                        }
                    }

                    if (procStruct[0].waitT + Time <= periodLimit || procStruct[0].waitT > 0) { //para quando conseguir executar "normal"
                        while (procStruct[0].waitT <= procStruct[checkExecute(procStruct, procCount)].periodT && procStruct[0].waitT > 0) {
                            //  printf("\t\tENTREI\n");
                            for (int i = 0; i < procCount; ++i) {
                                if ((Time % procStruct[i].periodT == 0 || procStruct[i].periodT == Time) && procStruct[i].waitT == 0) {
                                    procStruct[i].waitT = procStruct[i].execT;
                                }
                            }
                            Time++;

                            notIdle = 0;
                            if (TimeIdle > 0 && notIdle == 0) {
                                fprintf(arq, "idle for %d units\n", TimeIdle);
                            }
                            TimeIdle = 0;
                            procStruct[0].waitT--;
                            procStruct[0].holdT++;
                            if (procStruct[0].holdT > procStruct[0].execT) {
                                procStruct[0].holdT = procStruct[0].execT;
                            }
                          //  printf("\tTIME executando: %d\n", Time);

                            //for para verificar um de prioridade MAIOR!
                            for (int i = 1; i < procCount; ++i) {
                               // printf("\tTIME FOR CONDICIONAL: %d\n", Time);
                                if (procStruct[0].periodT > procStruct[i].periodT && Time % procStruct[i].periodT == 0) { //Caso executando encontre um de prioridade MAIOR!
                                    procStruct[i].waitT = procStruct[i].execT;
                                    stopExec = 1;
                                  //  printf("ENTREI PRIORIDADE: stopExec = %d\n", stopExec);
                                    //chegou prioridade! e guarda o wait!!
                                    if (procStruct[0].waitT > 0 && holdAux==0 && Time<periodLimit) { //só printar caso ainda tenha tempo!! Caso não, vai para as proximas condiçoes...
                                       // printStruct(procStruct,procCount);
                                        fprintf(arq, "[%s] for %d units - H\n", procStruct[0].procID, procStruct[0].execT - procStruct[0].waitT);
                                        procStruct[0].finishT = procStruct[0].execT - procStruct[0].waitT;
                                        holdAux=1;
                                    }else if(procStruct[0].waitT > 0 && holdAux==0 && Time>=periodLimit){ //para não printar o lost, pq ele é Kill!!
                                        procStruct[0].finishT = procStruct[0].execT - procStruct[0].waitT;
                                    }
                                    //verificar novo arrival!! COM WAITTIME
                                } else if (Time % procStruct[0].periodT == 0 && procStruct[0].waitT > 0 && procStruct[0].waitT + Time < periodLimit || checkExecute(procStruct, procCount) != 0) {


                                    for (int j = 0; j < procCount; ++j) {
                                        if (Time % procStruct[j].periodT == 0 && procStruct[j].waitT > 0 && (procStruct[j].waitT != procStruct[j].execT)) {
                                           // printStruct(procStruct,procCount);
                                            lostTime = procStruct[j].holdT - procStruct[j].finishT;
                                            printf("[%s] for %d units - L\n", procStruct[j].procID, lostTime);
                                            fprintf(arq, "[%s] for %d units - L\n", procStruct[j].procID, lostTime);
                                            procStruct[j].qtdlost++;
                                            lostTime = 0;
                                            procStruct[j].waitT = procStruct[j].execT;
                                          //  j++;
                                            holdAux=0;
                                        }
                                    }
                                } else if (Time + procStruct[0].waitT >= periodLimit) {
                                    holdAux=0;
                                    for (int j = 0; j < procCount; ++j) {
                                        if (Time >= periodLimit && procStruct[j].waitT > 0) {
                                            //verificar caso tenha chegado algum no final!! alem do 1...
                                            for (int k = 0; k < procCount; ++k) {
                                                if ((Time % procStruct[k].periodT == 0) && procStruct[k].waitT == 0) {
                                                    procStruct[k].waitT = procStruct[k].execT;
                                                }
                                            }
                                            killTime = procStruct[j].waitT;
                                            if ((procStruct[j].execT - killTime < 0 ||procStruct[j].waitT == procStruct[j].execT) && Time == periodLimit) { //ou seja, não tiver sido executado... nem printo!

                                            } else if (Time <= periodLimit && temp != procStruct[j].procID && Time + procStruct[j].waitT <= periodLimit ) { //&& temp != procStruct[j].procID - para evitar prints iguais!!
                                                fprintf(arq, "[%s] for %d units - K\n", procStruct[j].procID,procStruct[j].execT -killTime); //Resolver o prob de printar 2x no input4.txt
                                                if (procStruct[j].qtdkilled < 1) {
                                                    procStruct[j].qtdkilled++;
                                                    killTime = 0;
                                                }
                                                temp = procStruct[j].procID;
                                            }else if (Time <= periodLimit && temp != procStruct[j].procID && Time + procStruct[j].waitT > periodLimit ){
                                            //    printStruct(procStruct,procCount);
                                            //    printf("TIME: %d\n",Time);
                                            //    printf("killTime: %d\n",killTime);
                                            //    printf("j: %d\n",j);

                                                if((procStruct[j].execT - procStruct[j].finishT) - killTime > 0){
                                                    fprintf(arq, "[%s] for %d units - K\n", procStruct[j].procID, (procStruct[j].execT - procStruct[j].finishT) - killTime); //Resolver o prob de printar 2x no input4.txt
                                                }else if((procStruct[j].execT - procStruct[j].finishT) - killTime == 0){
                                                    fprintf(arq, "[%s] for %d units - K\n", procStruct[j].procID, killTime);
                                                }
                                                else if (procStruct[j].qtdkilled < 1) {
                                                    procStruct[j].qtdkilled++;
                                                    killTime = 0;
                                                }
                                                temp = procStruct[j].procID;
                                            }
                                            else if (procStruct[j].qtdkilled < 1) {

                                                procStruct[j].qtdkilled++;
                                                killTime = 0;
                                            } else {
                                                should_run = 2;
                                                killTime = 0;
                                            }
                                        }
                                    }
                                }
                            }

                            //FINISHED Time
                            if (procStruct[0].waitT == 0 && should_run == 1 && Time < periodLimit) {
                                //  fprintf(arq,"TIME [T%d] for %d TIME - F\n",procStruct[0].procID, Time); //usando tais condições para printar!
                                if (Time % procStruct[0].periodT !=procStruct[0].execT) { //Aqui utilizava ainda o mdc para diminuir e fazer um calculo com o .execT, mas era uma solucao "viciada"
                                    fprintf(arq, "[%s] for %d units - F\n", procStruct[0].procID,procStruct[0].execT- procStruct[0].finishT);
                                    procStruct[0].finishT=0;

                                } else if (Time % procStruct[0].periodT == procStruct[0].execT) {
                                    fprintf(arq, "[%s] for %d units - F\n", procStruct[0].procID, procStruct[0].holdT);
                                    procStruct[0].holdT = 0;
                                } else {
                                    fprintf(arq, "[%s] for %d units - F\n", procStruct[0].procID,procStruct[0].execT- procStruct[0].finishT);
                                    procStruct[0].finishT=0;
                                }
                                procStruct[0].qtdExec++;
                            }

                            if (stopExec == 1) {
                                break;
                            }

                        }
                    }

                  //  printStruct(procStruct, procCount);
                } else if (Time >= periodLimit) {
                    should_run = 2;
                } else if (notIdle == 1 || checkExecute(procStruct, procCount) == 0) { //idle time!! (all .waitT=0 and not waiting)
                    //  printf("TIME AFTER IDLE: %d\n",Time);
                    TimeIdle++;
                    Time++;
                    //need a arrival CHECK!!
                    for (int k = 0; k < procCount; ++k) {
                        if (Time == procStruct[k].periodT && procStruct[k].waitT == 0 && should_run == 1) {
                            procStruct[k].waitT = procStruct[k].execT;
                            //  printf("TO AQUI1\n");
                        } else if (Time % procStruct[k].periodT == 0 && procStruct[k].waitT == 0 && Time >= procStruct[k].periodT && should_run == 1) {
                            procStruct[k].waitT = procStruct[k].execT;
                            //  printf("TO AQUI2\n");
                        }
                    }
                    /*Check the idle time and behavor*/
                   /* printf("\tIDLE STRUCT: \n");
                    printStruct(procStruct, procCount);*/
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
            if(i+1==procCount){
                fprintf(arq,"[%s] %d",procStruct[i].procID,procStruct[i].qtdkilled);
            }else{
                fprintf(arq,"[%s] %d\n",procStruct[i].procID,procStruct[i].qtdkilled);
            }
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
/*LEMBRAR PDF!!!*/