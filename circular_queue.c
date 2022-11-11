#include <stdio.h>
#include <stdlib.h>
#include "datu_egiturak.h"
#include "circular_queue.h"


/**
 * Emandako pcb-a emandako lista estekatuan sartzen du lehenengo posizioan.
 * param processQueue lista estekatuaren pointerra
 * param newPcb pcb berriaren pointerra
 */
void insertFirst(struct node **processQueue, struct pcb *newPcb){

    struct node *lagNode = *processQueue;
    struct node *newNode = (struct node*)malloc(sizeof(struct node));
    
    newNode->data = newPcb;
    newNode->next = *processQueue;

    if (lagNode->previous == NULL){
        newNode->next = newNode;
        newNode->previous = newNode;
    }else{
        newNode->previous = lagNode->previous;
        lagNode->previous = newNode;
        newNode->previous->next = newNode;
    }
    *processQueue = newNode;


}

/**
 * Emandako lista estekatuaren lehenengo posizioa ezabatzen du. Ezabatutako nodoaren pcb-a itzultzen du.
 * param processQueue lista estekatuaren pointerra
 * return ezabatutako nodoaren pcb-a
 */
struct pcb* deleteFirst(struct node *processQueue){
    struct pcb *deletePcb = processQueue->data;
    //Ez bada bakarra listan
    if(processQueue->next != processQueue){
        //Head berria hurrengoa izango da
        processQueue->data = processQueue->next->data;
        processQueue->next = processQueue->next->next;
        processQueue->next->previous = processQueue;
    }else{
        //Bakarra bada listan borratu hau bere parametro guztiak NULL jarriz
        processQueue->next = NULL;
        processQueue->data = NULL;
        processQueue->previous = NULL;

    }
    return deletePcb;
}

/**
 * Emandako lista estekatua hutsik badago 1 itzuli bestela 0.
 * return 1 itzuli hutsik badago bestela 0
 */
int isEmpty(struct node *processQueue){
    return processQueue->next == NULL;
}

/**
 * Emandako lista estekatutik emandako nodoa ezabatzen du.
 * param processQueue lista estekatuaren pointerra
 * param deleteNode ezabatu beharreko nodoaren pointerra
 */
struct pcb* deleteByNode(struct node *processQueue, struct node *deleteNode){
    //Hutsik badago
    if(isEmpty(processQueue))
        return NULL;
    //Head eta borratu beharreko nodoa berdinak badira deleteFirst egin(Tratamendu espeziala)
    if (processQueue == deleteNode){
        return deleteFirst(processQueue);
    }else{
        //Ezabatu nodoa normalki
        deleteNode->previous->next = deleteNode->next;
        deleteNode->next->previous = deleteNode->previous;
    }
    return deleteNode->data;
}

/**
 * Emandako lista estekatuak dituen prozesu guztien pid-a pantailaratzen ditu.
 * param processQueue lista estekatuaren pointerra
 */
void printProcessQueue(struct node *processQueue){

    struct node *currentNode;
    currentNode = processQueue;
    printf("Prozesuen lista: \n");
    if(!isEmpty(processQueue)){
        while (currentNode->next != processQueue){
            printf("Process ID: %d\n", currentNode->data->ID);
            //Informazio gehiago nahi bada prozesuari buruz deskomentatu
            /*
            printf("Process next: %d\n", currentNode->next);
            printf("Process previous: %d\n", currentNode->previous);
            //printf("Process last: %d\n", currentNode->last);
            printf("Process me: %d\n", currentNode);
            printf("\n");*/
            currentNode = currentNode->next;
        } 
        printf("Process ID: %d\n", currentNode->data->ID);
        /*
        printf("Process next: %d\n", currentNode->next);
        printf("Process previous: %d\n", currentNode->previous);
        //printf("Process last: %d\n", currentNode->last);
        printf("Process me: %d\n", currentNode);
        printf("\n");*/
    }

}

/**
 * Lista estekatua sortzeko funtzioa.
 */
struct node* createCircularQueue(){
    struct node *processQueue = (struct node*)malloc(sizeof(struct node));
    processQueue->next = NULL;
    processQueue->data = NULL;
    processQueue->previous = NULL;
    return processQueue;

}

//Lista estekatuak(zirkularrak) probatzeko egin da main hau
/*
int main(int argc, char *argv[]) {
    int i = 0;
    struct node *processQueue = createCircularQueue();
    struct pcb *newPcb;
    
    while(i<6){
        
        //Prozesu berria sortu
        newPcb = (struct pcb*)malloc(sizeof(struct pcb));  //Memoria alokatu
        newPcb->ID = i;
        i++;                                                  //Prozesu kopurua handitu
        insertFirst(&processQueue,  newPcb);                           //Prozesuen ilaran sartu
        //printProcessQueue(processQueue);
        
    }
    

    //deleteLast(processQueue);
    printProcessQueue(processQueue);
    

    deleteByNode(processQueue, processQueue->next->next);
    printProcessQueue(processQueue);

    deleteByNode(processQueue, processQueue);
    printProcessQueue(processQueue);

    deleteByNode(processQueue, processQueue->next->next->next);
    printProcessQueue(processQueue);

    newPcb = (struct pcb*)malloc(sizeof(struct pcb));  //Memoria alokatu
    newPcb->ID = 6;
    insertFirst(&processQueue,  newPcb);
    printProcessQueue(processQueue);

    struct node *currentNode;
    currentNode = processQueue->next;
    while (currentNode != processQueue){
        deleteByNode(processQueue, currentNode);
        currentNode = currentNode->next;
    }
    deleteByNode(processQueue, processQueue);

    printProcessQueue(processQueue);

}*/