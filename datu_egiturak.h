#include <semaphore.h>
#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
// datu_egiturak.h
#ifndef DATU_EGITURAK_H
#define DATU_EGITURAK_H

//Schedulerraren eta prozesu sortzailearen frekuentziak
#define SCH_FREC 300000
#define CORE_FREC 100000

//Planifikazio politika aukeratzeko
#define FIFO_SCH 0
#define RR_SCH 1

//Prozesu baten egoerak(Oraingoz bakarrik READY eta EXECUTING erabiltzen dira)
#define READY 0
#define EXECUTING 1
#define BLOCKED 2
#define TERMINATED 3

//Memoria fisikoaren tamaina
#define PHYSICAL_MEMORY_SIZE 16777216

//Prozesu bakoitzak okupa dezakeena
#define DATA_SIZE 4096
#define TEXT_SIZE 4096

//Kernelaren espazioa
#define KERNEL_SIZE 1000000


extern pthread_mutex_t mutex1;
extern pthread_cond_t  cond1;
extern pthread_cond_t  cond2;

extern sem_t sem_scheduler;         //Schedulerraren semaforoa
extern sem_t sem_pr_sor;            //Prozesu sortzailearen semaforoa

extern int done;                    //Erlokuaren "baliabideak"(zenbat tenporizadoreek okntsumitu duten beraien baliabidea)
extern int tenp_kop;                //Tenporizadore kopurua

extern int prozesu_id;              //Prozesuen id-a sortzen duen aldagaia

extern struct node *processQueue;   //Prozesuen ilara zirkularra

extern struct pcb *prozesu_nulua;   //Prozesu nulua
extern struct pcb *executing;       //Unean exekutatzen ari den prozesua

extern int executed;                //Schedulerraren tenporizadoreari prozesu bat exekutatu den edo jakinarazten dion aldagaia

extern int terminated;              //Aldagai hau hurrengo zatirako da oraindik ez da erabiltzen( Exekuzioa hari bakar batean egiteko erabiliko da)

extern float denb;                  //Prozesuaren exekuzio denbora(CPU_TIME)

extern int sch;                     //Zein planifikatzaile nahi duzun erabili. 0 fifo, 1 roundrobin

extern int PHYSICAL_MEMORY[];       //Memoria fisikoaren aldagaia
extern int num_cores;               //Core kopurua

//Memory management
struct mm {
    int pgb;
    int code;
    int datuak;
};

//Prozesu baten pcb
struct pcb
{
    int ID;
    int STATE;
    float CPU_TIME;
    float beharrezko_denbora;
    struct mm *MEMORY_MANAGEMENT;
    int PC;
    int registers[15];
    int IR;
};

struct node {
   struct node *next;
   struct node *previous;
   struct pcb *data;
};

struct core{
    int ID;
    int PC;
    int IR;
    int sartu;
    int registers[15];
    struct pcb *executing;
    int egoera; //OKupatuta edo ez
    struct node *unekoNodo;
    
};





#endif // datu_egiturak.h
