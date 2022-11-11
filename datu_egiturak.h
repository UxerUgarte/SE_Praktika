#include <semaphore.h>
#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
// datu_egiturak.h
#ifndef DATU_EGITURAK_H
#define DATU_EGITURAK_H

#define SCH_FREC 300000
#define PRSOR_FREC 1000000

#define FIFO_SCH 0
#define RR_SCH 1

#define READY 0
#define EXECUTING 1
#define BLOCKED 2
#define TERMINATED 3


extern pthread_mutex_t mutex1;
extern pthread_cond_t  cond1;
extern pthread_cond_t  cond2;

extern sem_t sem_scheduler;
extern sem_t sem_pr_sor;

extern struct tenp *tenp_prsor;
extern struct tenp *tenp_sch;

extern int done;
extern int tenp_kop;

extern int prozesu_id;

extern struct node *processQueue;

extern struct pcb *prozesu_nulua;
extern struct pcb *executing;

extern int executed;
extern int lagExecuted;

extern int terminated;

extern int upper;
extern int lower;

extern float denb;

extern int sch;


struct pcb
{
    int ID;
    int STATE;
    float CPU_TIME;
    float beharrezko_denbora;
    /*
    int PRIORITY;
    int PC;*/
};

struct node {
   struct node *next;
   //struct node *last;
   struct node *previous;
   struct pcb *data;
};

struct tenp {
    int kont;
    sem_t sem;
};

#endif // datu_egiturak.h
